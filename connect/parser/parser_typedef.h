#ifndef __PARSER_TYPEDEF_H__
#define __PARSER_TYPEDEF_H__

#include <vector>


#ifndef YYtypeDEF_YY_SCANNER_T
#define YYtypeDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif


#define YYSTYPE TSQL_STYPE
#define YYLTYPE TSQL_LTYPE


struct TSQL_CUST_LTYPE {
	int first_line;
	int first_column;
	int last_line;
	int last_column;

	int total_column;

	// Placeholder
	int placeholder_id;
	std::vector<void*> placeholder_list;
};

#define TSQL_LTYPE TSQL_CUST_LTYPE
#define TSQL_LTYPE_IS_DECLARED 1




#endif