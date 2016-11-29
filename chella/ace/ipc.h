#ifndef ACE_IPC_H
#define ACE_IPC_H

#include <Python.h>
#include <structmember.h>
#include "../controlclient.h"

namespace Ace {

namespace IPC {

PyObject *PyAce_Init();
PyObject *PyAce_ModuleClass(ControlClient *control);

}

}

#endif // ACE_IPC_H
