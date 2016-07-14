#include <dlfcn.h>
#include <cassert>
#include "common/util.h"
#include "ace/interface.h"
#include "protoc/processjob.pb.h"
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
	exec->jobpartition_count = param.jobpartition_count;
	exec->jobstate = param.jobstate;
	exec->jobparent = param.jobparent;

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

	/* Call this setup once in the cluster */
	if (exec->jobstate == SPAWN) {
		exec->jobcontrol->setStateSetup();
		jobObject->SetupOnce();
	}

	/* Call setup routine */
	exec->jobcontrol->setStateSetup();
	jobObject->Setup();

	/* Call main routine */
	exec->jobcontrol->setStateRunning();
	jobObject->Run();

	/* Call teardown routine */
	exec->jobcontrol->setStateTeardown();
	jobObject->Teardown();

	/* Call this teardown once in the cluster */
	if (exec->jobstate == FUNNEL) {
		exec->jobcontrol->setStateSetup();
		jobObject->TeardownOnce();
	}

	/* Pull the chain */
	exec->chain = jobObject->PullChain();

	int r = dlclose(handle);
	if (r)
		dlclose(handle);


	/* Release resources allocated for this job */
	exec->sessionCleanup();

	/* Move worker in idle mode */
	exec->jobcontrol->setStateIdle();

	return;
}

void Execute::prospect() {
	Execute *exec = &Execute::getInstance();

	if (!exec->chain) {
		std::cout << "no chain, done" << std::endl;
		return;
	}

	std::cout << "Chain found" << std::endl;
	std::cout << "Subjobs " << exec->chain->size() << std::endl;

	for (unsigned int i = 0; i < exec->chain->size(); ++i) {
		auto subjob = exec->chain->at(i);

		ProcessJob job;
		job.set_name(subjob->name);
		job.set_id(i);
		job.set_quid("5f7920eb-ef78-4a2a-a04e-e8763cf35716");
		job.set_content(subjob->content);
		job.set_partition(i);
		job.set_partition_count(exec->chain->size());
		job.set_state(ProcessJob::PARTITION);
		job.set_quid_parent(exec->chain->parentQuid());

		std::cout << "Submit subjob " << i << " linked to parent " << exec->chain->parentQuid() + "(" + exec->chain->parentName() + ")" << std::endl;
	}

	delete exec->chain;
}
