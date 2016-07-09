#ifndef CALLBACK_H
#define CALLBACK_H

#include <iostream>

struct Callback {
	unsigned int jobid;
	unsigned int jobpartition;
	unsigned int workerid;
	unsigned int clusterjobs;
	unsigned short progress;
	std::string jobname;
	std::string jobquid;
	std::string module;

	virtual void updateProgress(unsigned short progress) = 0;

	/* Caching interface */
	virtual void cachePut(const std::string& key, const std::string value) = 0;
	virtual void cacheDelete(const std::string& key) = 0;
	virtual std::string cacheGet(const std::string& key) = 0;
};

#endif // CALLBACK_H
