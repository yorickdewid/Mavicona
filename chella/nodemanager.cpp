#include <zmq.hpp>
#include "nodemanager.h"

namespace NodeManager {

static unsigned int worker_counter = 0;

void runTask(ControlMessage& message, size_t queuesize) {
	switch (message.action()) {
		case ControlMessage::SOLICIT:
			message.set_id(worker_counter++);
			message.set_action(ControlMessage::CONFIRMED);
			std::cout << "Accept: Solicit from worker, assigned worker-" << message.id() << std::endl;
			break;
		case ControlMessage::IDLE:
			printf("worker-%d -> IDLE\n", message.id());
			break;
		case ControlMessage::ACCEPTED:
			printf("worker-%d -> ACCEPTED\n", message.id());
			break;
		case ControlMessage::SETUP:
			printf("worker-%d -> SETUP\n", message.id());
			break;
		case ControlMessage::RUNNING:
			printf("worker-%d -> RUNNING %.1f%%\n", message.id(), (float)((float)message.progress() / (float)10));
			break;
		case ControlMessage::TEARDOWN:
			printf("worker-%d -> TEARDOWN\n", message.id());
			break;
		case ControlMessage::FAILED:
			printf("worker-%d -> FAILED\n", message.id());
			break;
		case ControlMessage::SHUTDOWN:
			std::cout << "Shutdown: Worker-" << message.id() << " signed off" << std::endl;
			break;
		default:
			break;
	}

	message.set_cluster_jobs(queuesize);
}

}
