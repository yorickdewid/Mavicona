#include "provision.h"

void Provision::runTask() {
	while (_active) {
		if (_queue->totalTaskCount() > 0) {
			std::cout << "Active tasks in all queues: " << _queue->totalTaskCount() << std::endl;
			std::cout << "Provision next task: " << _queue->getNextTask() << std::endl;
		} else {
			// std::cout << "Provision next IDLE task: " << _queue->getNextIdleTask() << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::seconds(this->_timeout));
	}
}
