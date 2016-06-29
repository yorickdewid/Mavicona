#ifndef CONTROLCLIENT_H
#define CONTROLCLIENT_H

#include <iostream>
#include <thread>
#include <chrono>

#include "common/logger.h"

// TODO implement thread iface

class ControlClient {
	std::thread _mainRunner;
	bool _run = false;
	bool _active = false;
	bool _accepted = false;
	unsigned int _counter = 0;
	unsigned int _timeout;
	std::string _masterNode;
	FileLogger *_logger = nullptr;

  public:
	ControlClient() : _timeout(1) {
		this->_logger = new FileLogger("controller");
	}

	void runTask();

	inline void setMaster(const std::string masterNode) {
		this->_masterNode = masterNode;
	}

	inline void setTimeout(unsigned int sec) {
		this->_timeout = sec;
	}

	inline void start() {
		if (_run)
			return;

		_mainRunner = std::thread(&ControlClient::runTask, this);
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

	inline bool isAccepted() {
		return _accepted;
	}

	inline void stop() {
		if (!_run)
			return;

		_mainRunner.join();
		_run = false;
		_active = false;
	}
};

#endif // CONTROLCLIENT_H
