#include <zmq.hpp>

#include "protoc/processjob.pb.h"
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

			{
				zmq::context_t context(1);
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

				/* Get the reply */
				zmq::message_t reply;
				socket.recv(&reply);
			}

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
