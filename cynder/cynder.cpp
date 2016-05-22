#include <zmq.hpp>
#include <map>
#include <string>
#include <iostream>
// #include <quidpp.h>
//#include <ups/upscaledb.hpp>
//#include <leveldb/db.h>

#include "common/sdbm_hash.h"
#include "common/hdb.h"
#include "protoc/storagequery.pb.h"
#include "consistent_hash.h"
#include "server_node.h"
#include "node_config.h"
#include "engine.h"

#define SHARDING_SPREAD		4

static Consistent::HashRing<std::string, quidpp::Quid, SdbmHash> nodeRing(SHARDING_SPREAD, SdbmHash());
static std::map<std::string, ServerNode> servers;
static NodeConfig nc;

void initMaster() {
	std::cout << "Master" << std::endl;

	/* Setup node ring */
	nc.foreachSlaveNode([](const std::string & key, const std::string & value) -> void {
		nodeRing.addNode(value);
		std::cout << "Adding '" << value << "' to nodering" << std::endl;
	});

	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);

	{
		StorageQuery query;
		query.set_name("woei");
		query.set_id(17);
		query.set_quid("{a2191750-8df6-a151-9035-8d5ce838e031}");
		query.set_content("haha bier");
		query.set_queryaction(StorageQuery::INSERT);

		// Perform query
		std::string serialized;
		query.SerializeToString(&serialized);

		std::string host = nodeRing.getNode(query.quid());
		socket.connect(("tcp://" + host + ":5522").c_str());
		std::cout << "Connect to server " << host << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);

		query.ParseFromArray(reply.data(), reply.size());
	}

	{
		StorageQuery query;
		query.set_name("kaas");
		query.set_id(17);
		query.set_quid("{a0191750-8df6-a151-9035-8d5ce838e028}");
		query.set_content("woef");
		query.set_queryaction(StorageQuery::INSERT);

		// Perform query
		std::string serialized;
		query.SerializeToString(&serialized);

		std::string host = nodeRing.getNode(query.quid());
		socket.connect(("tcp://" + host + ":5522").c_str());
		std::cout << "Connect to server " << host << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);

		query.ParseFromArray(reply.data(), reply.size());
	}

	{
		StorageQuery query;
		query.set_name("woei");
		query.set_id(17);
		query.set_quid("{a2191750-8df6-a151-9035-8d5ce838e031}");
		query.set_queryaction(StorageQuery::SELECT);

		std::string serialized;
		query.SerializeToString(&serialized);

		std::string host = nodeRing.getNode(query.quid());
		socket.connect(("tcp://" + host + ":5522").c_str());
		std::cout << "Connect to server " << host << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);

		query.ParseFromArray(reply.data(), reply.size());

		std::cout << "SELECT result " << query.content() << std::endl;
	}

	exit(0); /* Should never reach */
}

void initSlave() {
	std::cout << "Slave" << std::endl;

	Engine coredb(EngineType::DB_ABI);
	Engine metadb(EngineType::DB_ADI);

	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5522");

	std::cout << "Waiting for connections " << std::endl;

	while (true) {
		zmq::message_t request;

		//  Wait for next request from client
		socket.recv(&request);

		StorageQuery query;
		query.ParseFromArray(request.data(), request.size());

		switch (query.queryaction()) {
			case StorageQuery::SELECT:
				std::cout << "Request " << query.id() << " [SELECT] " << query.quid() << " & " << query.name() << std::endl;
				query.set_content(coredb.get(query.quid()));
				break;
			case StorageQuery::INSERT:
				std::cout << "Request " << query.id() << " [INSERT] " << query.quid() << " & " << query.name() << std::endl;
				coredb.put(query.quid(), query.name(), query.content());
				// insert into meta [ADI]
				break;
			case StorageQuery::UPDATE:
				std::cerr << "NOP" << std::endl;
				break;
			case StorageQuery::DELETE:
				std::cout << "Request " << query.id() << " [DELETE] " << query.quid() << " & " << query.name() << std::endl;
				coredb.remove(query.quid(), query.name());
				break;
		}

		// Send back query structure
		std::string serialized;
		query.SerializeToString(&serialized);

		zmq::message_t reply(serialized.size());
		memcpy(reinterpret_cast<void *>(reply.data()), serialized.c_str(), serialized.size());
		socket.send(reply);
	}
}

int main(int argc, char *argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	if (argc > 2 && !strcmp(argv[1], "-a")) {
		nc.addSlaveNode(argv[2]);
		return 0;
	}

	if (argc > 1 && !strcmp(argv[1], "-m"))
		nc.setMaster();

	if (nc.isMaster())
		initMaster();

	/* Only initialize if we're not master */
	initSlave();

	return 0;
}
