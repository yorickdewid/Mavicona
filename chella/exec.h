#ifndef EXEC_H
#define EXEC_H

#include <iostream>

class Execute {
  public:
	Execute() {}

	struct Parameter {
		unsigned int jobid;
		unsigned int jobpartition;
		std::string jobname;
		std::string jobquid;
		unsigned int workerid;
	};

	static void run(const std::string& name, Parameter& param);
};

#endif // EXEC_H
