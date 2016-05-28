#ifndef ACTION_STORE_H
#define ACTION_STORE_H

#include <zmq.hpp>
#include <fstream>

#include "action.h"
#include "protoc/storagequery.pb.h"

class Store : public Action {
	zmq::context_t context;
	zmq::socket_t *socket;

  public:
	Store(const ScrapeData *payload) : Action(payload), context(1) {
		this->socket = new zmq::socket_t(context, ZMQ_REQ);
	}

	~Store() {
		delete this->socket;
	}

	void send(const std::string& serialized);
	bool run();

};

#endif // ACTION_STORE_H
