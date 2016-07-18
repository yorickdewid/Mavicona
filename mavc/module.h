#ifndef MODULE_H
#define MODULE_H

#include <iostream>

class Module {
  public:
	virtual std::string name() = 0;
	virtual bool runCommand(const std::string& command) = 0;
	virtual ~Module() {}
};

#endif // MODULE_H
