#ifndef ACE_DB_H
#define ACE_DB_H

#include <Python.h>
#include <structmember.h>
#include "../indexer.h"

static Indexer *_global_db = NULL;

namespace Ace {

namespace DB {

typedef struct {
	PyObject_HEAD
	Indexer *db;
} ace_db_t;

static PyObject *DB_put(PyObject *self, PyObject *args) {
	ace_db_t *_self = (ace_db_t *)self;
	const char *key = NULL;
	const char *value = NULL;
	int key_size = 0;
	int value_size = 0;

	if (!PyArg_ParseTuple(args, "s#s#:put", &key, &key_size, &value, &value_size))
		Py_RETURN_NONE;

	_self->db->putItem(key, key_size, value, value_size);

	printf("key_size %d\n", key_size);
	printf("value_size %d\n", value_size);

	Py_RETURN_NONE;
}

static PyObject *DB_get(PyObject *self, PyObject *args) {
	ace_db_t *_self = (ace_db_t *)self;
	const char *key = NULL;
	int key_size = 0;

	if (!PyArg_ParseTuple(args, "s#:get", &key, &key_size))
		Py_RETURN_NONE;

	//TODO: accept only C style here
	std::string value = _self->db->getItem(key, key_size);

	return Py_BuildValue("s#", value.data(), value.size());
}

static PyObject *DB_delete(PyObject *self, PyObject *args) {
	ace_db_t *_self = (ace_db_t *)self;
	const char *key = NULL;
	int key_size = 0;

	if (!PyArg_ParseTuple(args, "s#:get", &key, &key_size))
		Py_RETURN_NONE;

	_self->db->deleteItem(key, key_size);

	Py_RETURN_NONE;
}

static PyMethodDef module_methods[] = {
	{"put", (PyCFunction)DB_put, METH_VARARGS, "Put item in database"},
	{"get", (PyCFunction)DB_get, METH_VARARGS, "Get item from database"},
	{"delete", (PyCFunction)DB_delete, METH_VARARGS, "Delete item from database"},
	{NULL}  /* Sentinel */
};

PyObject *module_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	ace_db_t *self = (ace_db_t *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->db = _global_db;
	}

	return (PyObject *)self;
}

static PyTypeObject module_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"ace.db.DB",											/* tp_name */
	sizeof(ace_db_t),										/* tp_basicsize */
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
	"Ace local index database",								/* tp_doc */
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
	"ace.db",												/* m_name */
	"Example module that creates an extension type.",		/* m_doc */
	-1,														/* m_size */
	NULL,													/* m_methods */
	NULL,													/* m_reload */
	NULL,													/* m_traverse */
	NULL,													/* m_clear */
	NULL													/* m_free */
};

PyObject *PyAce_Init() {
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
	PyModule_AddObject(pModule, "DB", (PyObject *)&module_Type);

	return pModule;
}

PyObject *PyAce_ModuleClass(Indexer *db) {
	_global_db = db;
	PyObject *pType = PyObject_GetAttrString(PyAce_Init(), "DB");
	if (!pType || !PyCallable_Check(pType)) {
		PyErr_Print();
		return NULL;
	}

	return pType;
}

}

}

#endif // ACE_DB_H
