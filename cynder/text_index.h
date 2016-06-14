#ifndef TEXT_INDEX_H
#define TEXT_INDEX_H

#include "engine.h"
#include "additional_index.h"

class TextIndex : public AbstractEngine, public AbstractAdditionalIndex {
  public:
	TextIndex() : AbstractEngine("fti", true) {}

	void put(std::string quid, std::string key, std::string value, bool override = false) {
		if (quid.size() != quidpp::Quid::unpackedSize()) {
			std::cerr << "Invalid QUID concatenation" << std::endl;
		}

		if (!matchKeyword(value))
			return;
		
		std::cout << "Value quallifies for storage\n";
		AbstractEngine::put(key, quid + value, override);
	}

};

#endif // TEXT_INDEX_H
