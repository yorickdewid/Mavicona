#include <fstream>
#include <zmq.hpp>

#include "protoc/task.pb.h"
#include "pitcher.h"

/* Module specific commands */
void Pitcher::newTask(int paramc, char *param[]) {
	std::string name;
	std::cout << "Task name: ";
	std::cin >> name;

	std::string content;
	std::cout << "Job DSO: ";
	std::cin >> content;

	/* Read DSO */
	std::ifstream tcounter(content);
	std::string str_counter;

	tcounter.seekg(0, std::ios::end);
	str_counter.reserve(tcounter.tellg());
	tcounter.seekg(0, std::ios::beg);

	str_counter.assign((std::istreambuf_iterator<char>(tcounter)), std::istreambuf_iterator<char>());

	/* Connect */
	zmq::context_t context;
	zmq::socket_t socket(context, ZMQ_REQ);
	std::cout << "Connecting to pitcher..." << std::endl;
	socket.connect("tcp://localhost:5599");

	/* Create meta data object */
	Task task;
	task.set_name(name);
	task.set_id(0);
	task.set_quid("a6165475-9e69-45ed-9437-2f50faf0fe0e");
	task.set_priority(Task::NORMAL);
	task.set_content(str_counter);

	std::string serialized;
	task.SerializeToString(&serialized);

	zmq::message_t request(serialized.size());
	memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
	socket.send(request);

	// Get the reply
	zmq::message_t reply;
	socket.recv(&reply);

	if (!strcmp((const char *)reply.data(), "QUEUED")) {
		std::cout << "Sumbitted task to queue" << std::endl;
	} else {
		std::cerr << "Task transaction failure " << std::endl;
	}
	///

	/*zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);

	socket.connect("tcp://172.16.3.12:5566");
	// std::cout << "Connect to master " << exec.master << std::endl;

	std::ifstream tcounter("libdso_counter.so");
	std::string str_counter;

	tcounter.seekg(0, std::ios::end);
	str_counter.reserve(tcounter.tellg());
	tcounter.seekg(0, std::ios::beg);

	str_counter.assign((std::istreambuf_iterator<char>(tcounter)), std::istreambuf_iterator<char>());

	ProcessJob job;
	job.set_name(nextTask.name());
	job.set_id(nextTask.id());
	job.set_quid(nextTask.quid());
	job.set_state(ProcessJob::SPAWN);
	job.set_content(str_counter);

	std::string serialized;
	job.SerializeToString(&serialized);

	zmq::message_t request(serialized.size());
	memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
	socket.send(request);

	zmq::message_t reply;
	socket.recv(&reply)*/
}
