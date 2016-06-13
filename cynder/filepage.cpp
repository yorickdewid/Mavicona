#include <fstream>
#include <iostream> // temp!!

#include "filepage.h"

#define PAGE_MAGIC		"LFBX01E"	/* Magic checkup */

#define PAGE_FLAG_NIL	0x0
#define PAGE_FLAG_INV	0x2
#define PAGE_FLAG_GZIP	0x8
#define PAGE_FLAG_FULL	0x12

#define INDEX_FLAG_NIL	0x0
#define INDEX_FLAG_DEL	0x2

struct pageHeader {
	char magic[8];
	unsigned short elements = 0;
	unsigned int allocated = 0;
	unsigned int first_free = 0;
	int flags = PAGE_FLAG_NIL;
};

struct pageIndex {
	char name[40];
	unsigned int item = 0;
	char flags = INDEX_FLAG_NIL;
};

size_t Filepage::size() {
	return m_Elements;
}

void Filepage::create(unsigned int alloc) {
	m_pFile = fopen(m_File.c_str(), "w+");

	pageHeader header;
	strcpy(header.magic, PAGE_MAGIC);
	header.allocated = alloc;
	header.first_free = sizeof(pageHeader) + (sizeof(pageIndex) * header.allocated);
	fwrite((const char *)&header, sizeof(pageHeader), 1, m_pFile);

	/* Allocate page on disk */
	fseek(m_pFile, sizeof(pageHeader) + header.first_free + (ITEM_SIZE * header.allocated), SEEK_CUR);
	fwrite("\0", 1, 1, m_pFile);

	fflush(m_pFile);

	m_Elements = header.elements;
	m_Allocated = header.allocated;
	m_FirstFree = header.first_free;
}

void Filepage::open() {
	m_pFile = fopen(m_File.c_str(), "r+");

	pageHeader header;
	fread((char *)&header, sizeof(pageHeader), 1, m_pFile);

	if (strcmp((const char *)header.magic, PAGE_MAGIC)) {
		puts("Magic error 1"); // throw error
		return;
	}

	if (header.elements > header.allocated) {
		puts("Overflow error"); // throw error
		return;
	}

	m_Elements = header.elements;
	m_Allocated = header.allocated;
	m_FirstFree = header.first_free;
}

void Filepage::writeHeader() {
	fseek(m_pFile, 0, SEEK_SET);

	pageHeader header;
	strcpy(header.magic, PAGE_MAGIC);
	header.elements = m_Elements;
	header.allocated = m_Allocated;
	header.first_free = m_FirstFree;

	fwrite((const char *)&header, sizeof(pageHeader), 1, m_pFile);

	fflush(m_pFile);
}

void Filepage::grow() {
	m_Allocated *= 2;

	m_FirstFree += (sizeof(pageIndex) * (m_Allocated / 2));

	/* Allocate page on disk */
	fseek(m_pFile, m_FirstFree + (ITEM_SIZE * m_Allocated), SEEK_CUR);
	fwrite("\0", 1, 1, m_pFile);

	writeHeader();
}

void Filepage::storeItem(std::string name, std::string data) {
	if (name.size() > 40) {
		puts("Name overflows index");
		return;
	}

	std::cout << "Current allocation size: " << (m_Allocated * ITEM_SIZE) << std::endl;
	std::cout << "Elements: " << m_Elements << std::endl;
	std::cout << "Free offset: " << m_FirstFree << std::endl;
	std::cout << "New data size: " << data.size() << std::endl;

	if (m_FirstFree + data.size() > (m_Allocated * ITEM_SIZE)) {
		puts("Should grow");
		return;
	}

	if (m_Elements == m_Allocated) {
		puts("Should grow certainly, fullhouse");
		grow();
		return;
	}

	{
		size_t namepos = sizeof(pageHeader) + (sizeof(pageIndex) * m_Elements);
		std::cout << "Position for new name: " << namepos << std::endl;

		fseek(m_pFile, namepos, SEEK_CUR);

		pageIndex index;
		strcpy(index.name, name.c_str());
		index.item = m_FirstFree;

		fwrite((const char *)&index, sizeof(pageIndex), 1, m_pFile);
	}

	{
		std::cout << "Position for new data: " << m_FirstFree << std::endl;
		fseek(m_pFile, m_FirstFree, SEEK_CUR);

		fwrite(data.c_str(), 1, data.size(), m_pFile);

		m_FirstFree += data.size();
	}

	m_Elements++;

	writeHeader();

	fflush(m_pFile);
}
