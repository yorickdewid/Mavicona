#include <quidpp.h>

#include "key_index.h"

void KeyIndex::put(std::string quid, std::string key, std::string value) {
	if (quid.size() != quidpp::Quid::unpackedSize()) {
		quid = quidpp::Quid::crop(quid);
		if (quid.empty()) {
			std::cerr << "Invalid QUID concatenation" << std::endl;
			return;
		}
	}

	normalize(key);

	if (!matchKeyword(key))
		return;

	std::cout << "Key quallifies for storage " << key << " -> " << value << std::endl;
	AbstractEngine::put(key, quid + value, false, true);
}
