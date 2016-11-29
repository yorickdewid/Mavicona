#ifndef INDEXER_H
#define INDEXER_H

#include <queue>
#include <iostream>
#include <leveldb/db.h>

class Indexer {
	leveldb::DB *db = nullptr;
	unsigned int jobcounter = 0;

  public:
	Indexer(const std::string& dbName = "db") {
		leveldb::Options options;
		options.create_if_missing = true;
		leveldb::Status status = leveldb::DB::Open(options, dbName, &this->db);

		if (!status.ok())
			std::cerr << status.ToString() << std::endl;

		/* Retrieve cached tasks an put them back on the queue */
		std::string value;
		status = db->Get(leveldb::ReadOptions(), "jobcounter", &value);
		if (status.ok()) {
			unsigned int _number = 0;
			memcpy(&_number, value.c_str(), 4);
			jobcounter = _number;
		} else {
			storeCounter();
		}
	}

	~Indexer() {
		delete this->db;
	}

	void putItem(const char *_key, size_t _key_len, const char *_value, size_t _value_len) {
		leveldb::Slice key(_key, _key_len);
		leveldb::Slice value(_value, _value_len);
		leveldb::Status status = db->Put(leveldb::WriteOptions(), key, value);
		//TODO: check status
	}

	std::string getItem(const char *_key, size_t _key_len) {
		leveldb::Slice key(_key, _key_len);
		std::string value;
		leveldb::Status status = db->Get(leveldb::ReadOptions(), key, &value);
		if (status.ok())
			return value;

		return "";
	}

	void deleteItem(const char *_key, size_t _key_len) {
		leveldb::Slice key(_key, _key_len);
		leveldb::Status status = db->Delete(leveldb::WriteOptions(), key);
		//TODO: check status
	}

	inline void storeCounter() {
		leveldb::Slice value((char *)&jobcounter, sizeof(unsigned int));
		leveldb::Status status = db->Put(leveldb::WriteOptions(), "jobcounter", value);
		//TODO: check status
	}

	inline int jobCounter() {
		return jobcounter++;
	}
};

#endif // INDEXER_H
