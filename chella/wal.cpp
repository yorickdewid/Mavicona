#include <cstring>
#include "wal.h"

#define LOG_MAGIC		"WALU41Q"	/* Magic check */

struct pageHeader {
	char magic[8];
	char module[40];
	unsigned char done = 0;
	enum Wal::Checkpoint checkpoint = Wal::Checkpoint::NIL;
};

void Wal::writeHeader(bool isDone) {
	pageHeader header;
	strcpy(header.magic, LOG_MAGIC);
	strncpy(header.module, module.c_str(), 40);
	header.checkpoint = this->checkpoint;
	header.done = isDone ? 1 : 0;
	fwrite((const char *)&header, sizeof(pageHeader), 1, m_pFile);
}

void Wal::setCheckpoint(enum Wal::Checkpoint _checkpoint) {
	this->checkpoint = _checkpoint;
	writeHeader();
}

void Wal::markDone() {
	writeHeader(true);
}
