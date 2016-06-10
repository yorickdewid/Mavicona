#ifndef CATALOGUS_H
#define CATALOGUS_H

#include "engine.h"

class Catalogus : public AbstractEngine {

  public:
	Catalogus() : AbstractEngine("cat") {}

	void put(std::string key, std::string value, bool override = false) {
		AbstractEngine::put(key, value, override);
	}

	void put(std::string key, unsigned int i) {
		AbstractEngine::put(key, std::to_string(i), true);
	}

};

#endif // CATALOGUS_H
