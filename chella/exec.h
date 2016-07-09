#ifndef EXEC_H
#define EXEC_H

#include <iostream>
#include "controlclient.h"
#include "callback.h"
#include "art.h"

class Execute : public Callback {
	ControlClient *jobcontrol = nullptr;
	art_tree *cache = nullptr;

	Execute() {};

	~Execute() {
		if (this->cache)
			art_tree_destroy(this->cache);
	}

	Execute(Execute const&);
	void operator=(Execute const&);

	static Execute& getInstance() {
		static Execute instance;
		return instance;
	}

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

	void cachePut(const std::string& key, const std::string value);
	std::string cacheGet(const std::string& key);

	static void init(ControlClient *control) {
		Execute& exec = Execute::getInstance();
		exec.setControl(control);
	}

	static void run(const std::string& name, Parameter& param);
};

#endif // EXEC_H
