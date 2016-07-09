#include "exec.h"

void Execute::cachePut(const std::string& key, const std::string value) {
	if (!this->cache) {
		this->cache = (art_tree *)malloc(sizeof(art_tree));
		art_tree_init(this->cache);
	}

	art_insert(this->cache, (unsigned char *)key.c_str(), key.size(), new std::string(value));
}

void Execute::cacheDelete(const std::string& key) {
	if (!this->cache)
		return;

	art_delete(this->cache, (unsigned char *)key.c_str(), key.size());
}

std::string Execute::cacheGet(const std::string& key) {
	if (!this->cache)
		return "";

	void *result = art_search(this->cache, (unsigned char *)key.c_str(), key.size());
	if (!result)
		return "";

	return *static_cast<std::string *>(result);
}
