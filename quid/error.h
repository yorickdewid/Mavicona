#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"

#define ERROR_CODE	14

/*
 * Trace error through global structure
 */
struct error {
	char error_squid[ERROR_CODE];
	char *description;
};

void error_clear();
int iserror();
void error_throw(char *error_code, char *error_message);
void error_throw_fatal(char *error_code, char *error_message);
char *get_error_code();
char *get_error_description();

#ifdef __cplusplus
}
#endif

#endif // ERROR_H_INCLUDED
