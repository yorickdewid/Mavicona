#include <algorithm>

#include "rule_scan.h"
#include "rule_parse.h"

#include "action_log.h"
#include "action_discard.h"
#include "ruler.h"

#define LOGFILENAME		"rule_action.log"

bool Ruler::verifyRules(std::vector<RuleNode *> *ruleset) {
	if (!ruleset)
		return false;

	std::vector<RuleNode *>::iterator it = std::find_if(ruleset->begin(), ruleset->end(), [](const RuleNode * rule) -> bool {
		return rule->nodeType == UNKNOWN;
	});

	if (it == ruleset->end())
		return false;

	return ruleset->size();
}

std::vector<RuleNode *> *Ruler::parseConfigFile(const char *configFile) {
	FILE *file = fopen(configFile, "r");
	if (!file) {
		std::cerr << "Cannot read config file " << configFile << std::endl;
		return nullptr;
	}

	yyin = file;

	/* Parse through the input until there is no more */
	do {
		yyparse();
	} while (!feof(yyin));

	fclose(file);

	std::vector<RuleNode *> *ruleset = getRuleset();

	if (!verifyRules(ruleset))
		return nullptr;

	return ruleset;
}

bool Ruler::match(const int type, const std::string& name = "") {
	if (!this->m_Ruleset)
		return false;

	std::vector<RuleNode *>::iterator it = std::find_if(this->m_Ruleset->begin(), this->m_Ruleset->end(), [type](const RuleNode * rule) -> bool {
		return rule->nodeType == type;
	});

	if (it == this->m_Ruleset->end())
		return false;

	if ((*it)->nodeType != UNKNOWN) {
		if (*(*it)->nodeName != name)
			return false;
	}

	this->m_ActionList = (*it)->actionList;

	return true;
}

bool Ruler::matchMimeRule(Mime *mime) {
	return match(MIME, mime->type());
}

bool Ruler::matchCategoryRule(Mime *mime) {
	return match(CATEGORY, mime->category());
}

bool Ruler::matchExtensionRule(const std::string& extension) {
	return match(EXTENSION, extension);
}

bool Ruler::matchTypeRule(const std::string& typeName) {
	return match(TYPE, typeName);
}

void Ruler::runRuleActions() {
	if (!this->payload)
		return;

	/* Did not match anything, set unknown rule */
	if (!this->m_ActionList)
		match(UNKNOWN);

	for (int action : this->m_ActionList->actions) {
		Action *ruleAction = nullptr;

		switch (action) {
			case LOG:
				std::cout << "log dataprofile to file" << std::endl;
				ruleAction = new Log(LOGFILENAME, this->payload);
				break;
			case STORE:
				std::cout << "send dataprofile to storage cluster" << std::endl;
				break;
			case DISCARD:
				std::cout << "discard dataprofile" << std::endl;
				ruleAction = new Discard(this->payload);
				break;
			case QUEUE:
				std::cout << "send dataprofile to queue" << std::endl;
				break;
		}

		if (ruleAction)
			ruleAction->run();
	}

}

void Ruler::setDataProfile(const ScrapeData &data) {
	this->payload = &data;
}
