#include <zmq.hpp>

#include "nodemanager.h"
#include "protoc/controlmessage.pb.h"

void NodeManager::runTask() {
	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5544");

	(*_logger) << "Node manager: waiting for connections " << FileLogger::endl();

	while (true) {
		zmq::message_t request;

		/* Wait for next request from client */
		socket.recv(&request);

		ControlMessage msg;
		msg.ParseFromArray(request.data(), request.size());

		switch (msg.action()) {
			case ControlMessage::SOLICIT:
				msg.set_id(_counter++);
				msg.set_action(ControlMessage::CONFIRMED);
				(*_logger) << "Accept: Solicit from worker, assigned worker-" << msg.id() << FileLogger::endl();
				break;
			case ControlMessage::IDLE:
				puts("Checkin");
				break;
			default:
				break;
		}

		std::string serialized;
		msg.SerializeToString(&serialized);

		/* Send reply back to client */
		request.rebuild(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		_logger->flush();
	}
}
