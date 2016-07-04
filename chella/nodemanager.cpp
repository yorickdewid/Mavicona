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
				_workers.push_back(msg.id());
				break;
			case ControlMessage::IDLE:
				printf("worker-%d -> IDLE\n", msg.id());
				break;
			case ControlMessage::ACCEPTED:
				printf("worker-%d -> ACCEPTED\n", msg.id());
				break;
			case ControlMessage::SETUP:
				printf("worker-%d -> SETUP\n", msg.id());
				break;
			case ControlMessage::RUNNING:
				printf("worker-%d -> RUNNING %.1f%%\n", msg.id(), (float)((float)msg.progress() / (float)10));
				break;
			case ControlMessage::TEARDOWN:
				printf("worker-%d -> TEARDOWN\n", msg.id());
				break;
			default:
				break;
		}

		msg.set_cluster_jobs(_workers.size());
		// msg.cluster_workers();

		std::string serialized;
		msg.SerializeToString(&serialized);

		/* Send reply back to client */
		request.rebuild(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		_logger->flush();
	}
}
