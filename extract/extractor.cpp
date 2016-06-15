#include <zmq.hpp>
#include <string>
#include <csignal>
#include <iostream>
#include <unistd.h>

#include "common/util.h"
#include "common/config.h"
#include "common/cxxopts.h"
#include "protoc/scrapedata.pb.h"
#include "ruler.h"
#include "detect.h"

#define FORK 	1

static unsigned int dataCounter = 1000;
static std::vector<RuleNode *> *commonRuleset = nullptr;
static bool doFork = true;
static bool interrupted = false;

std::string configfile;

void parseData(ScrapeData& data, unsigned int counter);

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

pid_t handleRequest(ScrapeData& data) {
	dataCounter++;
#ifdef FORK
	if (!doFork) {
		parseData(data, dataCounter);
		return 1;
	}

	pid_t pid = fork();
	if (pid == 0) {

		/* Child process */
		parseData(data, dataCounter);

		return pid;
	} else if (pid < 0) {

		/* Fork failed */
		std::cerr << "fork failed" << std::endl;
		return -1;
	}

	return pid;
#else
	parseData(data, dataCounter);

	return 1;
#endif
}

void parseData(ScrapeData& data, unsigned int counter) {
	Detect detector;
	Ruler ruler(commonRuleset);

	data.set_id(counter);
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

	ruler.setConfigFile(configfile);
	ruler.setDataProfile(data);
	ruler.runRuleActions();
}

int main(int argc, char *argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	cxxopts::Options options(argv[0], " [FILE]");

	options.add_options("Help")
	("s,hbs", "Host based service config", cxxopts::value<std::string>(), "FILE")
#ifdef FORK
	("o,inorder", "Parse requests in order (default: parallel)")
#endif
	("h,help", "Print this help");

	options.add_options()
	("positional", "&", cxxopts::value<std::string>());

	try {
		options.parse_positional("positional");
		options.parse(argc, argv);
	} catch (const cxxopts::OptionException& e) {
		std::cerr << "error parsing options: " << e.what() << std::endl;
		return 1;
	}

	if (options.count("help")) {
		std::cerr << options.help({"Help"}) << std::endl;
		return 0;
	}

	if (!options.count("positional")) {
		std::cerr << options.help({"Help"}) << std::endl;
		return 1;
	}

	if (options.count("inorder")) {
		doFork = false;
		std::cout << "Running operations in order" << std::endl;
	}

	std::string name = options["positional"].as<std::string>();
	if (!file_exist(name)) {
		std::cerr << "error: " << name << ": No such file or directory" << std::endl;
		return 1;
	}

	/* Make sure we have an pitcher and cynder host even if the ruleset ignores this action */
	if (options.count("hbs")) {
		configfile = options["hbs"].as<std::string>();
		if (!file_exist(configfile)) {
			std::cerr << "error: " << configfile << ": No such file or directory" << std::endl;
			return 1;
		}

		ConfigFile config(configfile);
		if (!config.exist("pitch")) {
			std::cerr << "Must be at least 1 pitcher listed" << std::endl;
			return 1;
		}

		if (!config.exist("cynder-master")) {
			std::cerr << "Must be at least 1 cynder master listed" << std::endl;
			return 1;
		}
	}

	if (!(commonRuleset = Ruler::parseConfigFile(name))) {
		std::cerr << "Config error" << std::endl;
		return 1;
	}

	catch_signals();

	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);

	int opt = 1;
	socket.setsockopt(ZMQ_IPV6, &opt, sizeof(int));
	socket.bind("tcp://*:5577");

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
