#ifndef QUID_H
#define QUID_H

#include "module.h"

class Quid : public IModule, public Command<Quid> {
  public:
	Quid() {
		registerCommand("generate", "Create new task", &Quid::generateQuid);
	}

	inline const std::string name() const {
		return "Quid";
	}

	inline const std::string description() const {
		return "Quantica unique random identifiers";
	}

	void exec(const std::string& command, const std::vector<std::string>& argv) {
		Command::runCommand(command, argv);
	}

	void commandlist(std::function<void(const std::string&, const std::string&)> print) {
		Command::foreachCommand(print);
	}

	void generateQuid(int paramc, const std::vector<std::string>& params) {
		std::cout << "Run generateQuid, paramc: " << paramc << std::endl;
		if (paramc > 0)
			std::cout << "Argv0: " << params[0] << std::endl;
	}
};

#endif // QUID_H
