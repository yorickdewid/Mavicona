#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <libtar.h>
#include <Python.h>

#define VERSION "1.0"

int use_gnu = 0;
int verbose = 0;

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

				if (!stat(buf, &statbuf))
					if (S_ISDIR(statbuf.st_mode))
						r2 = remove_directory(buf);
					else
						r2 = unlink(buf);

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

void append_header(const char *tarfile) {
	FILE *fp = fopen(tarfile, "r+b");
	if (!fp)
		return;

	char jobfile[1024];
	snprintf(jobfile, sizeof(jobfile), "%s.job", tarfile);
	FILE *fpo = fopen(jobfile, "wb");
	if (!fpo)
		return;

	// obtain file size:
	fseek(fp , 0 , SEEK_END);
	size_t fsize = ftell(fp);
	rewind(fp);

	uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * fsize);
	if (!buffer)
		return;

	fread(buffer, 1, fsize, fp);

	struct jobheader {
		char signature[8];
		char version;
		char optimization;
	};

	const char magic[8] = {0x17, 'M', 'A', 'V', 'J', 'O', 'B', 0x80};

	struct jobheader header;
	strncpy(header.signature, magic, 8);
	header.version = 3;
	header.optimization = 1;

	fwrite(&header, 1, sizeof(struct jobheader), fpo);
	fwrite(buffer, sizeof(uint8_t), fsize, fpo);

	free(buffer);
	fclose(fpo);
	fclose(fp);
}

int package_create(const char *tarfile, char *rootdir, libtar_list_t *l) {
	TAR *tar;
	char buf[1024];
	libtar_listptr_t lp;

	if (tar_open(&tar, tarfile, NULL, O_WRONLY | O_CREAT, 0644, (verbose ? TAR_VERBOSE : 0) | (use_gnu ? TAR_GNU : 0)) == -1) {
		fprintf(stderr, "tar_open(): %s\n", strerror(errno));
		return -1;
	}

	libtar_listptr_reset(&lp);
	while (libtar_list_next(l, &lp) != 0) {
		char *pathname = (char *)libtar_listptr_data(&lp);
		if (pathname[0] != '/' && rootdir != NULL)
			snprintf(buf, sizeof(buf), "%s/%s", rootdir, pathname);
		else
			strncpy(buf, pathname, sizeof(buf));
		
		if (tar_append_tree(tar, buf, pathname) != 0) {
			fprintf(stderr, "tar_append_tree(\"%s\", \"%s\"): %s\n", buf, pathname, strerror(errno));
			tar_close(tar);
			return -1;
		}
	}

	if (tar_append_eof(tar) != 0) {
		fprintf(stderr, "tar_append_eof(): %s\n", strerror(errno));
		tar_close(tar);
		return -1;
	}

	if (tar_close(tar) != 0) {
		fprintf(stderr, "tar_close(): %s\n", strerror(errno));
		return -1;
	}

	append_header(tarfile);

	return 0;
}

void usage(const char *prog) {
	printf("JobPacker " VERSION " Copyright (C) 2015-2016 Mavicona, Quenza Inc.\n"
		   "All Rights Reserved\n"
		   "usage: %s <job> [source...]\n", prog);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		usage(argv[0]);
		return 1;
	}

	wchar_t *program = Py_DecodeLocale(argv[0], NULL);
	if (!program) {
		fprintf(stderr, "cannot decode argv[0]\n");
		return 1;
	}

	setenv("PYTHONPATH", ".", 1);
	Py_SetProgramName(program);
	Py_Initialize();

	int c, has_job_init = 0, has_ace_job = 0, has_package = 0;
	libtar_list_t *l = libtar_list_new(LIST_QUEUE, NULL);
	for (c = 2; c < argc; ++c) {
		libtar_list_add(l, argv[c]);

		if (find_in_file(argv[c], "def package") > 0) {
			char buf[512];
			char *str = strdup(argv[c]);
			char *module = strtok(str, ".");
			if (module) {
				snprintf(buf, sizeof(buf),
					"import json\n"
					"import %s\n"
					"with open('package.json', 'w') as fp:\n"
					"\tjson.dump(%s.package(), fp)\n", module, module);
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

		if (find_in_file(argv[c], "ace.Job") > 0) {
			has_ace_job = 1;
		}
	}

	if (!has_job_init) {
		libtar_list_free(l, NULL);
		fprintf(stderr, "missing job_init()\n");
		goto cleanup;
	}

	if (!has_ace_job) {
		libtar_list_free(l, NULL);
		fprintf(stderr, "class must inherit ace.Job\n");
		goto cleanup;
	}

	if (!has_package) {
		fprintf(stderr, "no package data provided\nthis is not required, but recommended\n");
	}

	FILE *pf = fopen("LICENSE", "w");
	if (pf) {
		fputs("Copyright (C) 2015-2016 Mavicona, Quenza Inc.\n", pf);
		fclose(pf);
		libtar_list_add(l, "LICENSE");
	}

	if (file_exist("ace.py"))
		libtar_list_add(l, "ace.py");

	int return_code = package_create(argv[1], ".", l);
	libtar_list_free(l, NULL);

cleanup:
	if (file_exist(argv[1]))
		unlink(argv[1]);
	if (file_exist("LICENSE"))
		unlink("LICENSE");
	if (file_exist("package.json"))
		unlink("package.json");

	remove_directory("__pycache__");

	// return extract(argv[1], ".");
	// return create(argv[1], ".");
	Py_Finalize();
	return return_code;
}
