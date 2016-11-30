#include <Python.h>
#include <structmember.h>
#include "ipc.h"

static ControlClient *_global_control = NULL;

typedef struct {
	PyObject_HEAD
	ControlClient *control;
} ace_callback_t;

static PyObject *Callback_worder_id(PyObject *self) {
	ace_callback_t *_self = (ace_callback_t *)self;

	return PyLong_FromLong(_self->control->workerId());
}

static PyObject *Callback_job_count(PyObject *self) {
	ace_callback_t *_self = (ace_callback_t *)self;

	return PyLong_FromLong(_self->control->clusterJobs());
}

static PyObject *Callback_update_progress(PyObject *self, PyObject *args) {
	ace_callback_t *_self = (ace_callback_t *)self;
	unsigned int progress = 0;

	if (!PyArg_ParseTuple(args, "I:update_progress", &progress))
		Py_RETURN_NONE;

	_self->control->updateStateRunning(progress);

	Py_RETURN_NONE;
}

static PyMethodDef module_methods[] = {
	{"worker_id", (PyCFunction)Callback_worder_id, METH_NOARGS, "Get worker id"},
	{"job_count", (PyCFunction)Callback_job_count, METH_NOARGS, "Request number of jobs in cluster"},
	{"update_progress", (PyCFunction)Callback_update_progress, METH_VARARGS, "Update job progress"},
	{NULL}  /* Sentinel */
};

static PyObject *module_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	ace_callback_t *self = (ace_callback_t *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->control = _global_control;
	}

	return (PyObject *)self;
}

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
	module_new,												/* tp_new */
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

PyObject *Ace::IPC::PyAce_Init() {
	if (PyType_Ready(&module_Type) < 0) {
		PyErr_Print();
		return NULL;
	}

	PyObject *pModule = PyModule_Create(&module);
	if (!pModule) {
		PyErr_Print();
		return NULL;
	}

	Py_INCREF(&module_Type);
	PyModule_AddObject(pModule, "Callback", (PyObject *)&module_Type);

	return pModule;
}

PyObject *Ace::IPC::PyAce_ModuleClass(ControlClient *control) {
	_global_control = control;
	PyObject *pType = PyObject_GetAttrString(PyAce_Init(), "Callback");
	if (!pType || !PyCallable_Check(pType)) {
		PyErr_Print();
		return NULL;
	}

	return pType;
}
