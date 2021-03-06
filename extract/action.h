#ifndef ACTION_H
#define ACTION_H

#include "protoc/scrapedata.pb.h"

class Action {
  protected:
	const ScrapeData *m_Payload;

  public:
	Action(const ScrapeData *payload) : m_Payload(payload) {}

	virtual ~Action() {}

	virtual bool config(const std::string& configfile) {
		return true;
	}

	virtual bool run() = 0;
};

#endif // ACTION_H
