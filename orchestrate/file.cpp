#include <sstream>

#include "file.h"

bool CFile::Open(std::string filepath) {
	const char *s = filepath.c_str();

	/* Open file */
	return Open(s);
}

bool CFile::Open(const char *filepath) {
	std::stringstream spath;

	/* Close file */
	Close();

	/* Save name */
	name = filepath;

	/* Open file */
	file.open(filepath);

	/* Check if opened */
	return file.is_open();
}

void CFile::Close(void) {
	/* Close file */
	file.close();
}

size_t CFile::Size(void) {
	size_t pos, size;

	/* Save position */
	pos = file.tellg();

	/* Move to the end */
	file.seekg(0, std::ios::end);

	/* Get position */
	size = file.tellg();

	/* Restore position */
	file.seekg(pos, std::ios::beg);

	return size;
}

std::string CFile::Type() {
	std::string ext = name.substr(name.find_last_of(".") + 1);
	if(ext == "js") {
		return "application/javascript";
	} else if(ext == "css") {
		return "text/css";
	} else if(ext == "html") {
		return "text/html";
	}

	return "";
}

bool CFile::Read(void *buffer, size_t len) {
	char *ptr = static_cast<char *>(buffer);

	/* Read data */
	file.read(ptr, len);

	return true;
}

bool CFile::Write(void *buffer, size_t len) {
	char *ptr = static_cast<char *>(buffer);

	/* Write data */
	file.write(ptr, len);

	return true;
}

char *CFile::Get(void) {
	char *buffer;

	size_t len;
	bool   res;

	/* Get file size */
	len = Size();

	/* Allocate buffer */
	buffer = new char[len];
	if (!buffer)
		return NULL;

	/* Read file */
	res = Read(buffer, len);

	/* Read error */
	if (!res) {
		/* Free buffer */
		delete buffer;

		/* NULL pointer */
		buffer = NULL;
	}

	return buffer;
}
