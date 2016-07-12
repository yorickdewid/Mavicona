/**
 * Copyright (c) 2016 Mavicona
 * All rights reserved.
 *
 * Mavicona Redis Client
 */

#include "xRedisClient.h"
#include <sstream>

void xRedisClient::quit() {
	Release();
}


bool xRedisClient::echo(const RedisDBIdx& dbi, const string& str, std::string& value) {
	if (0 == str.length()) {
		return false;
	}
	SETDEFAULTIOTYPE(MASTER);
	return command_string(dbi, value, "echo %s", str.c_str());
}














