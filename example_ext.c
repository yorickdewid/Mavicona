#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>

char *mav_main(int argc, char *argv[]) {
	struct passwd *p = getpwuid(getuid());
	if (!p)
		return NULL;

	printf("Current user: %s\n", p->pw_name);

	/* Return some data */
	return p->pw_name;
}