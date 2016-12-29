#include <fstream>
#include <zmq.hpp>
#include <quidpp.h>
#include <time.h>
#include "protoc/processjob.pb.h"
#include "chella.h"

/* Module specific commands */
void ModChella::sumbitJob(int paramc, const std::vector<std::string>& params) {
	std::string data;
	if (paramc < 1)
		throw TooFewParameters(1);

	if (params[0].empty())
		throw InvalidParameter("jobname empty");

	if (paramc == 2)
		data = params[1];

	srand (time(NULL));
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);

	socket.connect("tcp://localhost:5566");
	std::cout << "Connect to Chella master... " << std::endl;

	std::ifstream fjob(params[0]);
	std::string jobobject;

	fjob.seekg(0, std::ios::end);
	jobobject.reserve(fjob.tellg());
	fjob.seekg(0, std::ios::beg);

	jobobject.assign((std::istreambuf_iterator<char>(fjob)), std::istreambuf_iterator<char>());

	ProcessJob job;
	job.set_name(params[0]);
	job.set_id(rand() % 1000 + 1);
	job.set_quid(quidpp::Quid().toString(true));
	job.set_state(ProcessJob::SPAWN);
	job.set_content(jobobject);
	job.set_data(data);

	std::string serialized;
	job.SerializeToString(&serialized);

	zmq::message_t request(serialized.size());
	memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
	socket.send(request);

	/* Get the reply */
	zmq::message_t reply;
	socket.recv(&reply);
	std::cout << "Job submitted" << std::endl;
}
