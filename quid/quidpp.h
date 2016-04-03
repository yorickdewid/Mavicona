#ifndef QUIDPP_H
#define QUIDPP_H

extern "C" {
#include <quid.h>
}

namespace quidpp {
class Quid {
	quid_t quid;

  public:
	Quid() {
		quid_create(&this->quid);
	}

	std::string toString() const {
		char s[40];
		quidtostr(s, &this->quid);

		return std::string(s);
	}

	bool operator==(const Quid &quid2) {
		return quidcmp(&this->quid, &quid2.quid) == 0;
	}

	friend std::ostream& operator<<(std::ostream& os, const Quid& qt) {
		os << qt.toString();
		return os;
	}
};

}

#endif // QUIDPP_H
