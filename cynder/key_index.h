#ifndef KEY_INDEX_H
#define KEY_INDEX_H

#include "engine.h"
#include "additional_index.h"

class KeyIndex : public AbstractEngine, public AbstractAdditionalIndex {
  public:
	KeyIndex() : AbstractEngine("uki", true) {}

	void put(std::string quid, std::string key, std::string value, bool override = false) {
		if (quid.size() != quidpp::Quid::unpackedSize()) {
			std::cerr << "Invalid QUID concatenation" << std::endl;
		}

		if (!matchKeyword(key))
			return;
		
		std::cout << "Key quallifies for storage\n";
		AbstractEngine::put(key, quid + value, override);
	}

};

#endif // KEY_INDEX_H
