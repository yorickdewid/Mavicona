#include <zmq.hpp>
#include <string>
#include <vector>
#include <queue>
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

#define FORK 	1

static std::string masterProvision;
static std::string masterIPC;
static bool interrupted = false;
static char **init_argv = NULL;
static unsigned int jobcounter = 0;

std::queue<ProcessJob> jobqueue;

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

void setGuard(pid_t id) {
	std::cout << "Setting guard" << std::endl;

	while (1) {
		sleep(10);
		if (kill(id, 0) < 0) {
			std::cout << "Worker not running, reboot..." << std::endl;

			if (execv(init_argv[0], init_argv)) {
				/* ERROR, handle this yourself */
			}

			break;
		}
	}
}

int setupGuard() {
#ifdef FORK
	pid_t id = getpid();
	pid_t pid = fork();
	if (pid == 0) {

		/* Guarding process */
		setGuard(id);
		return pid;
	} else if (pid < 0) {

		/* Fork failed */
		std::cerr << "fork failed" << std::endl;
		return 0;
	}

	return pid;
#else
	return 1;
#endif
}

void handleWokerController(zmq::socket_t& socket) {
	std::string serialized;
	zmq::message_t request;

	socket.recv(&request);

	ControlMessage msg;
	msg.ParseFromArray(request.data(), request.size());

	NodeManager::runTask(msg, jobqueue.size());

	msg.SerializeToString(&serialized);

	/* Send reply back to client */
	request.rebuild(serialized.size());
	memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
	socket.send(request);
}

void handleWorkerRequest(zmq::socket_t& socket) {
	std::string serialized;
	zmq::message_t request;

	socket.recv(&request);

	/* Send empty response when no jobs in queue */
	if (jobqueue.empty()) {
		request.rebuild(0);
		socket.send(request);
		return;
	}

	/* Pick job from queue */
	ProcessJob nextjob = jobqueue.front();
	nextjob.SerializeToString(&serialized);

	request.rebuild(serialized.size());
	memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
	socket.send(request);

	jobqueue.pop();
}

void handleIncommingJob(zmq::socket_t& socket) {
	std::string serialized;
	zmq::message_t request;

	socket.recv(&request);

	ProcessJob job;
	job.ParseFromArray(request.data(), request.size());
	job.set_id(jobcounter++);

	jobqueue.push(job);

	/* Send reply back to client */
	request.rebuild(0);
	socket.send(request);
}

void initMaster() {
	std::cout << "Starting master" << std::endl;

	int opt = 1;
	zmq::context_t context(1);

	zmq::socket_t controller(context, ZMQ_REP);
	controller.bind("tcp://*:5544");
	controller.setsockopt(ZMQ_IPV6, &opt, sizeof(int));

	/* Socket to send messages on */
	zmq::socket_t worker(context, ZMQ_REP);
	worker.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	worker.bind("tcp://*:5555");

	/* Socket to send messages on */
	zmq::socket_t master(context, ZMQ_REP);
	master.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	master.bind("tcp://*:5566");

	/* Send 2 tasks */
	for (int task_nbr = 0; task_nbr < 2; task_nbr++) {
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
		job.set_id(jobcounter++);
		job.set_quid("429c00e5-290e-406f-8e54-65591ccace21");
		job.set_state(ProcessJob::SPAWN);
		job.set_content(str);

		jobqueue.push(job);
	}

	/* Initialize poll set */
	zmq::pollitem_t items[] = {
		{worker, 0, ZMQ_POLLIN, 0},
		{controller, 0, ZMQ_POLLIN, 0},
		{master, 0, ZMQ_POLLIN, 0}
	};

	while (true) {
		try {
			zmq::poll(&items[0], 3, -1);

			if (items[0].revents & ZMQ_POLLIN)
				handleWorkerRequest(worker);

			if (items[1].revents & ZMQ_POLLIN)
				handleWokerController(controller);

			if (items[2].revents & ZMQ_POLLIN)
				handleIncommingJob(master);

		} catch (zmq::error_t& e) {
			std::cout << "Exit gracefully" << std::endl;
			break;
		}
	}
}

void prepareJob(zmq::message_t& message) {
	SHA1 sha1;
	ProcessJob job;

	job.ParseFromArray(message.data(), message.size());

	/* Store in cache */
	sha1.update(job.content());
	std::string exeName = sha1.final();
	if (!file_exist("cache/module/" + exeName)) {
		std::ofstream file(("cache/module/" + exeName).c_str());
		file.write(job.content().c_str(), job.content().size());
		file.close();
	}

	/* Gather parameters for job */
	Execute::Parameter parameters;
	parameters.jobid = job.id();
	parameters.jobname = job.name();
	parameters.jobquid = job.quid();
	parameters.jobpartition = job.partition();
	parameters.jobpartition_count = job.partition_count();
	parameters.jobstate = static_cast<Callback::JobState>(job.state());
	parameters.jobparent = job.quid_parent();

	/* Run procedure */
	Execute::run(exeName, parameters);

	/* Setup subjobs if any */
	Execute::prospect(exeName);
}

void initSlave() {
	std::cout << "Running worker" << std::endl;

	ControlClient control;
	control.setMaster(masterIPC);
	control.start();

	/* Continue when accepted */
	while (!control.isAccepted()) {
		sleep(1);
	}

	/* Create cache directory */
	mkdir("cache", 0700);
	mkdir("cache/module", 0700);
	mkdir("cache/wal", 0700);

	/* Guard the process */
	if (!setupGuard())
		return;

	zmq::context_t context(1);
	zmq::socket_t receiver(context, ZMQ_REQ);
	receiver.connect(("tcp://" + masterProvision).c_str());

	Execute::init(&control);

	while (true) {
		try {
			zmq::message_t request(0);
			receiver.send(request);

			/* Get the reply */
			zmq::message_t reply;
			receiver.recv(&reply);

			if (!reply.size()) {
				/* Dispose cache when no jobs */
				Execute::dispose();

				sleep(2);
			} else {
				prepareJob(reply);
			}
		} catch (zmq::error_t& e) {
			std::cout << "Exit gracefully" << std::endl;
			break;
		}
	}
}

int main(int argc, char *argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	cxxopts::Options options(argv[0], "");
	init_argv = argv;

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
		if (!config.exist("chella-provision")) {
			std::cerr << "Must be at least 1 chella provision listed" << std::endl;
			return 1;
		}
		masterProvision = config.get<std::string>("chella-provision", "");

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

	google::protobuf::ShutdownProtobufLibrary();

	puts("DONE");

	return 0;
}
