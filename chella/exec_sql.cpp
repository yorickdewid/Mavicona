#include <exception>
#ifdef RDBMS
#include <backends/postgresql/soci-postgresql.h>
#include "exec.h"

void Execute::sqlConnect(const std::string& rdbms, const std::string& database, const std::string& user, const std::string& password) {
	try {
		this->session = new soci::session(rdbms, "host=localhost dbname=" + database + " user=" + user + " password=" + password);
	} catch (std::exception const& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void Execute::sqlQuery(const std::string& query) {
	if (!this->session)
		return;

	try {
		(*this->session) << query;
	} catch (std::exception const& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void Execute::sqlDisconnect() {
	if (!this->session)
		return;

	delete this->session;
	this->session = nullptr;
}
#endif
