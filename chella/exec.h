#ifndef EXEC_H
#define EXEC_H

#include <iostream>
#include "controlclient.h"

class Execute {
  public:
	Execute() {}

	struct Parameter {
		unsigned int jobid;
		unsigned int jobpartition;
		std::string jobname;
		std::string jobquid;
	};

	static void run(const std::string& name, Parameter& param, ControlClient& control);
};

#endif // EXEC_H
