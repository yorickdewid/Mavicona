#include <zmq.hpp>

#include "controlclient.h"

void ControlClient::runTask() {
	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);

	ControlMessage msg;
	msg.set_id(0);
	msg.set_quid("00000000-0000-0000-0000-000000000000");
	msg.set_action(ControlMessage::SOLICIT);

	std::string serialized;
	msg.SerializeToString(&serialized);

	int opt = 3000;
	socket.setsockopt(ZMQ_SNDTIMEO, &opt, sizeof(int));
	socket.setsockopt(ZMQ_RCVTIMEO, &opt, sizeof(int));
	socket.connect(("tcp://" + _masterNode).c_str());
	std::cout << "Connect to master " << _masterNode << std::endl;

	zmq::message_t request(serialized.size());
	memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
	socket.send(request);

	/* Get the reply */
	zmq::message_t reply;
	socket.recv(&reply);

	msg.ParseFromArray(reply.data(), reply.size());
	if (msg.action() == ControlMessage::CONFIRMED) {
		std::cout << "Solicit accepted, assigned worker-" << msg.id() << std::endl;
		this->_counter = msg.id();
		this->_accepted = true;
		this->_cluster_jobs = msg.cluster_jobs();
		this->_state = ControlMessage::IDLE;

		setenv("WORKERID", std::to_string(msg.id()).c_str(), 1);
	}

	while (_active) {
		std::this_thread::sleep_for(std::chrono::milliseconds(this->_timeout));

		assert(this->_progress >= 0 && this->_progress <= 1000);

		msg.set_id(this->_counter);
		msg.set_quid("5a04a669-dfd7-4c6b-a86a-d230af26c868");// TODO, should be job QUID
		msg.set_action(this->_state);
		msg.set_progress(this->_progress);

		std::string serialized;
		msg.SerializeToString(&serialized);

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		if (!socket.send(request)) {
			continue;
		}

		/* Get the reply */
		zmq::message_t reply;
		if (!socket.recv(&reply)){
			continue;
		}

		msg.ParseFromArray(reply.data(), reply.size());
		this->_cluster_jobs = msg.cluster_jobs();
	}

	socket.close();
}
