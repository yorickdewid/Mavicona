#ifndef WAL_H
#define WAL_H

#include <sstream>
#include <iomanip>
#include <cassert>

struct Wal {
  private:
	FILE *m_pFile = nullptr;
	std::string module;

	void writeLog(bool isDone = false);

	const std::string walname(const unsigned int id, const std::string& quid) {
		std::stringstream ss;
		ss << "cache/wal/";
		ss << quid;
		ss << std::setw(6) << std::setfill('0') << id;
		ss << ".wlog";

		return ss.str();
	}

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

	Wal(const unsigned int id, const std::string& quid, const std::string& filename) : module(filename) {
		m_pFile = fopen(walname(id, quid).c_str(), "w+");

		assert(module.size() == 40);
		assert(quid.size() == 36);

		writeLog();
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
