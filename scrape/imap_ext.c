/**
 * Copyright (c) 2016 Mavicona
 * All rights reserved.
 *
 * IMAP_Ext:	IMAP client scaper
 */

#include <stdio.h>
#include <curl/curl.h>

// #define DEBUG

#include <mavdso.h>
#include <ini.h>

/* This is a simple example showing how to fetch mail using maviconas's DSO
 * capabilities. It builds on the libcurl imap adding transport
 * security to protect the authentication details from being snooped.
 *
 * Note that this example requires libcurl 7.30.0 or above.
 */

struct string {
	char *ptr;
	size_t len;
};

typedef struct {
	const char *host;
	const char *username;
	const char *password;
} configuration;

int init_string(struct string *s) {
	s->len = 0;
	s->ptr = (char *)malloc(1);
	if (!s->ptr) {
		fprintf(stderr, "malloc() failed\n");
		return 0;
	}

	s->ptr[0] = '\0';

	return 1;
}

size_t write_buffer(void *ptr, size_t size, size_t nmemb, struct string *s) {
	size_t new_len = s->len + (size * nmemb);
	s->ptr = (char *)realloc(s->ptr, new_len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		return 0;
	}

	memcpy(s->ptr + s->len, ptr, size * nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size * nmemb;
}

int imap_call(string *buffer, const char *username, const char *password, const char *host) {
	CURL *curl;
	CURLcode res = CURLE_OK;
	char uri[1024];

	memset(uri, '\0', 1024);
	strcpy(uri, host);
	strcat(uri, "/INBOX/;UID=1");

	curl = curl_easy_init();
	if (curl) {
		/* Set username and password */
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

		/* This will fetch message 1 from the user's inbox. Note the use of
		* imaps:// rather than imap:// to request a SSL based connection. */
		curl_easy_setopt(curl, CURLOPT_URL, uri);

		/* Since the traffic will be encrypted, it is very useful to turn on debug
		 * information within libcurl to see what is happening during the
		 * transfer */
#ifdef DEBUG
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

		/* Output buffer */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

		/* Perform the fetch */
		res = curl_easy_perform(curl);

		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		/* Always cleanup */
		curl_easy_cleanup(curl);
	}

	return (res == CURLE_OK);
}

static int handler(void *cnf, const char *section, const char *name, const char *value) {
	configuration *pconfig = (configuration *)cnf;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	if (MATCH("general", "host")) {
		pconfig->host = strdup(value);
	} else if (MATCH("user", "username")) {
		pconfig->username = strdup(value);
	} else if (MATCH("user", "password")) {
		pconfig->password = strdup(value);
	} else {
		return 0;  /* unknown section/name, error */
	}

	return 1;
}

int mav_main(int argc, char *argv[]) {
	struct string s;
	configuration config;

	if (argc < 2) {
		fprintf(stderr, "%s [config]\n", argv[0]);
		return 1;
	}

	if (!init_string(&s))
		return 1;

	if (ini_parse(argv[1], handler, &config) < 0) {
		fprintf(stderr, "Cannot load '%s'\n", argv[1]);
		return 1;
	}

	if (!imap_call(&s, config.username, config.password, config.host))
		return 1;

	push("mail", s.ptr, s.len);

	free(s.ptr);

	return 0;
}