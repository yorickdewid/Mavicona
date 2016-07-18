#include <string>
#include <iomanip>

#include "common/config.h"
#include "param_util.h"

namespace ParamUtil {

bool VerifyHbs(const std::string& name) {
	ConfigFile config(name);
	bool error = false;

	if (!config.exist("extract")) {
		std::cerr << "Must be at least 1 extractor listed" << std::endl;
		error = true;
		goto hbs_result;
	} else {
		std::cout << "Extractor at " << config.get<std::string>("extract", "") << std::endl;
	}

	if (!config.exist("pitch")) {
		std::cerr << "Must be at least 1 pitcher listed" << std::endl;
		error = true;
		goto hbs_result;
	} else {
		std::cout << "Pitcher at " << config.get<std::string>("extract", "") << std::endl;
	}

	if (!config.exist("cynder-master")) {
		std::cerr << "Must be at least 1 cynder master listed" << std::endl;
		error = true;
		goto hbs_result;
	} else {
		std::cout << "Cynder master at " << config.get<std::string>("cynder-master", "") << std::endl;
	}

	if (!config.exist("chella-master")) {
		std::cerr << "Must be at least 1 chella master listed" << std::endl;
		error = true;
		goto hbs_result;
	} else {
		std::cout << "Chella master at " << config.get<std::string>("chella-master", "") << std::endl;
	}

	if (config.exist("orchestrate")) {
		std::cout << "Orchestrator at " << config.get<std::string>("orchestrate", "") << std::endl;
	}

	if (config.exist("connect")) {
		std::cout << "Connector at " << config.get<std::string>("connect", "") << std::endl;
	}

	if (config.exist("cynder-worker")) {
		ConfigFile::range r = config.find("cynder-worker");
		for (ConfigFile::iterator i = r.first; i != r.second; i++) {
			std::cout << "cynder worker at " << i->second << " " << std::endl;
		}
	}

	if (config.exist("chella-worker")) {
		ConfigFile::range r = config.find("chella-worker");
		for (ConfigFile::iterator i = r.first; i != r.second; i++) {
			std::cout << "chella worker at " << i->second << " " << std::endl;
		}
	}

	if (config.exist("rpcmd_port")) {
		std::cout << "RPC command port: " << config.operator[]<int>("rpcmd_port") << " " << std::endl;
	}

hbs_result:
	std::cout << std::setfill('-') << std::setw(30) << ' ' << std::endl;
	if (error) {
		std::cerr << "HBS configuration: FAILED" << std::endl;
		return false;
	}

	std::cout << "HBS configuration: PASSED" << std::endl;
	return true;
}

}
