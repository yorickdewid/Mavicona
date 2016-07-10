#include <dlfcn.h>
#include <cassert>
#include "common/util.h"
#include "ace/interface.h"
#include "exec.h"

typedef int (*regclass_t)();
typedef Ace::Job *(*facade_t)();

void Execute::run(const std::string& name, Parameter& param) {
	Execute *exec = &Execute::getInstance();

	std::cout << "Running module " << std::endl;

	/* Move worker in accept mode */
	exec->jobcontrol->setStateAccepted();

	/* Set members of shared object via callback */
	exec->workerid = exec->jobcontrol->workerId();
	exec->clusterjobs = exec->jobcontrol->clusterJobs();
	exec->module = name;
	exec->jobid = param.jobid;
	exec->jobname = param.jobname;
	exec->jobquid = param.jobquid;
	exec->jobpartition = param.jobpartition;

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
	jobObject->Inject(exec);

	/* Call setup routine */
	exec->jobcontrol->setStateSetup();
	jobObject->Setup();

	/* Call main routine */
	exec->jobcontrol->setStateRunning();
	jobObject->Run();

	/* Call teardown routine */
	exec->jobcontrol->setStateTeardown();
	jobObject->Teardown();

	int r = dlclose(handle);
	if (r)
		dlclose(handle);

	/* Release resources allocated for this job */
	exec->sessionCleanup();

	/* Move worker in idle mode */
	exec->jobcontrol->setStateIdle();

	return;
}
