#ifndef PARSE_OBSERVER_H
#define PARSE_OBSERVER_H

#include "detect.h"

class ParseObserver {
	Detect *m_Model;
	std::string m_MimeType;

  public:
	ParseObserver(Detect *mod, const std::string mimeType) : m_Model(mod), m_MimeType(mimeType) {
		mod->attach(mimeType, this);
	}

	virtual void handle() = 0;

  protected:
	Detect *getSubject() {
		return m_Model;
	}

};

#endif // PARSE_OBSERVER_H
