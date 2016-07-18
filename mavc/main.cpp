#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <vector>
#include <csignal>
#include <exception>

#include "common/util.h"
#include "common/cxxopts.h"
#include "param_util.h"
#include "cynder.h"
#include "pitcher.h"

static bool interrupted = false;
static std::vector<Module *> moduleList;
static Module *currentModule = nullptr;

void signal_handler(int signum) {
	interrupted = true;
}

static void catch_signals() {
	struct sigaction action;
	action.sa_handler = signal_handler;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
}

struct UnknownCommand : public std::exception {
	const char *what() const throw () {
		return "Unknown command or module";
	}
};

static void runModuleCommand(const std::string& command) {
	if (!currentModule)
		throw UnknownCommand();

	if (!currentModule->runCommand(command))
		throw UnknownCommand();
}

static bool findModule(const std::string& modname) {
	for (const auto module : moduleList) {
		if (!modname.compare(module->modName())) {
			std::cout << "Switched to module " << module->name() << std::endl;
			currentModule = module;
			return true;
		}
	}

	return false;
}

static void eval(std::string& command) {
	std::transform(command.begin(), command.end(), command.begin(), tolower);

	/* Exit loop */
	if (command == "\\q" || command == "exit" || command == "quit") {
		interrupted = true;
		return;
	}

	/* Show help */
	if (command == "help" || command == "?") {
		std::cout << "Console:\n";
		std::cout << "  <module>\tSwitch to module\n";
		std::cout << "  help\t\tThis console help\n";
		std::cout << "  exit\t\tExit console\n";
		std::cout << std::endl;
		return;
	}

	if (findModule(command))
		return;

	try {
		runModuleCommand(command);
	} catch (UnknownCommand& e) {
		std::cerr << e.what() << std::endl;
	}
}

static std::string shellState() {
	if (currentModule)
		return "mavc:" + currentModule->modName() + "> ";

	return "mavc> ";
}

void loadModules() {
	moduleList.push_back(new Cynder());
	moduleList.push_back(new Pitcher());
}

void unloadModules() {
	for (const auto module : moduleList) {
		delete module;
	}
}

void runShellLoop() {
	loadModules();

	catch_signals();

	std::cout << shellState();

	std::string input;
	while (std::getline(std::cin, input)) {
		eval(input);

		if (interrupted)
			break;

		std::cout << shellState();
	}

	unloadModules();
}

int main(int argc, char *argv[]) {

	cxxopts::Options options(argv[0], "");

	options.add_options("Help")
	("s,hbs", "Verify Host based service config", cxxopts::value<std::string>(), "FILE")
	("h,help", "Print this help");

	try {
		options.parse_positional("positional");
		options.parse(argc, argv);
	} catch (const cxxopts::OptionException& e) {
		std::cerr << "error parsing options: " << e.what() << std::endl;
		return 1;
	}

	if (options.count("help")) {
		std::cerr << options.help({"Help"}) << std::endl;
		return 0;
	}

	if (options.count("hbs")) {
		std::string name = options["hbs"].as<std::string>();
		if (!file_exist(name)) {
			std::cerr << "error: " << name << ": No such file or directory" << std::endl;
			return 1;
		}

		return ParamUtil::VerifyHbs(name) ? 0 : 1;
	}

	runShellLoop();

	std::cout << "Cheers" << std::endl;

	return 0;
}
