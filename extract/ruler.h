#ifndef RULER_H
#define RULER_H

#include <iostream>
#include <vector>

#include "rulenode.h"

class Ruler {
	std::vector<RuleNode *> *ruleset;
	bool parseConfigFile(const char *configFile);
	bool matchType(const int type);

  public:
	bool setConfig(const char *fname) {
		return parseConfigFile(fname);
	}

	size_t size() {
		if (!ruleset)
			return 0;

		return ruleset->size();
	}

	bool verifyRules();
	void runRule();
	void runRuleUnknown();

};

#endif // RULER_H
