#include <fstream>
#include <iostream> // temp!!

#include "filepage.h"

#define PAGE_MAGIC		"LFBX01E"	/* Magic checkup */
#define PAGE_CAPACITY	16			/* Capacity of page in items */

#define PAGE_FLAG_NIL	0x0
#define PAGE_FLAG_INV	0x2
#define PAGE_FLAG_FULL	0x12

struct pageHeader {
	const unsigned char magic[8] = PAGE_MAGIC;
	short elements = 0;
	int next_item = 0;
	int flags = PAGE_FLAG_NIL;
};

struct pageFooter {
	const unsigned char magic[8] = PAGE_MAGIC;
};

struct pageIndex {
	char name[40];
	int item = 0;
};

bool Filepage::verify() {
	pageHeader header;
	fs.read((char *)&header, sizeof(pageHeader));

	if (strcmp((const char *)header.magic, PAGE_MAGIC)) {
		puts("Magic error"); // throw error
		return false;
	}

	if (header.elements > PAGE_CAPACITY) {
		puts("Overflow error"); // throw error
		return false;
	}

	fs.seekg(sizeof(pageHeader) + (sizeof(pageIndex) * PAGE_CAPACITY) + (ITEM_SIZE * PAGE_CAPACITY));

	pageFooter footer;
	fs.read((char *)&footer, sizeof(pageFooter));

	if (strcmp((const char *)footer.magic, PAGE_MAGIC)) {
		puts("Magic err"); // throw error
		return false;
	}

	return true;
}

void Filepage::create(const std::string& file) {
	fs.open(file.c_str(), std::fstream::out | std::fstream::binary);

	pageHeader header;
	fs.write((const char *)&header, sizeof(pageHeader));

	fs.seekp(sizeof(pageHeader) + (sizeof(pageIndex) * PAGE_CAPACITY) + (ITEM_SIZE * PAGE_CAPACITY));

	pageFooter footer;
	fs.write((const char *)&footer, sizeof(pageFooter));

	fs.flush();

	m_Open = true;
}

void Filepage::open(const std::string& file) {
	if (!is_open()) {
		fs.open(file.c_str(), std::fstream::in | std::fstream::out | std::fstream::binary);
		if (!fs.is_open()) {
			puts("Access error"); // throw error
			return;
		}
	}

	if (!verify())
		return;

	m_Open = true;
}
