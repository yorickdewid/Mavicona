#include <quidpp.h>

#include "orchestrator.h"
#include "service.h"
#include "common/json.h"

std::string CSerivce::GetAllProcedures() {
	nlohmann::json object;
	object["active"] = true;
	object["rev"] = 100;
	object["server"] = std::string(APP_NAME) + "/" + std::string(APP_VERSION);
	object["type"] = "jsonrpc";
	object["success"] = true;
	object["message"] = nullptr;
	object["quid"] = quidpp::Quid().toString();
	object["procedure"] = nullptr;
	object["return"]["procedures"] = {
		{"jobcount", "Number of active chella jobs"},
		{"queue", "Items in queue"},
		{"nodes", "List active cluster nodes"},
		{"status", "Server status"},
		{"info", "Cluster information"},
		{"solicit", "Register instance"},
		{"quid", "Request new quid"},
		{"log", "Service logs"},
		{"service", "Service control"}
	};

	return object.dump(4);
}

std::string CSerivce::CallProcedure(const std::string& data) {
	nlohmann::json object;
	object["active"] = true;
	object["rev"] = 100;
	object["server"] = std::string(APP_NAME) + "/" + std::string(APP_VERSION);
	object["type"] = "jsonrpc";
	object["success"] = true;
	object["message"] = nullptr;
	object["quid"] = quidpp::Quid().toString();
	object["procedure"] = "quid";
	object["return"] = true;

	return object.dump(4);
}
