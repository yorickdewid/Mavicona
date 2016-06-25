#include <zmq.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <csignal>

#include "common/util.h"
#include "common/module.h"
#include "common/config.h"
#include "common/logger.h"
#include "common/cxxopts.h"
#include "protoc/processjob.pb.h"

static std::string masterNode;
static unsigned int workerCounter = 0;
static bool interrupted = false;

void signal_handler(int signum) {
	interrupted = true;
}

static void catch_signals() {
    struct sigaction action;
    action.sa_handler = signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

void initMaster() {
	std::cout << "Master" << std::endl;

	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5544");

	std::cout << "Waiting for connections " << std::endl;

	while (true) {
		zmq::message_t request;

		/* Wait for next request from client */
		socket.recv(&request);

		ProcessJob job;
		job.ParseFromArray(request.data(), request.size());

		switch (job.jobaction()) {
			case ProcessJob::SOLICIT:
				job.set_id(workerCounter++);
				job.set_jobaction(ProcessJob::ACCEPT);
				std::cout << "Accept: Solicit from worker, assigned worker-" << job.id() << std::endl;
				break;
			default:
				break;
		}

		std::string serialized;
		job.SerializeToString(&serialized);

		/* Send reply back to client */
		request.rebuild(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);
	}
}

void initSlave() {
	std::cout << "Slave" << std::endl;

	/* Sending solicit request */
	{
		zmq::context_t context(1);
		zmq::socket_t socket(context, ZMQ_REQ);

		ProcessJob job;
		job.set_name("Woei");
		job.set_id(12);
		job.set_partition(0);
		job.set_jobaction(ProcessJob::SOLICIT);
		job.set_jobresult(ProcessJob::SUCCESS);

		std::string serialized;
		job.SerializeToString(&serialized);

		socket.connect(("tcp://" + masterNode).c_str());
		std::cout << "Connect to master " << masterNode << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		/* Get the reply */
		zmq::message_t reply;
		socket.recv(&reply);

		job.ParseFromArray(reply.data(), reply.size());
		if (job.jobaction() == ProcessJob::ACCEPT) {
			std::cout << "Solicit accepted, now know as worker-" << job.id() << std::endl;
		}
	}
}

int main(int argc, char *argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	cxxopts::Options options(argv[0], "");

	options.add_options("Help")
	("s,hbs", "Host based service config", cxxopts::value<std::string>(), "FILE")
	("m,master", "Promote node to master")
	("v,version", "Framework version")
	("h,help", "Print this help");

	try {
		options.parse(argc, argv);
	} catch (const cxxopts::OptionException& e) {
		std::cerr << "error parsing options: " << e.what() << std::endl;
		return 1;
	}

	if (options.count("version")) {
		std::cerr << Module::version() << std::endl;
		return 0;
	}

	if (options.count("help")) {
		std::cerr << options.help({"Help"}) << std::endl;
		return 0;
	}

	/* Make sure we have an pitcher and chella host even if the ruleset ignores this action */
	if (options.count("hbs")) {
		std::string configfile = options["hbs"].as<std::string>();
		if (!file_exist(configfile)) {
			std::cerr << "error: " << configfile << ": No such file or directory" << std::endl;
			return 1;
		}

		ConfigFile config(configfile);
		if (!config.exist("chella-master")) {
			std::cerr << "Must be at least 1 chella master listed" << std::endl;
			return 1;
		}

		if (!config.exist("chella-ipc")) {
			std::cerr << "Must be at least 1 chella IPC listed" << std::endl;
			return 1;
		}

		masterNode = config.get<std::string>("chella-ipc", "");
	} else {
		std::cerr << "HBS config is required for this service" << std::endl;
		return 1;
	}

	catch_signals();

	if (options.count("master"))
		initMaster();
	else
		initSlave();

	return 0;
}
