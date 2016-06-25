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
	std::map<unsigned int, Filepage *> filepages;

	const std::string dbname(const char *dir) {
		std::stringstream ss;
		ss << std::string(dir);
		ss << "/";
		ss << name_prefix;
		ss << std::setw(4) << std::setfill('0') << counter++;
		ss << "dxdb";

		return ss.str();
	}

	unsigned int pagecounter(std::string path) {
		std::string _dbname = path.substr(path.find_last_of("/") + 1);

		return atoi(_dbname.substr(3, 4).c_str());
	}

	void acquirePage(const std::string& file) {
		// TODO check if full
		filepages[pagecounter(file)] = new Filepage(file);
	}

	unsigned int applicablePage() {
		return 0; // TODO Walk over applicable pages based on prio
	}

  public:
	Filebase(unsigned int cnt = 0, const char *storagedir = defaultStorageDir) : name_prefix("lfb"), dir(storagedir) {
		mkdir(storagedir, 0700);

		assert(strlen(name_prefix) == 3);

		/* Always one page */
		acquirePage(dbname(dir));

		/* Initialize all pages */
		for (unsigned int i = 1; i < cnt; ++i)
			acquirePage(dbname(dir));
	}

	unsigned int put(std::string name, std::string data) {
		unsigned int pageNum = applicablePage();
		filepages[pageNum]->storeItem(name, data);

		return pageNum;
	}

	std::vector<uint8_t> *get(unsigned int page, std::string name) {
		return filepages[page]->retrieveItem(name);
	}

	void remove(unsigned int page, std::string name) {
		filepages[page]->removeItem(name);
	}

	inline unsigned int dbcount() {
		return counter;
	}

	~Filebase() {

	}

};

#endif // FILEBASE_H
