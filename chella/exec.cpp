#include <dlfcn.h>
#include <cassert>
#include "common/util.h"
#include "ace/interface.h"
#include "protoc/processjob.pb.h"
#include "dirent.h"
#include "wal.h"
#include "exec.h"

typedef int (*regclass_t)();
typedef Ace::Job *(*facade_t)();

void Execute::init(ControlClient *control) {
	DIR *dir = nullptr;
	struct dirent *ent = nullptr;

	Execute& exec = Execute::getInstance();
	exec.setControl(control);

	if ((dir = opendir("cache/wal/")) != NULL) {

		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;

			Wal::rollback(ent->d_name);
		}

		closedir(dir);
	} else {

		/* could not open directory */
		perror("");
	}
}

void Execute::run(const std::string& name, Parameter& param) {
	Wal *executionLog = new Wal(name, param);

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

	executionLog->setCheckpoint(Wal::Checkpoint::INIT);

	if (!file_exist("cache/module/" + name)) {
		std::cerr << "Cannot access library" << std::endl;
		return;
	}

	void *handle = dlopen(("cache/module/" + name).c_str(), RTLD_LAZY);
	if (!handle) {
		std::cerr << "Cannot open library: " << dlerror() << std::endl;
		return;
	}

	regclass_t exec_register = (int (*)()) dlsym(handle, "register_class");
	facade_t exec_facade = (Ace::Job * (*)()) dlsym(handle, "object_facade");
	executionLog->setCheckpoint(Wal::Checkpoint::LOAD);

	/* Inject job and cluster */
	assert(exec_register() == ACE_MAGIC);
	Ace::Job *jobObject = (Ace::Job *)exec_facade();
	try {
		jobObject->Inject(exec);
		executionLog->setCheckpoint(Wal::Checkpoint::INJECT);
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	/* Call this setup once in the cluster */
	if (exec->jobstate == SPAWN) {
		try {
			exec->jobcontrol->setStateSetup();
			jobObject->SetupOnce();
			executionLog->setCheckpoint(Wal::Checkpoint::SETUP_ONCE);
		} catch (const std::exception& ex) {
			std::cerr << ex.what() << std::endl;
		}
	}

	/* Call setup routine */
	try {
		exec->jobcontrol->setStateSetup();
		jobObject->Setup();
		executionLog->setCheckpoint(Wal::Checkpoint::SETUP);
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	/* Call main routine */
	try {
		exec->jobcontrol->setStateRunning();
		jobObject->Run();
		executionLog->setCheckpoint(Wal::Checkpoint::RUN);
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	/* Call teardown routine */
	try {
		exec->jobcontrol->setStateTeardown();
		jobObject->Teardown();
		executionLog->setCheckpoint(Wal::Checkpoint::TEARDOWN);
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	/* Call this teardown once in the cluster */
	if (exec->jobstate == FUNNEL) {
		try {
			exec->jobcontrol->setStateSetup();
			jobObject->TeardownOnce();
			executionLog->setCheckpoint(Wal::Checkpoint::TEARDOWN_ONCE);
		} catch (const std::exception& ex) {
			std::cerr << ex.what() << std::endl;
		}
	}

	/* Pull the chain */
	try {
		exec->chain = jobObject->PullChain();
		executionLog->setCheckpoint(Wal::Checkpoint::PULLCHAIN);
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	int r = dlclose(handle);
	if (r)
		dlclose(handle);

	/* Release resources allocated for this job */
	exec->sessionCleanup();

	/* Move worker in idle mode */
	exec->jobcontrol->setStateIdle();

	/* Mark WAL done */
	executionLog->markDone();

	delete executionLog;
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

void Execute::dispose() {
	DIR *dir = nullptr;
	struct dirent *ent = nullptr;

	if ((dir = opendir("cache/module/")) != NULL) {

		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;
			remove(("cache/module/" + std::string(ent->d_name)).c_str());
		}

		closedir(dir);
	} else {

		/* could not open directory */
		perror("");
	}

	if ((dir = opendir("cache/wal/")) != NULL) {

		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;
			remove(("cache/wal/" + std::string(ent->d_name)).c_str());
		}

		closedir(dir);
	} else {

		/* could not open directory */
		perror("");
	}
}
