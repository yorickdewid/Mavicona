#ifndef ACE_ASYS_H
#define ACE_ASYS_H

#include <Python.h>

namespace Ace {

namespace Asys {

static PyObject *Asys_write(PyObject *self, PyObject *args) {
	const char *str = NULL;

	if (!PyArg_ParseTuple(args, "s", &str))
		Py_RETURN_NONE;

#ifdef DEBUG
	printf("%s", str);
#endif

	Py_RETURN_NONE;
}

static PyObject *Asys_flush() {
	Py_RETURN_NONE;
}

static PyObject *Asys_close() {
	Py_RETURN_NONE;
}

static PyMethodDef module_methods[] = {
	{"write", (PyCFunction)Asys_write, METH_VARARGS, "Write to buffer"},
	{"flush", (PyCFunction)Asys_flush, METH_NOARGS, "Flush buffer"},
	{"close", (PyCFunction)Asys_close, METH_NOARGS, "Close stream"},
	{NULL}  /* Sentinel */
};

static PyModuleDef module = {
	PyModuleDef_HEAD_INIT,
	"ace.asys",												/* m_name */
	"Example module that creates an extension type.",		/* m_doc */
	-1,														/* m_size */
	module_methods,											/* m_methods */
	NULL,													/* m_reload */
	NULL,													/* m_traverse */
	NULL,													/* m_clear */
	NULL													/* m_free */
};

PyObject *PyAce_Init() {
	PyObject *pModule = PyModule_Create(&module);
	if (!pModule) {
		PyErr_Print();
		return NULL;
	}

	return pModule;
}

PyObject *PyAce_ModuleClass() {
	PyObject *pModule = PyAce_Init();
	if (!pModule)
		return NULL; 
	
	PySys_SetObject("stdout", pModule);

	return pModule;
}

}

}

#endif // ACE_ASYS_H
