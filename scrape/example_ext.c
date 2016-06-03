#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include <mavdso.h>

/*
 * mav_main is called from the scraper and allows
 * the module to do initialization, collect data and
 * push it onto the data stack.
 */
int mav_main(int argc, char *argv[]) {
	struct passwd *p = getpwuid(getuid());
	if (!p)
		return 1;

	printf("Current user: %s\n", p->pw_name);

	/* Push data onto stack */
	push("username", p->pw_name, strlen(p->pw_name));

	return 0;
}
