#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <iostream>
#include <thread>
#include <chrono>

#include "common/logger.h"
#include "protoc/controlmessage.pb.h"

namespace NodeManager {

void runTask(ControlMessage& message, size_t queuesize);

};

#endif // NODEMANAGER_H
