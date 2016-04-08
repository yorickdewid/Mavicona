#ifndef SERVER_NODE_H
#define SERVER_NODE_H

#include <map>
#include <string>

class ServerNode {
	std::string m_address;
	std::string m_name;
	std::map<std::string, std::string> m_cache;

  public:
	ServerNode() {}
	ServerNode(std::string address) : m_address(address) {}
	ServerNode(std::string address, std::string name) : m_address(address), m_name(name) {}

	std::string info() const {
		if (m_address.empty())
			return "unknown";

		if (m_name.empty())
			return m_address;

		return m_address + " (" + m_name + ")";
	}

	void put(const std::string& key, const std::string& value) {
		m_cache[key] = value;
	}

	std::string get(const std::string& key) const {
		std::string value;
		std::map<std::string, std::string>::const_iterator it = m_cache.find(key);
		if (it != m_cache.end()) {
			value = it->second;
		}
		return value;
	}

	void remove(const std::string& key) {
		std::map<std::string, std::string>::iterator it = m_cache.find(key);
		if (it != m_cache.end()) {
			m_cache.erase(it);
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const ServerNode& sn) {
		os << sn.info();
		return os;
	}
};

#endif // SERVER_NODE_H
