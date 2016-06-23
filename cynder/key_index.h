#ifndef KEY_INDEX_H
#define KEY_INDEX_H

#include <list>

#include "engine.h"
#include "additional_index.h"
#include "record_index.h"
#include "data_index.h"

class KeyIndex : public AbstractEngine, public AbstractAdditionalIndex {
	RecordIndex *ari = nullptr;
	DataIndex *adi = nullptr;

  public:
	KeyIndex(unsigned int counter) : AbstractEngine("uki", true, counter) {}

	void attach(RecordIndex *_ari) {
		this->ari = ari;
	}

	void attach(DataIndex *_adi) {
		this->adi = _adi;
	}

	void put(std::string quid, std::string key, std::string value);

	void getMulti(std::string key, std::list<std::string> *list);

};

#endif // KEY_INDEX_H
