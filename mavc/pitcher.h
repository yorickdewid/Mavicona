#ifndef PITCHER_H
#define PITCHER_H

#include "module.h"

class Pitcher : public IModule, public Command<Pitcher> {
  public:
	Pitcher() {
		registerCommand("kaze", "The kaze command", &Pitcher::kaze);
		registerCommand("wurst", "The wurst command", &Pitcher::wurst);
		registerCommand("newjob", "Create new job", &Pitcher::newJob);
	}

	inline const std::string name() const {
		return "Pitcher";
	}

	inline const std::string description() const {
		return "Queue management";
	}

	void exec(const std::string& command) {
		Command::runCommand(command);
	}

	void commandlist(std::function<void(const std::string&, const std::string&)> print) {
		Command::foreachCommand(print);
	}

	/* Module specific commands */
	void newJob(int paramc, char *param[]);
	void kaze(int paramc, char *param[]);
	void wurst(int paramc, char *param[]);
};

#endif // PITCHER_H
