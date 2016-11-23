#ifndef ACE_IPC_H
#define ACE_IPC_H

#include <Python.h>
#include <structmember.h>

namespace Ace {

namespace IPC {

typedef struct {
	PyObject_HEAD
} ace_callback_t;

static PyObject *Callback_job_count(PyObject *self) {
    return PyLong_FromLong(88);
}

static PyObject *Callback_update_progress(PyObject *self, PyObject *args) {
	unsigned int progress = 0;
	if (!PyArg_ParseTuple(args, "I:update_progress", &progress))
        return NULL;

    Py_RETURN_NONE;
}

static PyMethodDef module_methods[] = {
    {"job_count", (PyCFunction)Callback_job_count, METH_NOARGS, "Return the name, combining the first and last name" },
    {"update_progress", (PyCFunction)Callback_update_progress, METH_VARARGS, "Return the name, combining the first and last name" },
    {NULL}  /* Sentinel */
};

static PyTypeObject module_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"ace.ipc.Callback",										/* tp_name */
	sizeof(ace_callback_t),									/* tp_basicsize */
	0,														/* tp_itemsize */
	NULL,													/* tp_dealloc */
	NULL,													/* tp_print */
	NULL,													/* tp_getattr */
	NULL,													/* tp_setattr */
	NULL,													/* tp_reserved */
	NULL,													/* tp_repr */
	NULL,													/* tp_as_number */
	NULL,													/* tp_as_sequence */
	NULL,													/* tp_as_mapping */
	NULL,													/* tp_hash  */
	NULL,													/* tp_call */
	NULL,													/* tp_str */
	PyObject_GenericGetAttr,								/* tp_getattro */
	PyObject_GenericSetAttr,								/* tp_setattro */
	NULL,													/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,										/* tp_flags */
	"Ace IPC callback routine",								/* tp_doc */
	NULL,													/* tp_traverse */
	NULL,													/* tp_clear */
	NULL,													/* tp_richcompare */
	0,														/* tp_weaklistoffset */
	NULL,													/* tp_iter */
	NULL,													/* tp_iternext */
	module_methods,											/* tp_methods */
	NULL,													/* tp_members */
	NULL,													/* tp_getset */
	NULL,													/* tp_base */
	NULL,													/* tp_dict */
	NULL,													/* tp_descr_get */
	NULL,													/* tp_descr_set */
	0,														/* tp_dictoffset */
	NULL,													/* tp_init */
	PyType_GenericAlloc,									/* tp_alloc */
	PyType_GenericNew,										/* tp_new */
};

static PyModuleDef module = {
	PyModuleDef_HEAD_INIT,
	"ace.ipc",												/* m_name */
	"Example module that creates an extension type.",		/* m_doc */
	-1,														/* m_size */
	NULL,													/* m_methods */
	NULL,													/* m_reload */
	NULL,													/* m_traverse */
	NULL,													/* m_clear */
	NULL													/* m_free */
};

PyObject *PyAce_Init() {
	if (PyType_Ready(&module_Type) < 0)
		return NULL;

	PyObject *pModule = PyModule_Create(&module);
	if (!pModule)
		return NULL;

	Py_INCREF(&module_Type);
	PyModule_AddObject(pModule, "Callback", (PyObject *)&module_Type);

	return pModule;
}

PyObject *PyAce_ModuleClass() {
	PyObject *pType = PyObject_GetAttrString(PyAce_Init(), "Callback");
	if (!pType || !PyCallable_Check(pType)) {
		PyErr_Print();
		return NULL;
	}

	return pType;
}

}

}

#endif // ACE_IPC_H
