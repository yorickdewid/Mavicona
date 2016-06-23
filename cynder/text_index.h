#ifndef TEXT_INDEX_H
#define TEXT_INDEX_H

#include <list>

#include "engine.h"
#include "additional_index.h"
#include "record_index.h"
#include "data_index.h"

class TextIndex : public AbstractEngine, public AbstractAdditionalIndex {
	RecordIndex *ari = nullptr;
	DataIndex *adi = nullptr;

  public:
	TextIndex(unsigned int counter) : AbstractEngine("fti", true,counter) {}

	void attach(RecordIndex *_ari) {
		this->ari = ari;
	}

	void attach(DataIndex *_adi) {
		this->adi = _adi;
	}

	void put(std::string quid, std::string key, std::string value);

	void getMulti(std::string key, std::list<std::string> *list);

};

#endif // TEXT_INDEX_H
