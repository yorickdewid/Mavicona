#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <ups/upscaledb.hpp>

#include "common/util.h"

constexpr char defaultDataDir[] = "data";

class AbstractEngine {
  protected:
	upscaledb::env env;       /* upscaledb environment object */
	upscaledb::db db;         /* upscaledb database object */

  private:
	const char *name_prefix;
	unsigned int counter = 0;

	const std::string dbname(const char *dir) {
		std::stringstream ss;
		ss << std::string(dir);
		ss << "/";
		ss << name_prefix;
		ss << std::setw(4) << std::setfill('0') << counter++;
		ss << "cidb";

		return ss.str();
	}

  public:
	AbstractEngine(const char *prefix, bool allow_duplicates = false, const char *datadir = defaultDataDir) : name_prefix(prefix) {
		mkdir(datadir, 0700);

		assert(strlen(name_prefix) == 3);

		ups_parameter_t params[] = {
			{0,}
		};

		try {
			const char *dbfile = dbname(datadir).c_str();
			if (!file_exist(dbfile)) {
				env.create(dbfile);
				db = env.create_db(1, allow_duplicates ? UPS_ENABLE_DUPLICATE_KEYS : 0, &params[0]);
			} else {
				env.open(dbfile);
				db = env.open_db(1);
			}
		} catch (upscaledb::error& e) {
			std::cerr << "Error accessing database: " << e.get_string() << std::endl;
		}
	}

	inline unsigned int dbcount() {
		return counter;
	}

	void close() {
		db.close();
		env.close();
	}

	virtual ~AbstractEngine() {
		close();
	}

	virtual void put(std::string key, std::string value, bool override = false, bool duplicate = false) {
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

	virtual std::string get(std::string key) {
		upscaledb::key _key((char *)key.c_str(), key.size());

		upscaledb::record _record = db.find(&_key);

		return std::string(reinterpret_cast<const char *>(_record.get_data()), _record.get_size());
	}

	virtual void remove(std::string key) {
		upscaledb::key _key((char *)key.c_str(), key.size());

		db.erase(&_key);
		env.flush();
	}
};

#endif // ENGINE_H
