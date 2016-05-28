#include <iostream>

#include "action_store.h"

void Store::send(const std::string& data) {
	zmq::message_t request(data.size());
	memcpy(reinterpret_cast<void *>(request.data()), data.c_str(), data.size());
	socket->send(request);

	// Get the reply
	zmq::message_t reply;
	socket->recv(&reply);

	if (!strcmp((const char *)reply.data(), "DONE")) {
		std::cout << "Sumbitted record to storage cluster" << std::endl;
	} else {
		std::cerr << "Storage transaction failure " << std::endl;
	}
}

bool Store::run() {
	std::cout << "Connecting to cynder..." << std::endl;
	socket->connect("tcp://localhost:5533");

	// Perform query
	std::string serialized;
	m_Payload->SerializeToString(&serialized);

	send(serialized);

	return true;
}
