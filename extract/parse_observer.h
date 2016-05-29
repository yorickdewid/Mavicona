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

	int getId() {
		return getSubject()->getDataProfile()->id();
	}

	const ScrapeData::Data *getPayload() {
		return &getSubject()->getDataProfile()->content();
	}

	inline void addMeta(const std::string& key, const std::string& value) {
		ScrapeData::MetaEntry *meta = getSubject()->getDataProfile()->add_meta();
		meta->set_key(key);
		meta->set_value(value);
	}

	inline void addMeta(const std::string& key, const int value) {
		ScrapeData::MetaEntry *meta = getSubject()->getDataProfile()->add_meta();
		meta->set_key(key);
		meta->set_value(std::to_string(value));
	}

};

#endif // PARSE_OBSERVER_H
