#ifndef RDBMS_H
#define RDBMS_H

namespace Ace {

class Rdbms {
	Callback *cb = NULL;

  public:
	Rdbms() {}

	void Connect(const std::string& rdbms, const std::string& database, const std::string& user, const std::string& password) {
		cb->sqlConnect(rdbms, database, user, password);
	}

	void Query(const std::string& query) {
		cb->sqlQuery(query);
	}

	void Disconnect() {
		cb->sqlDisconnect();
	}

	void setCallback(Callback *_cb) {
		cb = _cb;
	}
};

}

#endif // RDBMS_H
