#include <zmq.hpp>
#include <string>
#include <iostream>

#include <ups/upscaledb.hpp>

int main(int argc, char *argv[]) {

	// GOOGLE_PROTOBUF_VERIFY_VERSION;
	upscaledb::env env;

	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5588");

	std::cout << "Waiting for connections " << std::endl;

	while (true) {
		zmq::message_t request;

		/* Wait for next request from client */
		socket.recv(&request);

		// Task task;
		// task.ParseFromArray(request.data(), request.size());

		/* Handle incomming task */
		// parseTask(task);

		/* Send reply back to client */
		zmq::message_t reply(7);
		memcpy(reply.data(), "STORED", 7);
		socket.send(reply);
	}

	return 0;
}
