#ifndef TEXT_INDEX_H
#define TEXT_INDEX_H

#include "engine.h"
#include "additional_index.h"

class TextIndex : public AbstractEngine, public AbstractAdditionalIndex {
  public:
	TextIndex() : AbstractEngine("fti", true) {}

	void put(std::string quid, std::string key, std::string value, bool override = false) {
		if (quid.size() != quidpp::Quid::unpackedSize()) {
			quid = quidpp::Quid::crop(quid);
			if (quid.empty()) {
				std::cerr << "Invalid QUID concatenation" << std::endl;
				return;
			}
		}

		if (!matchKeyword(value))
			return;
		
		std::cout << "Value quallifies for storage " << key << " " << value << std::endl;
		AbstractEngine::put(key, quid + value, override);
	}

};

#endif // TEXT_INDEX_H
