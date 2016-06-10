#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>
#include <iomanip>
#include <sys/stat.h>
#include <ups/upscaledb.hpp>

constexpr char defaultDataDir[] = "data";

class AbstractEngine {
	upscaledb::env env;       /* upscaledb environment object */
	upscaledb::db db;         /* upscaledb database object */
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

		/* Create a new environment for ADI file and a database in this environment */
		try{
			env.create(dbname(datadir).c_str());
			db = env.create_db(1, allow_duplicates ? UPS_ENABLE_DUPLICATE_KEYS : 0, &params[0]);
		} catch (upscaledb::error& e) {
			std::cerr << "Error creating database: " << e.get_string() << std::endl;
		}
	}

	virtual ~AbstractEngine() {
		db.close();
		env.close();
	}

	virtual void put(std::string key, std::string value, bool override = false) {
		upscaledb::key _key((char *)key.c_str(), key.size());
		upscaledb::record _record((char *)value.c_str(), value.size());

		db.insert(&_key, &_record, override ? UPS_OVERWRITE : 0);
	}

	virtual std::string get(std::string key) {
		upscaledb::key _key((char *)key.c_str(), key.size());

		upscaledb::record _record = db.find(&_key);

		return std::string(reinterpret_cast<const char *>(_record.get_data()), _record.get_size());
	}

	virtual void remove(std::string key) {
		upscaledb::key _key((char *)key.c_str(), key.size());

		db.erase(&_key);
	}
};

#endif // ENGINE_H
