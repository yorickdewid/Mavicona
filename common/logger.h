/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <ctime>
#include <cstring>
#include <fstream>
#include <sstream>

class FileLogger {
	enum class logType {
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR,
		LOG_DEBUG1,
		LOG_DEBUG2,
	};

	std::ofstream logFile;
	std::stringstream buffer;
	logType type = logType::LOG_INFO;

	bool do_echo = true;
	bool do_timestamp = true;

	static std::string getTimesamp() {
		time_t rawtime;
		char datetime[32];

		/* Get date */
		time(&rawtime);
		struct tm *timeinfo = localtime(&rawtime);

		/* Generate message */
		strftime(datetime, sizeof(datetime), "[%x - %X] ", timeinfo);

		return datetime;
	}

	std::string getType() {
		switch (this->type) {
			case logType::LOG_ERROR:
				return "[error] ";

			case logType::LOG_WARNING:
				return "[warning] ";

			case logType::LOG_DEBUG1:
				return "[debug 1] ";

			case logType::LOG_DEBUG2:
				return "[debug 2] ";

			default:
				return "[info] ";
		}

		return "";
	}

  public:
	struct debug {
		int level;
		debug(int l) : level(l) {}
	};
	struct warning {};
	struct error {};
	struct endl {};

	explicit FileLogger(const std::string& module, bool appendMod = true) : FileLogger(module.c_str(), appendMod) {}

	explicit FileLogger(const char *module, bool appendMod = true) {
		char filename[128];
		strncpy(filename, module, 128);
		
		if (appendMod)
			strncat(filename, "_module.log", 128);

		logFile.open(filename, std::ios::out | std::ios::app);
		if (logFile.fail())
			std::cerr << "Cannot start logger" << std::endl;
	}

	void flush() {
		std::string line;

		while (std::getline(buffer, line, '\n')) {
			if (do_timestamp)
				logFile << getTimesamp() << getType() << line << std::endl;
			else
				logFile << line << std::endl;
		}

		logFile.flush();
		buffer.clear();
		buffer.str(std::string());
		type = logType::LOG_INFO;
	}

	void close() {
		flush();
		logFile.close();
	}

	inline bool is_open() {
		return logFile.is_open();
	}

	inline bool enableEcho() {
		return do_echo = true;
	}

	inline bool disableEcho() {
		return do_echo = false;
	}

	inline bool enableTimestamp() {
		return do_timestamp = true;
	}

	inline bool disableTimestamp() {
		return do_timestamp = false;
	}

	void write(const char *s, size_t n) {
		this->buffer.write(s, n);
		if (do_echo)
			std::cout << std::string(s, n);
	}

	~FileLogger() {
		close();
	}

	template<typename T>
	FileLogger &operator <<(const T &t) {
		this->buffer << t;
		if (do_echo)
			std::cout << t;
		return *this;
	}

	FileLogger &operator <<(const error e) {
		type = logType::LOG_ERROR;
		return *this;
	}

	FileLogger &operator <<(const warning w) {
		type = logType::LOG_WARNING;
		return *this;
	}

	FileLogger &operator <<(const debug d) {
		if (d.level == 1)
			type = logType::LOG_DEBUG1;
		if (d.level == 2)
			type = logType::LOG_DEBUG2;
		return *this;
	}

	FileLogger &operator <<(const endl e) {
		this->buffer << std::endl;
		if (do_echo)
			std::cout << std::endl;
		flush();
		return *this;
	}

	FileLogger(const FileLogger &) = delete;
	FileLogger &operator= (const FileLogger &) = delete;
};

#endif // LOGGER_H
