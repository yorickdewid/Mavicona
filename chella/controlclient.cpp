#include <zmq.hpp>

#include "controlclient.h"
#include "protoc/controlmessage.pb.h"

void ControlClient::runTask() {
	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);

	ControlMessage msg;
	msg.set_id(0);
	msg.set_quid("7234679823aeb");
	msg.set_action(ControlMessage::SOLICIT);

	std::string serialized;
	msg.SerializeToString(&serialized);

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
	}

	while (_active) {
		std::this_thread::sleep_for(std::chrono::seconds(this->_timeout));

		msg.set_id(this->_counter);
		msg.set_quid("7234679823aeb");// TODO
		msg.set_action(ControlMessage::IDLE);

		std::string serialized;
		msg.SerializeToString(&serialized);

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		/* Get the reply */
		zmq::message_t reply;
		socket.recv(&reply);
	}
}
