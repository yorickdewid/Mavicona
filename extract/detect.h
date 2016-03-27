#ifndef DETECT_H
#define DETECT_H

#include <map>
#include <cstddef>
#include <iostream>

#include "protoc/scrapedata.pb.h"
#include "mime.h"

class Detect {
	void parseMagic(const char *magicrs);
	void parseCharset(std::string charset);
	void loadMime();
	void loadMimeParser();

	std::string m_Charset;
	Mime *mimeMatch = nullptr;
	std::map<std::string, Mime*> mimeList;
	std::map<std::string, class ParseObserver *> mimeParserList;
	const ScrapeData *payload = nullptr;

  public:
	Detect() {
		loadMime();
		loadMimeParser();
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


	void attach(const std::string mimeType, ParseObserver *ob) {
		this->mimeParserList[mimeType] = ob;
	}

	void notify();
	void mimeFromBuffer(const char *buffer, std::size_t sz);
	void mimeFromExtension(const std::string& extension);
	void setDataProfile(const ScrapeData &data);
	const ScrapeData *GetDataProfile();

	Detect(const Detect &) = delete;
	Detect &operator= (const Detect &) = delete;
};

#endif // DETECT_H
