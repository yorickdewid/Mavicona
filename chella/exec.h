#ifndef EXEC_H
#define EXEC_H

#include <iostream>
#include <core/soci.h>
#include "controlclient.h"
#include "callback.h"
#include "art.h"

inline int iter_cb(void *data, const unsigned char *key, uint32_t key_len, void *val) {
	delete static_cast<std::string *>(val);
	return 0;
}

class Execute : public Callback {
	ControlClient *jobcontrol = nullptr;
	art_tree *cache = nullptr;
	soci::session *session = nullptr;

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
		std::string jobname;
		std::string jobquid;
	};

	void setControl(ControlClient *control) {
		jobcontrol = control;
	}

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

		if (session) {
			delete session;
			session = nullptr;
		}
	}

	void cachePut(const std::string& key, const std::string value);
	void cacheDelete(const std::string& key);
	std::string cacheGet(const std::string& key);

	void sqlConnect(const std::string& rdbms, const std::string& database, const std::string& user, const std::string& password);
	void sqlQuery(const std::string& query);
	void sqlDisconnect();

	static void init(ControlClient *control) {
		Execute& exec = Execute::getInstance();
		exec.setControl(control);
	}

	static void run(const std::string& name, Parameter& param);
};

#endif // EXEC_H
