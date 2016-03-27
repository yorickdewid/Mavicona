#include <iostream>

#include "action_log.h"

bool Log::run() {
	m_Logfile << "Item[" << m_Payload->id() << "] -----------ITEM----------" << std::endl;

	switch (m_Payload->type()) {
		case ScrapeData::PLAIN:
			m_Logfile << "Item[" << m_Payload->id() << "] type: PLAIN" << std::endl;
			break;
		case ScrapeData::FILE:
			m_Logfile << "Item[" << m_Payload->id() << "] type: FILE" << std::endl;
			break;
		case ScrapeData::BINARY:
			m_Logfile << "Item[" << m_Payload->id() << "] type: BINARY" << std::endl;
			break;
		case ScrapeData::STREAM:
			m_Logfile << "Item[" << m_Payload->id() << "] type: STREAM" << std::endl;
			break;
	}

	for (int i = 0; i < m_Payload->meta_size(); i++) {
		ScrapeData::MetaEntry metaMime = m_Payload->meta(i);

		m_Logfile << "Item[" << m_Payload->id() << "] " << metaMime.key();
		if (metaMime.has_value()) {
			m_Logfile << " => " << metaMime.value() << std::endl;
		} else if (metaMime.meta_size()) {

			/* Decending */
			m_Logfile << " => <...> " << std::endl;;
			for (int j = 0; j < metaMime.meta_size(); j++) {
				ScrapeData::MetaEntry metaMime2 = metaMime.meta(j);

				m_Logfile << "Item[" << m_Payload->id() << "]  " << metaMime2.key() << " => " << metaMime2.value() << std::endl;
			}

		} else {
			m_Logfile << " => NULL" << std::endl;
		}
	}

	m_Logfile.flush();

	return true;
}
