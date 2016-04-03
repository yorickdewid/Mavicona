#include <zmq.hpp>
#include <string>
#include <iostream>

#include "protoc/task.pb.h"
#include "provision.h"
#include "queue.h"

static Queue<Task> taskQueue;

void parseTask(Task& task) {
	switch (task.priority()) {
		case Task::REALTIME:
			taskQueue.push(task, 0);
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

	Provision event;
	event.setQueuer(&taskQueue);
	event.setTimeout(2); /* 2sec */
	event.start();

	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5599");

	std::cout << "Waiting for connections " << std::endl;

	while (true) {
		zmq::message_t request;

		/*  Wait for next request from client */
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
