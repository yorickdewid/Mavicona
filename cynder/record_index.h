#ifndef RECORD_INDEX_H
#define RECORD_INDEX_H

#include "engine.h"

class RecordIndex : public AbstractEngine {
  public:
	RecordIndex(unsigned int counter) : AbstractEngine("ari", false, counter) {}

	void put(std::string quid, std::string value, bool override = false) {
		if (quid.size() != quidpp::Quid::unpackedSize()) {
			quid = quidpp::Quid::crop(quid);
			if (quid.empty()) {
				std::cerr << "Invalid QUID concatenation" << std::endl;
				return;
			}
		}

		AbstractEngine::put(quid, value, override);
	}

};

#endif // RECORD_INDEX_H
