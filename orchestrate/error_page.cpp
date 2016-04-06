#include <sstream>

#include "error_page.h"
#include "http.h"
#include "orchestrator.h"

std::string CErrorPage::Get(int reply) {
	std::string page;

	/* Check reply code */
	switch (reply) {
		case REPLY_NOENT:	/* 404 */
			page = E404();
			break;
		case REPLY_INTERR:	/* 500 */
			page = E500();
			break;
		case REPLY_NOTIMP:	/* 501 */
			page = E501();
			break;
		default:
			page = Generate(reply);
	}

	return page;
}

std::string CErrorPage::Generate(int reply) {
	std::stringstream ss;
	std::string       desc;

	/* Get the description */
	desc = CHTTP::GetDescription(reply);

	/* No description */
	if (desc.empty())
		desc = "Unknown";

	/* HTML code */
	ss << "<html>" << std::endl;
	ss << "<head><title>Error " << reply << " - " << desc << "</title></head>" << std::endl;
	ss << "<body><h1>Error "    << reply << " - " << desc << "</h1>" << std::endl;
	ss << "<hr>" << APP_NAME << "/" << APP_VERSION << "</hr></body>" << std::endl;
	ss << "</html>";

	/* Return string */
	return ss.str();
}

std::string CErrorPage::E404(void) {
	std::stringstream ss;
	std::string       desc;

	/* Get the description */
	desc = CHTTP::GetDescription(404);

	/* HTML code */
	ss << "<html>" << std::endl;
	ss << "<head><title>Error 404 - " << desc << "</title></head>" << std::endl;
	ss << "<body><h1>Error 404 - " << desc << "</h1>" << std::endl;
	ss << "<h3>The requested page does not exist.</h3>" << std::endl;
	ss << "<hr>" << APP_NAME << "/" << APP_VERSION << "</hr></body>" << std::endl;
	ss << "</html>";

	/* Return string */
	return ss.str();
}

std::string CErrorPage::E500(void) {
	std::stringstream ss;
	std::string       desc;

	/* Get the description */
	desc = CHTTP::GetDescription(500);

	/* HTML code */
	ss << "<html>" << std::endl;
	ss << "<head><title>Error 500 - " << desc << "</title></head>" << std::endl;
	ss << "<body><h1>Error 500 - " << desc << "</h1>" << std::endl;
	ss << "<hr>" << APP_NAME << "/" << APP_VERSION << "</hr></body>" << std::endl;
	ss << "</html>";

	/* Return string */
	return ss.str();
}

std::string CErrorPage::E501(void) {
	std::stringstream ss;
	std::string       desc;

	/* Get the description */
	desc = CHTTP::GetDescription(501);

	/* HTML code */
	ss << "<html>" << std::endl;
	ss << "<head><title>Error 501 - " << desc << "</title></head>" << std::endl;
	ss << "<body><h1>Error 501 - " << desc << "</h1>" << std::endl;
	ss << "<hr>" << APP_NAME << "/" << APP_VERSION << "</hr></body>" << std::endl;
	ss << "</html>";

	/* Return string */
	return ss.str();
}
