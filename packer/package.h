#ifndef PACKAGE_H_INCLUDED
#define PACKAGE_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#include <libtar.h>

int package_create(const char *tarfile, char *rootdir, libtar_list_t *list);
int package_extract(const char *jobfile, const char *rootdir);
int package_verify(const char *pkgfile);
void package_info(const char *pkgfile);
void package_set_verbose(int f);
void package_set_compression(int f);

#ifdef __cplusplus
}
#endif

#endif // PACKAGE_H_INCLUDED
