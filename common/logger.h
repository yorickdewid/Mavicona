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

	explicit FileLogger(const std::string & module) : FileLogger(module.c_str()) {}

	explicit FileLogger(const char *module) {
		char filename[128];
		strncpy(filename, module, 128);
		strncat(filename, "_module.log", 128);

		logFile.open(filename, std::ios::out | std::ios::app);

		if (logFile.is_open()) {
			//logFile << getTimesamp() << getType() << "Module started" << std::endl;
		} else {
			std::cerr << "Cannot start logger" << std::endl;
		}
	}

	void flush() {
		std::string line;

		while (std::getline(buffer, line, '\n')) {
			logFile << getTimesamp() << getType() << line << std::endl;
		}

		logFile.flush();
		buffer.clear();
		buffer.str(std::string());
		type = logType::LOG_INFO;
	}

	void close() {
		flush();
		//logFile << getTimesamp() << getType() << "Module stopped" << std::endl;
		logFile.close();
	}

	inline bool is_open() {
		return logFile.is_open();
	}

	/*FileLogger &write(const char *s, size_t n) {
		this->buffer << s;
		std::cout << s;
		return *this;
	}*/

	~FileLogger() {
		close();
	}

	template<typename T>
	FileLogger &operator <<(const T &t) {
		this->buffer << t;
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
		std::cout << std::endl;
		flush();
		return *this;
	}

	FileLogger(const FileLogger &) = delete;
	FileLogger &operator= (const FileLogger &) = delete;
};

#endif // LOGGER_H
