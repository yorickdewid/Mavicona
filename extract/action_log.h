#ifndef ACTION_LOG_H
#define ACTION_LOG_H

#include <fstream>

#include "common/logger.h"
#include "action.h"
	
class Log : public Action {
	FileLogger *m_Logfile = nullptr;
		FileLogger *kaas = NULL;

  public:
	Log(const std::string& logFileName, const ScrapeData *payload) : Action(payload) {
		m_Logfile = new FileLogger(logFileName);
	}

	~Log() {
		delete m_Logfile;
	}

	bool run();

};

#endif // ACTION_LOG_H
