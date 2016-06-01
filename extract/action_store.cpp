#include <iostream>

#include "common/config.h"
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
	socket->connect(("tcp://" + host).c_str());

	// Perform query
	std::string serialized;
	m_Payload->SerializeToString(&serialized);

	send(serialized);

	return true;
}

bool Store::config(const std::string& configfile) {
	ConfigFile config(configfile);
	if (!config.exist("cynder-master")) {
		std::cerr << "Must be at least 1 cynder master listed" << std::endl;
		return false;
	}

	host = config.get<std::string>("cynder-master", "");

	return true;
}
