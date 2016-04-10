#include <zmq.hpp>
#include <map>
#include <string>
#include <iostream>
#include <quidpp.h>
#include <ups/upscaledb.hpp>
#include <leveldb/db.h>

#include "common/sdbm_hash.h"
#include "common/hdb.h"
#include "consistent_hash.h"
#include "server_node.h"

#define SHARDING_SPREAD		4

void initializeNode() {
	const std::string noteList = "meta";
	leveldb::DB *db;

	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, noteList, &db);

	if (!status.ok())
		std::cerr << status.ToString() << std::endl;

	std::string value;
	leveldb::Status s = db->Get(leveldb::ReadOptions(), "quid", &value);
	if (!s.ok()) {
		quidpp::Quid quid;
		db->Put(leveldb::WriteOptions(), "quid", quid.toString());
	}

	delete db;
}

int main(int argc, char *argv[]) {

	// GOOGLE_PROTOBUF_VERIFY_VERSION;
	// upscaledb::env env;

	initializeNode();

	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5588");

	std::cout << "Waiting for connections " << std::endl;

	while (true) {
		zmq::message_t request;

		/* Wait for next request from client */
		socket.recv(&request);

		// Task task;
		// task.ParseFromArray(request.data(), request.size());

		/* Handle incomming task */
		// parseTask(task);

		/* Send reply back to client */
		zmq::message_t reply(7);
		memcpy(reply.data(), "STORED", 7);
		socket.send(reply);
	}

	return 0;
}
