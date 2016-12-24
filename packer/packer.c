/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#if defined(LINUX)
# ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE
# endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <Python.h>
#include <libpar.h>
#include <util.h>
#include "package.h"

#define VERSION "1.2"

int job_check = 1;

static void create_license() {
	FILE *pf = fopen("LICENSE", "w");
	if (!pf)
		return;

	fputs("Copyright (C) 2015-2016 Mavicona, Quenza Inc.\nAll Rights Reserved\n\n", pf);
	fputs("Content can not be copied and/or distributed without \n", pf);
	fputs("the express permission of the author.\n", pf);
	fclose(pf);
}

static void create_inittab() {
	FILE *pf = fopen(".inittab", "w");
	if (!pf)
		return;

	fputs("# Inittab\n", pf);
	fputs("# Execute commands before job is loaded\n#\n", pf);
	fputs("# Commands are run per line and can\n", pf);
	fputs("# be used to setup a local environment\n#\n", pf);
	
	fclose(pf);
}

static void create_initwrap() {
	FILE *pf = fopen(".initwrap", "w");
	if (!pf)
		return;

	fputs("# Initwrap\n", pf);
	fputs("# Execute commands after job is unloaded\n#\n", pf);
	fputs("# Commands are run per line and can\n", pf);
	fputs("# be used to teardown a local environment\n#\n", pf);
	
	fclose(pf);
}

static void usage(const char *prog) {
	printf("Packer " VERSION " Copyright (C) 2015-2016 Mavicona, Quenza Inc.\n"
		   "All Rights Reserved\n"
		   "usage: %s [OPTION] <job> [source...]\n\n"
		   "  --export <job>    Export package info\n"
		   "  --skip-check      Ignore job and framework checks\n"
		   "  --extract <job>   Unpack job\n"
		   "  --verify <job>    Verify job is runnable\n"
		   "  --version         Show version and exit\n"
		   "  --help            This help message\n", prog);
}

int PyCheck(int argc, char *argv[]) {
	int jobname_idx = 1;
	setenv("PYTHONPATH", ".", 1);
	PyUnicode_FromString(argv[0]);

	Py_SetProgramName((wchar_t *)argv[0]);
	Py_Initialize();

	int c, has_job_init = 0, has_ace_job = 0, has_package = 0;
	libtar_list_t *list = libtar_list_new(LIST_QUEUE, NULL);
	for (c = 1; c < argc; ++c) {
		if (!strcmp(argv[c], "--skip-check")) {
			job_check = 0;
			++jobname_idx;
			continue;
		}

		/* Skip over first argument */
		if (c == 1)
			continue;

		if (!file_exist(argv[c])) {
			fprintf(stderr, "cannot find file %s, skipping ...\n", argv[c]);
			continue;
		}

		/* Test python files */
		const char *ext = file_extension(argv[c]);
		if (ext && !strcmp(ext, "py")) {
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
						"\tr['meta']['main']='%s'[:-3]\n"
						"\tr['meta']['invoke']='job_init'\n"
						"\tr['meta']['host']=socket.gethostname()\n"
						"\tr['meta']['timestamp']=time.time()\n"
						"\tjson.dump(r, fp)\n", module, module, argv[c]);
					PyRun_SimpleString(buf);

					if (file_exist("package.json"))
						libtar_list_add(list, "package.json");
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
		}

		/* Append to archieve */
		libtar_list_add(list, argv[c]);
	}

	int return_code = 0;
	if (!has_job_init && job_check) {
		libtar_list_free(list, NULL);
		fprintf(stderr, "missing job_init()\n");
		goto cleanup;
	}

	if (!has_ace_job && job_check) {
		libtar_list_free(list, NULL);
		fprintf(stderr, "class must inherit ace.job.Job\n");
		goto cleanup;
	}

	if (!has_package && job_check) {
		fprintf(stderr, "no package data provided\nthis is not required, but recommended\n");
	}

	if (!file_exist(".inittab"))
		create_inittab();
	libtar_list_add(list, ".inittab");

	if (!file_exist(".initwrap"))
		create_initwrap();
	libtar_list_add(list, ".initwrap");

	/* Add license */
	if (!file_exist("LICENSE"))
		create_license();
	libtar_list_add(list, "LICENSE");

	/* Add framework */
	if (file_exist("ace/job.py"))
		libtar_list_add(list, "ace/job.py");

	return_code = package_create(argv[jobname_idx], ".", list);
	libtar_list_free(list, NULL);

cleanup:
	if (file_exist("package.json"))
		unlink("package.json");

	remove_directory("__pycache__");

	Py_Finalize();
	return return_code;
}

int main(int argc, char *argv[]) {
	char *progname = argv[0];
	if (argc < 2) {
		usage(progname);
		return 1;
	}

	if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
		usage(progname);
		return 1;
	}

	if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v") || !strcmp(argv[1], "-V")) {
		printf("Packer " VERSION "\n");
		return 1;
	}

	if (!strcmp(argv[1], "--verify") && argc == 3) {
		return package_verify(argv[2]);
	}

	if (!strcmp(argv[1], "--extract") && argc == 3) {
		return package_extract(argv[2], NULL);
	}

	return PyCheck(argc, argv);
}
