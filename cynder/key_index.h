#ifndef KEY_INDEX_H
#define KEY_INDEX_H

#include "engine.h"

class KeyIndex : public AbstractEngine {
	std::vector<std::string> commonIndexKeywords = {{
		"name", "author", "owner", "user", "username",
		"date", "datetime", "timestamp", "locale",
		"age", "type", "classification", "category",
		"group", "set", "rating", "result", "status",
		"location", "language", "place", "target",
		"url", "address", "path", "description",
		"remark", "subject", "comment",	"note",	"tag",
	}};

  public:
	KeyIndex() : AbstractEngine("uki", true) {}

	void put(std::string quid, std::string key, std::string value, bool override = false) {
		if (quid.size() != quidpp::Quid::unpackedSize()) {
			std::cerr << "Invalid QUID concatenation" << std::endl;
		}

		std::vector<std::string>::iterator it = find(commonIndexKeywords.begin(), commonIndexKeywords.end(), key);
		if (it == commonIndexKeywords.end())
			return;
		
		std::cout << "Key quallifies for storage\n";
		AbstractEngine::put(key, quid + value, override);
	}

};

#endif // KEY_INDEX_H
