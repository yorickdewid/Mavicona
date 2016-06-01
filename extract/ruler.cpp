#include <algorithm>

#include "rule_scan.h"
#include "rule_parse.h"

#include "action_log.h"
#include "action_discard.h"
#include "action_queue.h"
#include "action_store.h"
#include "ruler.h"

#define LOGFILENAME		"rule_action"

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
				std::cout << "Item[" << this->payload->id() << "] action [log]" << std::endl;
				ruleAction = new Log(LOGFILENAME, this->payload);
				break;
			case STORE:
				std::cout << "Item[" << this->payload->id() << "] action [store]" << std::endl;
				ruleAction = new Store(this->payload);
				break;
			case DISCARD:
				std::cout << "Item[" << this->payload->id() << "] action [discard]" << std::endl;
				ruleAction = new Discard(this->payload);
				break;
			case QUEUE:
				std::cout << "Item[" << this->payload->id() << "] action [queue]" << std::endl;
				ruleAction = new Queue(this->payload);
				break;
		}

		ruleAction->config(this->configfile);
		ruleAction->run();
		delete ruleAction;
	}

}
