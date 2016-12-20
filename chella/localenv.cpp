#include <ctime>
#include <iostream>
#include <vector>
#include <unistd.h>
#include "../common/util.h"
#include "../common/json.h"
#include "localenv.h"

bool LocalEnv::setupHome() {
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

	//TODO: check if directories already exist
	mkdir((m_homedir + "/run").c_str(), 0700);
	mkdir((m_homedir + "/tmp").c_str(), 0700);
	return true;
}

void LocalEnv::setLock() {
	std::ofstream lock;
	lock.open(m_homedir + "/MXLOCK");
	if (lock.fail()) {
		std::cerr << m_homedir << " not writable" << std::endl;
		return;
	}
	lock << m_jobid;
	lock.close();
}

bool LocalEnv::setupEnv() {
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

	setenv("WORKERID", std::to_string(m_workerid).c_str(), 1);

	environ.close();
	return true;
}

std::string LocalEnv::packageMain() {
	std::ifstream package;
	package.open(m_homedir + "/package.json", std::ofstream::in);
	if (package.fail()) {
		return "";
	}

	std::string str((std::istreambuf_iterator<char>(package)),
		std::istreambuf_iterator<char>());

	auto object = nlohmann::json::parse(str);
	if (!object.is_object())
		return "";

	return object["meta"]["main"].get<std::string>();
}

std::string LocalEnv::packageInvoke() {
	std::ifstream package;
	package.open(m_homedir + "/package.json", std::ofstream::in);
	if (package.fail()) {
		return "";
	}

	std::string str((std::istreambuf_iterator<char>(package)),
		std::istreambuf_iterator<char>());

	auto object = nlohmann::json::parse(str);
	if (!object.is_object())
		return "";

	return object["meta"]["invoke"].get<std::string>();
}

bool LocalEnv::teardown(double runtime) {
	std::ofstream lastrun, lastjobid, lastruntime;

	time_t t = time(NULL);
	struct tm *now = localtime(&t);

	/* Last job */
	lastrun.open(m_homedir + "/lastrun", std::ofstream::out);
	lastjobid.open(m_homedir + "/lastjobid", std::ofstream::out);
	lastruntime.open(m_homedir + "/lastruntime", std::ofstream::out);

	lastrun << (now->tm_year + 1900) << '-' 
		 << (now->tm_mon + 1) << '-'
		 << now->tm_mday << ' '
		 << now->tm_hour << ':'
		 << now->tm_min << ':'
		 << now->tm_sec
		 << std::endl;

	lastjobid << m_jobid << std::endl;

	lastruntime << runtime << std::endl;

	lastruntime.close();
	lastjobid.close();
	lastrun.close();

	/* Per job results */
	std::ofstream jobrun, jobruntime;

	jobrun.open(m_homedir + "/run/" + std::to_string(m_jobid) + "_runat", std::ofstream::out);
	jobruntime.open(m_homedir + "/run/" + std::to_string(m_jobid) + "_runtime", std::ofstream::out);

	jobrun << (now->tm_year + 1900) << '-' 
		 << (now->tm_mon + 1) << '-'
		 << now->tm_mday << ' '
		 << now->tm_hour << ':'
		 << now->tm_min << ':'
		 << now->tm_sec
		 << std::endl;

	jobruntime << runtime << std::endl;

	jobruntime.close();
	jobrun.close();

	unlink((m_homedir + "/MXLOCK").c_str());
	return true;
}
