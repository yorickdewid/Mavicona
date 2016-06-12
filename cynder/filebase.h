#ifndef FILEBASE_H
#define FILEBASE_H

#include <cassert>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

#include "filepage.h"

constexpr char defaultStorageDir[] = "data";

class Filebase {
	const char *name_prefix;
	const char *dir;
	unsigned int counter = 0;
	std::vector<Filepage *> filepages;

	const std::string dbname(const char *dir) {
		std::stringstream ss;
		ss << std::string(dir);
		ss << "/";
		ss << name_prefix;
		ss << std::setw(4) << std::setfill('0') << counter++;
		ss << "dxdb";

		return ss.str();
	}

	Filepage *createPage(const std::string& file) {
		Filepage *page = new Filepage();
		page->create(file);

		return page;
	}

	Filepage *openPage(const std::string& file) {
		Filepage *page = new Filepage();
		page->open(file);

		return page;
	}

  public:
	Filebase(const char *storagedir = defaultStorageDir) : name_prefix("lfb"), dir(storagedir) {
		mkdir(storagedir, 0700);

		assert(strlen(name_prefix) == 3);

		std::string file = dbname(dir);
		Filepage *page = createPage(file);
		filepages.push_back(page);

		page->verify();

		// Filepage *page = openPage(file);

		// openPage(file);
	}

	unsigned int put(std::string name, std::string data);

	std::string get(unsigned int page, std::string name);

	void remove(unsigned int page, std::string name);

	inline unsigned int dbcount() {
		return counter;
	}

	~Filebase() {

	}

};

#endif // FILEBASE_H
