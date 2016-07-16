#ifndef CHAIN_H
#define CHAIN_H

#include <vector>

namespace Ace {

struct Subjob {
	std::string name;
	std::string content;

  public:
	explicit Subjob(const std::string _name) : name(_name) {}

	void setName(const std::string _name) {
		name = _name;
	}
};

class Chain {
	std::string parent_quid;
	std::string parent_name;
	std::vector<Subjob> subjobList;

  public:
	explicit Chain(const std::string quid) : parent_quid(quid) {}

	std::string parentQuid() const {
		return parent_quid;
	}

	std::string parentName() const {
		return parent_name;
	}

	void setParentName(const std::string name) {
		parent_name = name;
	}

	void add(Subjob job) {
		subjobList.push_back(job);
	}

	inline Subjob *at(unsigned int i) {
		return &subjobList.at(i);
	}

	inline size_t size() {
		return subjobList.size();
	}
};

}

#endif // CHAIN_H
