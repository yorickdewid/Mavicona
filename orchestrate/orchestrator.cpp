#include <iostream>
#include <cstdlib>
#include <signal.h>

#include "common/module.h"
#include "common/cxxopts.h"
#include "http.h"
#include "server.h"

/* Global variables */
static CServer *Server = nullptr;
static unsigned short port = 5500;
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

int main(int argc, char *argv[]) {
	int res;

	cxxopts::Options options(argv[0], "");

	options.add_options("Help")
	("s,hbs", "Host based service config", cxxopts::value<std::string>(), "FILE")
	("p,port", "Listen to port (default: 5500)", cxxopts::value<unsigned int>(), "PORT")
	("d,dir", "Webledge directory (default: www)", cxxopts::value<unsigned int>(), "DIR")
	("v,version", "Framework version")
	("h,help", "Print this help");

	try {
		options.parse(argc, argv);
	} catch (const cxxopts::OptionException& e) {
		std::cerr << "error parsing options: " << e.what() << std::endl;
		return 1;
	}

	if (options.count("version")) {
		std::cerr << Module::version() << std::endl;
		return 0;
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
		std::cerr << "Cannot create server" << std::endl;
		return 1;
	}

	/* Start server */
	res = Server->Start();
	if (!res) {
		std::cerr << "Server cannot be started" << std::endl;
		return 1;
	}

	/* Register signal */
	catch_signals();

	std::cout << "Waiting for connections" << std::endl;

	/* Keep waiting for connections */
	while (Server->Accept() && !interrupted);

	/* Quit */
	std::cout << "Terminating..." << std::endl;

	/* Stop server */
	Server->Stop();

	/* Delete server */
	delete Server;

	return 0;
}
