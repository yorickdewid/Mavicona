#include <quidpp.h>

#include "data_index.h"

#define DATA_MAGIC		72
#define DATA_FLAG_NIL	0x0
#define DATA_FLAG_ADI	0x1
#define DATA_FLAG_LFB	0x2

struct StorageType {
	unsigned short page = 0;
	unsigned int pointer = 0; /* No one knows why there's a pointer hanging around here... */
	char magic;
	char flags = DATA_FLAG_NIL;
};

void DataIndex::put(std::string quid, std::string value, bool override) {
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
	if (value.size() > 240000 /*ITEM_SIZE*/) {
		type.flags = DATA_FLAG_LFB;
		type.page = lfb->put(quid, value);

		value.clear();
		value.reserve(sizeof(StorageType));
	}

	value.append((const char *)&type, sizeof(StorageType));

	AbstractEngine::put(quid, value, override);
}

std::string DataIndex::get(std::string quid) {
	if (quid.size() != quidpp::Quid::unpackedSize()) {
		quid = quidpp::Quid::crop(quid);
		if (quid.empty()) {
			std::cerr << "Invalid QUID concatenation" << std::endl;
			return "";
		}
	}
	
	std::string data = AbstractEngine::get(quid);
	size_t data_end = data.size() - sizeof(StorageType);

	StorageType type;
	data.copy((char *)&type, sizeof(StorageType), data_end);

	if (type.magic != DATA_MAGIC) {
		std::cerr << "Invalid magic type" << std::endl;
		return ""; // throw something
	}

	/* Retrieve content from LFB */
	if (type.flags == DATA_FLAG_LFB) {
		std::vector<uint8_t> *value = lfb->get(type.page, quid);
		data = std::string(value->begin(), value->end());
	} else {
		data.resize(data_end);
	}

	return data;
}
