#include <zmq.hpp>
#include <string>
#include <iostream>
#include <Python.h>

static int numargs = 0;

/* Push data on the stack */
static PyObject *mav_push(PyObject *self, PyObject *args) {
	const char *name;
	const char *data;
	int sts;

	if (!PyArg_ParseTuple(args, "ss", &name, &data))
		return NULL;

	std::cout << "Sending " << name << " with " << data << std::endl;
	// sts = system(command);
	// if (sts < 0) {
	// return NULL;
	// }

	return Py_True;
}

/* Commit data to cluster */
static PyObject *mav_save(PyObject *self, PyObject *args) {
	if (!PyArg_ParseTuple(args, ":numargs"))
		return NULL;

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

	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);

	if (argc < 2) {
		std::cerr << "Usage: scrape file [args]" << std::endl;
		return 1;
	}

	pyrunner(argv[1]);

	std::cout << "Connecting to extractor..." << std::endl;
	socket.connect("tcp://localhost:5577");

	//  Do 10 requests, waiting each time for a response
	for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
		zmq::message_t request(5);
		memcpy(request.data(), "Hello", 5);
		std::cout << "Sending result to dispatch " << request_nbr << "..." << std::endl;
		socket.send(request);

		//  Get the reply.
		zmq::message_t reply;
		socket.recv(&reply);
		if (!strcmp((const char *)reply.data(), "DONE")) {
			std::cout << "Data on dispatch queue" << std::endl;
		}
	}

	return 0;
}
