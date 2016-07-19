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
};

#endif // CYNDER_H
