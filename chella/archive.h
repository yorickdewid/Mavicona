#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <libtar.h>

class Archive {
	TAR *t;

  public:
	Archive(const char *file);
	Archive(const std::string& file) : Archive(file.c_str()) {}

	~Archive();

	void list();
	void extract(const char *dest);
	void extract(const std::string& dest) {
		extract(dest.c_str());
	}

};

#endif /* ARCHIVE_H */
