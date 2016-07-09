#include <iostream>
#include <ace/interface.h>

using namespace Ace;

class Example : public Job {
  public:
	Example() {}

	void Setup() {
		std::cout << std::endl;

		/* Print job info */
		std::cout << "Job id: " << Id() << std::endl;
		std::cout << "Job name: " << Name() << std::endl;
		std::cout << "Job quid: " << Quid() << std::endl;
		std::cout << "Job partition: " << Partition() << std::endl;

		/* Print the environment */
		std::cout << "Working dir: " << Env()->CurrentDirectory() << std::endl;
		std::cout << "Cache dir: " << Env()->CacheDirectory() << std::endl;
		std::cout << "Datetime: " << Env()->CurrentDateTime() << std::endl;
		std::cout << "Hostname: " << Env()->Hostname() << std::endl;
		std::cout << "Login: " << Env()->Login() << std::endl;

		/* Print cluster info */
		std::cout << "Worker: " << WorkerId() << std::endl;
		std::cout << "Worker: " << Worker() << std::endl;
		std::cout << "Module: " << Module() << std::endl;
		std::cout << "Jobs: " << ClusterJobs() << std::endl;

		std::cout << std::endl;

		sleep(2);
	}

	/* The actual execution of the job */
	void Run() {
		updateProgress(250);
		std::cout << "Doing the work..." << std::endl;
		sleep(1);

		updateProgress(384);
		std::cout << "More work..." << std::endl;
		sleep(1);

		updateProgress(532);
		std::cout << "Bit more..." << std::endl;
		sleep(1);

		updateProgress(862);
		std::cout << "Almost done!" << std::endl;
		sleep(1);
	}

	void Teardown() {
		sleep(1);

		std::cout << "Goodbye" << std::endl;
	}

};

ACE_OBJECT(Example);
