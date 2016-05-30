#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <iomanip>
#include <exception>

#include "common/util.h"
#include "common/config.h"
#include "common/cxxopts.h"
#include "cyndr.h"

struct UnknownCommand : public std::exception {
    const char *what() const throw () {
        return "Unknown command";
    }
};

static enum Modules {
    GENERAL,
    CYNDR,
    CHELLA,
    PITCHER,
    EXTRACTOR,
    SCRAPER
} activeMod = GENERAL;

void runModuleCommand(const std::string& command) {
    Module *mod = NULL;
    switch (activeMod) {
        case CYNDR:
            mod = new Cyndr();
            break;
        case CHELLA:
        case PITCHER:
        case EXTRACTOR:
        case SCRAPER:
        case GENERAL:
            throw UnknownCommand();
    }

    if (!mod->runCommand(command))
        throw UnknownCommand();
}

void eval(std::string& command) {
    if (command == "\\q" || command == "exit") {
        exit(0);
    } else if (command == "cyndr") {
        activeMod = CYNDR;
    } else if (command == "chella") {
        activeMod = CHELLA;
    } else if (command == "pitcher") {
        activeMod = PITCHER;
    } else if (command == "extractor") {
        activeMod = EXTRACTOR;
    } else if (command == "scraper") {
        activeMod = SCRAPER;
    } else if (command == "all" || command == ".." || command == "general") {
        activeMod = GENERAL;
    } else {
        std::transform(command.begin(), command.end(), command.begin(), tolower);
        runModuleCommand(command);
    }
}

std::string shellState() {
    switch (activeMod) {
        case CYNDR:
            return "mavc:cyndr> ";
        case CHELLA:
            return "mavc:chella> ";
        case PITCHER:
            return "mavc:pitcher> ";
        case EXTRACTOR:
            return "mavc:extractor> ";
        case SCRAPER:
            return "mavc:scraper> ";
        case GENERAL:
        default:
            return "mavc> ";
    }
}

void shellLoop() {
    while (true) {
        try {
            std::string command;
            std::cout << shellState();
            std::cin >> command;
            eval(command);
        } catch(UnknownCommand& e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

void VerifyHbs(const std::string& name) {
    ConfigFile config(name);
    bool error = false;

    if (!config.exist("extract")){
        std::cerr << "Must be at least 1 extractor listed" << std::endl;
        error = true;
        goto hbs_result;
    } else {
        std::cout << "Extractor at " << config.get<std::string>("extract", "") << std::endl;
    }
    
    if (!config.exist("pitch")) {
        std::cerr << "Must be at least 1 pitcher listed" << std::endl;
        error = true;
        goto hbs_result;
    } else {
        std::cout << "Pitcher at " << config.get<std::string>("extract", "") << std::endl;
    }

    if (!config.exist("cynder-master")) {
        std::cerr << "Must be at least 1 cynder master listed" << std::endl;
        error = true;
        goto hbs_result;
    } else {
        std::cout << "Cynder master at " << config.get<std::string>("cynder-master", "") << std::endl;
    }

    if (!config.exist("chella-master")) {
        std::cerr << "Must be at least 1 chella master listed" << std::endl;
        error = true;
        goto hbs_result;
    } else {
        std::cout << "Chella master at " << config.get<std::string>("chella-master", "") << std::endl;
    }

    if (config.exist("orchestrate")) {
        std::cout << "Orchestrator at " << config.get<std::string>("orchestrate", "") << std::endl;
    }

    if (config.exist("connect")) {
        std::cout << "Connector at " << config.get<std::string>("connect", "") << std::endl;
    }

    if (config.exist("cynder-worker")) {
        ConfigFile::range r = config.find("cynder-worker");
        for(ConfigFile::iterator i = r.first; i != r.second; i++) {
            std::cout << "cynder worker at " << i->second << " " << std::endl;
        }
    }

    if (config.exist("chella-worker")) {
        ConfigFile::range r = config.find("chella-worker");
        for(ConfigFile::iterator i = r.first; i != r.second; i++) {
            std::cout << "chella worker at " << i->second << " " << std::endl;
        }
    }

    if (config.exist("rpcmd_port")) {
        std::cout << "RPC command port: " <<config.operator[]<int>("rpcmd_port")<< " " << std::endl;
    }

hbs_result:
    std::cout << std::setfill('-') << std::setw(30) << ' ' << std::endl;
    if (error) {
        std::cerr << "HBS configuration: FAILED" << std::endl;
    } else {
        std::cout << "HBS configuration: PASSED" << std::endl;
    }
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

        VerifyHbs(name);
        return 0;
    }

    shellLoop();

    return 0;
}

