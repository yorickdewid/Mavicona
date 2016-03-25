#ifndef RULER_H
#define RULER_H

#include <iostream>
#include <vector>

#include "mime.h"
#include "rulenode.h"

class Ruler {
	std::vector<RuleNode *> *m_Ruleset = nullptr;
	ActionNode *m_ActionList = nullptr;
	bool match(const int type, const std::string& name);

  public:
	static bool verifyRules(std::vector<RuleNode *> *ruleset);
	static std::vector<RuleNode *> *parseConfigFile(const char *configFile);

	Ruler(std::vector<RuleNode *> *ruleset) : m_Ruleset(ruleset) {}

	bool hasActionList() {
		if (!this->m_ActionList)
			return false;

		return true;
	}

	bool matchMimeRule(Mime *mime);
	bool matchCategoryRule(Mime *mime);
	bool matchExtensionRule(const std::string& extension);
	bool matchTypeRule(const std::string& typeName);
	void runRule();
};

#endif // RULER_H
