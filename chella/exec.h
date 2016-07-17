#ifndef EXEC_H
#define EXEC_H

#include <iostream>
#ifdef RDBMS
#include <core/soci.h>
#endif
#include "controlclient.h"
#include "callback.h"
#include "ace/chain.h"
#include "art.h"

inline int iter_cb(void *data, const unsigned char *key, uint32_t key_len, void *val) {
	delete static_cast<std::string *>(val);
	return 0;
}

class Execute : public Callback {
	std::string master;
	ControlClient *jobcontrol = nullptr;
	Ace::Chain *chain = nullptr;
	art_tree *cache = nullptr;
#ifdef RDBMS
	soci::session *session = nullptr;
#endif

	Execute() {};

	~Execute() {
		sessionCleanup();
	}

	Execute(Execute const&);
	void operator=(Execute const&);

	static Execute& getInstance() {
		static Execute instance;
		return instance;
	}

  public:
	struct Parameter {
		unsigned int jobid;
		unsigned int jobpartition;
		unsigned int jobpartition_count;
		std::string jobname;
		std::string jobquid;
		std::string jobparent;
		std::string jobdata;
		JobState jobstate;
	};

	void updateProgress(unsigned short progress) {
		jobcontrol->updateStateRunning(progress);
	}

	void sessionCleanup() {
		if (this->cache) {
			std::string out;
			art_iter(this->cache, iter_cb, &out);
			art_tree_destroy(this->cache);
			free(this->cache);
			this->cache = nullptr;
		}

#ifdef RDBMS
		if (session) {
			delete session;
			session = nullptr;
		}
#endif
	}

	void cachePut(const std::string& key, const std::string value);
	void cacheDelete(const std::string& key);
	std::string cacheGet(const std::string& key);

#ifdef RDBMS
	void sqlConnect(const std::string& rdbms, const std::string& database, const std::string& user, const std::string& password);
	void sqlQuery(const std::string& query);
	void sqlDisconnect();
#else
	void sqlConnect(const std::string& rdbms, const std::string& database, const std::string& user, const std::string& password) {}
	void sqlQuery(const std::string& query) {}
	void sqlDisconnect() {}
#endif

	static void init(ControlClient *control, const std::string& master);
	static void run(const std::string& name, Parameter& param);
	static void prospect(const std::string& name);
	static void dispose();
};

#endif // EXEC_H
