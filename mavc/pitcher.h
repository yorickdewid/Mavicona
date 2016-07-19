#ifndef PITCHER_H
#define PITCHER_H

#include "module.h"

class Pitcher : public IModule, public Command<Pitcher> {
  public:
	Pitcher() {
		registerCommand("kaze", &Pitcher::kaze);
		registerCommand("wurst", &Pitcher::wurst);
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

	/* Module specific commands */
	void kaze(int paramc, char *param[]) {
		puts("kaze");
	}

	void wurst(int paramc, char *param[]) {
		puts("wurst");
	}
};

#endif // PITCHER_H
