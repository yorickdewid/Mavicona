#ifndef MODULE_H
#define MODULE_H

#include <algorithm>
#include <map>
#include <iostream>
#include <functional>
#include <exception>

struct UnknownCommand : public std::exception {
	const char *what() const throw () {
		return "Unknown module command";
	}
};

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
	virtual void commandlist(std::function<void(const std::string&, const std::string&)> print) = 0;
};

template<typename T>
class Command {
	typedef void (T::*cmd_t)(int, char *argv[]);

	std::map<std::string, std::pair<std::string, cmd_t>> functions;

  public:
	virtual ~Command() {}

	virtual bool runCommand(const std::string& command) {
		if (functions.find(command) == functions.end())
			throw UnknownCommand();

		char *pz = nullptr;
		(dynamic_cast<T *>(this)->*(functions[command].second))(0, &pz);
		return true;
	}

	virtual void foreachCommand(std::function<void(const std::string&, const std::string&)> callback) {
		for (auto abc : functions) {
			callback(abc.first, abc.second.first);
		}
	}

	virtual void registerCommand(const std::string& name, const std::string& desc, cmd_t f) final {
		functions[name] = std::make_pair(desc, f);
	}
};

#endif // MODULE_H
