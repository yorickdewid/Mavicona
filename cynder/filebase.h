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

	/* Substract counter from file */
	unsigned int pagecounter(std::string path) {
		std::string _dbname = path.substr(path.find_last_of("/") + 1);

		return atoi(_dbname.substr(3, 4).c_str());
	}

	unsigned int acquirePage(const std::string& file) {
		unsigned int pageNum = pagecounter(file);
		filepages[pageNum] = new Filepage(file);

		return pageNum;
	}

	unsigned int applicablePage() {
		for (auto const& v : filepages) {
			if (v.second->isFull())
				continue;

			return v.first;
		}

		return acquirePage(dbname(dir));
	}

  public:
	Filebase(unsigned int cnt = 0, const char *storagedir = defaultStorageDir) : name_prefix("lfb"), dir(storagedir) {
		mkdir(storagedir, 0700);

		assert(strlen(name_prefix) == 3);

		/* At least one page */
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

	std::vector<uint8_t> *get(unsigned int pageNum, std::string name) {
		return filepages[pageNum]->retrieveItem(name);
	}

	void remove(unsigned int pageNum, std::string name) {
		filepages[pageNum]->removeItem(name);
	}

	inline unsigned int dbcount() const {
		return counter;
	}

	~Filebase() {
		//TODO release pages
	}

};

#endif // FILEBASE_H
