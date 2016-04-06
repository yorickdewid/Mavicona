#include <iostream>
#include <cstdlib>
#include <signal.h>

#include "http.h"
#include "log.h"
#include "server.h"

/* Global variables */
static CServer *Server = nullptr;
static const unsigned short port = 5500;

void Quit(int dummy) {
	CLog::Print("Terminating...");

	/* Stop server */
	Server->Stop();

	/* Delete server */
	delete Server;

	/* Exit */
	exit(0);
}

int main(int argc, char **argv) {
	int res;

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

	/* Keep waiting for connections */
	while (Server->Accept());

	/* Quit */
	Quit(0);

	return 0;
}
