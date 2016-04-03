#include <iostream>

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
	socket->connect("tcp://localhost:5599");

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
