#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <vector>
#include <csignal>
#include <iomanip>
#include <exception>

#include "common/util.h"
#include "common/cxxopts.h"
#include "param_util.h"
#include "cynder.h"
#include "pitcher.h"

static bool interrupted = false;
static std::vector<IModule *> moduleList;
static IModule *currentModule = nullptr;

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

struct UnknownModule : public std::exception {
	const char *what() const throw () {
		return "Unknown module";
	}
};

static void runModuleCommand(const std::string& command) {
	if (!currentModule)
		throw UnknownCommand();

	currentModule->exec(command);
}

static bool findModule(const std::string& modname) {
	for (const auto module : moduleList) {
		if (!modname.compare(module->modName())) {
			currentModule = module;
			return true;
		}
	}

	return false;
}

template<typename T>
void printHelpElement(T name, T desc, const int& width = 20) {
	std::cout << std::left << std::setw(width) << std::setfill(' ') << name;
	std::cout << std::left << std::setw(width) << std::setfill(' ') << desc;
	std::cout << std::endl;
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
		std::cout << "Console:" << std::endl;

		printHelpElement("  <module>", "Switch to module");
		printHelpElement("  help", "This console help");
		std::cout << std::endl;

		/* Print functions per module */
		for (const auto module : moduleList) {
			printHelpElement(module->name(), module->description());
			module->commandlist([](const std::string & name, const std::string & desc) {
				printHelpElement("  " + name, desc);
			});

			std::cout << std::endl;
		}

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

	std::cout << "Type 'help' to get started" << std::endl;
	std::cout << shellState();

	std::string input;
	while (std::getline(std::cin, input)) {
		if (input.empty())
			continue;

		/* Evaluate command */
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