#ifndef CYNDER_H
#define CYNDER_H

#include "module.h"

class ModCynder : public IModule {
  public:
	ModCynder() {}

	inline const std::string name() const {
		return "Cynder";
	}

	inline const std::string description() const {
		return "Control to storage cluster";
	}

	void exec(const std::string& command, const std::vector<std::string>& argv) {

	}

	void commandlist(std::function<void(const std::string&, const std::string&)> print) {

	}
};

#endif // CYNDER_H
