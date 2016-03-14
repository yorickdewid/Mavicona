#include <zmq.hpp>
#include <string>
#include <iostream>
#include <map>
#include <Python.h>
#include <dlfcn.h>

#include "logger.h"

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

	std::cout << datastack.size() << std::endl;

	for (auto const &ent : datastack) {
		std::cout << "Sending... " << ent.first << " => " << ent.second;

		zmq::message_t request(ent.second.length());
		memcpy(request.data(), ent.second.c_str(), ent.second.length());
		socket.send(request);

		//  Get the reply.
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
	PyRun_SimpleFile(py_file, name);
	fclose(py_file);

	Py_Finalize();
}

void dsorunner(int argc, char *argv[]) {
	// open the library
	std::cout << "Opening hello.so...\n";
	void *handle = dlopen(argv[1], RTLD_LAZY);
	if (!handle) {
		std::cerr << "Cannot open library: " << dlerror() << '\n';
		return;
	}

	// load the symbol
	std::cout << "Loading symbol hello...\n";
	typedef char *(*main_t)(int, char **);

	// reset errors
	dlerror();
	main_t exec_main = (main_t)dlsym(handle, "mav_main");
	const char *dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Cannot load symbol 'hello': " << dlsym_error << '\n';
		dlclose(handle);
		return;
	}

	// use it to do the calculation
	std::cout << "Calling hello...\n";
	char *resp = exec_main(argc, argv);

	std::cout << resp << std::endl;

	// close the library
	std::cout << "Closing library...\n";
	dlclose(handle);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: scrape <file|library> [args]" << std::endl;
		return 1;
	}

	flog << "Connecting to extractor...";
	socket.connect("tcp://localhost:5577");

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
