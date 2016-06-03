#include <iostream>
#include <cstdlib>
#include <signal.h>

#include "common/cxxopts.h"
#include "http.h"
#include "log.h"
#include "server.h"

/* Global variables */
static CServer *Server = nullptr;
static unsigned short port = 5500;

void Quit(int dummy) {
	CLog::Print("Terminating...");

	/* Stop server */
	Server->Stop();

	/* Delete server */
	delete Server;

	/* Exit */
	exit(0);
}

int main(int argc, char *argv[]) {
	int res;

	cxxopts::Options options(argv[0], "");

	options.add_options("Help")
	("s,hbs", "Host based service config", cxxopts::value<std::string>(), "FILE")
	("p,port", "Listen to port (default: 5500)", cxxopts::value<unsigned int>(), "PORT")
	("d,dir", "Webledge directory (default: www)", cxxopts::value<unsigned int>(), "DIR")
	("h,help", "Print this help");

	try {
		// options.parse_positional("positional");
		options.parse(argc, argv);
	} catch (const cxxopts::OptionException& e) {
		std::cerr << "error parsing options: " << e.what() << std::endl;
		return 1;
	}

	if (options.count("help")) {
		std::cerr << options.help({"Help"}) << std::endl;
		return 0;
	}

	if (options.count("port")) {
		port = options["port"].as<unsigned int>();
	}

	/* Initialize HTTP class */
	CHTTP::Init();

	/* Create server */
	Server = new CServer(port);
	if (!Server) {
		CLog::PrintErr("ERROR: Server could NOT be created!");
		return 1;
	}

	/* Start server */
	res = Server->Start();
	if (!res) {
		CLog::PrintErr("ERROR: Server could NOT be started!");
		return 1;
	}

	/* Register signal */
	signal(SIGINT, Quit);

	CLog::Print("Waiting for connections");

	/* Keep waiting for connections */
	while (Server->Accept());

	/* Quit */
	Quit(0);

	return 0;
}
