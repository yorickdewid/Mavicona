#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>
#include <sys/stat.h>
#include <ups/upscaledb.hpp>

constexpr char defaultDataDir[] = "data";

class Engine {
	upscaledb::env env;          /* upscaledb environment object */
	upscaledb::db db;            /* upscaledb database object */
	unsigned int cmt_counter = 0;

	inline const std::string dbname(const char *dir) {
		return std::string(dir) + "/abi000" + std::to_string(cmt_counter++) + "cidb";
	}

  public:
	Engine(const char *datadir = defaultDataDir) {
		mkdir(datadir, 0700);

		ups_parameter_t params[] = {
			{0,}
		};

		/* Create a new environment file and a database in this environment */
		env.create(dbname(datadir).c_str());
		db = env.create_db(1, 0, &params[0]);
	}

	~Engine() {
		db.close();
		env.close();
	}

	void put(std::string k, std::string v) {
		upscaledb::key key;
		upscaledb::record record;

		key.set_size(k.size());
		key.set_data(const_cast<char *>(k.c_str()));

		record.set_size(v.size());
		record.set_data(const_cast<char *>(v.c_str()));

		db.insert(&key, &record);
	}

	std::string get(std::string k) {
		upscaledb::key key;
		upscaledb::record record;

		key.set_size(k.size());
		key.set_data(const_cast<char *>(k.c_str()));

		record = db.find(&key);

		return std::string(reinterpret_cast<const char *>(record.get_data()), record.get_size());
	}

	void remove(std::string k) {
		upscaledb::key key;

		key.set_size(k.size());
		key.set_data(const_cast<char *>(k.c_str()));

		db.erase(&key);
	}

};

#endif // ENGINE_H
