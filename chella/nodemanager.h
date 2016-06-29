#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <iostream>
#include <thread>
#include <chrono>

#include "common/logger.h"

// TODO implement thread iface

class NodeManager {
	std::thread _mainRunner;
	bool _run = false;
	bool _active = false;
	// Queue<Task> *_queue = nullptr;
	unsigned int _counter = 0;
	unsigned int _timeout;
	FileLogger *_logger = nullptr;

  public:
	NodeManager() : _timeout(1) {
		this->_logger = new FileLogger("manager");
	}

	void runTask();

	// inline void setQueuer(Queue<Task> *queue) {
	// 	this->_queue = queue;
	// }

	inline void setTimeout(unsigned int sec) {
		this->_timeout = sec;
	}

	inline void start() {
		if (_run)
			return;

		_mainRunner = std::thread(&NodeManager::runTask, this);
		_mainRunner.detach();
		_run = true;
		_active = true;
	}

	inline void pause() {
		_active = false;
	}

	inline void resume() {
		_active = true;
	}

	inline void stop() {
		if (!_run)
			return;

		_mainRunner.join();
		_run = false;
		_active = false;
	}
};

#endif // NODEMANAGER_H
