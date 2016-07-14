#ifndef WAL_H
#define WAL_H

#include <iostream>
#include <cassert>

struct Wal {
	FILE *m_pFile = nullptr;
	std::string module;

	void writeHeader(bool isDone = false);

  public:
	enum Checkpoint {
		NIL,
		INIT,
		LOAD,
		INJECT,
		SETUP_ONCE,
		SETUP,
		RUN,
		TEARDOWN,
		TEARDOWN_ONCE,
		PULLCHAIN,
	};

	Wal(const std::string& quid, const std::string& filename) : module(filename) {
		m_pFile = fopen(("cache/wal/" + quid).c_str(), "w+");

		assert(module.size() == 40);

		writeHeader();
	}

	void setCheckpoint(enum Checkpoint checkpoint);
	void markDone();

	~Wal() {
		fclose(m_pFile);
	}

  private:
	enum Checkpoint checkpoint = Checkpoint::NIL;
};

#endif // WAL_H
