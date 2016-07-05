#include <string>
#include <algorithm>
#include <cctype>
#include <functional>
#include "url.h"

void url::parse(const std::string& url_s) {
	const std::string prot_end("://");
	std::string::const_iterator prot_i = std::search(url_s.begin(), url_s.end(), prot_end.begin(), prot_end.end());
	protocol_.reserve(distance(url_s.begin(), prot_i));

	std::transform(url_s.begin(), prot_i,
	               back_inserter(protocol_),
	               std::ptr_fun<int, int>(tolower));
	if (prot_i == url_s.end())
		return;

	std::advance(prot_i, prot_end.length());
	std::string::const_iterator path_i = find(prot_i, url_s.end(), '/');
	host_.reserve(std::distance(prot_i, path_i));
	std::transform(prot_i, path_i,
	               back_inserter(host_),
	               std::ptr_fun<int, int>(tolower));
	std::string::const_iterator query_i = find(path_i, url_s.end(), '?');
	path_.assign(path_i, query_i);
	if ( query_i != url_s.end() )
		++query_i;

	query_.assign(query_i, url_s.end());
}
