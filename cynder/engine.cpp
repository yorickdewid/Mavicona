#include <list>
#include <string>
#include <ups/upscaledb.hpp>
#include <quidpp.h>

#include "engine.h"

void AbstractEngine::put(std::string key, std::string value, bool override, bool duplicate) {
	upscaledb::key _key((char *)key.c_str(), key.size());
	upscaledb::record _record((char *)value.c_str(), value.size());

	uint32_t flag = 0;
	if (override)
		flag = UPS_OVERWRITE;
	else if (duplicate)
		flag = UPS_DUPLICATE;

	db.insert(&_key, &_record, flag);
	env.flush();
}

std::string AbstractEngine::get(std::string key) {
	upscaledb::key _key((char *)key.c_str(), key.size());

	upscaledb::record _record = db.find(&_key);

	return std::string(reinterpret_cast<const char *>(_record.get_data()), _record.get_size());
}

void AbstractEngine::remove(std::string key) {
	upscaledb::key _key((char *)key.c_str(), key.size());

	db.erase(&_key);
	env.flush();
}

void AbstractEngine::getMulti(std::string key, std::list<std::string> *list) {
	upscaledb::cursor cursor(&db);

	upscaledb::record _record;
	upscaledb::key _key((char *)key.c_str(), key.size());

	try {
		cursor.find(&_key, &_record);
	} catch (upscaledb::error& error) {
		if (error.get_errno() == UPS_KEY_NOT_FOUND)
			return;
	}

	size_t count = cursor.get_duplicate_count();
	for (unsigned int i = 0; i < count; ++i) {
		std::string content = std::string(reinterpret_cast<const char *>(_record.get_data()), _record.get_size());
		std::string quid = content.substr(0, quidpp::Quid::unpackedSize());

		/* Ignore if key exist */
		bool found = (std::find(list->begin(), list->end(), quid) != list->end());
		if (found)
			continue;

		list->push_back(quid);
		cursor.move_next(&_key, &_record);
	}
}
