%{
	#include <cstdio>
	#include <cstdlib>
	#include <iostream>
	#include <string>
	#include <vector>

	extern int yylex();
	extern void yyerror(const char *);
%}

%code requires {
	#include "rulenode.h"

	std::vector<RuleNode *> *getRuleset();
}

%union {
	std::string *str_val;
	int type;
	RuleNode *node;
	ActionNode *act;
}

%code {
	std::vector<RuleNode *> rules;

	std::vector<RuleNode *> *getRuleset() {
		return &rules;
	}

	void print_tree() {
		for (std::vector<RuleNode *>::iterator it = rules.begin(); it != rules.end(); ++it) {

			std::cout << "With ";
			switch ((*it)->nodeType){
				case UNKNOWN:
					std::cout << "unknown ";
					break;
				case MIME:
					std::cout << "mime ";
					break;
				case TYPE:
					std::cout << "type ";
					break;
				case EXTENSION:
					std::cout << "extension ";
					break;
				case CATEGORY:
					std::cout << "category ";
					break;
			}

			if ((*it)->nodeType != UNKNOWN) {
				std::cout << *(*it)->nodeName;
			}
 			std::cout << " take action:" << std::endl;

			for (int action : (*it)->actionList->actions) {
				switch (action){
					case LOG:
						std::cout << "  log" << std::endl;
						break;
					case STORE:
						std::cout << "  store" << std::endl;
						break;
					case DISCARD:
						std::cout << "  discard" << std::endl;
						break;
					case QUEUE:
						std::cout << "  queue" << std::endl;
						break;
				}
			}
		}
	}
}

%token LBRACE RBRACE SEMICOLON
%token THEN
%token <type> STORE DISCARD QUEUE LOG
%token <type> MIME TYPE EXTENSION CATEGORY UNKNOWN
%token <str_val> STRING
%type <node> line
%type <act> stmts
%type <type> stmt action

%debug

%start config

%%

config:
		lines
		;
lines:
		lines line 									{ rules.push_back($2); }
		| line 										{ rules.push_back($1); }
		;
line:
		UNKNOWN LBRACE stmts RBRACE					{ $$ = new RuleNode(UNKNOWN, $3); }
		| MIME STRING LBRACE stmts RBRACE			{ $$ = new RuleNode(MIME, $2, $4); }
		| TYPE STRING LBRACE stmts RBRACE			{ $$ = new RuleNode(TYPE, $2, $4); }
		| EXTENSION STRING LBRACE stmts RBRACE		{ $$ = new RuleNode(EXTENSION, $2, $4); }
		| CATEGORY STRING LBRACE stmts RBRACE		{ $$ = new RuleNode(CATEGORY, $2, $4); }
		;
stmts:
		stmts stmt 									{ $1->addAction($2); }
		| stmt 										{ $$ = new ActionNode($1); }
		;
stmt:
		THEN action SEMICOLON						{ $$ = $2; }
		;
action:
		LOG											{ $$ = LOG; }
		| STORE										{ $$ = STORE; }
		| DISCARD									{ $$ = DISCARD; }
		| QUEUE										{ $$ = QUEUE; }
		;

