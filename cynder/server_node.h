#ifndef SERVER_NODE_H
#define SERVER_NODE_H

#include <map>
#include <string>

class ServerNode {
	std::string m_address;
	std::string m_name;

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

	friend std::ostream& operator<<(std::ostream& os, const ServerNode& sn) {
		os << sn.info();
		return os;
	}
};

#endif // SERVER_NODE_H
