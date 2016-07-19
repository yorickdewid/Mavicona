#include <iostream>

#include "common/config.h"
#include "action_queue.h"

void Queue::send(const std::string& data) {
	zmq::message_t request(data.size());
	memcpy(reinterpret_cast<void *>(request.data()), data.c_str(), data.size());
	socket->send(request);

	// Get the reply
	zmq::message_t reply;
	socket->recv(&reply);

	if (!strcmp((const char *)reply.data(), "QUEUED")) {
		std::cout << "Sumbitted task to queue" << std::endl;
	} else {
		std::cerr << "Task transaction failure " << std::endl;
	}
}

bool Queue::run() {
	std::cout << "Connecting to pitcher..." << std::endl;
	socket->connect(("tcp://" + host).c_str());

	/* Create meta data object */
	Task task;
	task.set_name(m_Payload->name());
	task.set_id(m_Payload->id());
	task.set_quid(m_Payload->quid());
	task.set_priority(Task::NORMAL);

	std::string serialized;
	task.SerializeToString(&serialized);

	send(serialized);

	return true;
}

bool Queue::config(const std::string& configfile) {
	ConfigFile config(configfile);
	if (!config.exist("pitch")) {
		std::cerr << "Must be at least 1 pitcher listed" << std::endl;
		return false;
	}

	host = config.get<std::string>("pitch", "");

	return true;
}

