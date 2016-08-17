#include <iostream>
#include <cstring>
#include <fcntl.h>
#include "archive.h"

Archive::Archive(const char *file) {
	if (tar_open(&t, file, NULL, O_RDONLY, 0, 0) < 0) {
		std::cerr << strerror(errno) << std::endl;
	}
}

Archive::~Archive() {
	if (tar_close(t) < 0) {
		std::cerr << strerror(errno) << std::endl;
	}
}

void Archive::list() {
	int i;

	while ((i = th_read(t)) == 0) {
		th_print_long_ls(t);
		if (TH_ISREG(t) && tar_skip_regfile(t) != 0) {
			std::cerr << strerror(errno) << std::endl;
		}
	}
}

void Archive::extract(const char *dest) {
	if (tar_extract_all(t, (char *)dest) < 0) {
		std::cerr << strerror(errno) << std::endl;
	}
}
