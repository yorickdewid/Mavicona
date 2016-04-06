#include "http.h"

/* HTTP reply map */
std::map<int, const char *> CHTTP::replyMap;


void CHTTP::Init(void) {
	/* Fill the HTTP reply array */
	replyMap[REPLY_OK]     = "OK";
	replyMap[REPLY_BADREQ] = "Bad Request";
	replyMap[REPLY_NOENT]  = "Not Found";
	replyMap[REPLY_INTERR] = "Internal Error";
}

std::string CHTTP::GetDescription(int reply) {
	const char *description;

	/* Get description */
	description = replyMap[reply];

	/* Return string */
	return std::string(description);
}
