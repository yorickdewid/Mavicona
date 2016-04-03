#include <iostream>

#include "queue.h"

Queue::Queue() {
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, "qstore", &this->db);

	if (!status.ok())
		std::cerr << status.ToString() << std::endl;
}

void Queue::push(int task, int prio) {
	this->taskList.push(task);
}

int Queue::getNextTask() {
	int task = taskList.front();
	taskList.pop();
	return task;
}

void Queue::doneTask(int task) {
	taskList.pop();
}

Queue::~Queue() {
	delete this->db;
}
