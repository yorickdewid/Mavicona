#include <zmq.hpp>
#include <string>
#include <iostream>
#include <map>
#include <Python.h>
#include <dlfcn.h>
#include <quidpp.h>

#include "common/util.h"
#include "common/config.h"
#include "common/logger.h"
#include "common/cxxopts.h"
#include "protoc/scrapedata.pb.h"

#define DEFAULT_EXTRACTOR_HOST  "localhost:5577"

static int itemCount = 0;

//  Prepare our context and socket
zmq::context_t context(1);
zmq::socket_t socket(context, ZMQ_REQ);

FileLogger logger("scrape");

std::multimap<std::string, std::string> datastack;

/* Push data on the stack */
static PyObject *mav_push(PyObject *self, PyObject *args) {
	const char *name;
	const char *data;
	size_t data_len = 0;

	if (!PyArg_ParseTuple(args, "st#", &name, &data, &data_len))
		return NULL;

	std::string bytea(reinterpret_cast<char const*>(data), data_len);
	datastack.insert(std::pair<std::string, std::string>(name, bytea));

	return Py_True;
}

/* Commit data to cluster */
static PyObject *mav_save(PyObject *self, PyObject *args) {
	if (!PyArg_ParseTuple(args, ":numargs"))
		return NULL;

	unsigned int complete = 0;
	for (auto const &ent : datastack) {
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

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);

		if (!strcmp((const char *)reply.data(), "DONE"))
			complete++;
	}

	if (complete == datastack.size()) {
		logger << "Sumbitted datastack to dispatch" << FileLogger::endl();
		datastack.clear();
	} else {
		logger << "Datastack transaction failure" << FileLogger::endl();
	}


	return Py_True;
}

void pyrunner(const char *name) {
	Py_Initialize();

	static PyMethodDef MavMethods[] = {
		{
			"push", mav_push, METH_VARARGS,
			"Push data to scraper."
		},
		{
			"save", mav_save, METH_VARARGS,
			"Save data to cluster."
		},
		{NULL, NULL, 0, NULL}
	};

	Py_InitModule("mavicona", MavMethods);
	PyRun_SimpleString("import mavicona");

	FILE *py_file = fopen(name, "r");
	if (!py_file) {
		logger << FileLogger::error() << "Cannot open python file: " << name << FileLogger::endl();
		return;
	}

	PyRun_SimpleFile(py_file, name);
	fclose(py_file);

	Py_Finalize();
}

void dsorunner(int argc, char *argv[]) {
	void *handle = dlopen(argv[1], RTLD_LAZY);
	if (!handle) {
		logger << FileLogger::error() << "Cannot open library: " << dlerror() << FileLogger::endl();
		return;
	}

	logger << "Loading symbol main..." << FileLogger::endl();
	typedef char *(*main_t)(int, char **);

	dlerror();
	main_t exec_main = (main_t)dlsym(handle, "mav_main");
	const char *dlsym_error = dlerror();
	if (dlsym_error) {
		logger << FileLogger::error() << "Cannot load symbol 'mav_main': " << dlsym_error << FileLogger::endl();
		dlclose(handle);
		return;
	}

	logger << "Calling module..." << FileLogger::endl();
	char *resp = exec_main(argc, argv);

	std::cout << resp << std::endl;

	dlclose(handle);
}

int main(int argc, char *argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	cxxopts::Options options(argv[0], " [FILE]");

	options.add_options("Help")
		("s,hbs", "Host based service config", cxxopts::value<std::string>(), "FILE")
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

	if (options.count("help")) {
		std::cerr << options.help({"Help"}) << std::endl;
		return 0;
	}

	if (!options.count("positional")) {
		std::cerr << options.help({"Help"}) << std::endl;
		return 1;
	}

	std::string name = options["positional"].as<std::string>();
	if (!file_exist(name)) {
		std::cerr << "error: " << name << ": No such file or directory" << std::endl;
		return 1;
	}

	std::string host = DEFAULT_EXTRACTOR_HOST;
    if (options.count("hbs")) {
        std::string configfile = options["hbs"].as<std::string>();
        if (!file_exist(configfile)) {
            std::cerr << "error: " << configfile << ": No such file or directory" << std::endl;
            return 1;
        }

        ConfigFile config(configfile);
	    if (!config.exist("extract")){
	        std::cerr << "Must be at least 1 extractor listed" << std::endl;
	        return 1;
	    } else {
	        host = config.get<std::string>("extract", DEFAULT_EXTRACTOR_HOST);
	    }
    } else {

    }

	int linger = 0;
	logger << "Connecting to extractor..." << FileLogger::endl();
	socket.connect(("tcp://" + host).c_str());
	socket.setsockopt(ZMQ_LINGER, &linger, sizeof(int));

	/* Item counter */
	srand(time(NULL));
	itemCount = rand() % 10000;

	//std::string name = std::string(argv[1]);
	if (name.substr(name.find_last_of(".") + 1) == "py") {

		/* When python file defined */
		pyrunner(name.c_str());
	} else if (name.substr(name.find_last_of(".") + 1) == "so" || name.substr(name.find_last_of(".") + 1) == "dll") {

		/* When dSO defined */
		// dsorunner(argc, argv);
	}

	return 0;
}
