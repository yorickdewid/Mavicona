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

	return true;
}
