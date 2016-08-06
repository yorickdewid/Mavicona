#ifdef STATIC
#undef LIBPYTHON
#undef LIBLAVRIL
#endif

#include <zmq.hpp>
#include <string>
#include <iostream>
#include <map>
#ifdef LIBPYTHON
#include <Python.h>
#endif
#ifdef LIBLAVRIL
#include <lavril.h>
#endif
#include <csignal>
#include <dlfcn.h>
#include <quidpp.h>

#include "common/util.h"
#include "common/module.h"
#include "common/config.h"
#include "common/logger.h"
#include "common/cxxopts.h"
#include "protoc/scrapedata.pb.h"

#define DEFAULT_EXTRACTOR_HOST  "localhost:5577"
#define MAGIC_CHECK 0xe37abb23

static int itemCount = 0;

//  Prepare our context and socket
zmq::context_t context(1);
zmq::socket_t socket(context, ZMQ_REQ);

FileLogger logger("scrape");

std::multimap<std::string, std::string> datastack;

void signal_handler(int signum) {

}

static void catch_signals() {
	struct sigaction action;
	action.sa_handler = signal_handler;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
}

void dispatch_commit() {
	unsigned int complete = 0;
	for (auto const& ent : datastack) {
		quidpp::Quid quid;

		/* Create meta data object */
		ScrapeData data;
		data.set_name(ent.first);
		data.set_id(itemCount++);
		data.set_quid(quid.toString());
		data.set_type(ScrapeData::PLAIN);

		ScrapeData::Data *payload = new ScrapeData::Data;
		payload->set_payload(ent.second);
		payload->set_size(ent.second.size());
		// payload->set_extension(".zip");
		data.set_allocated_content(payload);

		std::string serialized;
		data.SerializeToString(&serialized);

		try {
			zmq::message_t request(serialized.size());
			memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
			socket.send(request);

			// Get the reply
			zmq::message_t reply;
			socket.recv(&reply);

			if (!strcmp((const char *)reply.data(), "DONE"))
				complete++;
		} catch (zmq::error_t& e) {
			std::cout << "Exit gracefully" << std::endl;
			std::cout << "Data NOT submitted to extractor" << std::endl;
			return;
		}
	}

	if (complete == datastack.size()) {
		logger << "Sumbitted datastack to dispatch" << FileLogger::endl();
		datastack.clear();
	} else {
		logger << "Datastack transaction failure" << FileLogger::endl();
	}
}

#ifdef LIBPYTHON

/* Push data on the stack */
static PyObject *py_mav_push(PyObject *self, PyObject *args) {
	const char *name;
	const char *data;
	size_t data_len = 0;

	if (!PyArg_ParseTuple(args, "st#", &name, &data, &data_len))
		return NULL;

	std::string bytea(reinterpret_cast<char const *>(data), data_len);
	datastack.insert(std::pair<std::string, std::string>(name, bytea));

	return Py_True;
}

/* Commit data to cluster */
static PyObject *py_mav_commit(PyObject *self, PyObject *args) {
	if (!PyArg_ParseTuple(args, ":numargs"))
		return NULL;

	dispatch_commit();

	return Py_True;
}

void pyrunner(const char *name) {
	Py_Initialize();

	static PyMethodDef MavMethods[] = {
		{
			"push", py_mav_push, METH_VARARGS,
			"Push data to scraper."
		},
		{
			"commit", py_mav_commit, METH_VARARGS,
			"Commit data to cluster."
		},
		{NULL, NULL, 0, NULL}
	};

	Py_InitModule("mavicona", MavMethods);

	FILE *py_file = fopen(name, "r");
	if (!py_file) {
		logger << FileLogger::error() << "Cannot open python file: " << name << FileLogger::endl();
		return;
	}

	PyRun_SimpleFile(py_file, name);
	fclose(py_file);

	Py_Finalize();
}

#endif // LIBPYTHON

#ifdef LIBLAVRIL

void print_func(HSQUIRRELVM LV_UNUSED_ARG(v), const SQChar *s, ...) {
	va_list vl;
	va_start(vl, s);
	vfprintf(stdout, s, vl);
	va_end(vl);
}

void error_func(HSQUIRRELVM LV_UNUSED_ARG(v), const SQChar *s, ...) {
	va_list vl;
	va_start(vl, s);
	vfprintf(stderr, s, vl);
	va_end(vl);
}

/* Push data on the stack */
static SQInteger lv_mav_push(HSQUIRRELVM v) {
	const SQChar *name, *data;
	if (LV_SUCCEEDED(lv_getstring(v, 2, &name))) {
		lv_getstring(v, 3, &data);
		printf("%s : %s\n", name, data);
		std::string bytea(reinterpret_cast<char const *>(data));
		datastack.insert(std::pair<std::string, std::string>(name, bytea));
	}

	return 0;
}

/* Commit data to cluster */
static SQInteger lv_mav_commit(HSQUIRRELVM v) {
	dispatch_commit();

	return 0;
}

void lvrunner(const char *name) {
	HSQUIRRELVM v = lv_open(1024);

	lv_setprintfunc(v, print_func, error_func);

	lv_pushroottable(v);

	init_module(blob, v);
	init_module(io, v);
	init_module(string, v);
	init_module(system, v);
	init_module(math, v);
	init_module(crypto, v);
	init_module(curl, v);

	lv_registererrorhandlers(v);

	lv_pushstring(v, _SC("mav"), -1);
	lv_newclass(v, SQFalse);

	lv_pushstring(v, _SC("push"), -1);
	lv_newclosure(v, lv_mav_push, 0);
	lv_setparamscheck(v, 3, ".ss");
	lv_setnativeclosurename(v, -1, _SC("push"));
	lv_newslot(v, -3, SQFalse);

	lv_pushstring(v, _SC("commit"), -1);
	lv_newclosure(v, lv_mav_commit, 0);
	lv_setparamscheck(v, 0, NULL);
	lv_setnativeclosurename(v, -1, _SC("commit"));
	lv_newslot(v, -3, SQFalse);

	lv_newslot(v, -3, SQFalse);

	if (LV_SUCCEEDED(lv_loadfile(v, name, SQTrue))) {
		lv_pushroottable(v);
		lv_call(v, 1, SQFalse, SQTrue);
	}

	lv_close(v);
}

#endif // LIBLAVRIL

#ifndef STATIC
void dsorunner(const char *libname, int argc, char *argv[]) {
	void *handle = dlopen(libname, RTLD_LAZY);
	if (!handle) {
		logger << FileLogger::error() << "Cannot open library: " << dlerror() << FileLogger::endl();
		return;
	}

	typedef struct  {
		char *name;
		void *data;
		size_t size;
	} item_t;

	struct s_datastack {
		unsigned int magic;
		item_t *data;
		size_t size;
	};

	typedef unsigned int (*init_t)();
	typedef int (*main_t)(int, char **);
	typedef struct s_datastack *(*commit_t)();

	init_t exec_init = (init_t)dlsym(handle, "mav_init"); //TODO err handling
	main_t exec_main = (main_t)dlsym(handle, "mav_main");
	commit_t exec_commit = (commit_t)dlsym(handle, "mav_commit");

	const char *dlsym_error = dlerror(); //TODO no way, this is UGLY!
	if (dlsym_error) {
		logger << FileLogger::error() << "Cannot load symbol 'mav_main': " << dlsym_error << FileLogger::endl();
		dlclose(handle);
		return;
	}

	logger << FileLogger::debug(1) << "Calling module..." << FileLogger::endl();
	assert(exec_init() == MAGIC_CHECK);
	int return_code = exec_main(argc, argv);
	struct s_datastack *return_stack = (struct s_datastack *) exec_commit();

	assert(return_stack->magic == MAGIC_CHECK);
	if (return_code != 0) {
		logger << FileLogger::warning() << "Module exit with non-zero return " << return_code << FileLogger::endl();
		return;
	}

	for (unsigned int i = 0; i < return_stack->size; ++i) {
		std::string bytea(reinterpret_cast<char const *>(return_stack->data[i].data), return_stack->data[i].size);
		datastack.insert(std::pair<std::string, std::string>(return_stack->data[i].name, bytea));

		free(return_stack->data[i].name);
		free(return_stack->data[i].data);
	}

	free(return_stack->data);

	dispatch_commit();

	dlclose(handle);
}
#else
extern "C" {
	typedef struct  {
		char *name;
		void *data;
		size_t size;
	} item_t;

	struct s_datastack {
		unsigned int magic;
		item_t *data;
		size_t size;
	};

	unsigned int mav_init();
	int mav_main(int argc, char *argv[]);
	struct s_datastack *mav_commit();
}

void slorunner(int argc, char *argv[]) {
	assert(mav_init() == MAGIC_CHECK);
	int return_code = mav_main(argc, argv);
	struct s_datastack *return_stack = mav_commit();

	assert(return_stack->magic == MAGIC_CHECK);
	if (return_code != 0) {
		logger << FileLogger::warning() << "Module exit with non-zero return " << return_code << FileLogger::endl();
		return;
	}

	for (unsigned int i = 0; i < return_stack->size; ++i) {
		std::string bytea(reinterpret_cast<char const *>(return_stack->data[i].data), return_stack->data[i].size);
		datastack.insert(std::pair<std::string, std::string>(return_stack->data[i].name, bytea));

		free(return_stack->data[i].name);
		free(return_stack->data[i].data);
	}

	free(return_stack->data);

	dispatch_commit();
}
#endif // STATIC

int main(int argc, char *argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

#ifdef STATIC
	cxxopts::Options options(argv[0], "");
#else
	cxxopts::Options options(argv[0], " [FILE]");
#endif // STATIC

	options.add_options("Help")
	("s,hbs", "Host based service config", cxxopts::value<std::string>(), "FILE")
	("v,version", "Framework version")
	("h,help", "Print this help");

	options.add_options()
	("positional", "&", cxxopts::value<std::string>());

	try {
		options.parse_positional("positional");
		options.parse(argc, argv);
	} catch (const cxxopts::OptionException& e) {
		std::cerr << "error parsing options: " << e.what() << std::endl;
		return 1;
	}

	if (options.count("version")) {
		std::cerr << Module::version() << std::endl;
		return 0;
	}

	if (options.count("help")) {
		std::cerr << options.help({"Help"}) << std::endl;
		return 0;
	}

#ifndef STATIC
	if (!options.count("positional")) {
		std::cerr << options.help({"Help"}) << std::endl;
		return 1;
	}

	std::string name = options["positional"].as<std::string>();
	if (!file_exist(name)) {
		std::cerr << "error: " << name << ": No such file or directory" << std::endl;
		return 1;
	}
#endif // STATIC

	std::string host = DEFAULT_EXTRACTOR_HOST;
	if (options.count("hbs")) {
		std::string configfile = options["hbs"].as<std::string>();
		if (!file_exist(configfile)) {
			std::cerr << "error: " << configfile << ": No such file or directory" << std::endl;
			return 1;
		}

		ConfigFile config(configfile);
		if (!config.exist("extract")) {
			std::cerr << "Must be at least 1 extractor listed" << std::endl;
			return 1;
		} else {
			host = config.get<std::string>("extract", DEFAULT_EXTRACTOR_HOST);
		}
	}

	int linger = 0;
	logger << "Connecting to extractor..." << FileLogger::endl();
	socket.connect(("tcp://" + host).c_str());
	socket.setsockopt(ZMQ_LINGER, &linger, sizeof(int));

	/* Item counter */
	srand(time(NULL));
	itemCount = rand() % 10000;

	catch_signals();

#ifdef STATIC
	slorunner(argc, argv);
#else
	if (name.substr(name.find_last_of(".") + 1) == "so" || name.substr(name.find_last_of(".") + 1) == "dll") {
		/* When dSO defined */
		dsorunner(name.c_str(), argc, argv);
	}

#ifdef LIBPYTHON
	else if (name.substr(name.find_last_of(".") + 1) == "py") {

		/* When python file given */
		pyrunner(name.c_str());
	}
#endif // LIBPYTHON

#ifdef LIBLAVRIL
	else if (name.substr(name.find_last_of(".") + 1) == "lav") {

		/* When python file given */
		lvrunner(name.c_str());
	}
#endif // LIBPYTHON

	else {
		std::cerr << "Invalid file" << std::endl;
	}
#endif // STATIC

	return 0;
}
