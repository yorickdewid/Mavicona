#include <algorithm>

#include "rule_scan.h"
#include "rule_parse.h"

#include "ruler.h"

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
	std::cout << "matching rule mime: " << mime->type() << std::endl;
	return match(MIME, mime->type());
}

bool Ruler::matchCategoryRule(Mime *mime) {
	std::cout << "matching rule category" << std::endl;
	return match(CATEGORY, mime->category());
}

bool Ruler::matchExtensionRule(const std::string& extension) {
	std::cout << "matching rule extension" << std::endl;
	return match(EXTENSION, extension);
}

bool Ruler::matchTypeRule(const std::string& typeName) {
	std::cout << "matching rule type" << std::endl;
	return match(TYPE, typeName);
}

void Ruler::runRule() {
	// Did not match anything, set unknown rule
	if (!this->m_ActionList) {
		std::cout << "using unknown" << std::endl;
		match(UNKNOWN);
	}

	std::cout << "run actions" << std::endl;
	std::cout << "actions: " << this->m_ActionList->actions.size() << std::endl;
	// here we have the actionlist and the dataprofile, send to services
}
