#ifndef CYNDER_H
#define CYNDER_H

#include "module.h"

class Cynder : public IModule {
  public:
	Cynder() {}

	inline const std::string name() const {
		return "Cynder";
	}

	inline const std::string description() const {
		return "Control to storage cluster";
	}

	void exec(const std::string& command) {

	}

	void commandlist(std::function<void(const std::string&, const std::string&)> print) {

	}
};

#endif // CYNDER_H
