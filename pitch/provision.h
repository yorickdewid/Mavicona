#ifndef PROVISION_H
#define PROVISION_H

#include <iostream>
#include <thread>
#include <chrono>

#include "protoc/task.pb.h"
#include "queue.h"

class Provision {
	std::thread _mainRunner;
	bool _run = false;
	bool _active = false;
	Queue<Task> *_queue = nullptr;
	unsigned int _timeout;

  public:
	Provision() : _timeout(1) {}

	void runTask();

	inline void setQueuer(Queue<Task> *queue) {
		this->_queue = queue;
	}

	inline void setTimeout(unsigned int sec) {
		this->_timeout = sec;
	}

	inline void start() {
		if (_run)
			return;

		_mainRunner = std::thread(&Provision::runTask, this);
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

#endif // PROVISION_H
