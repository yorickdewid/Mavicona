#ifndef FILEPAGE_H
#define FILEPAGE_H

#include <cassert>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <map>
#include <vector>
#include <sys/stat.h>

#include "common/util.h"

#define ITEM_SIZE			8 * 1024 * 1024
#define DEFAULT_PAGE_ALLOC	16			/* Capacity of page in items */

class Filepage {
	unsigned short m_Elements;
	unsigned int m_FirstFree;
	unsigned int m_Grow;
	unsigned int m_LastIndex;
	FILE *m_pFile = nullptr;
	const std::string m_File;
	std::map<std::string, std::pair<unsigned int, unsigned int>> contents;

	void writeHeader();
	void grow();

  public:
	Filepage(const std::string& file) : m_File(file) {
		if (!file_exist(m_File)) {
			create();
		} else {
			open();
		}
	}

	void create(unsigned int alloc = DEFAULT_PAGE_ALLOC);
	void open();

	void storeItem(std::string name, std::string data);

	std::vector<uint8_t> *retrieveItem(std::string name);

	void removeItem(unsigned int page, std::string name);

	size_t size();

	inline bool empty() {
		return size() == 0;
	}

	inline bool is_open() {
		return m_pFile != nullptr;
	}

	~Filepage() {
		fclose(m_pFile);
	}

};

#endif // FILEPAGE_H
