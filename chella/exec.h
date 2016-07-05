#ifndef EXEC_H
#define EXEC_H

#include <iostream>
#include "controlclient.h"
#include "callback.h"

class Execute : public Callback {
	ControlClient *jobcontrol = nullptr;

	Execute() {};

	Execute(Execute const&);
	void operator=(Execute const&);

  public:
	struct Parameter {
		unsigned int jobid;
		unsigned int jobpartition;
		std::string jobname;
		std::string jobquid;
	};

	void setControl(ControlClient *control) {
		jobcontrol = control;
	}

	void updateProgress(unsigned short progress) {
		jobcontrol->updateStateRunning(progress);
	}

	static Execute& getInstance() {
		static Execute instance;
		return instance;
	}

	static void init(ControlClient *control) {
		Execute& exec = Execute::getInstance();
		exec.setControl(control);
	}

	static void run(const std::string& name, Parameter& param);
};

#endif // EXEC_H
