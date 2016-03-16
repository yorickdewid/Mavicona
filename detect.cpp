#include "detect.h"
#include "magic.h"

const char *Detect::parseMagic(const char *magicrs) {
	printf("[%s]\n", magicrs);

	std::string str(magicrs);
	std::size_t pos = str.find(';');

	if (pos != std::string::npos) {
		std::cout << "[" << str.substr(0, pos) << "]" << std::endl;

		if (str.at(pos + 1) == ' ') {
			++pos;
		}

		std::cout << "[" << str.substr(pos + 1) << "]" << std::endl;
	}

}

const char *Detect::magicFromBuffer(const char *buffer, std::size_t sz) {
	magic_t myt = magic_open(MAGIC_CONTINUE | MAGIC_ERROR | MAGIC_MIME);

	magic_load(myt, NULL);
	const char *rs = magic_buffer(myt, buffer, sz);
	parseMagic(rs);

	magic_close(myt);
	return NULL;
}

Detect::Detect(const std::string& data) {
	magicFromBuffer(data.c_str(), data.size());
}