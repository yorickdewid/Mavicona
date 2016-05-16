#ifndef HBD_H
#define HBD_H

#include <fstream>
#include "linklist.h"

static const unsigned char magic[] = {0x12, 0x34, 0xf1, 0xc4, 0x02, 0xa1, 0xe2, 0xc0};

class ChecksumMismatchHashDB;

class HashDB {
	std::fstream m_db;
	LinkList *array;
	int length;

	void checksum() {
		char buffer[8];
		m_db.read(buffer, 8);
		if (memcmp(buffer, magic, 8)) {
			std::cerr << "Checksum failed" << std::endl;
		}
	}

	int hash(std::string itemKey) {
		int value = 0;
		for (unsigned int i = 0; i < itemKey.length(); ++i)
			value += itemKey[i];

		return (value * itemKey.length()) % length;
	}

  public:
	explicit HashDB(const std::string& name, int tableLength = 13) {
		if (std::ifstream(name)) {
			std::cout << "File already exists" << std::endl;

			m_db.open(name, std::fstream::in | std::fstream::out | std::fstream::binary);
			checksum();
		} else {
			std::cout << "File not already exists" << std::endl;

			char buffer[8];
			memcpy(buffer, magic, 8);
			m_db.open(name, std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
			m_db.write(buffer, 8);
			m_db.flush();

			std::cout << "Done" << std::endl;
		}

		if (tableLength <= 0)
			tableLength = 13;

		array = new LinkList[tableLength];
		length = tableLength;
	}

	~HashDB() {
		delete[] array;
		m_db.close();
	}

	void insert(Item *newItem) {
		int index = hash(newItem->key);
		array[index].insertItem(newItem);
	}

	bool remove(std::string itemKey) {
		int index = hash(itemKey);
		return array[index].removeItem(itemKey);
	}

	Item *getItemByKey(std::string itemKey) {
		int index = hash(itemKey);
		return array[index].getItem(itemKey);
	}

	inline int getLength() {
		return length;
	}
};

#endif // HBD_H
