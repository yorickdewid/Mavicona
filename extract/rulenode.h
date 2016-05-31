#ifndef RULENODE_H
#define RULENODE_H

#include <iostream>
#include <vector>

class ActionNode {
  public:
	std::vector<int> actions;

	ActionNode(int action) {
		actions.push_back(action);
	}

	inline void addAction(int action) {
		actions.push_back(action);
	}

};

class RuleNode {
  public:
	int nodeType;
	std::string *nodeName;
	ActionNode *actionList = nullptr;

	RuleNode(int type, ActionNode *list) {
		nodeType = type;
		actionList = list;
	}

	RuleNode(int type, std::string *name, ActionNode *list) {
		nodeType = type;
		nodeName = name;
		actionList = list;

		if (nodeName->at(0) == '\'' || nodeName->at(0) == '"') {
			nodeName->erase(0, 1);
			nodeName->erase(nodeName->size() - 1, 1);
		}
	}

};

#endif // RULENODE_H
