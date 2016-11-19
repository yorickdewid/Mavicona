#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <libtar.h>
#include <Python.h>
#include <zlib.h>

#define VERSION "1.0"
#define PKGVER 0x3

int use_gnu = 0;
int use_zlib = 1;
int verbose = 0;
int job_check = 1;
int compression = 1;

struct jobheader {
	uint8_t signature[8];
	uint8_t version;
	uint8_t compression;
};

const char magic[8] = {0x17, 'M', 'A', 'V', 'J', 'O', 'B', 0x80};

gzFile gzopen_frontend(char *pathname, int oflags, int mode) {
	char *gzoflags;
	gzFile gzf;
	int fd;

	switch (oflags & O_ACCMODE) {
		case O_WRONLY:
			gzoflags = "wb";
			break;
		case O_RDONLY:
			gzoflags = "rb";
			break;
		default:
		case O_RDWR:
			errno = EINVAL;
			return NULL;
	}

	fd = open(pathname, oflags, mode);
	if (fd == -1)
		return NULL;

	if ((oflags & O_CREAT) && fchmod(fd, mode)) {
		close(fd);
		return NULL;
	}

	gzf = gzdopen(fd, gzoflags);
	if (!gzf) {
		errno = ENOMEM;
		return NULL;
	}

	/* This is a bad thing to do on big-endian lp64 systems, where the
	   size and placement of integers is different than pointers.
	   However, to fix the problem 4 wrapper functions would be needed and
	   an extra bit of data associating GZF with the wrapper functions.  */
	return gzf;
}

tartype_t gztype = {
	(openfunc_t)gzopen_frontend,
	(closefunc_t)gzclose,
	(readfunc_t)gzread,
	(writefunc_t)gzwrite
};

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

static int verify_header(struct jobheader *header) {
	if (memcmp(header->signature, magic, 8)) {
		fprintf(stderr, "not a mavicona package\n");
		return 1;
	}

	if (header->version != PKGVER) {
		fprintf(stderr, "invalid package version\n");
		return 1;
	}

	return 0;
}

static void append_header(const char *tarfile) {
	FILE *fp = fopen(tarfile, "r+b");
	if (!fp)
		return;

	char jobfile[1024];
	snprintf(jobfile, sizeof(jobfile), "%s.job", tarfile);
	FILE *fpo = fopen(jobfile, "wb");
	if (!fpo)
		return;

	fseek(fp, 0, SEEK_END);
	size_t fsize = ftell(fp);
	rewind(fp);

	uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * fsize);
	if (!buffer)
		return;

	fread(buffer, 1, fsize, fp);

	struct jobheader header;
	strncpy(header.signature, magic, 8);
	header.version = PKGVER;
	header.compression = 0;

	fwrite(&header, 1, sizeof(struct jobheader), fpo);
	fwrite(buffer, 1, fsize, fpo);

	free(buffer);
	fclose(fpo);
	fclose(fp);
}

static void remove_header(const char *jobfile) {
	FILE *fp = fopen(jobfile, "rb");
	if (!fp)
		return;

	struct jobheader header;
	fread(&header, 1, sizeof(struct jobheader), fp);
	if (verify_header(&header)) {
		fclose(fp);
		return;
	}

	rewind(fp);

	char *str = strdup(jobfile);
	char *tarfile = strtok(str, ".");
	if (!tarfile) {
		free(str);
		return;
	}

	FILE *fpo = fopen(tarfile, "wb");
	if (!fpo)
		return;

	fseek(fp, 0, SEEK_END);
	size_t fsize = ftell(fp);
	fseek(fp, sizeof(struct jobheader), SEEK_SET);

	uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * (fsize - sizeof(struct jobheader)));
	if (!buffer)
		return;

	fread(buffer, 1, (fsize - sizeof(struct jobheader)), fp);
	fwrite(buffer, 1, fsize, fpo);

	free(str);
	free(buffer);
	fclose(fpo);
	fclose(fp);
}

int package_create(const char *tarfile, char *rootdir, libtar_list_t *list) {
	TAR *tar;
	char buf[1024];
	libtar_listptr_t listpointer;

	if (tar_open(&tar, tarfile, (use_zlib ? &gztype : NULL), O_WRONLY | O_CREAT, 0644, (verbose ? TAR_VERBOSE : 0) | (use_gnu ? TAR_GNU : 0)) == -1) {
		fprintf(stderr, "tar_open(): %s\n", strerror(errno));
		return 1;
	}

	libtar_listptr_reset(&listpointer);
	while (libtar_list_next(list, &listpointer) != 0) {
		char *pathname = (char *)libtar_listptr_data(&listpointer);
		if (pathname[0] != '/' && rootdir != NULL)
			snprintf(buf, sizeof(buf), "%s/%s", rootdir, pathname);
		else
			strncpy(buf, pathname, sizeof(buf));
		
		if (tar_append_tree(tar, buf, pathname) != 0) {
			fprintf(stderr, "tar_append_tree(\"%s\", \"%s\"): %s\n", buf, pathname, strerror(errno));
			tar_close(tar);
			return 1;
		}
	}

	if (tar_append_eof(tar) != 0) {
		fprintf(stderr, "tar_append_eof(): %s\n", strerror(errno));
		tar_close(tar);
		return 1;
	}

	if (tar_close(tar) != 0) {
		fprintf(stderr, "tar_close(): %s\n", strerror(errno));
		return 1;
	}

	append_header(tarfile);

	unlink(tarfile);

	return 0;
}

int package_extract(const char *jobfile) {
	TAR *tar;
	char dirname[1024];

	remove_header(jobfile);

	char *str = strdup(jobfile);
	char *tarfile = strtok(str, ".");

	strcpy(dirname, "package_");
	strcat(dirname, tarfile);

	mkdir(dirname, 0775);

	if (tar_open(&tar, tarfile, (use_zlib ? &gztype : NULL), O_RDONLY, 0, (verbose ? TAR_VERBOSE : 0) | (use_gnu ? TAR_GNU : 0)) == -1) {
		fprintf(stderr, "tar_open(): %s\n", strerror(errno));
		return 1;
	}

	if (tar_extract_all(tar, dirname) != 0) {
		fprintf(stderr, "tar_extract_all(): %s\n", strerror(errno));
		tar_close(tar);
		return 1;
	}

	if (tar_close(tar) != 0) {
		fprintf(stderr, "tar_close(): %s\n", strerror(errno));
		return 1;
	}

	unlink(tarfile);

	free(str);
	return 0;
}

int package_verify(const char *pkgfile) {
	FILE *fp = fopen(pkgfile, "rb");
	if (!fp)
		return 1;

	struct jobheader header;
	fread(&header, 1, sizeof(struct jobheader), fp);
	if (verify_header(&header)) {
		fclose(fp);
		return 1;
	}

	fclose(fp);

	puts("Package is OK");

	return 0;
}

void package_info(const char *pkgfile) {
	FILE *fp = fopen(pkgfile, "rb");
	if (!fp)
		return;

	struct jobheader header;
	fread(&header, 1, sizeof(struct jobheader), fp);
	if (verify_header(&header)) {
		fclose(fp);
		return;
	}

	printf(
		"Version: %u\n"
		"Use compression: %s\n"
		, header.version
		, header.compression  ? "Yes" : "No");

	fclose(fp);
}

void build_package() {

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
		return package_extract(argv[2]);
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
			verbose = 1;
			++jobname_idx;
			continue;
		}

		if (!strcmp(argv[c], "--skip-check")) {
			job_check = 0;
			++jobname_idx;
			continue;
		}

		if (!strcmp(argv[c], "--no-compression")) {
			compression = 0;
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
