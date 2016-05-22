#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>
#include <sys/stat.h>
#include <ups/upscaledb.hpp>

constexpr char defaultDataDir[] = "data";

#define DBIDX_QUID		0
#define DBIDX_NAME		1

enum EngineType {
	DB_ABI, /* Associated block index */
	DB_ADI, /* Additional data index */
};

class Engine {
	upscaledb::env env;          /* upscaledb environment object */
	upscaledb::db db[2];         /* upscaledb database object */
	EngineType m_Type;
	unsigned int cmt_counter = 0;

	inline const std::string dbname(const char *dir) {
		switch (m_Type) {
			case DB_ABI:
				return std::string(dir) + "/abi000" + std::to_string(cmt_counter++) + "cidb";
			case DB_ADI:
				return std::string(dir) + "/adi000" + std::to_string(cmt_counter++) + "cidb";
			default:
				break;
		}

		return "";
	}

  public:
	Engine(EngineType type, const char *datadir = defaultDataDir) : m_Type(type) {
		mkdir(datadir, 0700);

		ups_parameter_t params[] = {
			{0,}
		};

		/* Create a new environment file and a database in this environment */
		env.create(dbname(datadir).c_str());
		db[DBIDX_QUID] = env.create_db(DBIDX_QUID + 1, 0, &params[0]);
		db[DBIDX_NAME] = env.create_db(DBIDX_NAME + 1, UPS_ENABLE_DUPLICATE_KEYS, &params[0]);
	}

	~Engine() {
		db[DBIDX_NAME].close();
		db[DBIDX_QUID].close();
		env.close();
	}

	void put(std::string kquid, std::string kname, std::string v) {
		upscaledb::key key_quid;
		upscaledb::key key_name;
		upscaledb::record record;
		upscaledb::record n2q;

		key_quid.set_size(kquid.size());
		key_quid.set_data(const_cast<char *>(kquid.c_str()));

		key_name.set_size(kname.size());
		key_name.set_data(const_cast<char *>(kname.c_str()));

		record.set_size(v.size());
		record.set_data(const_cast<char *>(v.c_str()));

		n2q.set_size(kquid.size());
		n2q.set_data(const_cast<char *>(kquid.c_str()));

		try {
			db[DBIDX_QUID].insert(&key_quid, &record, UPS_OVERWRITE);
		} catch (upscaledb::error &e) {
			std::cerr << "DBIDX_QUID failed: " << e.get_string() << std::endl;
		}

		try {
			db[DBIDX_NAME].insert(&key_name, &n2q, UPS_OVERWRITE);
		} catch (upscaledb::error &e) {
			std::cerr << "DBIDX_NAME failed: " << e.get_string() << std::endl;
		}
	}

	std::string get(std::string k) {
		upscaledb::key key;
		upscaledb::record record;

		key.set_size(k.size());
		key.set_data(const_cast<char *>(k.c_str()));

		record = db[DBIDX_QUID].find(&key);

		return std::string(reinterpret_cast<const char *>(record.get_data()), record.get_size());
	}

	void remove(std::string kquid, std::string kname) {
		upscaledb::key key_quid;
		upscaledb::key key_name;

		key_quid.set_size(kquid.size());
		key_quid.set_data(const_cast<char *>(kquid.c_str()));

		key_name.set_size(kname.size());
		key_name.set_data(const_cast<char *>(kname.c_str()));

		db[DBIDX_QUID].erase(&key_quid);
		db[DBIDX_NAME].erase(&key_name);
	}

};

#endif // ENGINE_H
