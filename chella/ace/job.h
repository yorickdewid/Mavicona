#ifndef JOB_H
#define JOB_H

namespace Ace {

class Job {
	unsigned int jobid;
	std::string jobname;
	Environment jobenv;

  public:
	Job() {}

	virtual ~Job() {}

	void Inject() {

	}

	virtual void SetId(unsigned int id) {
		jobid = id;
	}

	virtual void SetName(const std::string& name) {
		jobname = name;
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
