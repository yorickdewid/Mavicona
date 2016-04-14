#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <exception>

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
            return "mavc:cyndr>";
        case CHELLA:
            return "mavc:chella>";
        case PITCHER:
            return "mavc:pitcher>";
        case EXTRACTOR:
            return "mavc:extractor>";
        case SCRAPER:
            return "mavc:scraper>";
        case GENERAL:
        default:
            return "mavc>";
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

int main() {
    shellLoop();

    return 0;
}

