#ifndef LOCALENV_H
#define LOCALENV_H

#include <string.h>
#include "../common/logger.h"

class LocalEnv {
	std::string m_homedir;
	int m_jobid;

  public:
	LocalEnv(const std::string& homedir, int jobid) : m_homedir(homedir), m_jobid(jobid) {}

	enum StreamType {
		STDOUT = 1,
		STDERR = 2,
	};

	bool setupHome();
	bool setupEnv();
	std::string packageMain();
	std::string packageInvoke();

	inline bool hasHome() {
		return file_exist(m_homedir + "/.jobhome");
	}

	FileLogger *openStream(enum StreamType type) {
		std::string logfile = m_homedir + "/run/" + std::to_string(m_jobid) + "_outstd" + std::to_string(type);
		auto log = new FileLogger(logfile, false);
		log->disableTimestamp();
		log->disableEcho();
		return log;
	}

	bool teardown(double runtime);
};

#endif // LOCALENV_H
