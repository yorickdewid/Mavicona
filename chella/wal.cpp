#include <cstring>
#include "wal.h"

#define LOG_MAGIC		"WALU41Q"	/* Magic check */

struct pageHeader {
	char magic[8];
	char module[40];
	char quid[36];
	char parent_quid[36];
	unsigned char failcount = 0;
	unsigned char done = 0;
	unsigned int jobid;
	unsigned int jobpartition;
	unsigned int jobpartition_count;
	int jobstate;
	int jobnane_size;
	enum Wal::Checkpoint checkpoint = Wal::Checkpoint::NIL;
};

void Wal::writeLog(bool isDone) {
	fseek(m_pFile, 0, SEEK_SET);

	pageHeader header;
	strcpy(header.magic, LOG_MAGIC);
	strncpy(header.module, module.c_str(), 40);
	header.checkpoint = this->checkpoint;
	header.done = isDone ? 1 : 0;
	fwrite((const char *)&header, sizeof(pageHeader), 1, m_pFile);
}

void Wal::setCheckpoint(enum Wal::Checkpoint _checkpoint) {
	this->checkpoint = _checkpoint;
	writeLog();
}

void Wal::markDone() {
	writeLog(true);
}
