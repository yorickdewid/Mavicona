#ifndef PITCHER_H
#define PITCHER_H

#include "module.h"

class ModPitcher : public IModule { // , public Command<ModPitcher>
  public:
	ModPitcher() {
		// registerCommand("newtask", "Create new task", &ModPitcher::newTask);
	}

	inline const std::string name() const {
		return "Pitcher";
	}

	inline const std::string description() const {
		return "Job queue management";
	}

	void exec(const std::string& command, const std::vector<std::string>& argv) {
	// 	Command::runCommand(command, argv);
	}

	void commandlist(std::function<void(const std::string&, const std::string&)> print) {
	// 	Command::foreachCommand(print);
	}

	/* Module specific commands */
	void newTask(int paramc, char *param[]);
};

#endif // PITCHER_H
