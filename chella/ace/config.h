#ifndef ACE_CONFIG_H
#define ACE_CONFIG_H

#include <Python.h>
#include <structmember.h>

namespace Ace {

namespace Config {

typedef struct {
	PyObject_HEAD
	PyObject *invoke;
	PyObject *mode;
	int max_runtime;
	int retry;
	char keep_order;
} ace_config_t;

void module_dealloc(PyObject *self) {
	ace_config_t *_self = (ace_config_t *)self;
	Py_XDECREF(_self->invoke);
	Py_XDECREF(_self->mode);
	Py_TYPE(_self)->tp_free(self);
}

PyObject *module_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	ace_config_t *self;

	self = (ace_config_t *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->max_runtime = 600;
		self->retry = 3;
		self->keep_order = 0;
	}

	return (PyObject *)self;
}

static PyMemberDef module_members[] = {
	{"invoke", T_OBJECT_EX, offsetof(ace_config_t, invoke), 0, "job class"},
	{"mode", T_OBJECT_EX, offsetof(ace_config_t, mode), 0, "charding mode"},
	{"max_runtime", T_INT, offsetof(ace_config_t, max_runtime), 0, "maximum runtime"},
	{"retry", T_INT, offsetof(ace_config_t, retry), 0, "retry count"},
	{"keep_order", T_BOOL, offsetof(ace_config_t, keep_order), 0, "keep runtime order"},
	{NULL}  /* Sentinel */
};

static PyTypeObject module_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"ace.config.Config",									/* tp_name */
	sizeof(ace_config_t),									/* tp_basicsize */
	0,														/* tp_itemsize */
	module_dealloc,											/* tp_dealloc */
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
	"Ace job configuration",								/* tp_doc */
	NULL,													/* tp_traverse */
	NULL,													/* tp_clear */
	NULL,													/* tp_richcompare */
	0,														/* tp_weaklistoffset */
	NULL,													/* tp_iter */
	NULL,													/* tp_iternext */
	NULL,													/* tp_methods */
	module_members,											/* tp_members */
	NULL,													/* tp_getset */
	NULL,													/* tp_base */
	NULL,													/* tp_dict */
	NULL,													/* tp_descr_get */
	NULL,													/* tp_descr_set */
	0,														/* tp_dictoffset */
	NULL,													/* tp_init */
	NULL,													/* tp_alloc */
	module_new,												/* tp_new */
};

static PyModuleDef module = {
	PyModuleDef_HEAD_INIT,
	"ace.config",											/* m_name */
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
	PyModule_AddObject(pModule, "Config", (PyObject *)&module_Type);

	return pModule;
}

PyObject *PyAce_ModuleClass() {
	PyObject *pType = PyObject_GetAttrString(PyAce_Init(), "Config");
	if (!pType || !PyCallable_Check(pType)) {
		PyErr_Print();
		return NULL;
	}

	return pType;
}

}

}

#endif // ACE_CONFIG_H
