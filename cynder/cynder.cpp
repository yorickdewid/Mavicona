#include <zmq.hpp>
#include <map>
#include <string>
#include <iostream>
// #include <quidpp.h>
//#include <ups/upscaledb.hpp>
//#include <leveldb/db.h>

#include "common/sdbm_hash.h"
#include "common/crc32.h"
#include "common/hdb.h"
#include "protoc/storagequery.pb.h"
#include "consistent_hash.h"
#include "server_node.h"
#include "node_config.h"
#include "engine.h"

#define SHARDING_SPREAD		10

static Consistent::HashRing<std::string, quidpp::Quid, Crc32> nodeRing(SHARDING_SPREAD, Crc32());
static std::map<std::string, ServerNode> servers;
static NodeConfig nc;

void initMaster() {
	std::cout << "Master" << std::endl;

	/* Setup node ring */
	nc.foreachSlaveNode([](const std::string & key, const std::string & value) -> void {
		std::cout << "Adding '" << value << ":" << nodeRing.addNode(value) << "' to nodering" << std::endl;
	});

	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);

	int id = 17;
	{
		StorageQuery query;
		query.set_name("woei");
		query.set_id(id++);
		query.set_quid("{8ca9e93a-59ad-4564-b677-5dc59c2f0250}");
		query.set_content("haha bier");
		query.set_queryaction(StorageQuery::INSERT);
		query.set_queryresult(StorageQuery::SUCCESS);

		// Perform query
		std::string serialized;
		query.SerializeToString(&serialized);

		std::string host = nodeRing.getNode(query.quid());
		socket.connect(("tcp://" + host + ":5522").c_str());
		std::cout << "Connect to server " << host << " for record " << query.quid() << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);
		socket.disconnect(("tcp://" + host + ":5522").c_str());

		query.ParseFromArray(reply.data(), reply.size());

		std::cout << "RS: " << query.queryresult() << std::endl;
	}

	{
		StorageQuery query;
		query.set_name("kaas");
		query.set_id(id++);
		query.set_quid("{42ec500a-c0af-40b6-a1f0-37121d6777f7}");
		query.set_content("woef");
		query.set_queryaction(StorageQuery::INSERT);
		query.set_queryresult(StorageQuery::SUCCESS);

		// Perform query
		std::string serialized;
		query.SerializeToString(&serialized);

		std::string host = nodeRing.getNode(query.quid());
		socket.connect(("tcp://" + host + ":5522").c_str());
		std::cout << "Connect to server " << host << " for record " << query.quid() << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);
		socket.disconnect(("tcp://" + host + ":5522").c_str());

		query.ParseFromArray(reply.data(), reply.size());

		std::cout << "RS: " << query.queryresult() << std::endl;
	}

	{
		StorageQuery query;
		query.set_name("blub");
		query.set_id(id++);
		query.set_quid("{a24521f5-2cb8-4c41-aa98-1366a439024e}");
		query.set_content("is ook erg grappig enzo");
		query.set_queryaction(StorageQuery::INSERT);
		query.set_queryresult(StorageQuery::SUCCESS);

		// Perform query
		std::string serialized;
		query.SerializeToString(&serialized);

		std::string host = nodeRing.getNode(query.quid());
		socket.connect(("tcp://" + host + ":5522").c_str());
		std::cout << "Connect to server " << host << " for record " << query.quid() << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);
		socket.disconnect(("tcp://" + host + ":5522").c_str());

		query.ParseFromArray(reply.data(), reply.size());

		std::cout << "RS: " << query.queryresult() << std::endl;
	}

	{
		StorageQuery query;
		query.set_name("worst");
		query.set_id(id++);
		query.set_quid("{5a875b54-b167-4b28-8cfd-7b994598a455}");
		query.set_content("hoort er ook bij");
		query.set_queryaction(StorageQuery::INSERT);
		query.set_queryresult(StorageQuery::SUCCESS);

		// Perform query
		std::string serialized;
		query.SerializeToString(&serialized);

		std::string host = nodeRing.getNode(query.quid());
		socket.connect(("tcp://" + host + ":5522").c_str());
		std::cout << "Connect to server " << host << " for record " << query.quid() << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);
		socket.disconnect(("tcp://" + host + ":5522").c_str());

		query.ParseFromArray(reply.data(), reply.size());

		std::cout << "RS: " << query.queryresult() << std::endl;
	}

	{
		StorageQuery query;
		query.set_name("woei");
		query.set_id(id++);
		query.set_quid("{8ca9e93a-59ad-4564-b677-5dc59c2f0250}");
		query.set_content("haha bier, en nog meer");
		query.set_queryaction(StorageQuery::UPDATE);
		query.set_queryresult(StorageQuery::SUCCESS);

		std::string serialized;
		query.SerializeToString(&serialized);

		std::string host = nodeRing.getNode(query.quid());
		socket.connect(("tcp://" + host + ":5522").c_str());
		std::cout << "Connect to server " << host << " for record " << query.quid() << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);
		socket.disconnect(("tcp://" + host + ":5522").c_str());

		query.ParseFromArray(reply.data(), reply.size());

		std::cout << "SELECT result " << query.content() << std::endl;
		std::cout << "RS: " << query.queryresult() << std::endl;
	}

	{
		StorageQuery query;
		query.set_name("woei");
		query.set_id(id++);
		query.set_quid("{8ca9e93a-59ad-4564-b677-5dc59c2f0250}");
		query.set_queryaction(StorageQuery::SELECT);
		query.set_queryresult(StorageQuery::SUCCESS);

		std::string serialized;
		query.SerializeToString(&serialized);

		std::string host = nodeRing.getNode(query.quid());
		socket.connect(("tcp://" + host + ":5522").c_str());
		std::cout << "Connect to server " << host << " for record " << query.quid() << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);
		socket.disconnect(("tcp://" + host + ":5522").c_str());

		query.ParseFromArray(reply.data(), reply.size());

		std::cout << "SELECT result " << query.content() << std::endl;
		std::cout << "RS: " << query.queryresult() << std::endl;
	}

	{
		StorageQuery query;
		query.set_name("woei");
		query.set_id(id);
		query.set_quid("{8ca9e93a-59ad-4564-b677-5dc59c2f0250}");
		query.set_queryaction(StorageQuery::DELETE);
		query.set_queryresult(StorageQuery::SUCCESS);

		std::string serialized;
		query.SerializeToString(&serialized);

		std::string host = nodeRing.getNode(query.quid());
		socket.connect(("tcp://" + host + ":5522").c_str());
		std::cout << "Connect to server " << host << " for record " << query.quid() << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);
		socket.disconnect(("tcp://" + host + ":5522").c_str());

		query.ParseFromArray(reply.data(), reply.size());

		std::cout << "RS: " << query.queryresult() << std::endl;
	}

	{
		StorageQuery query;
		query.set_name("woei");
		query.set_id(id++);
		query.set_quid("{8ca9e93a-59ad-4564-b677-5dc59c2f0250}");
		query.set_queryaction(StorageQuery::SELECT);
		query.set_queryresult(StorageQuery::SUCCESS);

		std::string serialized;
		query.SerializeToString(&serialized);

		std::string host = nodeRing.getNode(query.quid());
		socket.connect(("tcp://" + host + ":5522").c_str());
		std::cout << "Connect to server " << host << " for record " << query.quid() << std::endl;

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		// Get the reply
		zmq::message_t reply;
		socket.recv(&reply);
		socket.disconnect(("tcp://" + host + ":5522").c_str());

		query.ParseFromArray(reply.data(), reply.size());

		std::cout << "RS: " << query.queryresult() << std::endl;
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
		query.set_queryresult(StorageQuery::SUCCESS);

		try {
			switch (query.queryaction()) {
				case StorageQuery::SELECT:
					std::cout << "Request " << query.id() << " [SELECT] " << query.quid() << " named '" << query.name() << "'" << std::endl;
					query.set_content(coredb.get(query.quid()));
					break;
				case StorageQuery::INSERT:
					std::cout << "Request " << query.id() << " [INSERT] " << query.quid() << " named '" << query.name() << "'" << std::endl;
					coredb.put(query.quid(), query.name(), query.content());
					// insert into meta [ADI]
					break;
				case StorageQuery::UPDATE:
					std::cout << "Request " << query.id() << " [UPDATE] " << query.quid() << " named '" << query.name() << "'" << std::endl;
					coredb.put(query.quid(), query.name(), query.content(), true);
					break;
				case StorageQuery::DELETE:
					std::cout << "Request " << query.id() << " [DELETE] " << query.quid() << " named '" << query.name() << "'" << std::endl;
					coredb.remove(query.quid(), query.name());
					break;
			}
		} catch (upscaledb::error &e) {
			switch (e.get_errno()) {
				case UPS_KEY_NOT_FOUND:
					query.set_queryresult(StorageQuery::NOTFOUND);
					break;
				case UPS_DUPLICATE_KEY:
					query.set_queryresult(StorageQuery::DUPLICATE);
					break;
				default:
					std::cerr << "Operation failed: " << e.get_string() << " :: " << e.get_errno() << std::endl;
					break;
			}
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

	if (argc > 1 && !strcmp(argv[1], "-m")) {
		nc.setMaster();
	}

	if (nc.isMaster())
		initMaster();

	/* Only initialize if we're not master */
	initSlave();

	return 0;
}
