/**
 * Copyright (c) 2016 Mavicona
 * All Rights Reserved.
 *
 * Connector	Query storage data
 */

#include <stdlib.h>
#include <string>

#include "sqlparser.h"
#include "sqlhelper.h"

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		fprintf(stderr, "Usage: %s \"SELECT * FROM ebaa898970d64d84afeb77f1e16aaa41;\"\n", argv[0]);
		return -1;
	}
	std::string query = argv[1];

	/* Parse a given query */
	tsql::SQLParserResult *result = tsql::SQLParser::parseSQLString(query);

	/* Check whether the parsing was successful */
	if (!result->isValid) {
		printf("Invalid query: %s\n", result->errorMsg);
		return 1;
	}

	printf("Syntax OK\n");
	printf("Statements: %lu\n", result->size());

	for (tsql::SQLStatement *stmt : result->statements)
		tsql::printStatementInfo(stmt);

	return 0;
}