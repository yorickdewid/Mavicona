#ifndef CATALOGUS_H
#define CATALOGUS_H

#include "engine.h"

class Catalogus : public AbstractEngine {

  public:
	Catalogus() : AbstractEngine("cat") {}

	void put(std::string key, std::string value, bool override = false) {
		AbstractEngine::put(key, value, override);
	}

	void putVersionCount(const char *prefix, unsigned int i) {
		assert(strlen(prefix) == 3);

		AbstractEngine::put(std::string(prefix) + "_count", std::to_string(i), true);
	}

	unsigned int getVersionCount(const char *prefix) {
		assert(strlen(prefix) == 3);

		std::string version = "0";

		try {
			version = AbstractEngine::get(std::string(prefix) + "_count");
		} catch (upscaledb::error &error) {
			if (error.get_errno() == UPS_KEY_NOT_FOUND) {
				return 0;
			}
		}

		return atoi(version.c_str());
	}

};

#endif // CATALOGUS_H
