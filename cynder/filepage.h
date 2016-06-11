#ifndef FILEPAGE_H
#define FILEPAGE_H

#include <cassert>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

#define ITEM_SIZE	8 * 1024 * 1024

constexpr char defaultStorageDir[] = "data";

class Filepage {
	const char *name_prefix;
	const char *dir;
	unsigned int counter = 0;

	const std::string dbname(const char *dir) {
		std::stringstream ss;
		ss << std::string(dir);
		ss << "/";
		ss << name_prefix;
		ss << std::setw(4) << std::setfill('0') << counter++;
		ss << "dxdb";

		return ss.str();
	}

	void createPage();

  public:
	Filepage(const char *storagedir = defaultStorageDir) : name_prefix("lfb"), dir(storagedir) {
		mkdir(storagedir, 0700);

		assert(strlen(name_prefix) == 3);
	}

	unsigned int store(std::string name, std::string data);

	std::string retrieve(unsigned int page, std::string name);

	void purge(unsigned int page, std::string name);

	inline unsigned int dbcount() {
		return counter;
	}

	~Filepage() {

	}

};

#endif // FILEPAGE_H
