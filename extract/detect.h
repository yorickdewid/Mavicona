#ifndef DETECT_H
#define DETECT_H

#include <map>
#include <cstddef>
#include <iostream>

#include "mime.h"

class Detect {
	void parseMagic(const char *magicrs);
	void parseCharset(std::string charset);
	void loadMime();

	std::string m_Charset;
	Mime *mimeMatch = nullptr;
	std::map<std::string, Mime*> mimeList;

  public:
	Detect() {
		loadMime();
	}

	~Detect();

	bool found() {
		if (mimeMatch)
			return true;

		return false;
	}

	Mime *mime() {
		return mimeMatch;
	}

	std::string charset() {
		return m_Charset;
	}

	void notify() {
		if (!found())
			return;
	}

	void attach() {

	}

	void mimeFromBuffer(const char *buffer, std::size_t sz);
	void mimeFromExtension(const std::string& extension);

	Detect(const Detect &) = delete;
	Detect &operator= (const Detect &) = delete;
};

#endif // DETECT_H
