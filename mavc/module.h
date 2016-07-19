#ifndef MODULE_H
#define MODULE_H

#include <algorithm>
#include <map>
#include <iostream>
#include <functional>

class IModule {
  public:
	virtual const std::string name() const = 0;
	virtual const std::string description() const = 0;
	virtual ~IModule() {}
	const std::string modName() const {
		std::string name = this->name();
		std::transform(name.begin(), name.end(), name.begin(), tolower);
		return name;
	}

	virtual void exec(const std::string& command) = 0;
};

template<typename T>
class Command {
	typedef void (T::*cmd_t)(int, char *argv[]);

	std::map<std::string, cmd_t> functions;

  public:
	virtual ~Command() {}

	virtual bool runCommand(const std::string& command) {
		if (functions.find(command) == functions.end())
			return false;

		char *pz = nullptr;
		(dynamic_cast<T *>(this)->*functions[command])(1, &pz);
		return true;
	}

	virtual void registerCommand(const std::string& fname, cmd_t f) final {
		std::cout << "Adding " << fname << std::endl;
		functions[fname] = f;
	}
};

#endif // MODULE_H
