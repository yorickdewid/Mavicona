#ifndef ACE_ASYS_H
#define ACE_ASYS_H

#include <Python.h>
#include "common/logger.h"

namespace Ace {

namespace Asys {

static PyObject *Asys_write(PyObject *self, PyObject *args) {
	const char *str = NULL;

	if (!PyArg_ParseTuple(args, "s", &str))
		Py_RETURN_NONE;

	PyObject *pCapsule = PyObject_GetAttrString(self, "_stream");
	if (pCapsule) {
		FileLogger *stream = (FileLogger *)PyCapsule_GetPointer(pCapsule, NULL);
		(*stream) << str;
	}

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

static PyObject *Asys_readline() {
	return Py_BuildValue("s", "000", 3);
}

static PyMethodDef module_methods[] = {
	{"write", (PyCFunction)Asys_write, METH_VARARGS, "Write to buffer"},
	{"flush", (PyCFunction)Asys_flush, METH_NOARGS, "Flush buffer"},
	{"close", (PyCFunction)Asys_close, METH_NOARGS, "Close stream"},
	{"readline", (PyCFunction)Asys_readline, METH_NOARGS, "Mock streamreader"},
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

PyObject *PyAce_ModuleClass(FileLogger *stream) {
	PyObject *pModule = PyAce_Init();
	if (!pModule)
		return NULL; 

	/* Create a Capsule containing the API pointer array's address */
	PyObject *pCapsule = PyCapsule_New((void *)stream, NULL, NULL);
	if (!pCapsule) {
		PyErr_Print();
		return NULL;
	}

	PyModule_AddObject(pModule, "_stream", pCapsule);
	return pModule;
}

}

}

#endif // ACE_ASYS_H
