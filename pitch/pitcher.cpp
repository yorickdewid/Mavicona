#include <zmq.hpp>
#include <string>
#include <iostream>

#include "provision.h"
#include "queue.h"

static Queue taskQueue;

int main(int argc, char *argv[]) {

	// GOOGLE_PROTOBUF_VERIFY_VERSION;

	Provision event;
	event.setQueuer(&taskQueue);
	event.setTimeout(1);
	event.start();

	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5599");

	std::cout << "Waiting for connections " << std::endl;

	taskQueue.push(5, 1);
	taskQueue.push(7, 1);
	taskQueue.push(2, 1);
	taskQueue.push(6, 1);
	taskQueue.push(12, 1);

	while (true) {
		zmq::message_t request;

		//  Wait for next request from client
		socket.recv(&request);
		std::cout << "Received task" << std::endl;

		// ScrapeData data;
		// data.ParseFromArray(request.data(), request.size());

		/* Handle incomming data */
		// parseData(data);

		/* Send reply back to client */
		zmq::message_t reply(7);
		memcpy(reply.data(), "QUEUED", 7);
		socket.send(reply);
	}

	event.stop();

	return 0;
}
