#include "cyndr.h"

/*Cyndr::Cyndr() {
    std::cout << "kaas\n";
}*/

bool Cyndr::runCommand(const std::string& command) {
    if (command == "addnode") {
        std::cout << "New node added to Cyndr cluster" << std::endl;
        return true;
    }

    return false;
}
