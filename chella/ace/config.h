#ifndef ACE_CONFIG_H
#define ACE_CONFIG_H

#include <Python.h>

namespace Ace {

namespace Config {

PyObject *PyAce_Init();
PyObject *PyAce_ModuleClass();

}

}

#endif // ACE_CONFIG_H
