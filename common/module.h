#ifndef MODULE_H
#define MODULE_H

#include <map>
#include <string>

// Auto generated <
#define MAV_VERSION	"a357f61"
#define MAV_BUILD	"2016-06-25:15:38:24"
// >

class Module {
	std::string m_address;
	std::string m_name;
	unsigned int m_port;

  public:
	Module() {}
	Module(std::string address) : m_address(address) {}
	Module(std::string address, std::string name, unsigned int port) : m_address(address), m_name(name), m_port(port) {}

	std::string info() const {
		if (m_address.empty())
			return "";

		if (m_name.empty())
			return m_address;

		return m_address + " (" + m_name + ")";
	}

	static const char *version() {
		return "Version: " MAV_VERSION "\nBuild: " MAV_BUILD;
	}

	friend std::ostream& operator<<(std::ostream& os, const Module& sn) {
		os << sn.info();
		return os;
	}
};

#endif // MODULE_H
