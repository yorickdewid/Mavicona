#include <zmq.hpp>
#include <string>
#include <iostream>

int main(int argc, char *argv[]) {

	// GOOGLE_PROTOBUF_VERIFY_VERSION;

	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5599");

	std::cout << "Waiting for connections " << std::endl;

	while (true) {
		zmq::message_t request;

		//  Wait for next request from client
		socket.recv(&request);

		// ScrapeData data;
		// data.ParseFromArray(request.data(), request.size());

		/* Handle incomming data */
		// parseData(data);

		/* Send reply back to client */
		zmq::message_t reply(6);
		memcpy(reply.data(), "QUEUED", 6);
		socket.send(reply);
	}

	return 0;
}
