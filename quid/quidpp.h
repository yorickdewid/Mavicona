/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifndef QUIDPP_H
#define QUIDPP_H

#include <string>
#include <memory.h>

extern "C" {
#include <quid.h>
}

namespace quidpp {

class InvalidQuid {/*TODO set what()*/};

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

	static size_t unpackedSize() {
		return UNPACKED_LENGTH;
	}

	static std::string crop(const std::string& cquid) {
		if (cquid.size() == (UNPACKED_LENGTH + 2) && cquid[0] == '{' && cquid[UNPACKED_LENGTH + 1] == '}')
			return cquid.substr(1, UNPACKED_LENGTH);

		return "";
	}

	void uniform(char node[4]) const {
		memcpy(node, this->quid.node, 4);
	}

	std::string uniform() const {
		char u[4];
		this->uniform(u);
		return std::string(u, 4);
	}

	std::string toString(bool compact = false) const {
		char s[40];
		quidtostr(s, &this->quid);

		if (compact)
			return crop(s);

		return std::string(s);
	}

	bool operator==(const Quid& quid2) {
		return quidcmp(&this->quid, &quid2.quid) == 0;
	}

	friend std::ostream& operator<<(std::ostream& os, const Quid& qt) {
		os << qt.toString();
		return os;
	}
};

}

#endif // QUIDPP_H
