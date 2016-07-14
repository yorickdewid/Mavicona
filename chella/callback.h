#ifndef CALLBACK_H
#define CALLBACK_H

#include <iostream>

struct Callback {
	enum JobState {
		SPAWN,
		PARTITION,
		FUNNEL
	};

	unsigned int jobid;
	unsigned int jobpartition;
	unsigned int jobpartition_count;
	unsigned int workerid;
	unsigned int clusterjobs;
	unsigned short progress;
	std::string jobname;
	std::string jobquid;
	std::string jobparent;
	std::string module;
	JobState jobstate;

	virtual void updateProgress(unsigned short progress) = 0;

	/* Caching interface */
	virtual void cachePut(const std::string& key, const std::string value) = 0;
	virtual void cacheDelete(const std::string& key) = 0;
	virtual std::string cacheGet(const std::string& key) = 0;

	/* SQL interface */
	virtual void sqlConnect(const std::string& rdbms, const std::string& database, const std::string& user, const std::string& password) = 0;
	virtual void sqlQuery(const std::string& query) = 0;
	virtual void sqlDisconnect() = 0;
};

#endif // CALLBACK_H
