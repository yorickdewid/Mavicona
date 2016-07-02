#include <iostream>
#include <ace/interface.h>

using namespace Ace;

class Example : public Job {
  public:
	Example() {}

	void Setup() {

	}

	void Run() {
		std::cout << "Cheese is awesome!" << std::endl;
	}

};

ACE_OBJECT(Example);
