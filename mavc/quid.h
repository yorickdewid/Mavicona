#ifndef MAVC_QUID_H
#define MAVC_QUID_H

#include "module.h"

class ModQuid : public IModule, public Command<ModQuid> {
  public:
	ModQuid() {
		registerCommand("generate", "Create new task", &ModQuid::generateQuid);
	}

	inline const std::string name() const {
		return "Quid";
	}

	inline const std::string description() const {
		return "Quantica unique random identifiers";
	}

	void generateQuid(int paramc, const std::vector<std::string>& params);

	defaultModCommand();
};

#endif // MAVC_QUID_H
