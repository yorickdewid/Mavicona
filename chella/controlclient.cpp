#include "controlclient.h"

void ControlClient::runTask() {
retry:
	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);

	int opt = 3000;
	socket.setsockopt(ZMQ_SNDTIMEO, &opt, sizeof(int));
	socket.setsockopt(ZMQ_RCVTIMEO, &opt, sizeof(int));
	
	socket.connect(("tcp://" + _masterNode).c_str());
	std::cout << "Connect to master " << _masterNode << std::endl;

	ControlMessage msg;
	while (_active) {
		std::this_thread::sleep_for(std::chrono::milliseconds(this->_timeout));
		assert(this->_progress >= 0 && this->_progress <= 1000);

		msg.set_id(this->_workerid);
		msg.set_jobid(this->_jobid);
		msg.set_quid(this->_quid);
		msg.set_action(this->_state);
		msg.set_progress(this->_progress);

		std::string serialized;
		msg.SerializeToString(&serialized);

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		if (!socket.send(request)) {
			std::cerr << "Lost connection to master" << std::endl;
			socket.close();
			context.close();
			goto retry;
		}

		/* Get the reply */
		zmq::message_t reply;
		if (!socket.recv(&reply)){
			std::cerr << "Lost connection to master" << std::endl;
			socket.close();
			context.close();
			goto retry;
		}

		msg.ParseFromArray(reply.data(), reply.size());
		this->_cluster_jobs = msg.cluster_jobs();
	}

	socket.close();
	context.close();
}
