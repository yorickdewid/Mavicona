#include "provision.h"

void Provision::runTask() {
	while (_active) {
		if (_queue->totalTaskCount() > 0) {
			Task nextTask = _queue->getNextTask();
			(*_logger) << "Accepting task: " << nextTask.id() << FileLogger::endl();
			(*_logger) << "  Task[" << nextTask.id() << "] object: " << nextTask.quid() << FileLogger::endl();
			(*_logger) << "  Task[" << nextTask.id() << "] name: " << nextTask.name() << FileLogger::endl();
			(*_logger) << "Provision task " << nextTask.id() << " to cluster" << FileLogger::endl();
			(*_logger) << "Active tasks in queues: " << _queue->totalTaskCount() << FileLogger::endl();

			_queue->doneTask(nextTask);
		} else if (_queue->idleTaskCount() > 0) {
			Task nextTask = _queue->getNextIdleTask();
			(*_logger) << "Accepting slow task: " << nextTask.id() << FileLogger::endl();
			(*_logger) << "  Task[" << nextTask.id() << "] object: " << nextTask.quid() << FileLogger::endl();
			(*_logger) << "  Task[" << nextTask.id() << "] name: " << nextTask.name() << FileLogger::endl();
			(*_logger) << "Provision slow task " << nextTask.id() << " to cluster" << FileLogger::endl();
			(*_logger) << "Active slow tasks in queues: " << _queue->idleTaskCount() << FileLogger::endl();

			_queue->doneTask(nextTask);
		}

		_logger->flush();

		std::this_thread::sleep_for(std::chrono::seconds(this->_timeout));
	}
}
