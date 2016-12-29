#ifndef MODULE_H
#define MODULE_H

#include <algorithm>
#include <map>
#include <iostream>
#include <sstream>
#include <vector>
#include <functional>
#include <exception>

#define defaultModCommand() \
	void exec(const std::string& command, const std::vector<std::string>& argv) { \
		Command::runCommand(command, argv); \
	} \
	void commandlist(std::function<void(const std::string&, const std::string&)> print) { \
		Command::foreachCommand(print); \
	}

struct UnknownCommand : public std::exception {
	const char *what() const throw () {
		return "Unknown module command";
	}
};

struct InvalidParameter : public std::exception {
	InvalidParameter(const char *msg) : m_msg("Invalid parameter: ") {
		m_msg.append(msg);
	}

	const char *what() const throw () {
		return m_msg.c_str();
	}

private:
	std::string m_msg;
};

struct TooFewParameters : public std::exception {
	TooFewParameters(int paramc) {
		std::stringstream ss;
		ss << "Too few parameters, expected ";
		ss << paramc;
		ss << " parameters";
		m_msg = ss.str().c_str();
	}

	const char *what() const throw () {
		return m_msg.c_str();
	}

private:
	std::string m_msg;
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

	virtual void exec(const std::string& command, const std::vector<std::string>& argv) = 0;
	virtual void commandlist(std::function<void(const std::string&, const std::string&)> print) = 0;
};

template<typename T>
class Command {
	typedef void (T::*VirtualFunc)(int, const std::vector<std::string>&);

	std::map<std::string, std::pair<std::string, VirtualFunc>> functions;

  public:
	virtual ~Command() {}

	virtual bool runCommand(const std::string& command, const std::vector<std::string>& argv) {
		if (functions.find(command) == functions.end())
			throw UnknownCommand();

		(dynamic_cast<T *>(this)->*(functions[command].second))(argv.size(), argv);
		return true;
	}

	virtual void foreachCommand(std::function<void(const std::string&, const std::string&)> callback) {
		for (auto abc : functions) {
			callback(abc.first, abc.second.first);
		}
	}

	virtual void registerCommand(const std::string& name, const std::string& desc, VirtualFunc f) final {
		functions[name] = std::make_pair(desc, f);
	}
};

#endif // MODULE_H
