#ifndef DATA_INDEX_H
#define DATA_INDEX_H

#include "engine.h"
#include "filebase.h"

class DataIndex : public AbstractEngine {
	Filebase *lfb = nullptr;

  public:
	DataIndex() : AbstractEngine("adi") {}

	void attach(Filebase *_lfb) {
		this->lfb = _lfb;
	}

	void put(std::string quid, std::string value, bool override = false);
	
	std::string get(std::string key);

};

#endif // DATA_INDEX_H
