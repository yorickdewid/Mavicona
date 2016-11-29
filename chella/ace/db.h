#ifndef ACE_DB_H
#define ACE_DB_H

#include <Python.h>
#include "../indexer.h"

namespace Ace {

namespace DB {

PyObject *PyAce_Init();
PyObject *PyAce_ModuleClass(Indexer *db);

}

}

#endif // ACE_DB_H
