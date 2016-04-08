#ifndef SDBM_HASH_H
#define SDBM_HASH_H

struct SdbmHash {
	size_t operator()(const char * str) const {
		size_t hash = 0;
		int c;

		while ((c = *str++)) {
			hash = c + (hash << 6) + (hash << 16) - hash;
		}

		return hash;
	}
};

#endif // SDBM_HASH_H
