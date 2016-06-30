#ifndef JOB_H
#define JOB_H

namespace Ace {

class Job {
  public:
	Job() {}

	virtual ~Job() {}

	void Inject() {

	}

	virtual void Setup() {};
	virtual void Run() = 0;
	virtual void Teardown() {};
};

}

#endif // JOB_H
