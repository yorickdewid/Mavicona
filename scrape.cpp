#include <zmq.hpp>
#include <string>
#include <iostream>
#include <map>
#include <Python.h>
#include <dlfcn.h>

#include "logger.h"
#include "scrapedata.pb.h"

static int itemCount = 0;

//  Prepare our context and socket
zmq::context_t context(1);
zmq::socket_t socket(context, ZMQ_REQ);

FileLogger flog("0.1", "scrape_main.log");

std::multimap<std::string, std::string> datastack;

/* Push data on the stack */
static PyObject *mav_push(PyObject *self, PyObject *args) {
	const char *name;
	const char *data;
	int sts;

	if (!PyArg_ParseTuple(args, "ss", &name, &data))
		return NULL;

	datastack.insert(std::pair<std::string, std::string>(name, data));

	return Py_True;
}

/* Commit data to cluster */
static PyObject *mav_save(PyObject *self, PyObject *args) {
	if (!PyArg_ParseTuple(args, ":numargs"))
		return NULL;

	for (auto const &ent : datastack) {
		/* Create meta data object */
		ScrapeData data;
		data.set_name(ent.first);
		data.set_id(itemCount++);
		data.set_type(ScrapeData::PLAIN);

		ScrapeData::Data *payload = new ScrapeData::Data;
		payload->set_payload(ent.second);
		payload->set_size(ent.second.size());
		data.set_allocated_content(payload);

		std::string serialized;
		data.SerializeToString(&serialized);

		zmq::message_t request(serialized.size());
		memcpy((void *) request.data(), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);
		if (!strcmp((const char *)reply.data(), "DONE")) {
			std::cout << "  [DONE]" << std::endl;
		}
	}

	flog << "Sumbitted datastack to dispatch";
	datastack.clear();

	return Py_True;
}

void pyrunner(char *name) {
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
		flog << "Cannot open python file: " << name;
		return;
	}

	PyRun_SimpleFile(py_file, name);
	fclose(py_file);

	Py_Finalize();
}

void dsorunner(int argc, char *argv[]) {
	void *handle = dlopen(argv[1], RTLD_LAZY);
	if (!handle) {
		flog << "Cannot open library: " << dlerror();
		return;
	}

	flog << "Loading symbol main...";
	typedef char *(*main_t)(int, char **);

	dlerror();
	main_t exec_main = (main_t)dlsym(handle, "mav_main");
	const char *dlsym_error = dlerror();
	if (dlsym_error) {
		flog << "Cannot load symbol 'mav_main': " << dlsym_error;
		dlclose(handle);
		return;
	}

	flog << "Calling module...";
	char *resp = exec_main(argc, argv);

	std::cout << resp << std::endl;

	dlclose(handle);
}

int main(int argc, char *argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	if (argc < 2) {
		std::cerr << "Usage: scrape <file|library> [args]" << std::endl;
		return 1;
	}

	flog << "Connecting to extractor...";
	socket.connect("tcp://localhost:5577");

	/* Item counter */
	srand(time(NULL));
	itemCount = rand() % 10000;

	std::string name = std::string(argv[1]);
	if (name.substr(name.find_last_of(".") + 1) == "py") {

		/* When python file defined */
		pyrunner(argv[1]);
	} else if (name.substr(name.find_last_of(".") + 1) == "so") {

		/* When dSO defined */
		dsorunner(argc, argv);
	}

	return 0;
}
