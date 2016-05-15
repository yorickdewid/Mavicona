#include <zmq.hpp>
#include <map>
#include <string>
#include <iostream>
// #include <quidpp.h>
#include <ups/upscaledb.hpp>
// #include <leveldb/db.h>

#include "common/sdbm_hash.h"
#include "common/hdb.h"
#include "consistent_hash.h"
#include "server_node.h"
#include "node_config.h"

#define SHARDING_SPREAD		4

static Consistent::HashRing<std::string, quidpp::Quid, SdbmHash> nodeRing(SHARDING_SPREAD, SdbmHash());
static std::map<std::string, ServerNode> servers;
static NodeConfig nc;

void test() {
	// Create some cache servers
	servers["cache1.example.com"] = ServerNode("cache1.example.com");
	servers["cache2.example.com"] = ServerNode("cache2.example.com", "layer3");
	servers["cache3.example.com"] = ServerNode("cache3.example.com");

	// Add their host names to the hash ring
	for (std::map<std::string, ServerNode>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
		std::cout << "Adding " << it->first << " with hash " << nodeRing.addNode(it->first) << std::endl;
	}

	// Store some data (keys)
	const quidpp::Quid index[] = {
		"98b2963a-8df6-a151-9cc3-1d5e122ae010",
		"98b43990-8df6-a151-ae5b-f281c32a7091",
		"a01730ca-8df6-a151-b133-a883f23ad02c",
		"a017e56a-8df6-a151-8bc8-fdc712b75026",
		"a0187d2c-8df6-a151-b803-aef9e811200c",
		"a0191750-8df6-a151-9035-8d5ce838e031"
	};
	// values
	const char *colours[] = {"red", "green", "yellow", "orange", "black", "pink"};

	for (unsigned int f = 0; f < 6; f++) {
		std::string host = nodeRing.getNode(index[f]);
		std::cout << "Storing key " << index[f] << " on server " << host << std::endl;
		servers[host].put(index[f].toString(), colours[f]);
	}

	// Read it back
	for (unsigned int f = 0; f < 6; f++) {
		std::string host = nodeRing.getNode(index[f]);
		std::string colour = servers[host].get(index[f].toString());
		std::cout << "Found key " << index[f] << " on server " << servers[host].info() << " record=>(" << colour << ")" << std::endl;
	}
}

void initMaster() {
	std::cout << "Master" << std::endl;

	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);

	socket.connect("tcp://localhost:5522");

	zmq::message_t request(5);
	memcpy(request.data(), "KAAS", 5);
	socket.send(request);

	// Get the reply
	zmq::message_t reply;
	socket.recv(&reply);

	if (!strcmp((const char *)reply.data(), "RESULT")) {
		puts("Yes");
	}

	exit(0);
}

void initSlave() {
	std::cout << "Slave" << std::endl;

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

		std::cout << (const char *)request.data() << std::endl;

		//ScrapeData data;
		//data.ParseFromArray(request.data(), request.size());

		/* Send reply back to client */
		zmq::message_t reply(7);
		memcpy(reply.data(), "RESULT", 7);
		socket.send(reply);
	}
}

int main(int argc, char *argv[]) {

	// GOOGLE_PROTOBUF_VERIFY_VERSION;

	if (argc > 1 && !strcmp(argv[1], "-m"))
		nc.setMaster();

	if (nc.isMaster())
		initMaster();

	/* Only initialize if we're not master */
	initSlave();

	return 0;
}
