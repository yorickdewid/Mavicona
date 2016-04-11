#ifndef NODE_CONFIG_H
#define NODE_CONFIG_H

#include <queue>
#include <quidpp.h>
#include <leveldb/db.h>

class NodeConfigCorrupt {};

class NodeConfig {
	quidpp::Quid *quid;
	leveldb::DB *db;

  public:
	NodeConfig(const std::string& storeLocation = "meta") {
		leveldb::Options options;
		options.create_if_missing = true;
		leveldb::Status status = leveldb::DB::Open(options, storeLocation, &this->db);

		if (!status.ok()) {
			std::cerr << status.ToString() << std::endl;
			throw NodeConfigCorrupt();
		}

		std::string value;
		status = db->Get(leveldb::ReadOptions(), "quid", &value);
		if (status.ok() && value.length() > 0) {
			quid = new quidpp::Quid(value);
		} else {
			quid = new quidpp::Quid();
			db->Put(leveldb::WriteOptions(), "quid", quid->toString());
		}
	}

	inline bool isMaster() const {
		std::string value;
		leveldb::Status status = db->Get(leveldb::ReadOptions(), "master", &value);
		if (status.ok() && value.at(0) == '1') {
			return true;
		}

		return false;
	}

	inline void setMaster() {
		db->Put(leveldb::WriteOptions(), "master", "1");
	}

	inline quidpp::Quid *id() {
		return quid;
	}

	~NodeConfig() {
		delete this->quid;
		delete this->db;
	}
};

#endif // NODE_CONFIG_H
