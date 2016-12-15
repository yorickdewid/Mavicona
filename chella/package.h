#ifndef PACKAGE_H
#define PACKAGE_H

#include <iostream>

class Package {
	PAR *t;
	std::string packagename;

  public:
	Package(const std::string name) : packagename(name) {
		//
	}

	void verify();
	void extract(const std::string& rootdir);
	void create(const std::string& rootdir, libtar_list_t *list);
};

#endif // PACKAGE_H
