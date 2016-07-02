#include <iostream>
#include <ace/interface.h>

using namespace Ace;

class Example : public Job {
  public:
	Example() {}

	void Setup() {
		// SetId(17);
		// SetName("test");

		std::cout << std::endl;

		/* Print job info */
		std::cout << "Job id: " << Id() << std::endl;
		std::cout << "Job name: " << Name() << std::endl;
		std::cout << "Job quid: " << Quid() << std::endl;
		std::cout << "Job partition: " << Partition() << std::endl;

		std::cout << std::endl;

		/* Print the environment */
		std::cout << "Working dir: " << Env()->CurrentDirectory() << std::endl;
		std::cout << "Cache dir: " << Env()->CacheDirectory() << std::endl;
		std::cout << "Datetime: " << Env()->CurrentDateTime() << std::endl;
		std::cout << "Hostname: " << Env()->Hostname() << std::endl;
		std::cout << "Login: " << Env()->Login() << std::endl;
		std::cout << "Worker: " << Env()->WorkerIdent() << std::endl;
	}

	/* The actual execution of the job */
	void Run() {
		std::cout << "Cheese is awesome!" << std::endl;
	}

};

ACE_OBJECT(Example);
