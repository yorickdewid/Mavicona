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

};

#endif // CALLBACK_H
