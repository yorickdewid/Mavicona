#ifndef ACTION_LOG_H
#define ACTION_LOG_H

#include <fstream>

#include "action.h"

class Log : public Action {
	std::ofstream m_Logfile;

  public:
	Log(const std::string& logFileName, const ScrapeData *payload) : Action(payload) {
		m_Logfile.open(logFileName, std::ios::out | std::ios::app);
	}

	~Log() {
		m_Logfile.close();
	}

	bool run();

};

#endif // ACTION_LOG_H
