#include <fstream>
#include <limits.h>

#include "filepage.h"

#define PAGE_MAGIC		"LFBX01E"	/* Magic checkup */

#define PAGE_FLAG_NIL	0x0
#define PAGE_FLAG_INV	0x1
#define PAGE_FLAG_GZIP	0x2
#define PAGE_FLAG_FULL	0x10

#define INDEX_FLAG_NIL	0x0
#define INDEX_FLAG_DEL	0x1

#define INDEX_NEXT_EMPTY	0x11

struct pageHeader {
	char magic[8];
	unsigned short elements = 0;
	unsigned int first_free = 0;
	unsigned int grow_items = 0;
	unsigned int item_size = 0;
	int flags = PAGE_FLAG_NIL;
};

struct pageIndexItem {
	char name[36];
	unsigned int size;
	unsigned int item = 0;
	char flags = INDEX_FLAG_NIL;
};

size_t Filepage::size() {
	return m_Elements;
}

void Filepage::create(unsigned int alloc, unsigned int size) {
	if (isOpen())
		return;

	m_pFile = fopen(m_File.c_str(), "w+");

	pageHeader header;
	strcpy(header.magic, PAGE_MAGIC);
	header.grow_items = alloc;
	header.item_size = size;
	header.first_free = sizeof(pageHeader) + (sizeof(pageIndexItem) * header.grow_items) + sizeof(unsigned int);
	fwrite((const char *)&header, sizeof(pageHeader), 1, m_pFile);

	/* Pointer to current index */
	m_LastIndex = sizeof(pageHeader);

	/* Set pointer to next index */
	fseek(m_pFile, (sizeof(pageIndexItem) * header.grow_items), SEEK_CUR);
	int next_index = INDEX_NEXT_EMPTY;
	fwrite((const char *)&next_index, sizeof(unsigned int), 1, m_pFile);

	/* Allocate page on disk */
	fseek(m_pFile, header.item_size * header.grow_items, SEEK_END);
	fwrite("\0", 1, 1, m_pFile);

	fflush(m_pFile);

	m_Elements = header.elements;
	m_FirstFree = header.first_free;
	m_Grow = header.grow_items;
	m_ItemSize = header.item_size;
}

void Filepage::open() {
	if (isOpen())
		return;

	m_pFile = fopen(m_File.c_str(), "r+");

	pageHeader header;
	fread(&header, sizeof(pageHeader), 1, m_pFile);

	if (strcmp((const char *)header.magic, PAGE_MAGIC)) {
		puts("Magic error 1"); // throw error
		return;
	}

	if (header.flags & PAGE_FLAG_FULL)
		m_pageFull = true;

	/* Pointer to current index */
	m_LastIndex = sizeof(pageHeader);

	for (int i = 0; i < header.elements; ++i) {
		/* Move to next index */
		if ((m_LastIndex + (sizeof(pageIndexItem) * header.grow_items)) == (unsigned int)ftell(m_pFile)) {
			fread(&m_LastIndex, sizeof(unsigned int), 1, m_pFile);
			fseek(m_pFile, m_LastIndex, SEEK_SET);
		}

		pageIndexItem item;
		fread(&item, sizeof(pageIndexItem), 1, m_pFile);

		/* Ignore deleted items */
		if (item.flags & INDEX_FLAG_DEL)
			continue;

		contents[std::string(item.name, 36)] = std::pair<unsigned int, unsigned int>(item.item, item.size);
	}

	m_Elements = header.elements;
	m_FirstFree = header.first_free;
	m_Grow = header.grow_items;
	m_ItemSize = header.item_size;
}

void Filepage::writeHeader() {
	if (!isOpen())
		return;

	fseek(m_pFile, 0, SEEK_SET);

	pageHeader header;
	strcpy(header.magic, PAGE_MAGIC);
	header.elements = m_Elements;
	header.first_free = m_FirstFree;
	header.grow_items = m_Grow;
	header.item_size = m_ItemSize;

	if (m_pageFull)
		header.flags = PAGE_FLAG_FULL;

	fwrite((const char *)&header, sizeof(pageHeader), 1, m_pFile);

	fflush(m_pFile);
}

void Filepage::growPage() {
	unsigned int new_index = m_FirstFree;
	m_FirstFree += (sizeof(pageIndexItem) * m_Grow);

	/* Set pointer to next index */
	fseek(m_pFile, m_FirstFree, SEEK_SET);
	int next_index = INDEX_NEXT_EMPTY;
	fwrite((const char *)&next_index, sizeof(unsigned int), 1, m_pFile);
	m_FirstFree += sizeof(unsigned int);

	/* Set previous pointer to this index */
	fseek(m_pFile, m_LastIndex + (sizeof(pageIndexItem) * m_Grow), SEEK_SET);
	next_index = new_index;
	m_LastIndex = new_index;
	fwrite((const char *)&next_index, sizeof(unsigned int), 1, m_pFile);

	/* Allocate page on disk */
	fseek(m_pFile, m_FirstFree + (m_ItemSize * m_Grow), SEEK_CUR);
	fwrite("\0", 1, 1, m_pFile);

	writeHeader();
}

void Filepage::shouldClose() {
	if (!isOpen())
		return;

	/* Close page if INT limit exceeds */
	if (m_FirstFree + m_ItemSize > UINT_MAX)
		m_pageFull = true;
}

void Filepage::storeItem(std::string name, std::string data) {
	if (!isOpen())
		return;

	if (name.size() > 40) {
		puts("Name overflows index"); // throw
		return;
	}

	/* Grow page if last index is full */
	if (!(m_Elements % m_Grow) && m_Elements > 1)
		growPage();

	/* Write metadata to index */
	size_t namepos = m_LastIndex + (sizeof(pageIndexItem) * (m_Elements % m_Grow));
	fseek(m_pFile, namepos, SEEK_SET);

	pageIndexItem item;
	strcpy(item.name, name.c_str());
	item.size = data.size();
	item.item = m_FirstFree;
	fwrite(&item, sizeof(pageIndexItem), 1, m_pFile);

	/* Write data */
	fseek(m_pFile, m_FirstFree, SEEK_SET);
	fwrite(data.c_str(), 1, data.size(), m_pFile);
	m_FirstFree += data.size();

	/* Add index to content list */
	contents[name] = std::pair<unsigned int, unsigned int>(item.item, item.size);

	/* Increase elements */
	m_Elements++;

	/* Determine if this page is full */
	shouldClose();

	/* Flush page properties */
	writeHeader();
}

std::vector<uint8_t> *Filepage::retrieveItem(std::string name) {
	if (!isOpen())
		return nullptr;

	std::map<std::string, std::pair<unsigned int, unsigned int>>::const_iterator it = contents.find(name);
	if (it == contents.end()) {
		puts("not found");
		return nullptr;
	}

	auto pair = it->second;
	fseek(m_pFile, pair.first, SEEK_SET);

	std::vector<uint8_t> *buffer = new std::vector<uint8_t>(pair.second);
	std::vector<uint8_t>& bufferref = *buffer;
	fread(&bufferref[0], sizeof(uint8_t), pair.second, m_pFile);

	return buffer;
}

void Filepage::removeItem(std::string name) {
	if (!isOpen())
		return;

	std::map<std::string, std::pair<unsigned int, unsigned int>>::iterator it = contents.find(name);
	if (it == contents.end()) {
		puts("not found");
		return;
	}

	/* Pointer to current index */
	unsigned int indexPointer = sizeof(pageHeader);
	fseek(m_pFile, indexPointer, SEEK_SET);

	for (int i = 0; i < m_Elements; ++i) {
		/* Move to next index */
		if ((indexPointer + (sizeof(pageIndexItem) * m_Grow)) == (unsigned int)ftell(m_pFile)) {
			fread(&indexPointer, sizeof(unsigned int), 1, m_pFile);
			fseek(m_pFile, indexPointer, SEEK_SET);
		}

		pageIndexItem item;
		fread(&item, sizeof(pageIndexItem), 1, m_pFile);
		if (!strcmp(item.name, name.c_str())) {
			/* Set remove bit and write index */
			fseek(m_pFile, -sizeof(pageIndexItem), SEEK_CUR);
			item.flags |= INDEX_FLAG_DEL;
			fwrite(&item, sizeof(pageIndexItem), 1, m_pFile);
		}
	}
}
