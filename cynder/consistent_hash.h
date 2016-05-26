#ifndef CONSISTENT_HASH_H
#define CONSISTENT_HASH_H

#include <map>
#include <string>
#include <sstream>

namespace {

class StringifyException {};

template <class T>
std::string Stringify(const T& t) {
	std::ostringstream os;
	if (!(os << t)) {
		throw StringifyException();
	}
	return os.str();
}

template <>
std::string Stringify(const std::string& str) {
	return str;
}

template <>
std::string Stringify(const char* const& str) {
	return str;
}

}

namespace Consistent {

class EmptyRingException {};

template <class Node, class Key, class Hash = std::hash<const char *>>
class HashRing {
	std::map<size_t, Node> ring_;
	unsigned int m_spread;
	Hash hash_;

  public:
	HashRing(unsigned int spread) : m_spread(spread), hash_(std::hash<const char*>()) {}
	HashRing(unsigned int spread, const Hash& hash) : m_spread(spread), hash_(hash) {}

	inline void setSpread(unsigned int spread) {
		m_spread = spread;
	}

	size_t addNode(const Node& node) {
		size_t hash;
		std::string nodestr = Stringify(node);
		for (unsigned int r = 0; r < m_spread; r++) {
			hash = hash_((nodestr + Stringify(r)).c_str());
			ring_[hash] = node;
		}
		return hash;
	}

	void removeNode(const Node& node) {
		std::string nodestr = Stringify(node);
		for (unsigned int r = 0; r < m_spread; r++) {
			size_t hash = hash_((nodestr + Stringify(r)).c_str());
			ring_.erase(hash);
		}
	}

	const Node& getNode(const Key& key) const {
		if (ring_.empty()) {
			throw EmptyRingException();
		}
		size_t hash = hash_(Stringify(key).c_str());

		// Look for the first node >= hash
		typename std::map<size_t, Node>::const_iterator it = ring_.lower_bound(hash);
		if (it == ring_.end()) {
			// Wrapped around; get the first node
			it = ring_.begin();
		}

		return it->second;
	}
};

}

#endif // CONSISTENT_HASH_H
