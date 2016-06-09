#include <ctime>

#include "header.h"
#include "http.h"
#include "orchestrator.h"

/* Constants */
#define CRLF	"\r\n"


CHeader::CHeader(void) {
	/* Clear header */
	Clear();
}

CHeader::CHeader(std::string header) {
	/* Set the header string */
	Set(header);
}

void CHeader::Clear(void) {
	/* Clear header string */
	header.str("");
}

std::string CHeader::Generate(void) {
	/* Return header string */
	return header.str();
}

bool CHeader::GetType(std::string &type) {
	std::string str = Generate();

	size_t npos;

	/* Find the first space */
	npos = str.find_first_of(" ");

	/* Not found */
	if (npos == std::string::npos)
		return false;

	/* Extract the request type */
	type = str.substr(0, npos);

	return true;
}

bool CHeader::GetPath(std::string &path) {
	std::string str = Generate();

	size_t pos, npos;

	/* Find the first space */
	pos = str.find_first_of(" ");

	/* Not found */
	if (pos == std::string::npos)
		return false;

	/* Find the second space */
	npos = str.find_first_of(" ", ++pos);

	/* Not found */
	if (npos == std::string::npos)
		return false;

	/* Extract the request path */
	path = str.substr(pos, npos - pos);

	return true;
}

void CHeader::Set(std::string _header) {
	/* Set the header string */
	header << _header;
}

void CHeader::Set(const char *header) {
	std::string s(header);

	/* Set the header string */
	Set(s);
}

void CHeader::AddReply(int reply) {
	/* Add the HTTP reply string */
	header << "HTTP/1.1 " << reply << " " << CHTTP::GetDescription(reply) << CRLF;
}

void CHeader::AddType(const std::string& type) {
	/* Add the content type */
	if (!type.empty())
		header << "Content-type: " << type << CRLF;
}

void CHeader::AddLocation(const std::string& location) {
	/* Add the content type */
	header << "Location: " << location << CRLF;
}

void CHeader::AddConnection(bool close) {
	/* Add the content type */
	header << "Connection: " << (close ? "Close" : "Keep-Alive") << CRLF;
}

void CHeader::AddLength(size_t len) {
	/* Add the content length */
	header << "Content-length: " << len << CRLF;
}

void CHeader::AddAdditional() {
	header << "X-Webledge: kirama" << CRLF;
	header << "X-Webledge-SubId: 325434522" << CRLF;
	header << "X-Webledge-Act: contract" << CRLF;
	header << "Via: Webledge " APP_VERSION << CRLF;
}

void CHeader::AddDate(void) {
	time_t     rawtime;
	struct tm *timeinfo;

	char date[128];

	/* Get time */
	time(&rawtime);

	/* Timezone conversion */
	timeinfo = localtime(&rawtime);

	/* Generate the date string */
	strftime(date, sizeof(date), "%a, %d %b %Y %X %Z", timeinfo);

	/* Add the date */
	header << "Date: " << date << CRLF;
}

void CHeader::AddServer(void) {
	/* Add the server name */
	header << "Server: " << APP_NAME << "/" << APP_VERSION << CRLF;
}

void CHeader::AddEnd(void) {
	header << CRLF;
}
