#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <string>
#include <sys/stat.h>

inline bool file_exist(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

inline bool dir_exist(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

inline std::string normalize(std::string& str) {
	std::transform(str.begin(), str.end(), str.begin(), [](char c) {
		return tolower(c);
	});

	return str;
}

#endif // UTIL_H
