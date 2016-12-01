#ifndef LOCALENV_H
#define LOCALENV_H

#include <ctime>
#include <iostream>

#include <string.h>
#include "error.h"

class LocalEnv {
	std::string m_homedir;
	int m_jobid;

  public:
	LocalEnv(const std::string& homedir, int jobid) : m_homedir(homedir), m_jobid(jobid) {
	}

	enum StreamType {
		STDOUT = 1,
		STDERR = 2,
	};

	bool setupHome() {
		std::ofstream hometest;
		hometest.open(m_homedir + "/.jobhome", std::ofstream::out);
		if (hometest.fail()) {
			std::cerr << m_homedir << " not writable" << std::endl;
			return false;
		}
		hometest.close();

		std::ofstream tab;
		tab.open(m_homedir + "/.inittab");
		if (tab.fail()) {
			std::cerr << m_homedir << " not writable" << std::endl;
			return false;
		}
		tab.close();

		std::ofstream lock;
		lock.open(m_homedir + "/MXLOCK");
		if (lock.fail()) {
			std::cerr << m_homedir << " not writable" << std::endl;
			return false;
		}
		lock << m_jobid;
		lock.close();

		//TODO: check if directories already exist
		mkdir((m_homedir + "/run").c_str(), 0700);
		mkdir((m_homedir + "/tmp").c_str(), 0700);
		return true;
	}

	bool setupEnv() {
		std::ifstream environ;
		environ.open(m_homedir + "/.env", std::ofstream::in);
		if (environ.fail()) {
			environ.clear();
			return true;
		}

		std::string line;
		while (std::getline(environ, line)) {
			size_t pos = line.find("=");
			auto key = line.substr(0, pos);
			auto value = line.substr(pos + 1);

			setenv(key.c_str(), value.c_str(), 1);
		}

		environ.close();
		return true;
	}

	FileLogger *openStream(enum StreamType type) {
		std::string logfile = m_homedir + "/run/" + std::to_string(m_jobid) + "_outstd" + std::to_string(type);
		auto log = new FileLogger(logfile, false);
		log->disableTimestamp();
		log->disableEcho();
		return log;
	}

	bool teardown() {
		std::ofstream lastrun, lastjobid;

		time_t t = time(NULL);
		struct tm *now = localtime(&t);

		lastrun.open(m_homedir + "/lastrun", std::ofstream::out);
		lastjobid.open(m_homedir + "/lastjobid", std::ofstream::out);

		lastrun << (now->tm_year + 1900) << '-' 
			 << (now->tm_mon + 1) << '-'
			 << now->tm_mday << ' '
			 << now->tm_hour << '-'
			 << now->tm_min << '-'
			 << now->tm_sec
			 << std::endl;

		lastjobid << m_jobid << std::endl;

		lastjobid.close();
		lastrun.close();
		return true;
	}
};

#endif // LOCALENV_H
