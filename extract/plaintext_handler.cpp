#include <ctime>
#include <algorithm>

#include "plaintext_handler.h"

bool isNumber(const std::string& s) {
	return std::find_if(s.begin(), s.end(), [](char c) -> bool {
		return !std::isdigit(c);
	}) == s.end();
}

bool isHex(const std::string& s) {
	return std::find_if(s.begin(), s.end(), [](char c) -> bool {
		return !std::isxdigit(c);
	}) == s.end();
}

std::string timestampToString(const time_t rawtime) {
	struct tm * dt;
	char buffer [30];
	dt = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%FT%TZ", dt);
	return std::string(buffer);
}

void PlainTextHandler::handle() {
	std::cout << "Plain text handler called" << std::endl;

	const ScrapeData::Data *x = getPayload();

	switch (x->payload().size()) {
		case 32:
			/* This may indicate an MD5 sum */
			if (x->payload().find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos) {
				addMeta("hash", "md5");
			}
			break;

		case 40:
			/* This may indicate an SHA1 sum */
			if (x->payload().find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos) {
				addMeta("hash", "sha");
			}
			break;
	}

	if (isNumber(x->payload())) {
		int iDec = std::stoi(x->payload());

		/* This could pass as timstamp */
		if (x->payload().size() > 4 && x->payload().size() < 11) {
			std::time_t result = (int)iDec;
			addMeta("timestamp", timestampToString(result));
		}
	}

}