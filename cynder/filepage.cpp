#include <fstream>

#include "filepage.h"

#define PAGE_MAGIC		"LFBX01E"	/* Magic checkup */
#define PAGE_CAPACITY	16			/* Capacity of page in items */

struct pageHeader {
	const unsigned char magic[8] = PAGE_MAGIC;
	short elements = 0;
	int next_item = 0;
	int flags = 0x0;
};

struct pageFooter {
	const unsigned char magic[8] = PAGE_MAGIC;
};

struct pageIndex {
	char name[40];
	int item = 0;
};

void Filepage::createPage() {
	std::ofstream ofp(dbname(dir));

	pageHeader header;
	ofp.write((const char *)&header, sizeof(pageHeader));

	ofp.seekp(sizeof(pageHeader) + (sizeof(pageIndex) * PAGE_CAPACITY) + (ITEM_SIZE * PAGE_CAPACITY));

	pageFooter footer;
	ofp.write((const char *)&footer, sizeof(pageFooter));

	ofp.close();
}

unsigned int Filepage::store(std::string name, std::string data) {
	return 0;
}

std::string Filepage::retrieve(unsigned int page, std::string key) {
	return "";
}

void Filepage::purge(unsigned int page, std::string key) {

}
