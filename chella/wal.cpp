#include <cstring>
#include <ctime>
#include "wal.h"

#define LOG_MAGIC		"WALU41Q"	/* Magic check */

struct pageHeader {
	char magic[8];
	char module[40];
	char quid[36];
	char parent_quid[36];
	unsigned char failcount = 1;
	unsigned char done = 0;
	unsigned int jobid;
	unsigned int jobpartition;
	unsigned int jobpartition_count;
	int jobstate;
	int jobname_size;
	unsigned int timestamp;
	enum Wal::Checkpoint checkpoint = Wal::Checkpoint::NIL;
};

void Wal::commit(bool isDone) {
	rewind(m_pFile);

	pageHeader header;
	memset(&header, '\0', sizeof(pageHeader));
	strcpy(header.magic, LOG_MAGIC);
	strncpy(header.module, module.c_str(), 40);
	strncpy(header.quid, jobparameters.jobquid.c_str(), 36);
	strncpy(header.parent_quid, jobparameters.jobparent.c_str(), 36);
	header.checkpoint = this->checkpoint;
	header.done = isDone ? 1 : 0;
	header.jobid = jobparameters.jobid;
	header.jobpartition = jobparameters.jobpartition;
	header.jobpartition_count = jobparameters.jobpartition_count;
	header.jobstate = static_cast<int>(jobparameters.jobstate);
	header.jobname_size = jobparameters.jobname.size();
	header.timestamp = (unsigned int)time(NULL);
	fwrite((const char *)&header, sizeof(pageHeader), 1, m_pFile);
	fwrite(jobparameters.jobname.c_str(), sizeof(char), jobparameters.jobname.size(), m_pFile);
}

void Wal::setCheckpoint(enum Wal::Checkpoint _checkpoint) {
	this->checkpoint = _checkpoint;
	commit();
}

void Wal::rollback(const std::string& name, std::function<void(const std::string& name, Execute::Parameter& param)> const& callback) {
	pageHeader header;
	short recovery_likeliness = 50;

	FILE *m_pFile = fopen((WALDIR "/" + name).c_str(), "r+");
	if (!m_pFile){
		std::cerr << "Cannot read WAL log" << std::endl;
		return;
	}
	fread(&header, sizeof(pageHeader), 1, m_pFile);

	if (strcmp((const char *)header.magic, LOG_MAGIC)) {
		std::cerr << "Magic error" << std::endl;
		return;
	}

	std::cout << "Checking log " << std::string(header.quid, 36) << ":" << header.jobid << std::endl;

	/* Check if WAL was marked done */
	if (header.done) {
		fclose(m_pFile);
		unlink((WALDIR "/" + name).c_str());
		return;
	}

	/* Only try so many times */
	recovery_likeliness -= (header.failcount * -10);

	/* Is module still in cache */
	if (!file_exist(PKGDIR "/" + std::string(header.module, 40))) {
		std::cerr << "Module missing, unable to recover" << std::endl;
		recovery_likeliness -= 75;
	}

	enum Wal::Checkpoint checkpoint = header.checkpoint;
	switch (checkpoint) {
		case Wal::Checkpoint::NIL:
			// puts("Checkpoint NIL");
			recovery_likeliness += 30;
			break;
		case Wal::Checkpoint::INIT:
			// puts("Checkpoint INIT");
			recovery_likeliness += 20;
			break;
		case Wal::Checkpoint::LOAD:
			// puts("Checkpoint LOAD");
			recovery_likeliness += 10;
			break;
		case Wal::Checkpoint::INJECT:
			// puts("Checkpoint INJECT");
			recovery_likeliness += 20;
			break;
		case Wal::Checkpoint::SETUP_ONCE:
			// puts("Checkpoint SETUP_ONCE");
			std::cout << "Possible job failure due to SIGSEGV in SetupOnce()" << std::endl;
			recovery_likeliness += 15;
			break;
		case Wal::Checkpoint::SETUP:
			// puts("Checkpoint SETUP (User error)");
			std::cout << "Possible job failure due to SIGSEGV in Setup()" << std::endl;
			recovery_likeliness += 10;
			break;
		case Wal::Checkpoint::RUN:
			// puts("Checkpoint RUN (User error)");
			std::cout << "Possible job failure due to SIGSEGV in Run()" << std::endl;
			recovery_likeliness += 5;
			break;
		case Wal::Checkpoint::TEARDOWN:
			// puts("Checkpoint TEARDOWN (User error)");
			std::cout << "Possible job failure due to SIGSEGV in Teardown()" << std::endl;
			recovery_likeliness += 20;
			break;
		case Wal::Checkpoint::TEARDOWN_ONCE:
			// puts("Checkpoint TEARDOWN_ONCE (User error)");
			std::cout << "Possible job failure due to SIGSEGV in TeardownOnce()" << std::endl;
			recovery_likeliness += 25;
			break;
		case Wal::Checkpoint::PULLCHAIN:
			// puts("Checkpoint PULLCHAIN");
			recovery_likeliness += 30;
			break;
	}

	Callback::JobState jobstate = static_cast<Callback::JobState>(header.jobstate);
	switch (jobstate) {
		case Callback::JobState::SPAWN:
			// puts("Jobstate SPAWN");
			recovery_likeliness += 15;
			break;
		case Callback::JobState::PARTITION:
			// puts("Jobstate PARTITION");
			recovery_likeliness += 20;
			break;
		case Callback::JobState::FUNNEL:
			// puts("Jobstate FUNNEL");
			recovery_likeliness += 10;
			break;
	}

	char *namebuf =  new char[header.jobname_size + 1];
	fread(namebuf, sizeof(char), header.jobname_size, m_pFile);
	namebuf[header.jobname_size] = '\0';

	std::cout << "Likeliness of recovery score " << recovery_likeliness << std::endl;
	if (recovery_likeliness < 40) {
		std::cerr << "Threshold to low, giving up..." << std::endl;
		return;
	}

	/* Gather parameters for job */
	Execute::Parameter parameters;
	parameters.jobid = header.jobid;
	parameters.jobname = namebuf;
	parameters.jobquid = std::string(header.quid, 36);
	parameters.jobpartition = header.jobpartition;
	parameters.jobpartition_count = header.jobpartition_count;
	parameters.jobstate = jobstate;
	parameters.jobparent = std::string(header.parent_quid, 36);

	callback(std::string(header.module, 40), parameters);

	delete[] namebuf;

	fclose(m_pFile);
}
