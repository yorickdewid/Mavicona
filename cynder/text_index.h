#ifndef TEXT_INDEX_H
#define TEXT_INDEX_H

#include "engine.h"
#include "additional_index.h"

class TextIndex : public AbstractEngine, public AbstractAdditionalIndex {
  public:
	TextIndex(unsigned int counter) : AbstractEngine("fti", true, counter) {}

	void put(std::string quid, std::string key, std::string value);

};

#endif // TEXT_INDEX_H
