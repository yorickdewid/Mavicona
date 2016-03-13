#include <zmq.hpp>
#include <string>
#include <iostream>
#include <map>
#include <Python.h>

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
		std::cout << "Sending... " << ent.first;

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

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: scrape file [args]" << std::endl;
		return 1;
	}

	flog << "Connecting to extractor...";
	socket.connect("tcp://localhost:5577");

	pyrunner(argv[1]);

	return 0;
}
