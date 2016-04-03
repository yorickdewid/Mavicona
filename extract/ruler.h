#ifndef RULER_H
#define RULER_H

#include <iostream>
#include <vector>

#include "rule_parse.h"

#include "mime.h"
#include "rulenode.h"

class Ruler {
	std::vector<RuleNode *> *m_Ruleset = nullptr;
	ActionNode *m_ActionList = nullptr;
	const ScrapeData *payload = nullptr;
	bool match(const int type, const std::string& name);

  public:
	static bool verifyRules(std::vector<RuleNode *> *ruleset);
	static std::vector<RuleNode *> *parseConfigFile(const char *configFile);

	Ruler(std::vector<RuleNode *> *ruleset) : m_Ruleset(ruleset) {}

	inline bool hasActionList() {
		if (!this->m_ActionList)
			return false;

		return true;
	}

	inline bool matchMimeRule(Mime *mime) {
		return match(MIME, mime->type());
	}

	inline bool matchCategoryRule(Mime *mime) {
		return match(CATEGORY, mime->category());
	}

	inline bool matchExtensionRule(const std::string& extension) {
		return match(EXTENSION, extension);
	}

	inline bool matchTypeRule(const std::string& typeName) {
		return match(TYPE, typeName);
	}

	inline void setDataProfile(const ScrapeData &data) {
		this->payload = &data;
	}

	void runRuleActions();
};

#endif // RULER_H
