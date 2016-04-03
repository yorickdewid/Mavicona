#ifndef ACTION_QUEUE_H
#define ACTION_QUEUE_H

#include <zmq.hpp>
#include <fstream>

#include "action.h"
#include "protoc/task.pb.h"

class Queue : public Action {
	zmq::context_t context;
	zmq::socket_t *socket;

  public:
	Queue(const ScrapeData *payload) : Action(payload), context(1) {
		this->socket = new zmq::socket_t(context, ZMQ_REQ);
	}

	~Queue() {
		delete this->socket;
	}

	void send(const std::string& serialized);
	bool run();

};

#endif // ACTION_QUEUE_H
