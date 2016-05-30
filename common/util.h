#ifndef UTIL_H
#define UTIL_H

#include <sys/stat.h>

inline bool file_exist(const std::string& name) {
	struct stat buffer;   
	return (stat(name.c_str(), &buffer) == 0); 
}

#endif // UTIL_H
