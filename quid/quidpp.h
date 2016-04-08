#ifndef QUIDPP_H
#define QUIDPP_H

extern "C" {
#include <quid.h>
}

namespace quidpp {

class InvalidQuid {};

class Quid {
	quid_t quid;

  public:
	Quid() {
		quid_create(&this->quid);
	}

	Quid(const std::string& cquid) : Quid(cquid.c_str()) {}

	Quid(const char *strquid) {
		char rs = strquid_format(strquid);
		if (!rs) {
			throw InvalidQuid();
		}

		strtoquid(strquid, &this->quid);
	}

	void uniform(char node[4]) const {
		memcpy(node, this->quid.node, 4);
	}

	std::string uniform() const {
		char u[4];
		this->uniform(u);
		return std::string(u, 4);
	}

	std::string toString() const {
		char s[40];
		quidtostr(s, &this->quid);

		return std::string(s);
	}

	bool operator==(const Quid & quid2) {
		return quidcmp(&this->quid, &quid2.quid) == 0;
	}

	friend std::ostream& operator<<(std::ostream & os, const Quid & qt) {
		os << qt.toString();
		return os;
	}
};

}

#endif // QUIDPP_H
