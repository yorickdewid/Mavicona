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
#include "nodemanager.h"
#include "controlclient.h"

static std::string masterNode;
static std::string masterIPC;
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

#define within(num) (int) ((float)num * random () / (RAND_MAX + 1.0))
#include <unistd.h>

void initMaster() {
	std::cout << "Starting master" << std::endl;

	NodeManager master;
	master.start();

	{
		zmq::message_t message(10);
		zmq::context_t context(1);

		/* Socket to send messages on */
		zmq::socket_t sender(context, ZMQ_PUSH);
		sender.bind("tcp://*:5555");

		/* Initialize random number generator */
		srandom((unsigned)time(NULL));

		/* Send 100 tasks */
		int total_msec = 0;     //  Total expected cost in msecs
		for (int task_nbr = 0; task_nbr < 100; task_nbr++) {

			//  Random workload from 1 to 100msecs
			int workload = within(100) + 1;
			total_msec += workload;

			message.rebuild(10);
			memset(message.data(), '\0', 10);
			sprintf((char *)message.data(), "%d", workload);
			sender.send(message);

			std::cout << ">" << workload << std::endl;
			sleep(1);
		}
	}
}

void initSlave() {
	std::cout << "Running worker" << std::endl;

	ControlClient control;
	control.setMaster(masterIPC);
	control.setTimeout(10 /* 1min */);
	control.start();

	mkdir("cache", 0700);

	zmq::context_t context(1);
	zmq::socket_t receiver(context, ZMQ_PULL);
	receiver.connect(("tcp://" + masterNode).c_str());

	/* Process tasks forever */
	while (1) {
		zmq::message_t message;

		receiver.recv(&message);
		std::string smessage(static_cast<char *>(message.data()), message.size());

		std::cout << ">" << smessage << std::endl;

		/* Do the work */
		sleep(1);

		/* Send results to sink */
		message.rebuild();
		// sender.send(message);
	}

	int c = getchar();
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
		masterNode = config.get<std::string>("chella-master", "");

		if (!config.exist("chella-ipc")) {
			std::cerr << "Must be at least 1 chella IPC listed" << std::endl;
			return 1;
		}

		masterIPC = config.get<std::string>("chella-ipc", "");
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
