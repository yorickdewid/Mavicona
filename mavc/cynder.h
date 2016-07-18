#ifndef CYNDER_H
#define CYNDER_H

#include "module.h"

class Cynder : public Module {
  public:
	Cynder() {}

	inline const std::string name() {
		return "Cynder";
	}

	inline const std::string description() {
		return "Controll to storage cluster";
	}

	bool runCommand(const std::string& command);
};

#endif // CYNDER_H
