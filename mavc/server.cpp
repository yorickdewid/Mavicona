#include <vector>
#include "server.h"

/* Module specific commands */
void ModServer::startServer(int paramc, const std::vector<std::string>& params) {
	int port = 0;
	std::string directory;
	if (paramc < 2)
		throw TooFewParameters(2);

	try {
		port = std::stoi(params[0]);
	} catch (std::invalid_argument&) {
		throw InvalidParameter("Not an integer");
	}

	directory = std::stoi(params[1]);
}
