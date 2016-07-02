#include <dlfcn.h>
#include <cassert>
#include "common/util.h"
#include "ace/interface.h"
#include "exec.h"

typedef int (*regclass_t)();
typedef Ace::Job *(*retrieve_t)();

void Execute::run(const std::string& name, Parameter& param) {
	Ace::Environment env;

	std::cout << "Running module " << std::endl;

	env.SetWorkerIdent(param.workerid);

	if (!file_exist("cache/" + name)) {
		std::cerr << "Cannot access library" << std::endl;
		return;
	}

	void *handle = dlopen(("cache/" + name).c_str(), RTLD_LAZY);
	if (!handle) {
		std::cerr << "Cannot open library: " << dlerror() << std::endl;
		return;
	}

	regclass_t exec_register = (int (*)()) dlsym(handle, "register_class");
	retrieve_t exec_retrieve = (Ace::Job * (*)()) dlsym(handle, "retrieve_object");

	assert(exec_register() == ACE_MAGIC);
	Ace::Job *jobObject = (Ace::Job *)exec_retrieve();
	jobObject->Inject(env, param.jobid, param.jobname, param.jobquid, param.jobpartition);
	jobObject->Setup();
	jobObject->Run();
	jobObject->Teardown();

	int r = dlclose(handle);
	if (r)
		dlclose(handle);

	return;
}
