#include <algorithm>

#include "rule_scan.h"
#include "rule_parse.h"

#include "ruler.h"

bool Ruler::parseConfigFile(const char *configFile) {
	FILE *file = fopen(configFile, "r");
	if (!file) {
		std::cerr << "Cannot read config file " << configFile << std::endl;
		return false;
	}

	yyin = file;

	/* Parse through the input until there is no more */
	do {
		yyparse();
	} while (!feof(yyin));

	fclose(file);

	this->ruleset = getRuleset();

	return verifyRules();
}

bool Ruler::matchType(const int type) {
	std::vector<RuleNode *>::iterator it = std::find_if(this->ruleset->begin(), this->ruleset->end(), [type](const RuleNode * rule) -> bool {
		return rule->nodeType == type;
	});

	if (it == this->ruleset->end())
		return false;

	return true;
}

bool Ruler::verifyRules() {
	if (!this->size())
		return false;

	if (!matchType(UNKNOWN))
		return false;

	return true;
}

void Ruler::runRule() {

}

void Ruler::runRuleUnknown() {

}
