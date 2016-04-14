#ifndef CYNDR_H
#define CYNDR_H

#include "module.h"

class Cyndr : public Module
{
  public:
    Cyndr() {}

    inline std::string name() {
        return "Cyndr";
    }

    bool runCommand(const std::string& command);
};

#endif // CYNDR_H
