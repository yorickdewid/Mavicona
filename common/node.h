#ifndef NODE_H
#define NODE_H

#include <map>
#include <string>

class Node {
	std::string m_address;
	std::string m_name;
	unsigned int m_port;

  public:
	Node() {}
	Node(std::string address) : m_address(address) {}
	Node(std::string address, std::string name, unsigned int port) : m_address(address), m_name(name), m_port(port) {}

	std::string info() const {
		if (m_address.empty())
			return "unknown";

		if (m_name.empty())
			return m_address;

		return m_address + " (" + m_name + ")";
	}

	friend std::ostream& operator<<(std::ostream& os, const ServerNode& sn) {
		os << sn.info();
		return os;
	}
};

#endif // NODE_H
