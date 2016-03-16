#include <map>

#include "detect.h"
#include "magic.h"
#include "mime.h"

void Detect::loadMime() {
	this->mimeList["application/vnd.hzn-3d-crossword"] = new Mime("3D Crossword Plugin", "application/vnd.hzn-3d-crossword", ".x3d");
	this->mimeList["video/3gpp"] = new Mime("3GP", "video/3gpp", ".3gp");
	this->mimeList["text/plain"] = new Mime("Text File", "text/plain", ".txt");
	this->mimeList["application/x-empty"] = new Mime("Empty", "application/x-empty");
}

void Detect::parseMagic(const char *magicrs) {
	std::string str(magicrs);
	std::size_t pos = str.find(';');

	if (pos != std::string::npos) {

		/* Can we match this to any known mime types */
		this->mimeMatch = this->mimeList[str.substr(0, pos)];
		if (!this->mimeMatch)
			return;

		if (str.at(pos + 1) == ' ') {
			++pos;
		}

		this->m_Charset = str.substr(pos + 1);
	}

}

void Detect::mimeFromBuffer(const char *buffer, std::size_t sz) {
	magic_t myt = magic_open(MAGIC_CONTINUE | MAGIC_ERROR | MAGIC_MIME);

	magic_load(myt, NULL);
	const char *rs = magic_buffer(myt, buffer, sz);
	if (rs)
		parseMagic(rs);

	magic_close(myt);
}
