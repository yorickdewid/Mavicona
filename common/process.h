#include <stdio.h>

int main(int argc, char **argv) {
	FILE *fpipe;
	char *command="/usr/share/application/firefox";
	char line[256];

	if (!(fpipe = (FILE*)popen(command,"r"))) {
		perror("Problems with pipe");
		exit(1);
	}

	while (fgets(line, sizeof line, fpipe)) {
		printf("%s", line);
	}

	pclose(fpipe);
	return 0;
}