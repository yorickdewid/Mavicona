#ifndef CHELLA_H
#define CHELLA_H

#include "module.h"

class ModChella : public IModule, public Command<ModChella> {
  public:
	ModChella() {
		registerCommand("submit", "Submit raw job to cluster", &ModChella::sumbitJob);
	}

	inline const std::string name() const {
		return "Chella";
	}

	inline const std::string description() const {
		return "Job processing cluster";
	}

	void sumbitJob(int paramc, const std::vector<std::string>& params);

	defaultModCommand();
};

#endif // CHELLA_H
