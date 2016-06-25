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
	unsigned int m_ItemSize;
	FILE *m_pFile = nullptr;
	const std::string m_File;
	std::map<std::string, std::pair<unsigned int, unsigned int>> contents;
	bool m_pageFull = false;

	void writeHeader();
	void growPage();
	void shouldClose();

  public:
	Filepage(const std::string& file) : m_File(file) {
		if (!file_exist(m_File)) {
			create();
		} else {
			open();
		}
	}

	void create(unsigned int alloc = DEFAULT_PAGE_ALLOC, unsigned int size = ITEM_SIZE);
	void open();

	/* Store new item */
	void storeItem(std::string name, std::string data);

	/* Get item by name */
	std::vector<uint8_t> *retrieveItem(std::string name);

	/* Remove item from page */
	void removeItem(std::string name);

	bool isFull() {
		return m_pageFull;
	}

	size_t size();

	inline bool empty() {
		return size() == 0;
	}

	inline bool isOpen() {
		return m_pFile != nullptr;
	}

	~Filepage() {
		writeHeader();
		fclose(m_pFile);
	}

};

#endif // FILEPAGE_H
