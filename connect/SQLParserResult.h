#ifndef __SQLPARSERRESULT__
#define __SQLPARSERRESULT__

#include "sql/SQLStatement.h"

namespace tsql {
	/**
	 * Represents the result of the SQLParser.
	 * If parsing was successful it contains a list of SQLStatement.
	 */
	class SQLParserResult {
	  public:

		SQLParserResult();
		SQLParserResult(SQLStatement* stmt);
		virtual ~SQLParserResult();

		void addStatement(SQLStatement* stmt);

		SQLStatement *getStatement(int id);

		size_t size();

		// public properties
		std::vector<SQLStatement*> statements;
		bool isValid;

		const char *errorMsg;
		int errorLine;
		int errorColumn;
	};

} // namespace tsql

#endif // __SQLPARSERRESULT__
