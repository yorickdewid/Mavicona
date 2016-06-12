#ifndef FILEPAGE_H
#define FILEPAGE_H

#include <cassert>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

#define ITEM_SIZE	8 * 1024 * 1024

class Filepage {
	bool m_Open = false;
	std::fstream fs;

  public:
	Filepage() {

	}

	void create(const std::string& file);
	void open(const std::string& file);
	bool verify();

	unsigned int storeItem(std::string name, std::string data);

	std::string retrieveItem(unsigned int page, std::string name);

	void removeItem(unsigned int page, std::string name);

	inline bool is_open() {
		return m_Open;
	}

	~Filepage() {
		if (m_Open)
			fs.close();
	}

};

#endif // FILEPAGE_H
