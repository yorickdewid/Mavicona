#ifndef MODULE_H
#define MODULE_H

#include <algorithm>
#include <iostream>

class Module {
  public:
	virtual const std::string name() = 0;
	virtual bool runCommand(const std::string& command) = 0;
	// virtual void registerCommand(const std::string& command) = 0;
	virtual ~Module() {}
	const std::string modName() {
		std::string name = this->name();
		std::transform(name.begin(), name.end(), name.begin(), tolower);
		return name;
	}
};

#endif // MODULE_H
