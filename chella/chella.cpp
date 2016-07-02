#include <zmq.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <csignal>
#include <unistd.h>

#include "common/util.h"
#include "common/module.h"
#include "common/config.h"
#include "common/logger.h"
#include "common/cxxopts.h"
#include "protoc/processjob.pb.h"
#include "nodemanager.h"
#include "controlclient.h"
#include "sha1.h"
#include "exec.h"

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

void initMaster() {
	std::cout << "Starting master" << std::endl;

	NodeManager master;
	master.start();

	zmq::message_t message;
	zmq::context_t context(1);

	/* Socket to send messages on */
	zmq::socket_t sender(context, ZMQ_PUSH);
	sender.bind("tcp://*:5555");

	/* Send 10 tasks */
	for (int task_nbr = 0; task_nbr < 5; task_nbr++) {
		// {
		std::ifstream t("libdso_example.so");
		std::string str;

		t.seekg(0, std::ios::end);
		str.reserve(t.tellg());
		t.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		// }

		ProcessJob job;
		job.set_name("woei");
		job.set_id(task_nbr);
		job.set_quid("429c00e5-290e-406f-8e54-65591ccace21");
		job.set_content(str);
		job.set_partition(0);

		std::string serialized;
		job.SerializeToString(&serialized);

		/* Send reply back to client */
		message.rebuild(serialized.size());
		memcpy(reinterpret_cast<void *>(message.data()), serialized.c_str(), serialized.size());
		sender.send(message);

		sleep(1);
	}

	getchar();
}

void initSlave() {
	std::cout << "Running worker" << std::endl;

	ControlClient control;
	control.setMaster(masterIPC);
	control.setTimeout(10 /* 1min */);
	control.start();

	/* Continue when accepted */
	while (!control.isAccepted()) {
		sleep(1);
	}

	/* Create cache directory */
	mkdir("cache", 0700);

	zmq::context_t context(1);
	zmq::socket_t receiver(context, ZMQ_PULL);
	receiver.connect(("tcp://" + masterNode).c_str());

	/* Process tasks forever */
	while (1) {
		SHA1 sha1;
		zmq::message_t message;

		receiver.recv(&message);

		ProcessJob job;
		job.ParseFromArray(message.data(), message.size());

		std::cout << "Job " << job.id() << std::endl;

		/* Store in cache */
		sha1.update(job.content());
		std::string exeName = sha1.final();
		if (!file_exist("cache/" + exeName)) {
			std::ofstream file(("cache/" + exeName).c_str());
			file.write(job.content().c_str(), job.content().size());
			file.close();
		}

		/* Gather parameters for job */
		Execute::Parameter parameters;
		parameters.jobid = job.id();
		parameters.jobname = job.name();
		parameters.jobquid = job.quid();
		parameters.jobpartition = job.partition();
		parameters.workerid = control.workerIdent();

		/* Run procedure */
		Execute::run(exeName, parameters);

		sleep(1);
	}

	getchar();
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
