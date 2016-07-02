#ifndef JOB_H
#define JOB_H

namespace Ace {

class Job {
	unsigned int jobid;
	unsigned int jobpartition;
	std::string jobname;
	std::string jobquid;
	Environment jobenv;

  public:
	Job() {}

	virtual ~Job() {}

	void Inject(Environment env, unsigned int id, const std::string& name, const std::string& quid, unsigned int partition) {
		jobid = id;
		jobname = name;
		jobquid = quid;
		jobenv = env;
		jobpartition = partition;
	}

	unsigned int Partition() const {
		return jobpartition;
	}

	unsigned int Id() const {
		return jobid;
	}

	virtual void SetId(unsigned int id) {
		jobid = id;
	}

	std::string Name() const {
		return jobname;
	}

	virtual void SetName(const std::string& name) {
		jobname = name;
	}

	std::string Quid() const {
		return jobquid;
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
