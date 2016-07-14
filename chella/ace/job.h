#ifndef JOB_H
#define JOB_H

namespace Ace {

class Job {
	Environment jobenv;
	CacheArt cache;
	StoreIO io;
	Rdbms sql;
	Chain *chain = NULL;
	Callback *cb = NULL;

  public:
	Job() {}

	virtual ~Job() {}

	void Inject(Callback *_cb) {
		cb = _cb;
		cb->progress = 0;

		cache.setCallback(_cb);
		io.setCallback(_cb);
		sql.setCallback(_cb);
	}

	Chain *PullChain() {
		return chain;
	}

	void BindChain(Chain *_chain) {
		chain = _chain;
	}

	std::string Module() const {
		return cb->module;
	}

	std::string Status() {
		switch (cb->jobstate) {
			case Callback::SPAWN:
				return "spawn once";
			case Callback::PARTITION:
				return "scatter using partitioning";
			case Callback::FUNNEL:
				return "funneling";
		}

		return "";
	}

	inline unsigned int Partition() const {
		return cb->jobpartition;
	}

	inline unsigned int TotalPartitions() const {
		return cb->jobpartition_count;
	}

	inline unsigned int Id() const {
		return cb->jobid;
	}

	std::string Name() const {
		return cb->jobname;
	}

	std::string Quid() const {
		return cb->jobquid;
	}

	std::string Parent() const {
		return cb->jobparent;
	}

	inline unsigned int WorkerId() const {
		return cb->workerid;
	}

	std::string Worker() {
		std::ostringstream ss;
		ss << "worker-" << cb->workerid;
		return ss.str();
	}

	unsigned int ClusterJobs() const {
		return cb->clusterjobs;
	}

	void UpdateProgress(unsigned short progress) {
		cb->updateProgress(progress);
	}

	inline bool isSpawn() {
		return cb->jobstate == Callback::SPAWN;
	}

	inline bool isPartition() {
		return cb->jobstate == Callback::PARTITION;
	}

	inline bool isFunnel() {
		return cb->jobstate == Callback::FUNNEL;
	}

	/* Ace framework classes */
	Rdbms *SQL() {
		return &sql;
	}

	StoreIO *IO() {
		return &io;
	}

	CacheArt *Cache() {
		return &cache;
	}

	Environment *Env() {
		return &jobenv;
	}

	/* Job defined functions */
	virtual void SetupOnce() {};
	virtual void Setup() {};
	virtual void Run() = 0;
	virtual void Teardown() {};
	virtual void TeardownOnce() {};
};

}

#endif // JOB_H
