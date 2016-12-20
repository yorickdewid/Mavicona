#include <cppzmq/zmq.h>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <leveldb/db.h>

#include "common/util.h"
#include "common/module.h"
#include "common/config.h"
#include "common/logger.h"
#include "common/cxxopts.h"
#include "protoc/processjob.pb.h"
#include "nodemanager.h"
#include "controlclient.h"
#include "indexer.h"
#include "sha1.h"
#include "exec.h"

// #define GUARD 	1

static std::string masterProvision;
static std::string masterIPC;
static std::string master;
static bool interrupted = false;
static bool jobfork = true;
static char **init_argv = NULL;
static unsigned int worker_counter = 0;

std::queue<ProcessJob> jobqueue;

Indexer *db = nullptr;

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

	/* Accept worker */
	if (request.size() > 0) {
		if (!strncmp((const char *)request.data(), "SOLICIT", request.size())) {
			ProcessJob workerInfo;
			workerInfo.set_name("worker");
			workerInfo.set_id(worker_counter++);
			workerInfo.set_quid("00000000-0000-0000-0000-000000000000");
			workerInfo.set_state(ProcessJob::SPAWN);
			workerInfo.SerializeToString(&serialized);
			request.rebuild(serialized.size());
			memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
			socket.send(request);
			std::cout << "Accept: Solicit from worker, assigned worker-" << (worker_counter - 1) << std::endl;
			return;
		}
	}

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

	std::cout << "Provision job #" << nextjob.id() << std::endl;

	jobqueue.pop();
}

void handleIncommingJob(zmq::socket_t& socket) {
	std::string serialized;
	zmq::message_t request;

	socket.recv(&request);

	ProcessJob job;
	job.ParseFromArray(request.data(), request.size());
	job.set_id(db->jobCounter());

	jobqueue.push(job);

	std::cout << "Incomming job #" << job.id() <<  " -> " << job.quid() << std::endl;

	if (!job.id() % 2)
		db->storeCounter();

	/* Send reply back to client */
	request.rebuild(0);
	socket.send(request);
}

void initMaster() {
	std::cout << "Starting master" << std::endl;

	/* Create cache directory */
	mkdir(VARDIR, 0700);
	db = new Indexer(MSTDIR);

	int opt = 1;
	zmq::context_t context(1);

	/* Socket to receive worker status */
	zmq::socket_t controller(context, ZMQ_REP);
	controller.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	controller.bind("tcp://*:5544");

	/* Socket to receive worker requests */
	zmq::socket_t worker(context, ZMQ_REP);
	worker.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	worker.bind("tcp://*:5555");

	/* Socket to receive incomming jobs */
	zmq::socket_t master(context, ZMQ_REP);
	master.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	master.bind("tcp://*:5566");

	/* Initialize poll set */
	std::vector<zmq::pollitem_t> items = {
		{static_cast<void *>(worker), 0, ZMQ_POLLIN, 0},
		{static_cast<void *>(controller), 0, ZMQ_POLLIN, 0},
		{static_cast<void *>(master), 0, ZMQ_POLLIN, 0},
	};

	while (!interrupted) {
		try {
			zmq::poll(items, -1);

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

	db->storeCounter();
	delete db;
}

void prepareJob(zmq::message_t& message) {
	SHA1 sha1;
	ProcessJob job;

	job.ParseFromArray(message.data(), message.size());

	/* Store in cache */
	sha1.update(job.content());
	std::string exeName = sha1.final();
	if (!file_exist((PKGDIR "/" + exeName).c_str())) {
		std::ofstream file((PKGDIR "/" + exeName).c_str());
		file.write(job.content().c_str(), job.content().size());
		file.close();
	} else {
		std::cout << "Package in local cache" << std::endl;
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
	parameters.jobdata = job.data();

	/* Run procedure */
	if (Execute::run(exeName, parameters, jobfork) && jobfork) {
		/* Setup subjobs if any */
		Execute::prospect(exeName);
		exit(0);
	}
}

void initSlave() {
	/* Create cache directory */
	mkdir(VARDIR, 0700);
	mkdir(PKGDIR, 0700);
	mkdir(OPTDIR, 0700);
	mkdir(WALDIR, 0700);
	mkdir(LOCALDIR, 0700);
	mkdir(TMPDIR, 0700);
	mkdir(ETCDIR, 0700);
	
	Indexer db(DBDIR);

#ifdef GUARD
	/* Guard the process */
	if (!setupGuard())
		return;
#endif

	zmq::context_t context(1);
	zmq::socket_t receiver(context, ZMQ_REQ);
	receiver.connect(("tcp://" + masterProvision).c_str());

	/* Request worker ID */
	zmq::message_t request("SOLICIT", 7);
	receiver.send(request);

	zmq::message_t reply;
	receiver.recv(&reply);

	ProcessJob job;
	job.ParseFromArray(reply.data(), reply.size());

	std::cout << "Solicit accepted, assigned worker-" << job.id() << std::endl;

	/* Initialize global job env */
	Execute::init(job.id(), masterIPC, master, &db);

	unsigned int cache_counter = 0;
	while (!interrupted) {
		try {
			zmq::message_t request(0);
			receiver.send(request);

			/* Get the reply */
			zmq::message_t reply;
			receiver.recv(&reply);

			if (!reply.size()) {
				/* Dispose cache every once in a while */
				if (cache_counter == 100) {
					Execute::dispose();
					cache_counter = 0;
				}

				sleep(2);
			} else {
				cache_counter++;
				prepareJob(reply);
				if (jobfork)
					sleep(20);
			}
		} catch (zmq::error_t& e) {
			std::cout << "Exit gracefully " << std::endl;
			break;
		}
	}

	receiver.close();
}

int main(int argc, char *argv[]) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	cxxopts::Options options(argv[0], "");
	init_argv = argv;

	options.add_options("Help")
	("s,hbs", "Host based service config", cxxopts::value<std::string>(), "FILE")
	("m,master", "Promote node to master")
	("v,version", "Framework version")
	("n,nofork", "Do not run in parallel")
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

	if (options.count("nofork")) {
		jobfork = false;
	}

	/* Make sure we have an pitcher and chella host even if the ruleset ignores this action */
	if (options.count("hbs")) {
		std::string configfile = options["hbs"].as<std::string>();
		if (!file_exist(configfile.c_str())) {
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

		if (!config.exist("chella-master")) {
			std::cerr << "Must be at least 1 chella master listed" << std::endl;
			return 1;
		}

		master = config.get<std::string>("chella-master", "");
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

	return 0;
}
