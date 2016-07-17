#include <stdio.h>
#include <string>

#include "sqlparser.h"
#include "parser/bison_parser.h"
#include "parser/flex_lexer.h"

namespace tsql {

SQLParser::SQLParser() {
	fprintf(stderr, "SQLParser only has static methods atm! Do not initialize!\n");
}

SQLParserResult *SQLParser::parseSQLString(const char *text) {
	SQLParserResult *result = NULL;
	yyscan_t scanner;
	YY_BUFFER_STATE state;

	if (tsql_lex_init(&scanner)) {
		// couldn't initialize
		fprintf(stderr, "[Error] SQLParser: Error when initializing lexer!\n");
		return NULL;
	}

	state = tsql__scan_string(text, scanner);

	if (tsql_parse(&result, scanner)) {
		// Returns an error stmt object
		return result;
	}

	tsql__delete_buffer(state, scanner);
	tsql_lex_destroy(scanner);

	return result;
}

/* Translate std:string */
SQLParserResult *SQLParser::parseSQLString(const std::string& text) {
	return parseSQLString(text.c_str());
}


} // namespace tsql
