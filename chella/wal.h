#ifndef WAL_H
#define WAL_H

#include <sstream>
#include <iomanip>
#include <cassert>

#include "common/util.h"
#include "exec.h"

struct Wal {
  private:
	FILE *m_pFile = nullptr;
	std::string module;
	Execute::Parameter jobparameters;

	void commit(bool isDone = false);

	const std::string walname(const unsigned int id, const std::string& quid) {
		std::stringstream ss;
		ss << WALDIR;
		ss << "/";
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

	/* Create WAL if not exist, otherwise override */
	Wal(const std::string& name, Execute::Parameter& param) : module(name), jobparameters(param) {
		m_pFile = fopen(walname(param.jobid, param.jobquid).c_str(), "w+");
		if (!m_pFile) {
			std::cerr << "Cannot create WAL log" << std::endl;
		}

		assert(module.size() == 40);
		assert(param.jobquid.size() == 36);

		commit();
	}

	static void rollback(const std::string& name, std::function<void(const std::string& name, Execute::Parameter& param)> const& callback);

	void setCheckpoint(enum Checkpoint checkpoint);
	
	inline void markDone() {
		commit(true);
	}

	~Wal() {
		fclose(m_pFile);
	}

  private:
	enum Checkpoint checkpoint = Checkpoint::NIL;
};

#endif // WAL_H
