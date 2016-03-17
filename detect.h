#ifndef DETECT_H
#define DETECT_H

#include <map>
#include <cstddef>
#include <iostream>

#include "mime.h"

class Detect {
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

	void mimeFromBuffer(const char *buffer, std::size_t sz);
	void mimeFromExtension(const std::string& extension);

	Detect(const Detect &) = delete;
	Detect &operator= (const Detect &) = delete;

  private:
	void parseMagic(const char *magicrs);
	void loadMime();

	std::string m_Charset;
	Mime *mimeMatch = nullptr;
	std::map<std::string, Mime*> mimeList;
};

#endif // DETECT_H
