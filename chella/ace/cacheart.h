#ifndef CACHE_H
#define CACHE_H

namespace Ace {

class CacheArt {
	Callback *cb = NULL;

  public:
	CacheArt() {}

	void put(const std::string& key, const std::string& value) {
		cb->cachePut(key, value);
	}

	std::string get(const std::string& key) {
		return cb->cacheGet(key);
	}

	void setCallback(Callback *_cb) {
		cb = _cb;
	}
};

}

#endif // CACHE_H
