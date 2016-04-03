#ifndef PROVISION_H
#define PROVISION_H

#include <iostream>
#include <thread>
#include <chrono>

#include "queue.h"

class Provision {
	std::thread _mainRunner;
	bool _run = false;
	bool _active = false;
	Queue *_queue = nullptr;
	unsigned int _timeout;

  public:
	Provision() : _timeout(1) {}

	void runTask();

	void setQueuer(Queue *queue) {
		this->_queue = queue;
	}

	void setTimeout(unsigned int sec) {
		this->_timeout = sec;
	}

	void start() {
		if (_run)
			return;

		_mainRunner = std::thread(&Provision::runTask, this);
		_mainRunner.detach();
		_run = true;
		_active = true;
	}

	void pause() {
		_active = false;
	}

	void resume() {
		_active = true;
	}

	void stop() {
		if (!_run)
			return;

		_mainRunner.join();
		_run = false;
		_active = false;
	}
};

#endif // PROVISION_H
