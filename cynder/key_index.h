#ifndef KEY_INDEX_H
#define KEY_INDEX_H

#include "engine.h"
#include "additional_index.h"

class KeyIndex : public AbstractEngine, public AbstractAdditionalIndex {
  public:
	KeyIndex(unsigned int counter) : AbstractEngine("uki", true, counter) {}

	void put(std::string quid, std::string key, std::string value);

};

#endif // KEY_INDEX_H
