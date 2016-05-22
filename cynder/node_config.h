#ifndef NODE_CONFIG_H
#define NODE_CONFIG_H

#include <queue>
#include <memory>
#include <quidpp.h>
#include <leveldb/db.h>

class NodeConfigCorrupt {};

class NodeConfig {
	quidpp::Quid *quid;
	leveldb::DB *db;
	unsigned int clientNodeCount = 0;

  public:
	NodeConfig(const std::string& dbName = "meta") {
		leveldb::Options options;
		options.create_if_missing = true;
		leveldb::Status status = leveldb::DB::Open(options, dbName, &this->db);

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

		status = db->Get(leveldb::ReadOptions(), "clientcount", &value);
		if (status.ok() && value.length() > 0) {
			clientNodeCount = atoi(value.c_str());
		} else {
			db->Put(leveldb::WriteOptions(), "clientcount", std::to_string(clientNodeCount));
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

	inline void addSlaveNode(const char *address) {
		std::string clientNode = "clientnode";
		clientNode += std::to_string(clientNodeCount++);
		db->Put(leveldb::WriteOptions(), clientNode, address);
	}

	/*void scanStorageRange(const leveldb::Slice& prefix) {
		std::shared_ptr<leveldb::Iterator> it(db->NewIterator(leveldb::ReadOptions()));

		for (it->Seek(prefix); it->Valid() && it->key().starts_with(prefix); it->Next()) {
			if (!it->value().empty()) {
				std::cout << it->key().ToString() << ": "  << it->value().ToString() << std::endl;
				//leveldb::Slice key(it->key());
				//leveldb::Slice data(it->data());
				// TODO: process the key/data
			}
		}
	}*/

	template<typename F>
	void foreachSlaveNode(F functor) {
		const leveldb::Slice& prefix = "clientnode";
		std::shared_ptr<leveldb::Iterator> it(db->NewIterator(leveldb::ReadOptions()));

		for (it->Seek(prefix); it->Valid() && it->key().starts_with(prefix); it->Next()) {
			if (!it->value().empty()) {
				functor(it->key().ToString(), it->value().ToString());
				//std::cout << it->key().ToString() << ": "  << it->value().ToString() << std::endl;
			}
		}

		// cout << functor(10) << endl;
	}

	/*inline std::string masterAddress() const {
		std::string value;
		leveldb::Status status = db->Get(leveldb::ReadOptions(), "masteraddress", &value);
		if (status.ok() && value.length() > 0) {
			return value;
		}

		return "";
	}*/

	inline quidpp::Quid *id() {
		return quid;
	}

	~NodeConfig() {
		db->Put(leveldb::WriteOptions(), "clientcount", std::to_string(++clientNodeCount));

		delete this->quid;
		delete this->db;
	}
};

#endif // NODE_CONFIG_H
