#ifndef DATA_INDEX_H
#define DATA_INDEX_H

#include "engine.h"
#include "filebase.h"

#define DATA_MAGIC		72
#define DATA_FLAG_NIL	0x0
#define DATA_FLAG_ADI	0x1
#define DATA_FLAG_LFB	0x2

class DataIndex : public AbstractEngine {
	struct StorageType {
		unsigned short page = 0;
		unsigned int pointer = 0;
		char magic;
		char flags = DATA_FLAG_NIL;
	};

  public:
	DataIndex() : AbstractEngine("adi") {}

	void put(std::string quid, std::string value, bool override = false) {
		if (quid.size() != quidpp::Quid::unpackedSize()) {
			quid = quidpp::Quid::crop(quid);
			if (quid.empty()) {
				std::cerr << "Invalid QUID concatenation" << std::endl;
				return;
			}
		}

		StorageType type;
		type.magic = DATA_MAGIC;
		type.flags = DATA_FLAG_ADI;

		/* Store content in LFB */
		if (value.size() > ITEM_SIZE) {
			std::cout << "Store in LFB" << std::endl;

			type.flags = DATA_FLAG_LFB;
			type.page = 2; // TODO example
			type.pointer = 3863522; // TODO example
		}

		std::cout << value.size() << std::endl;
		value.append((const char *)&type, sizeof(StorageType));
		std::cout << value.size() << std::endl;

		AbstractEngine::put(quid, value, override);
	}

	std::string get(std::string key) {
		std::string data = AbstractEngine::get(key);
		size_t data_end = data.size() - sizeof(StorageType);

		StorageType type;

		std::cout << data.size() << std::endl;
		data.copy((char *)&type, sizeof(StorageType), data_end);

		if (type.magic != DATA_MAGIC) {
			std::cerr << "Invalid magic type" << std::endl;
			return ""; // throw something
		}

		/* Retrieve content from LFB */
		if (type.flags == DATA_FLAG_LFB) {
			std::cout << "Retrieve from LFB with page " << type.page << " and offset " << type.pointer << std::endl;
		}

		data.resize(data_end);
		std::cout << data.size() << std::endl;

		return data;
	}

};

#endif // DATA_INDEX_H
