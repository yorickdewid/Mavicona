#ifndef LOCALENV_H
#define LOCALENV_H

#include <ctime>
#include <iostream>

namespace LocalEnv {
	
bool setupHome(const std::string& homedir) {
	std::ofstream hometest;
	hometest.open(homedir + "/.jobhome", std::ofstream::out);
	if (hometest.fail()) {
		std::cerr << homedir << " not writable" << std::endl;
		return false;
	}

	hometest.close();
	return true;
}

bool setupEnv(const std::string& homedir) {
	std::ifstream environ;
	environ.open(homedir + "/.env", std::ofstream::in);
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

bool teardown(const std::string& homedir) {
	std::ofstream lastrun;

	time_t t = time(NULL);
	struct tm *now = localtime(&t);

	lastrun.open(homedir + "/lastrun", std::ofstream::out);

	lastrun << (now->tm_year + 1900) << '-' 
		 << (now->tm_mon + 1) << '-'
		 << now->tm_mday << ' '
		 << now->tm_hour << '-'
		 << now->tm_min << '-'
		 << now->tm_sec
		 << std::endl;

	lastrun.close();
	return true;
}

}

#endif // LOCALENV_H
