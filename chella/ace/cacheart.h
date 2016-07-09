#ifndef CACHEAT_H
#define CACHEAT_H

namespace Ace {

class CacheArt {
	Callback *cb = NULL;

  public:
	CacheArt() {}

	void Put(const std::string& key, const std::string& value) {
		cb->cachePut(key, value);
	}

	std::string Get(const std::string& key) {
		return cb->cacheGet(key);
	}

	void setCallback(Callback *_cb) {
		cb = _cb;
	}
};

}

#endif // CACHEAT_H
