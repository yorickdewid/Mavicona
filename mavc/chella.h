#ifndef CHELLA_H
#define CHELLA_H

#include "module.h"

class Chella : public IModule {
  public:
	Chella() {}

	inline const std::string name() const {
		return "Chella";
	}

	inline const std::string description() const {
		return "Job processing cluster";
	}

	void exec(const std::string& command, const std::vector<std::string>& argv) {

	}

	void commandlist(std::function<void(const std::string&, const std::string&)> print) {

	}
};

#endif // CHELLA_H
