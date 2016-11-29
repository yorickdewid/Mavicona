#ifndef CONTROLCLIENT_H
#define CONTROLCLIENT_H

#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>

#include "common/logger.h"
#include "protoc/controlmessage.pb.h"

#define DEFAULT_HEARTBEAT	5 /* 5sec */

// TODO implement thread iface

class ControlClient {
	std::thread _mainRunner;
	bool _run = false;
	bool _active = false;
	bool _accepted = false;
	unsigned int _counter = 0;
	unsigned int _cluster_jobs = 0;
	unsigned int _timeout;
	unsigned int _progress = 0;
	std::string _masterNode;
	// FileLogger *_logger = nullptr;
	ControlMessage::Action _state;

  public:
	ControlClient() : _timeout(DEFAULT_HEARTBEAT * 1000) {
		// this->_logger = new FileLogger("controller");
		this->_state = ControlMessage::SOLICIT;
	}

	void runTask();

	inline void setMaster(const std::string masterNode) {
		this->_masterNode = masterNode;
	}

	inline void setTimeout(unsigned short sec) {
		this->_timeout = sec * 1000;
	}

	inline void start() {
		if (_run)
			return;

		_mainRunner = std::thread(&ControlClient::runTask, this);
		_mainRunner.detach();
		_run = true;
		_active = true;
	}

	inline void resetInternalState() {
		this->updateStateRunning(0);
	}

	inline void pause() {
		_active = false;
	}

	inline void resume() {
		_active = true;
	}

	inline unsigned int workerId() {
		return _counter;
	}

	inline unsigned int clusterJobs() {
		return _cluster_jobs;
	}

	inline bool isAccepted() {
		return _accepted;
	}

	inline void setStateIdle() {
		this->_state = ControlMessage::IDLE;
		this->_timeout = DEFAULT_HEARTBEAT * 1000;
	}

	inline void setStateAccepted() {
		this->_state = ControlMessage::ACCEPTED;
		this->_timeout = (DEFAULT_HEARTBEAT * 100);

		/* Node manager catchup */
		sleep(DEFAULT_HEARTBEAT + 1);
	}

	inline void setStateSetup() {
		this->_state = ControlMessage::SETUP;
	}

	inline void setStateRunning() {
		this->_state = ControlMessage::RUNNING;
	}

	inline void updateStateRunning(const unsigned int progress) {
		this->_progress = progress;
	}

	inline void setStateTeardown() {
		this->_state = ControlMessage::TEARDOWN;
	}

	inline void setStateFailed() {
		this->_state = ControlMessage::FAILED;
		this->_timeout = DEFAULT_HEARTBEAT * 1000;
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
