#ifndef MAVC_SERVER_H
#define MAVC_SERVER_H

#include "module.h"

class ModServer : public IModule, public Command<ModServer> {
  public:
	ModServer() {
		registerCommand("start", "Start server", &ModServer::startServer);
	}

	inline const std::string name() const {
		return "Server";
	}

	inline const std::string description() const {
		return "Static file server";
	}

	void startServer(int paramc, const std::vector<std::string>& params);

	defaultModCommand();
};

#endif // MAVC_SERVER_H
