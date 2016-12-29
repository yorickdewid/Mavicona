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
#include "chella.h"
#include "quid.h"
#include "server.h"

#define LoadMod(m) moduleList.push_back(new Mod##m());

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

static void runModuleCommand(const std::string& commandline) {
	std::stringstream ss;
	ss.str(commandline);
	std::string command;
	std::vector<std::string> argv;

	if (!currentModule)
		throw UnknownCommand();

	std::string item;
	while (std::getline(ss, item, ' ')) {
		if (command.empty()) {
			command = item;
			continue;
		}

		argv.push_back(item);
	}

	try {
		currentModule->exec(command, argv);
	} catch (TooFewParameters& e) {
		std::cerr << e.what() << std::endl;
	} catch (InvalidParameter& e) {
		std::cerr << e.what() << std::endl;
	}
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
		if (!currentModule) {
			std::cout << "Loaded modules:" << std::endl;

			for (const auto module : moduleList) {
				printHelpElement("  " + module->name(), module->description());
			}

			std::cout << std::endl;
		
			return;
		}

		/* Print functions per module */
		printHelpElement(currentModule->name(), currentModule->description());
		currentModule->commandlist([](const std::string& name, const std::string& desc) {
			printHelpElement("  " + name, desc);
		});

		std::cout << std::endl;

		return;
	}

	/* Move to main */
	if (command == ".." || command == "main") {
		currentModule = nullptr;
		return;
	}

	if (command == "." || command == "module") {
		std::cout << "Current module: ";
		if (currentModule)
			std::cout << currentModule->name();
		else
			std::cout << "(main)";
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
	LoadMod(Cynder);
	LoadMod(Pitcher);
	LoadMod(Chella);
	LoadMod(Quid);
	LoadMod(Server);
}

void unloadModules() {
	for (const auto module : moduleList)
		delete module;
}

void runShellLoop(const std::string& startmod = "", std::string startcmd = "") {
	loadModules();

	catch_signals();

	if (!startmod.empty())
		findModule(startmod);

	if (!startcmd.empty()) {
		eval(startcmd);
		return;
	}

	std::cout << "Type 'help' to get started" << std::endl;
	std::cout << shellState();

	std::string input;
	while (std::getline(std::cin, input)) {
		if (input.empty()) {
			std::cout << shellState();
			continue;
		}

		/* Evaluate command */
		eval(input);

		if (interrupted)
			break;

		std::cout << shellState();
	}

	unloadModules();
}

int main(int argc, char *argv[]) {

	std::string startmod, startcmd;
	cxxopts::Options options(argv[0], "");

	options.add_options("Help")
	("s,hbs", "Verify Host based service config", cxxopts::value<std::string>(), "FILE")
	("m,module", "Start with module", cxxopts::value<std::string>(), "module")
	("c,command", "Run command", cxxopts::value<std::string>(), "command args")
	("h,help", "Print this help");

	options.add_options()
	("positional", "&", cxxopts::value<std::vector<std::string>>());

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

	if (options.count("module")) {
		startmod = options["module"].as<std::string>();
	}

	if (options.count("command")) {
		startcmd = options["command"].as<std::string>();

		if (options.count("positional")) {
			auto args = options["positional"].as<std::vector<std::string>>();
			for (const auto& f : args) {
				startcmd += " " + f;
			}
		}
	}

	if (options.count("hbs")) {
		std::string name = options["hbs"].as<std::string>();
		if (!file_exist(name.c_str())) {
			std::cerr << "error: " << name << ": No such file or directory" << std::endl;
			return 1;
		}

		return ParamUtil::VerifyHbs(name) ? 0 : 1;
	}

	runShellLoop(startmod, startcmd);

	return 0;
}
