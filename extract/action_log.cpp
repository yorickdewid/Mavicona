#include <iostream>

#include "action_log.h"

bool Log::run() {
	(*m_Logfile) << "Item[" << m_Payload->id() << "] ----------< object >---------" << FileLogger::endl();
	(*m_Logfile) << "Item[" << m_Payload->id() << "] object: " << m_Payload->quid() << FileLogger::endl();
	(*m_Logfile) << "Item[" << m_Payload->id() << "] name: " << m_Payload->name() << FileLogger::endl();

	switch (m_Payload->type()) {
		case ScrapeData::PLAIN:
			(*m_Logfile) << "Item[" << m_Payload->id() << "] type: PLAIN" << FileLogger::endl();
			break;
		case ScrapeData::FILE:
			(*m_Logfile) << "Item[" << m_Payload->id() << "] type: FILE" << FileLogger::endl();
			break;
		case ScrapeData::BINARY:
			(*m_Logfile) << "Item[" << m_Payload->id() << "] type: BINARY" << FileLogger::endl();
			break;
		case ScrapeData::STREAM:
			(*m_Logfile) << "Item[" << m_Payload->id() << "] type: STREAM" << FileLogger::endl();
			break;
	}

	if (!m_Payload->content().extension().empty()) {
		(*m_Logfile) << "Item[" << m_Payload->id() << "] extension: " << m_Payload->content().extension() << FileLogger::endl();
	}

	(*m_Logfile) << "Item[" << m_Payload->id() << "] size: " << m_Payload->content().payload().size() << FileLogger::endl();

	for (int i = 0; i < m_Payload->meta_size(); i++) {
		ScrapeData::MetaEntry metaMime = m_Payload->meta(i);

		(*m_Logfile) << "Item[" << m_Payload->id() << "] " << metaMime.key();
		if (metaMime.has_value()) {
			(*m_Logfile) << " => " << metaMime.value() << FileLogger::endl();
		} else if (metaMime.meta_size()) {

			/* Decending */
			(*m_Logfile) << " =>" << FileLogger::endl();
			for (int j = 0; j < metaMime.meta_size(); j++) {
				ScrapeData::MetaEntry metaMime2 = metaMime.meta(j);

				(*m_Logfile) << "Item[" << m_Payload->id() << "]  " << metaMime2.key() << " => " << metaMime2.value() << FileLogger::endl();
			}

		} else {
			(*m_Logfile) << " => NULL" << FileLogger::endl();
		}
	}

	m_Logfile->flush();

	return true;
}
