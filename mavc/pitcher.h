#ifndef PITCHER_H
#define PITCHER_H

#include "module.h"

class Pitcher : public Module {
  public:
	Pitcher() {
		// registerCommand("kaze", &kaze);
	}

	inline const std::string name() {
		return "Pitcher";
	}

	inline const std::string description() {
		return "Queue management";
	}

	bool runCommand(const std::string& command) {
		return true;
	}

	/* Module specific commands */
	void kaze() {
		puts("kaze");
	}
};

#endif // PITCHER_H
