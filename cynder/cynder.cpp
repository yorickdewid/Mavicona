#include <zmq.hpp>
#include <map>
#include <string>
#include <iostream>
#include <csignal>
#include <quidpp.h>

#include "common/util.h"
#include "common/sdbm_hash.h"
#include "common/crc32.h"
#include "common/hdb.h"
#include "common/config.h"
#include "common/logger.h"
#include "common/cxxopts.h"
#include "protoc/scrapedata.pb.h"
#include "protoc/storagequery.pb.h"
#include "consistent_hash.h"
#include "catalogus.h"
#include "record_index.h"
#include "data_index.h"
#include "key_index.h"
#include "text_index.h"
#include "filebase.h"

#define SHARDING_SPREAD		10

static Consistent::HashRing<std::string, quidpp::Quid, Crc32> nodeRing(SHARDING_SPREAD, Crc32());
static bool interrupted = false;

void signal_handler(int signum) {
	interrupted = true;
}

static void catch_signals() {
    struct sigaction action;
    action.sa_handler = signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

void performQueryRequest(StorageQuery& query) {
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);

	std::string serialized;
	query.SerializeToString(&serialized);

	std::string host = nodeRing.getNode(query.quid());
	socket.connect(("tcp://" + host).c_str());
	std::cout << "Connect to server " << host << " for record " << query.quid() << std::endl;

	zmq::message_t request(serialized.size());
	memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
	socket.send(request);

	/* Get the reply */
	zmq::message_t reply;
	socket.recv(&reply);
	socket.disconnect(("tcp://" + host).c_str());

	query.ParseFromArray(reply.data(), reply.size());
}

void initMaster() {
	std::cout << "Master" << std::endl;

	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5533");

	std::cout << "Waiting for connections " << std::endl;

	while (true) {
		zmq::message_t request;

		//  Wait for next request from client
		socket.recv(&request);

		ScrapeData data;
		data.ParseFromArray(request.data(), request.size());

		StorageQuery query;
		query.set_name(data.name());
		query.set_id(data.id());
		query.set_quid(data.quid());
		query.set_content(data.content().payload());
		query.set_queryaction(StorageQuery::INSERT);
		query.set_queryresult(StorageQuery::SUCCESS);

		for (int i = 0; i < data.meta_size(); i++) {
			ScrapeData::MetaEntry skeygroup = data.meta(i);

			StorageQuery::MetaEntry *dkeygroup = query.add_meta();
			dkeygroup->set_key(skeygroup.key());

			if (skeygroup.has_value()) {
				dkeygroup->set_value(skeygroup.value());
			} else if (skeygroup.meta_size()) {

				/* Decending */
				for (int j = 0; j < skeygroup.meta_size(); j++) {
					ScrapeData::MetaEntry skey = skeygroup.meta(j);

					StorageQuery::MetaEntry *dkey = dkeygroup->add_meta();
					dkey->set_key(skey.key());
					dkey->set_value(skey.value());
				}
			}
		}

		performQueryRequest(query);

		/* Send reply back to client */
		zmq::message_t reply(5);
		memcpy(reply.data(), "DONE", 5);
		socket.send(reply);
	}
}

void initSlave() {
	std::cout << "Slave" << std::endl;

	// catalogus
	Catalogus cat;
	RecordIndex ari;
	DataIndex adi;
	KeyIndex uki;
	TextIndex fti;
	Filebase lfb;

	/* Save database counters */
	cat.put("ari_count", ari.dbcount());
	cat.put("adi_count", adi.dbcount());
	cat.put("uki_count", uki.dbcount());
	cat.put("fti_count", fti.dbcount());
	cat.put("lfb_count", lfb.dbcount());

	/* Cross data references */
	adi.attach(&lfb);
	uki.attach(&ari);
	uki.attach(&adi);
	fti.attach(&ari);
	fti.attach(&adi);

	/* Prepare our context and socket */
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5522");

	std::string serialized;
	std::cout << "Waiting for connections " << std::endl;

	while (true) {
		zmq::message_t request;

		/* Wait for next request from client */
		try {
			socket.recv(&request);
		} catch (zmq::error_t &e) {
			std::cout << "Exit gracefully" << std::endl;
			break;
		}

		StorageQuery query;
		query.ParseFromArray(request.data(), request.size());
		query.set_queryresult(StorageQuery::SUCCESS);

		/* Recursively insert meta data */
		std::function<void (const StorageQuery::MetaEntry *)> traverseMeta = [&] (const StorageQuery::MetaEntry *key) { 
			if (key->meta_size()) {
				for (int i = 0; i < key->meta_size(); i++)
					traverseMeta(&key->meta(i));
			} else {
				uki.put(query.quid(), key->key(), key->value());
				fti.put(query.quid(), key->value(), key->key());
			}
		};

		try {
			switch (query.queryaction()) {
				case StorageQuery::SELECT:
					std::cout << "Request " << query.id() << " [SELECT] " << query.quid() << " named '" << query.name() << "'" << std::endl;
					
					/* Restore the record */
					serialized = ari.get(query.quid());
					query.ParseFromArray(serialized.data(), serialized.size());

					/* Restore content */
					query.set_content(adi.get(query.quid()));

					break;
				case StorageQuery::INSERT:
					std::cout << "Request " << query.id() << " [INSERT] " << query.quid() << " named '" << query.name() << "'" << std::endl;
					
					/* Store content in LFB */
					if (query.content().size() > ITEM_SIZE) {
						std::cout << "Store in LFB" << std::endl;
					}

					/* Store content */
					adi.put(query.quid(), query.content());

					/* Store the record */
					query.clear_content();
					query.SerializeToString(&serialized);
					ari.put(query.quid(), serialized);

					/* Store additional data in unclustered indexes */
					for (int i = 0; i < query.meta_size(); i++)
						traverseMeta(&query.meta(i));

					break;
				case StorageQuery::UPDATE:
					std::cout << "Request " << query.id() << " [UPDATE] " << query.quid() << " named '" << query.name() << "'" << std::endl;
					/*datadb.put(query.quid(), query.name(), query.content(), true);*/

					break;
				case StorageQuery::DELETE:
					std::cout << "Request " << query.id() << " [DELETE] " << query.quid() << " named '" << query.name() << "'" << std::endl;
					/*datadb.remove(query.quid(), query.name());*/

					break;
			}
		} catch (upscaledb::error &error) {
			switch (error.get_errno()) {
				case UPS_KEY_NOT_FOUND:
					query.set_queryresult(StorageQuery::NOTFOUND);
					break;
				case UPS_DUPLICATE_KEY:
					query.set_queryresult(StorageQuery::DUPLICATE);
					break;
				default:
					std::cerr << "Operation failed: " << error.get_string() << " :: " << error.get_errno() << std::endl;
					break;
			}
		}

		/* Send back query structure */
		query.SerializeToString(&serialized);

		zmq::message_t reply(serialized.size());
		memcpy(reinterpret_cast<void *>(reply.data()), serialized.c_str(), serialized.size());
		socket.send(reply);
	}
}

int main(int argc, char *argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	cxxopts::Options options(argv[0], " [FILE]");

	options.add_options("Help")
	("s,hbs", "Host based service config", cxxopts::value<std::string>(), "FILE")
	("m,master", "Promote node to master")
	("h,help", "Print this help");

	try {
		options.parse(argc, argv);
	} catch (const cxxopts::OptionException& e) {
		std::cerr << "error parsing options: " << e.what() << std::endl;
		return 1;
	}

	if (options.count("help")) {
		std::cerr << options.help({"Help"}) << std::endl;
		return 0;
	}

	/* Make sure we have an pitcher and cynder host even if the ruleset ignores this action */
	if (options.count("hbs")) {
		std::string configfile = options["hbs"].as<std::string>();
		if (!file_exist(configfile)) {
			std::cerr << "error: " << configfile << ": No such file or directory" << std::endl;
			return 1;
		}

		ConfigFile config(configfile);
		if (!config.exist("cynder-master")) {
			std::cerr << "Must be at least 1 cynder master listed" << std::endl;
			return 1;
		}

		if (!config.exist("cynder-worker")) {
			std::cerr << "Must be at least 1 cynder worker listed" << std::endl;
			return 1;
		}

		ConfigFile::range r = config.find("cynder-worker");
		for (ConfigFile::iterator i = r.first; i != r.second; i++)
			nodeRing.addNode(i->second);
	} else {
		std::cerr << "HBS config is required for this service" << std::endl;
		return 1;
	}

	catch_signals();

	if (options.count("master"))
		initMaster();
	else
		initSlave();

	return 0;
}
