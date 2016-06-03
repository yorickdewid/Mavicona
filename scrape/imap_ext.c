#include <stdio.h>
#include <curl/curl.h>

#define DEBUG

#include <mavdso.h>

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
	s->ptr = (char *)realloc(s->ptr, new_len+1);
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

	curl = curl_easy_init();
	if(curl) {
		/* Set username and password */
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

		/* This will fetch message 1 from the user's inbox. Note the use of
		* imaps:// rather than imap:// to request a SSL based connection. */
		curl_easy_setopt(curl, CURLOPT_URL, host);

		/* Since the traffic will be encrypted, it is very useful to turn on debug
		 * information within libcurl to see what is happening during the
		 * transfer */
		// curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		/* Output buffer */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

		/* Perform the fetch */
		res = curl_easy_perform(curl);

		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		/* Always cleanup */
		curl_easy_cleanup(curl);
	}

	return (res == CURLE_OK);
}

int mav_main(int argc, char *argv[]) {
	struct string s;

	/*if (argc < 4) {
		fprintf(stderr, "%s [username] [password] [host]\n", argv[0]);
		return 1;
	}*/

	if (!init_string(&s))
		return 1;

	if (!imap_call(&s, "", "", ""))
		return 1;

	push("mail", s.ptr, s.len);

	free(s.ptr);

	return 0;
}