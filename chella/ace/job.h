#ifndef JOB_H
#define JOB_H

namespace Ace {

class Job {
	Environment jobenv;
	CacheArt cache;
	StoreIO io;
	Callback *cb = NULL;

  public:
	Job() {}

	virtual ~Job() {}

	void Inject(Callback *_cb) {
		cb = _cb;
		cb->progress = 0;
		cache.setCallback(_cb);
		io.setCallback(_cb);
	}

	std::string Module() const {
		return cb->module;
	}

	unsigned int Partition() const {
		return cb->jobpartition;
	}

	unsigned int Id() const {
		return cb->jobid;
	}

	std::string Name() const {
		return cb->jobname;
	}

	std::string Quid() const {
		return cb->jobquid;
	}

	unsigned int WorkerId() const {
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

	void updateProgress(unsigned short progress) {
		cb->updateProgress(progress);
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

	virtual void Setup() {};
	virtual void Run() = 0;
	virtual void Teardown() {};
};

}

#endif // JOB_H
