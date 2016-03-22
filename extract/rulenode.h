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

	void addAction(int action) {
		actions.push_back(action);
	}

};

class RuleNode {
  public:
	int nodeType;
	std::string *nodeName;
	ActionNode *actionList;

	RuleNode(int type, ActionNode *list) {
		nodeType = type;
		actionList = list;
	}

	RuleNode(int type, std::string *name, ActionNode *list) {
		nodeType = type;
		nodeName = name;
		actionList = list;
	}

	std::string *name() {
		nodeName->erase(0, 1);
		nodeName->erase(nodeName->size() - 1, 1);
		return nodeName;
	}

};

#endif // RULENODE_H
