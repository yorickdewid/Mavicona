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
	virtual void sessionCleanup() = 0;
};

#endif // CALLBACK_H
