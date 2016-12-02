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
	// pid_t pid = fork();
	// if (pid > 0)
	// 	return;

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

	LocalEnv jobenv(LOCALDIR "/" + name, exec.jobid);
	if (!jobenv.hasHome()) {
		/* Extract package in job directory */
		if (package_extract((PKGDIR "/" + name).c_str(), (LOCALDIR "/" + name).c_str())) {
			std::cerr << "Cannot open package " << std::endl;
			return;
		}

		/* Setup job home */
		if (!jobenv.setupHome())
			return;
	} else {
		std::cout << "Job environment already in cluster" << std::endl;
	}

	/* Setup job home */
	if (!jobenv.setupEnv())
		return;

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::INIT);

	wchar_t *program = Py_DecodeLocale("Chella Worker", NULL);
	if (!program) {
		std::cerr << "Cannot decode name" << std::endl;
		return;
	}

	/* Move to job home and prepare python */
	auto streamStdOut = jobenv.openStream(LocalEnv::STDOUT);
	auto streamStdErr = jobenv.openStream(LocalEnv::STDERR);
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
	PyObject *pModuleAsysStream1 = Ace::Asys::PyAce_ModuleClass(streamStdOut);
	PyObject *pModuleAsysStream2 = Ace::Asys::PyAce_ModuleClass(streamStdErr);
	PyObject *pMethodDB = Ace::DB::PyAce_ModuleClass(exec.db);

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::LOAD);
	gettimeofday(&t1, NULL);

	/* Set system settings */
	PySys_SetObject("stdout", pModuleAsysStream1);
	PySys_SetObject("stderr", pModuleAsysStream2);
	PySys_SetObject("stdin", pModuleAsysStream2);

	/* Create Ace config instance */
	PyObject *pInstanceConfig = PyObject_CallObject(pMethodConfig, NULL);
	if (!pInstanceConfig) {
		PyErr_Print();
		return;
	}
	Py_DECREF(pMethodConfig);

	/* Create Ace callback instance */
	PyObject *pInstanceCallback = PyObject_CallObject(pMethodCallback, NULL);
	if (!pInstanceCallback) {
		PyErr_Print();
		return;
	}
	Py_DECREF(pMethodCallback);

	/* Create Ace DB instance */
	PyObject *pInstanceDB = PyObject_CallObject(pMethodDB, NULL);
	if (!pInstanceDB) {
		PyErr_Print();
		return;
	}
	Py_DECREF(pMethodDB);

	/* Locate job init and call routine */
	PyObject *pFuncJobInit = PyObject_GetAttrString(pModuleJob, "job_init");
	if (!pFuncJobInit || !PyCallable_Check(pFuncJobInit)) {
		PyErr_Print();
		return;
	}
	Py_DECREF(pModuleJob);

	PyObject *pArgsJobInit = Py_BuildValue("(O)", pInstanceConfig);
	PyObject *pInstanceJobInit = PyObject_CallObject(pFuncJobInit, pArgsJobInit);
	if (!pInstanceJobInit) {
		PyErr_Print();
		return;
	}
	Py_DECREF(pFuncJobInit);
	Py_DECREF(pArgsJobInit);

	PyObject *pFuncJobInvoke = PyObject_GetAttrString(pInstanceJobInit, "invoke");
	if (!pFuncJobInvoke || !PyCallable_Check(pFuncJobInvoke)) {
		PyErr_Print();
		return;
	}
	Py_DECREF(pInstanceJobInit);

	/* Instantiate job class */
	PyObject *pInstanceJob = PyObject_CallObject(pFuncJobInvoke, NULL);
	if (!pInstanceJob) {
		PyErr_Print();
		return;
	}
	Py_DECREF(pFuncJobInvoke);

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

	Py_DECREF(pInstanceConfig);
	Py_DECREF(pInstanceCallback);
	Py_DECREF(pInstanceDB);

	PyObject *pMemberChains = NULL;
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
		goto py_failed;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::INJECT);

	pResult = PyObject_CallMethod(pInstanceJob, "setup_once", NULL);
	if (!pResult) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		goto py_failed;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::SETUP_ONCE);

	exec.jobcontrol->setStateSetup();
	pResult = PyObject_CallMethod(pInstanceJob, "setup", NULL);
	if (!pResult) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		goto py_failed;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::SETUP);

	exec.jobcontrol->setStateRunning();
	pResult = PyObject_CallMethod(pInstanceJob, "run", "(y#)", param.jobdata.data(), param.jobdata.size());
	if (!pResult) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		goto py_failed;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::RUN);

	exec.jobcontrol->setStateTeardown();
	pResult = PyObject_CallMethod(pInstanceJob, "teardown", NULL);
	if (!pResult) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		goto py_failed;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::TEARDOWN);

	pResult = PyObject_CallMethod(pInstanceJob, "teardown_once", NULL);
	if (!pResult) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		goto py_failed;
	}

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::TEARDOWN_ONCE);

	pMemberChains = PyObject_GetAttrString(pInstanceJob, "chains");	
	if (!pMemberChains) {
		PyErr_Print();
		exec.jobcontrol->setStateFailed();
		goto py_failed;
	}

	//TODO: handle chains

	/* Move WAL forward */
	executionLog->setCheckpoint(Wal::Checkpoint::PULLCHAIN);
py_failed:
	gettimeofday(&t2, NULL);

	/* Release resources allocated for this job */
	Py_Finalize();
	exec.sessionCleanup();
	chdir(cwd);

	/* Close output streams */
	streamStdOut->close();
	streamStdErr->close();
	delete streamStdOut;
	delete streamStdErr;

	/* Mark WAL done */
	executionLog->markDone();

	double runtime = (t2.tv_sec - t1.tv_sec) * 1000.0;
	runtime += (t2.tv_usec - t1.tv_usec) / 1000.0;

	/* Write to disk */
	std::cout << "Job routine reached end" << std::endl;
	std::cout << "Total runtime: " << runtime << "ms" << std::endl;

	/* Teardown */
	jobenv.teardown(runtime);

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

	std::cout << "Clear caches" << std::endl;

	if ((dir = opendir(PKGDIR)) != NULL) {

		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;
			remove((PKGDIR "/" + std::string(ent->d_name)).c_str());
		}

		closedir(dir);
	}
}
