/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <Python.h>
#include "package.h"

#define VERSION "1.0"

int job_check = 1;

int find_in_file(const char *fname, char *str) {
	FILE *fp;
	int line_num = 1;
	int find_result = 0;
	char temp[512];

	if ((fp = fopen(fname, "r")) == NULL) {
		return (-1);
	}

	while (fgets(temp, 512, fp) != NULL) {
		if ((strstr(temp, str)) != NULL) {
			find_result++;
		}
		line_num++;
	}

	if (fp) {
		fclose(fp);
	}
	return find_result;
}

int file_exist(const char *fname) {
	if (access(fname, F_OK) != -1) {
	    return 1;
	} else {
	    return 0;
	}
}

int remove_directory(const char *path) {
	DIR *dir = opendir(path);
	size_t path_len = strlen(path);
	int r = -1;

	if (dir) {
		struct dirent *p;
		r = 0;
		while (!r && (p = readdir(dir))) {
			int r2 = -1;

			/* Skip "." and ".." */
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
				continue;

			size_t len = path_len + strlen(p->d_name) + 2; 
			char *buf = (char *)malloc(len);
			if (buf) {
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, p->d_name);

				if (!stat(buf, &statbuf)) {
					if (S_ISDIR(statbuf.st_mode))
						r2 = remove_directory(buf);
					else
						r2 = unlink(buf);
				}

				free(buf);
			}

			r = r2;
		}

		closedir(dir);
	}

	if (!r)
		r = rmdir(path);

	return r;
}

void usage(const char *prog) {
	printf("Jobpacker " VERSION " Copyright (C) 2015-2016 Mavicona, Quenza Inc.\n"
		   "All Rights Reserved\n"
		   "usage: %s [OPTION] <job> [source...]\n\n"
		   "  --info <job>      Show job info\n"
		   "  --export <job>    Export package info\n"
		   "  --skip-check      Ignore job and framework checks\n"
		   "  --no-compression  SKip compression\n"
		   "  --extract <job>   Unpack job\n"
		   "  --verify <job>    Verify job is runnable\n"
		   "  --verbose         Verbose output message\n"
		   "  --help            This help message\n", prog);
}

int main(int argc, char *argv[]) {
	char *progname = argv[0];
	int jobname_idx = 1;
	if (argc < 2) {
		usage(progname);
		return 1;
	}

	if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
		usage(progname);
		return 1;
	}

	if (!strcmp(argv[1], "--verify") && argc == 3) {
		return package_verify(argv[2]);
	}

	if (!strcmp(argv[1], "--info") && argc == 3) {
		package_info(argv[2]);
		return 0;
	}

	if (!strcmp(argv[1], "--extract") && argc == 3) {
		return package_extract(argv[2], NULL);
	}

	wchar_t *program = Py_DecodeLocale(progname, NULL);
	if (!program) {
		fprintf(stderr, "cannot decode argv[0]\n");
		return 1;
	}

	setenv("PYTHONPATH", ".", 1);
	Py_SetProgramName(program);
	Py_Initialize();

	int c, has_job_init = 0, has_ace_job = 0, has_package = 0;
	libtar_list_t *l = libtar_list_new(LIST_QUEUE, NULL);
	for (c = 1; c < argc; ++c) {
		if (!strcmp(argv[c], "--verbose")) {
			package_set_verbose(1);
			++jobname_idx;
			continue;
		}

		if (!strcmp(argv[c], "--skip-check")) {
			job_check = 0;
			++jobname_idx;
			continue;
		}

		if (!strcmp(argv[c], "--no-compression")) {
			package_set_compression(0);
			++jobname_idx;
			continue;
		}

		if (find_in_file(argv[c], "def package") > 0) {
			char buf[512];
			char *str = strdup(argv[c]);
			char *module = strtok(str, ".");
			if (module) {
				snprintf(buf, sizeof(buf),
					"import socket\nimport json\n"
					"import time\nimport %s\n"
					"r=%s.package()\n"
					"with open('package.json', 'w') as fp:\n"
					"\tr['meta']={}\n"
					"\tr['meta']['main']='%s'\n"
					"\tr['meta']['invoke']='job_init'\n"
					"\tr['meta']['host']=socket.gethostname()\n"
					"\tr['meta']['timestamp']=time.time()\n"
					"\tjson.dump(r, fp)\n", module, module, argv[c]);
				PyRun_SimpleString(buf);

				if (file_exist("package.json"))
					libtar_list_add(l, "package.json");
				has_package = 1;
			}
			free(str);
		}

		if (find_in_file(argv[c], "def job_init") > 0) {
			has_job_init = 1;
		}

		if (find_in_file(argv[c], "ace.job.Job") > 0) {
			has_ace_job = 1;
		}
		
		if (file_exist(argv[c]))
			libtar_list_add(l, argv[c]);
	}

	if (!has_job_init && job_check) {
		libtar_list_free(l, NULL);
		fprintf(stderr, "missing job_init()\n");
		goto cleanup;
	}

	if (!has_ace_job && job_check) {
		libtar_list_free(l, NULL);
		fprintf(stderr, "class must inherit ace.job.Job\n");
		goto cleanup;
	}

	if (!has_package && job_check) {
		fprintf(stderr, "no package data provided\nthis is not required, but recommended\n");
	}

	FILE *pf = fopen("LICENSE", "w");
	if (pf) {
		fputs("Copyright (C) 2015-2016 Mavicona, Quenza Inc.\n", pf);
		fclose(pf);
		libtar_list_add(l, "LICENSE");
	}

	if (file_exist("ace/job.py"))
		libtar_list_add(l, "ace/job.py");

	int return_code = package_create(argv[jobname_idx], ".", l);
	libtar_list_free(l, NULL);

cleanup:
	if (file_exist("LICENSE"))
		unlink("LICENSE");
	if (file_exist("package.json"))
		unlink("package.json");

	remove_directory("__pycache__");

	Py_Finalize();
	return return_code;
}
