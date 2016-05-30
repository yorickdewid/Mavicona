#ifndef CYNDER_H
#define CYNDER_H

#include "module.h"

class Cynder : public Module
{
  public:
    Cynder() {}

    inline std::string name() {
        return "Cynder";
    }

    bool runCommand(const std::string& command);
};

#endif // CYNDER_H
