#include <zmq.hpp>
#include <string>
#include <iostream>

#include "logger.h"

int main(int argc, char *argv[]) {

	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5577");

	while (true) {
		zmq::message_t request;

		//  Wait for next request from client
		socket.recv(&request);
		std::cout << "Received: " << (char *)request.data() << std::endl;

		//  Send reply back to client
		zmq::message_t reply(5);
		memcpy(reply.data(), "DONE", 5);
		socket.send(reply);
	}
	return 0;
}
