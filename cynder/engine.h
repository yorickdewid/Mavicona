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
	DB_ABI, /* Associated block index, stores records and meta */
	DB_ADI, /* Authoritative data index, stores data contents */
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

		if (m_Type == DB_ADI)
			db[DBIDX_NAME] = env.create_db(DBIDX_NAME + 1, UPS_ENABLE_DUPLICATE_KEYS, &params[0]);
	}

	~Engine() {
		db[DBIDX_NAME].close();
		db[DBIDX_QUID].close();
		env.close();
	}

	void put(std::string kquid, std::string kname, std::string v, bool override = false) {
		if (m_Type == DB_ADI) {
			upscaledb::key key_quid;
			upscaledb::key key_name;
			upscaledb::record record;
			upscaledb::record n2q;

			key_quid.set_size(kquid.size());
			key_quid.set_data(const_cast<char *>(kquid.c_str()));

			key_name.set_size(kname.size());
			key_name.set_data(const_cast<char *>(kname.c_str()));

			if (v.size() > 1024 * 1024)
				std::cout << "Should send to LFB" << std::endl;

			record.set_size(v.size());
			record.set_data(const_cast<char *>(v.c_str()));

			n2q.set_size(kquid.size());
			n2q.set_data(const_cast<char *>(kquid.c_str()));

			db[DBIDX_QUID].insert(&key_quid, &record, override ? UPS_OVERWRITE : 0);
			db[DBIDX_NAME].insert(&key_name, &n2q, override ? UPS_OVERWRITE : 0);
		} else {
			upscaledb::key key;
			upscaledb::record record;

			std::string composed_key = kquid + kname;
			std::cout << "Key: " << composed_key << std::endl;
			std::cout << "Value: " << v << std::endl;

			key.set_size(composed_key.size());
			key.set_data(const_cast<char *>(composed_key.c_str()));

			record.set_size(v.size());
			record.set_data(const_cast<char *>(v.c_str()));

			db[DBIDX_QUID].insert(&key, &record, override ? UPS_OVERWRITE : 0);
		}
	}

	std::string get(std::string k1, std::string k2 = "") {
		upscaledb::key key;
		upscaledb::record record;

		if (m_Type == DB_ADI) {
			key.set_size(k1.size());
			key.set_data(const_cast<char *>(k1.c_str()));
		} else {
			std::string composed_key = k1 + k2;
			std::cout << "QQ Key: " << composed_key << std::endl;

			key.set_size(composed_key.size());
			key.set_data(const_cast<char *>(composed_key.c_str()));
		}

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
