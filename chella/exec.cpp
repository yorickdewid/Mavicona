#include <dlfcn.h>
#include <cassert>
#include <fstream>
#include <zmq.hpp>
#include <quidpp.h>
#include <package.h>
#include <Python.h>
#include "common/util.h"
#include "ace/interface.h"
#include "protoc/processjob.pb.h"
#include "dirent.h"
#include "wal.h"
#include "localenv.h"
#include "exec.h"

void Execute::init(ControlClient *control, const std::string& master, Indexer *db) {
	DIR *dir = nullptr;
	struct dirent *ent = nullptr;

	Execute& exec = Execute::getInstance();
	exec.jobcontrol = control;
	exec.master = master;
	exec.db = db;

	if ((dir = opendir(WALDIR)) != NULL) {

		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;

			Wal::rollback(ent->d_name, [](const std::string & name, Execute::Parameter & param) {

				/* Run procedure */
				Execute::run(name, param);

				/* Setup subjobs if any */
				Execute::prospect(name);
			});
		}

		closedir(dir);
	}
}

void Execute::run(const std::string& name, Parameter& param) {
	Wal *executionLog = new Wal(name, param);

	struct timeval t1, t2;
	char buff[PATH_MAX + 1];

	Execute& exec = Execute::getInstance();

	std::cout << "Running package " << name << std::endl;

	/* Move worker in accept mode */
	exec.jobcontrol->setStateAccepted();

	/* Set members of shared object via callback */
	exec.workerid = exec.jobcontrol->workerId();
	exec.clusterjobs = exec.jobcontrol->clusterJobs();
	exec.module = name;
	exec.jobid = param.jobid;
	exec.jobname = param.jobname;
	exec.jobquid = param.jobquid;
	exec.jobpartition = param.jobpartition;
	exec.jobpartition_count = param.jobpartition_count;
	exec.jobstate = param.jobstate;
	exec.jobparent = param.jobparent;
	char *cwd = getcwd(buff, PATH_MAX + 1);

	exec.jobcontrol->resetInternalState();

	/* Ensure package exist */
	if (!file_exist(PKGDIR "/" + name)) {
		exec.jobcontrol->setStateIdle();
		std::cerr << "Package does not exist" << std::endl;
		return;
	}

	if (!file_exist(LOCALDIR "/" + name + "/.jobhome")) {
		/* Extract package in job directory */
		if (package_extract((PKGDIR "/" + name).c_str(), (LOCALDIR "/" + name).c_str())) {
			std::cerr << "Cannot open package " << std::endl;
			return;
		}

		/* Setup job home */
		if (!LocalEnv::setupHome(LOCALDIR "/" + name, exec.jobid))
			return;

		/* Setup job home */
		if (!LocalEnv::setupEnv(LOCALDIR "/" + name))
			return;
	} else {
		std::cout << "Job environment already in cluster" << std::endl;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::INIT);

	wchar_t *program = Py_DecodeLocale("Chella Worker", NULL);
	if (!program) {
		std::cerr << "Cannot decode name" << std::endl;
		return;
	}

	/* Move to job home and prepare python */
	chdir((std::string(cwd) + "/" + LOCALDIR "/" + name).c_str());
	setenv("PYTHONPATH", ".", 1);
	
	Py_SetProgramName(program);
	Py_Initialize();

	PyObject *pModuleJob = PyImport_ImportModule("job_example");//TODO: parse json
	if (!pModuleJob) {
		PyErr_Print();
		return;
	}

	/* Initialize Ace modules */
	PyObject *pMethodConfig = Ace::Config::PyAce_ModuleClass();
	PyObject *pMethodCallback = Ace::IPC::PyAce_ModuleClass(exec.jobcontrol);
	PyObject *pMethodAsys = Ace::Asys::PyAce_ModuleClass();
	PyObject *pMethodDB = Ace::DB::PyAce_ModuleClass(exec.db);

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::LOAD);
	gettimeofday(&t1, NULL);

	/* Create Ace config instance */
	PyObject *pInstanceConfig = PyObject_CallObject(pMethodConfig, NULL);
	if (!pInstanceConfig) {
		PyErr_Print();
		return;
	}

	/* Create Ace callback instance */
	PyObject *pInstanceCallback = PyObject_CallObject(pMethodCallback, NULL);
	if (!pInstanceCallback) {
		PyErr_Print();
		return;
	}

	/* Create Ace DB instance */
	PyObject *pInstanceDB = PyObject_CallObject(pMethodDB, NULL);
	if (!pInstanceDB) {
		PyErr_Print();
		return;
	}

	/* Locate job init and call routine */
	PyObject *pFuncJobInit = PyObject_GetAttrString(pModuleJob, "job_init");
	if (!pFuncJobInit || !PyCallable_Check(pFuncJobInit)) {
		PyErr_Print();
		return;
	}

	PyObject *pArgsJobInit = Py_BuildValue("(O)", pInstanceConfig);
	PyObject *pInstanceJobInit = PyObject_CallObject(pFuncJobInit, pArgsJobInit);
	if (!pInstanceJobInit) {
		PyErr_Print();
		return;
	}

	PyObject *pFuncJobInvoke = PyObject_GetAttrString(pInstanceJobInit, "invoke");
	if (!pFuncJobInvoke || !PyCallable_Check(pFuncJobInvoke)) {
		PyErr_Print();
		return;
	}

	/* Instantiate job class */
	PyObject *pInstanceJob = PyObject_CallObject(pFuncJobInvoke, NULL);
	if (!pInstanceJob) {
		PyErr_Print();
		return;
	}
	
	/* Inject module instances into job instance */
	if (PyObject_SetAttrString(pInstanceJob, "cfg", pInstanceConfig) < 0) {
		PyErr_Print();
		return;
	}

	if (PyObject_SetAttrString(pInstanceJob, "__ipc__", pInstanceCallback) < 0) {
		PyErr_Print();
		return;
	}

	if (PyObject_SetAttrString(pInstanceJob, "__db__", pInstanceDB) < 0) {
		PyErr_Print();
		return;	
	}

	PyObject *pResult = NULL;
	pResult = PyObject_CallMethod(pInstanceJob, "inject", "(isssii)",
		exec.jobid,
		exec.jobname.c_str(),
		name.c_str(),
		exec.jobquid.c_str(),
		exec.jobpartition,
		exec.jobpartition_count);
	if (!pResult) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		return;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::INJECT);

	pResult = PyObject_CallMethod(pInstanceJob, "setup_once", NULL);
	if (!pResult) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		return;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::SETUP_ONCE);

	exec.jobcontrol->setStateSetup();
	pResult = PyObject_CallMethod(pInstanceJob, "setup", NULL);
	if (!pResult) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		return;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::SETUP);

	exec.jobcontrol->setStateRunning();
	pResult = PyObject_CallMethod(pInstanceJob, "run", "(y#)", param.jobdata.data(), param.jobdata.size());
	if (!pResult) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		return;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::RUN);

	exec.jobcontrol->setStateTeardown();
	pResult = PyObject_CallMethod(pInstanceJob, "teardown", NULL);
	if (!pResult) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		return;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::TEARDOWN);

	pResult = PyObject_CallMethod(pInstanceJob, "teardown_once", NULL);
	if (!pResult) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		return;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::TEARDOWN_ONCE);

	PyObject *pMemberChains = PyObject_GetAttrString(pInstanceJob, "chains");	
	if (!pMemberChains) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		return;
	}

	//TODO: handle chains

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::PULLCHAIN);
	gettimeofday(&t2, NULL);

	Py_DECREF(pResult);
	Py_DECREF(pMemberChains);
	Py_DECREF(pInstanceJob);
	Py_DECREF(pMethodDB);
	Py_DECREF(pMethodAsys);
	Py_DECREF(pMethodCallback);
	Py_DECREF(pMethodConfig);
	Py_DECREF(pModuleJob);

	/* Release resources allocated for this job */
	Py_Finalize();
	exec.sessionCleanup();
	chdir(cwd);

	/* Setup job home */
	if (!LocalEnv::teardown(LOCALDIR "/" + name, exec.jobid))
		return;

	/* Mark WAL done */
	executionLog->markDone();

	double runtime = (t2.tv_sec - t1.tv_sec) * 1000.0;
	runtime += (t2.tv_usec - t1.tv_usec) / 1000.0;

	// Write to disk
	std::cout << "Job routine reached end" << std::endl;
	std::cout << "Total runtime: " << runtime << std::endl;

	/* Move worker in idle mode */
	exec.jobcontrol->setStateIdle();

	delete executionLog;
}

void Execute::prospect(const std::string& name) {
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);
	Execute& exec = Execute::getInstance();

	if (!exec.chain) {
		return;
	}

	if (!file_exist(PKGDIR "/" + name)) {
		std::cerr << "Cannot access library" << std::endl;
		return;
	}

	std::ifstream ifs(PKGDIR "/" + name);
	std::string content((std::istreambuf_iterator<char>(ifs)),
						(std::istreambuf_iterator<char>()));

	socket.connect(("tcp://" + exec.master).c_str());
	std::cout << "Connect to master " << exec.master << std::endl;

	for (unsigned int i = 0; i < exec.chain->size(); ++i) {
		auto subjob = exec.chain->at(i);

		ProcessJob job;
		job.set_name(subjob->name);
		job.set_id(i);
		job.set_quid(quidpp::Quid().toString(true));
		job.set_content(content);
		job.set_partition(i);
		job.set_partition_count(exec.chain->size());
		job.set_state(ProcessJob::PARTITION);
		job.set_quid_parent(exec.chain->parentQuid());
		job.set_data(subjob->data);

		std::cout << "Submit subjob " << i << " linked to parent " << exec.chain->parentQuid() + "(" + exec.chain->parentName() + ")" << std::endl;

		std::string serialized;
		job.SerializeToString(&serialized);

		zmq::message_t request(serialized.size());
		memcpy(reinterpret_cast<void *>(request.data()), serialized.c_str(), serialized.size());
		socket.send(request);

		/* Get the reply */
		zmq::message_t reply;
		socket.recv(&reply);
	}

	delete exec.chain;
	exec.chain = nullptr;
}

void Execute::dispose() {
	DIR *dir = nullptr;
	struct dirent *ent = nullptr;

	if ((dir = opendir(PKGDIR)) != NULL) {

		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;
			remove((PKGDIR "/" + std::string(ent->d_name)).c_str());
		}

		closedir(dir);
	}

	if ((dir = opendir(WALDIR)) != NULL) {

		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;
			remove((WALDIR "/" + std::string(ent->d_name)).c_str());
		}

		closedir(dir);
	}
}
