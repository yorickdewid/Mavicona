#include "provision.h"

void Provision::runTask() {
	while (_active) {
		if (_queue->totalTaskCount() > 0) {
			Task nextTask = _queue->getNextTask();
			std::cout << "Accepting task: " << nextTask.id() << std::endl;
			std::cout << "  Task[" << nextTask.id() << "] object: " << nextTask.quid() << std::endl;
			std::cout << "  Task[" << nextTask.id() << "] name: " << nextTask.name() << std::endl;
			std::cout << "Provision task " << nextTask.id() << " to cluster" << std::endl;
			std::cout << "Active tasks in queues: " << _queue->totalTaskCount() << std::endl;
		} else if (_queue->idleTaskCount() > 0) {
			Task nextTask = _queue->getNextTask();
			std::cout << "Accepting slow task: " << nextTask.id() << std::endl;
			std::cout << "  Task[" << nextTask.id() << "] object: " << nextTask.quid() << std::endl;
			std::cout << "  Task[" << nextTask.id() << "] name: " << nextTask.name() << std::endl;
			std::cout << "Provision slow task " << nextTask.id() << " to cluster" << std::endl;
			std::cout << "Active slow tasks in queues: " << _queue->idleTaskCount() << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::seconds(this->_timeout));
	}
}
