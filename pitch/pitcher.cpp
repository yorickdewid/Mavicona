#include <zmq.hpp>
#include <string>
#include <iostream>

#include "common/util.h"
#include "common/config.h"
#include "common/cxxopts.h"
#include "protoc/task.pb.h"
#include "provision.h"
#include "queue.h"

static Queue<Task> taskQueue;

void parseTask(Task& task) {
	switch (task.priority()) {
		case Task::REALTIME:
			// runRealtimeTask()
			std::cout << "Realtime task; ignores all queues" << std::endl;
			break;
		case Task::HIGH:
			taskQueue.push(task, 1);
			break;
		case Task::NORMAL:
			taskQueue.push(task, 2);
			break;
		case Task::LOW:
			taskQueue.push(task, 3);
			break;
		case Task::IDLE:
			taskQueue.push(task, 100);
			break;
	}
}

int main(int argc, char *argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	cxxopts::Options options(argv[0], "");

	options.add_options("Help")
	("s,hbs", "Host based service config", cxxopts::value<std::string>(), "FILE")
	("h,help", "Print this help");

	try {
		options.parse(argc, argv);
	} catch (const cxxopts::OptionException& e) {
		std::cerr << "error parsing options: " << e.what() << std::endl;
		return 1;
	}

	if (options.count("help")) {
		std::cerr << options.help({"Help"}) << std::endl;
		return 0;
	}

	std::string host;
	if (options.count("hbs")) {
		std::string configfile = options["hbs"].as<std::string>();
		if (!file_exist(configfile)) {
			std::cerr << "error: " << configfile << ": No such file or directory" << std::endl;
			return 1;
		}

		ConfigFile config(configfile);
		if (!config.exist("extract")) {
			std::cerr << "Must be at least 1 extractor listed" << std::endl;
			return 1;
		} else {
			host = config.get<std::string>("extract", "");
		}
	}

	Provision event;
	event.setQueuer(&taskQueue);
	event.setTimeout(1 /* 1sec */);
	event.start();

	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5599");

	std::cout << "Waiting for connections " << std::endl;

	while (true) {
		zmq::message_t request;

		/* Wait for next request from client */
		socket.recv(&request);

		Task task;
		task.ParseFromArray(request.data(), request.size());

		/* Handle incomming task */
		parseTask(task);

		/* Send reply back to client */
		zmq::message_t reply(7);
		memcpy(reply.data(), "QUEUED", 7);
		socket.send(reply);
	}

	event.stop();

	return 0;
}
