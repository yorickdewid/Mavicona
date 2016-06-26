#include <quidpp.h>

#include "text_index.h"

void TextIndex::put(std::string quid, std::string key, std::string value) {
	if (quid.size() != quidpp::Quid::unpackedSize()) {
		quid = quidpp::Quid::crop(quid);
		if (quid.empty()) {
			std::cerr << "Invalid QUID concatenation" << std::endl;
			return;
		}
	}

	normalize(value);

	if (!matchKeyword(value))
		return;

	std::cout << "Value quallifies for storage " << key << " -> " << value << std::endl;
	AbstractEngine::put(key, quid + value, false, true);
}
