#ifndef KEY_INDEX_H
#define KEY_INDEX_H

#include <list>

#include "engine.h"
#include "additional_index.h"

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

	void put(std::string quid, std::string key, std::string value) {
		if (quid.size() != quidpp::Quid::unpackedSize()) {
			quid = quidpp::Quid::crop(quid);
			if (quid.empty()) {
				std::cerr << "Invalid QUID concatenation" << std::endl;
				return;
			}
		}

		normalize(key);

		if (!matchKeyword(key))
			return;
		
		std::cout << "Key quallifies for storage " << key << " -> " << value << std::endl;
		AbstractEngine::put(key, quid + value, false, true);
	}

	void getMulti(std::string key, std::list<std::string> *list) {
		upscaledb::cursor cursor(&db);

		upscaledb::record _record;
		upscaledb::key _key((char *)key.c_str(), key.size());

		cursor.find(&_key, &_record);
		size_t count = cursor.get_duplicate_count();

		for(unsigned int i = 0; i < count; ++i) {
			std::string content = std::string(reinterpret_cast<const char *>(_record.get_data()), _record.get_size());
			std::string quid = content.substr(0, quidpp::Quid::unpackedSize());

			/* Ignore if exist */
			bool found = (std::find(list->begin(), list->end(), quid) != list->end());
			if (found)
				continue;

			list->push_back(quid);
			cursor.move_next(&_key, &_record);
		}
	}

};

#endif // KEY_INDEX_H
