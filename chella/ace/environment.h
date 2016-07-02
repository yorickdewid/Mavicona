#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <ctime>
#include <sstream>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>

namespace Ace {

class Environment {
	unsigned int workerid;

  public:
	Environment() {}

	std::string CurrentDirectory() {
		char cwd[1024];
		memset(cwd, '\0', 1024);
		getcwd(cwd, sizeof(cwd));

		return cwd;
	}

	inline std::string CacheDirectory() {
		return CurrentDirectory() + "/cache";
	}

	const std::string CurrentDateTime() {
		time_t now = time(0);
		struct tm tstruct;
		char buf[80];
		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

		return buf;
	}

	const std::string Hostname() {
		char hostname[HOST_NAME_MAX];
		gethostname(hostname, HOST_NAME_MAX);

		return hostname;
	}

	const std::string Login() {
		char username[LOGIN_NAME_MAX];
		getlogin_r(username, LOGIN_NAME_MAX);

		return username;
	}

	void SetWorkerIdent(unsigned int id) {
		workerid = id;
	}

	unsigned int WorkerIdent() {
		return workerid;
	}

};

}

#endif // ENVIRONMENT_H
