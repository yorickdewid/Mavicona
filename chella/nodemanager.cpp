#include <zmq.hpp>
#include "nodemanager.h"

namespace NodeManager {

void runTask(ControlMessage& message, size_t queuesize) {
	switch (message.action()) {
		case ControlMessage::IDLE:
			printf("worker-%d job #%d -> IDLE\n", message.id(), message.jobid());
			break;
		case ControlMessage::AWAITING:
			printf("worker-%d job #%d -> AWAITING\n", message.id(), message.jobid());
			break;
		case ControlMessage::ACCEPTED:
			printf("worker-%d job #%d -> ACCEPTED\n", message.id(), message.jobid());
			break;
		case ControlMessage::SETUP:
			printf("worker-%d job #%d -> SETUP\n", message.id(), message.jobid());
			break;
		case ControlMessage::RUNNING:
			printf("worker-%d job #%d -> RUNNING %.1f%%\n", message.id(), message.jobid(), (float)((float)message.progress() / (float)10));
			break;
		case ControlMessage::TEARDOWN:
			printf("worker-%d job #%d -> TEARDOWN\n", message.id(), message.jobid());
			break;
		case ControlMessage::FAILED:
			printf("worker-%d job #%d -> FAILED\n", message.id(), message.jobid());
			break;
		default:
			break;
	}

	message.set_cluster_jobs(queuesize);
}

}
