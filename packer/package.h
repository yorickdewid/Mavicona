#ifndef PACKAGE_H_INCLUDED
#define PACKAGE_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include <libpar.h>

int package_create(char *file, char *rootdir, libtar_list_t *list);
int package_extract(char *file, char *rootdir);
int package_verify(char *file);

#ifdef __cplusplus
}
#endif

#endif // PACKAGE_H_INCLUDED
