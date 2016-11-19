#ifndef PACKAGE_H_INCLUDED
#define PACKAGE_H_INCLUDED

int package_create(const char *tarfile, char *rootdir, libtar_list_t *list);
int package_extract(const char *jobfile);
int package_verify(const char *pkgfile);
void package_info(const char *pkgfile);
void package_set_verbose(int f);
void package_set_compression(int f);

#endif // PACKAGE_H_INCLUDED
