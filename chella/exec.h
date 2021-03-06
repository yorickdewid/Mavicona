#ifndef EXEC_H
#define EXEC_H

#undef CACHE
#undef RDBMS

#include <iostream>
#ifdef RDBMS
#include <core/soci.h>
#endif
#include "controlclient.h"
#include "indexer.h"
#include "callback.h"
#include "ace/chain.h"

#define	VARDIR		"var"
#define	PKGDIR		VARDIR "/package"
#define	OPTDIR		VARDIR "/opt"
#define	WALDIR		VARDIR "/wald"
#define	LOCALDIR	VARDIR "/local"
#define	TMPDIR		VARDIR "/tmp"
#define	ETCDIR		VARDIR "/etc"
#define	DBDIR		VARDIR "/db"
#define	MSTDIR		VARDIR "/master"

class Execute : public Callback {
	std::string masterIPC;
	std::string masterProvision;
	int workerid;
	ControlClient *jobcontrol = nullptr;
	Ace::Chain *chain = nullptr;
	Indexer *db = nullptr;

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
		//TODO
	}

	static void init(int workerid, const std::string& masterIPC, const std::string& masterProvision, Indexer *db);
	static bool run(const std::string& name, Parameter& param, bool canfork);
	static void prospect(const std::string& name);
	static void dispose();
};

#endif // EXEC_H
