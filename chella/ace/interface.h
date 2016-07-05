#ifndef INTERFACE_H
#define INTERFACE_H

#include "../callback.h"
#include "environment.h"
#include "io.h"
#include "cache.h"
#include "job.h"

#define ACE_MAGIC 0x2a73
#define ACE_OBJECT(NAME) \
	int register_class() { \
		ClassGuard &cg = ClassGuard::getInstance(); \
		cg.registerClass(new NAME); \
		return ACE_MAGIC; \
	}

namespace Ace {

class Interface {
  public:
	Interface() {}

	void run(const std::string& name);
};

}

class ClassGuard {
  public:
	static ClassGuard& getInstance() {
		static ClassGuard instance;
		return instance;
	}

	void registerClass(Ace::Job *job) {
		this->object = job;
	}

	Ace::Job *getObject() {
		return this->object;
	}

	bool isEmpty() {
		return (this->object == NULL);
	}

  private:
	ClassGuard() {};

	~ClassGuard() {
		if (object)
			delete object;
	};

	ClassGuard(ClassGuard const&);
	void operator=(ClassGuard const&);

	Ace::Job *object = NULL;
};

extern "C" int register_class();

extern "C" Ace::Job *object_facade() {
	ClassGuard& cg = ClassGuard::getInstance();
	return cg.getObject();
}

#endif // INTERFACE_H
