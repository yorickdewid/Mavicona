#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unistd.h>
#include <stdio.h>
#include <memory.h>

namespace Ace {

class Environment {
  public:
	Environment() {}

	std::string CurrentDirectory() {
		char cwd[1024];
		memset(cwd, '\0', 1024);
		getcwd(cwd, sizeof(cwd));

		return cwd;
	}

	std::string CacheDirectory() {
		return CurrentDirectory() + "/cache";
	}
};

}

#endif // ENVIRONMENT_H
