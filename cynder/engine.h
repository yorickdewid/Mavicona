#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <ups/upscaledb.hpp>

#include "common/util.h"

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
	AbstractEngine(const char *prefix, bool allow_duplicates = false, unsigned _counter = 0,
	               const char *datadir = defaultDataDir) : name_prefix(prefix), counter(_counter) {

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

	inline unsigned int dbcount() const {
		return counter - 1;
	}

	void close() {
		db.close();
		env.close();
	}

	virtual ~AbstractEngine() {
		close();
	}

	virtual void put(std::string key, std::string value, bool override = false, bool duplicate = false);

	virtual std::string get(std::string key);

	virtual void remove(std::string key);

	void getMulti(std::string key, std::list<std::string> *list);

};

#endif // ENGINE_H
