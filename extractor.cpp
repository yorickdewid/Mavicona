#include <zmq.hpp>
#include <string>
#include <iostream>

#include "logger.h"
#include "scrapedata.pb.h"

int main(int argc, char *argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

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

		ScrapeData data;
		data.ParseFromArray(request.data(), request.size());

		std::cout << "Item[" << data.id() << "] received" << std::endl;
		std::cout << "Item[" << data.id() << "] name: " << data.name() << std::endl;

		switch (data.type()) {
			case ScrapeData::PLAIN:
				std::cout << "Item[" << data.id() << "] type: PLAIN" << std::endl;
				break;
			case ScrapeData::FILE:
				std::cout << "Item[" << data.id() << "] type: FILE" << std::endl;
				break;
			case ScrapeData::BINARY:
				std::cout << "Item[" << data.id() << "] type: BINARY" << std::endl;
				break;
			case ScrapeData::STREAM:
				std::cout << "Item[" << data.id() << "] type: STREAM" << std::endl;
				break;
		}

		ScrapeData::Data payload = data.content();
		std::cout << "Item[" << data.id() << "] data: " << payload.payload() << std::endl;

		//  Send reply back to client
		zmq::message_t reply(5);
		memcpy(reply.data(), "DONE", 5);
		socket.send(reply);
	}

	return 0;
}
