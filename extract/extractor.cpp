#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

#include "common/logger.h"
#include "protoc/scrapedata.pb.h"
#include "ruler.h"
#include "detect.h"

//#define FORK 	1

static std::vector<RuleNode *> *commonRuleset = nullptr;

void parseData(ScrapeData& data);

pid_t handleRequest(ScrapeData& data) {
#ifdef FORK
	pid_t pid = fork();
	if (pid == 0) {

		/* Child process */
		parseData(data);

		return pid;
	} else if (pid < 0) {

		/* Fork failed */
		std::cerr << "fork failed" << std::endl;
		return -1;
	}

	return pid;
#else
	parseData(data);

	return 1;
#endif
}

void parseData(ScrapeData& data) {
	Detect detector;
	Ruler ruler(commonRuleset);

	std::cout << "Item[" << data.id() << "] object: " << data.quid() << std::endl;
	std::cout << "Item[" << data.id() << "] name: " << data.name() << std::endl;

	switch (data.type()) {
		case ScrapeData::PLAIN:
			std::cout << "Item[" << data.id() << "] type: PLAIN" << std::endl;
			break;
		case ScrapeData::FILE:
			std::cout << "Item[" << data.id() << "] type: FILE" << std::endl;
			break;
		case ScrapeData::BINARY:
			std::cout << "Item[" << data.id() << "] type: BINARY" << std::endl;
			break;
		case ScrapeData::STREAM:
			std::cout << "Item[" << data.id() << "] type: STREAM" << std::endl;
			break;
	}

	ScrapeData::Data payload = data.content();

	/* Determine type and additional information */
	detector.mimeFromBuffer(payload.payload().c_str(), payload.payload().size());
	if (!detector.found() && !payload.extension().empty()) {

		/* We may match an file extension */
		detector.mimeFromExtension(payload.extension());
	}

	/* At this point al information is gathered so we need to execute the correct ruleset.
	 * We first try to match the most accurate information towards the unknown ruleset which
	 * basically maches anything.
	 */
	if (detector.found()) {
		std::cout << "Item[" << data.id() << "] mime name: " << detector.mime()->name() << std::endl;
		std::cout << "Item[" << data.id() << "] mime type: " << detector.mime()->type() << std::endl;
		std::cout << "Item[" << data.id() << "] mime category: " << detector.mime()->category() << std::endl;

		ScrapeData::MetaEntry *metaMime = data.add_meta();
		metaMime->set_key("mime");

		ScrapeData::MetaEntry *metaName = metaMime->add_meta();
		metaName->set_key("name");
		metaName->set_value(detector.mime()->name());

		ScrapeData::MetaEntry *metaType = metaMime->add_meta();
		metaType->set_key("type");
		metaType->set_value(detector.mime()->type());

		ScrapeData::MetaEntry *metaCategory = metaMime->add_meta();
		metaCategory->set_key("category");
		metaCategory->set_value(detector.mime()->category());

		if (!detector.charset().empty()) {
			std::cout << "Item[" << data.id() << "] (unattended) charset: " << detector.charset() << std::endl;

			ScrapeData::MetaEntry *metaCharset = data.add_meta();
			metaCharset->set_key("charset");
			metaCharset->set_value(detector.charset());
		}


		/* Match mime */
		if (!ruler.matchMimeRule(detector.mime())) {

			/* Match category */
			ruler.matchCategoryRule(detector.mime());
		}
	}

	if (!ruler.hasActionList() && !payload.extension().empty()) {

		/* Match extension */
		ruler.matchExtensionRule(payload.extension());
	}

	if (!ruler.hasActionList()) {

		/* Match type */
		switch (data.type()) {
			case ScrapeData::PLAIN:
				ruler.matchTypeRule("plain");
				break;
			case ScrapeData::FILE:
				ruler.matchTypeRule("file");
				break;
			case ScrapeData::BINARY:
				ruler.matchTypeRule("binary");
				break;
			case ScrapeData::STREAM:
				ruler.matchTypeRule("stream");
				break;
		}
	}

	/* Notify parse observers
	 * Handlers can extract aditional metadata from the payload and append this to the object
	 */
	detector.setDataProfile(data);
	detector.notify();

	// std::string serialized;
	// data.SerializeToString(&serialized);

	ruler.setDataProfile(data);
	ruler.runRuleActions();

	std::cout << std::endl;
}

int main(int argc, char *argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	if (argc < 2) {
		std::cerr << "Usage: extractor <config> [args]" << std::endl;
		return 1;
	}

	if (!(commonRuleset = Ruler::parseConfigFile(argv[1]))) {
		std::cerr << "Config error" << std::endl;
		return 1;
	}

	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5577");

	std::cout << "Waiting for connections " << std::endl;

	while (true) {
		zmq::message_t request;

		//  Wait for next request from client
		socket.recv(&request);

		ScrapeData data;
		data.ParseFromArray(request.data(), request.size());

		/* Handle incomming data */
		pid_t pid = handleRequest(data);
		if (pid == 0)
			exit(0);

		/* Send reply back to client */
		zmq::message_t reply(5);
		memcpy(reply.data(), "DONE", 5);
		socket.send(reply);
	}

	return 0;
}

