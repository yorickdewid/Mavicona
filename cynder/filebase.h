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

	Filepage *aquirePage(const std::string& file) {
		Filepage *page = new Filepage(file);
		filepages.push_back(page);

		return page;
	}

  public:
	Filebase(const char *storagedir = defaultStorageDir) : name_prefix("lfb"), dir(storagedir) {
		mkdir(storagedir, 0700);

		assert(strlen(name_prefix) == 3);
	}

	unsigned int put(std::string name, std::string data) {
		Filepage *page = aquirePage(dbname(dir));

		page->storeItem(name, data);

		return counter - 1; //TODO hack
	}

	std::string get(unsigned int page, std::string name);

	void remove(unsigned int page, std::string name);

	inline unsigned int dbcount() {
		return counter;
	}

	~Filebase() {

	}

};

#endif // FILEBASE_H
