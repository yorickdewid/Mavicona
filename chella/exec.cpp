#include <dlfcn.h>
#include <cassert>
#include "common/util.h"
#include "ace/interface.h"
#include "exec.h"

typedef int (*regclass_t)();
typedef Ace::Job *(*facade_t)();

void Execute::run(const std::string& name, Parameter& param, ControlClient& control) {
	Ace::Environment env;

	std::cout << "Running module " << std::endl;

	/* Move worker in accept mode */
	control.setStateAccepted();

	env.SetModule(name);
	env.SetWorkerIdent(control.workerIdent());
	env.SetClusterJobs(control.clusterJobs());

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
	facade_t exec_facade = (Ace::Job * (*)()) dlsym(handle, "object_facade");

	assert(exec_register() == ACE_MAGIC);
	Ace::Job *jobObject = (Ace::Job *)exec_facade();
	jobObject->Inject(env, param.jobid, param.jobname, param.jobquid, param.jobpartition);

	/* Call setup routine */
	control.setStateSetup();
	jobObject->Setup();

	/* Call main routine */
	control.setStateRunning(256);
	jobObject->Run();

	/* Call teardown routine */
	control.setStateTeardown();
	jobObject->Teardown();

	int r = dlclose(handle);
	if (r)
		dlclose(handle);

	/* Move worker in idle mode */
	control.setStateIdle();

	return;
}
