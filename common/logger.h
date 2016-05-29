#ifndef LOGGER_H
#define LOGGER_H

#include <ctime>
#include <fstream>
#include <sstream>

class FileLogger {
  public:
	enum class logType {
		LOG_ERROR,
		LOG_WARNING,
		LOG_INFO
	};

	explicit FileLogger(const char *module, bool debug = false) {
		char filename[128];
		strncpy(filename, module, 128);
		strncat(filename, "_debug.log", 128);

		logFile.open(filename, std::ofstream::out | std::ofstream::app);

		if (logFile.is_open()) {
			logFile << getTimesamp() << "Log started" << std::endl;
		}
	}

	~FileLogger() {
		if (logFile.is_open()) {
			logFile << getTimesamp() << "Log stopped" << std::endl;
			logFile.close();
		}
	}

	friend FileLogger &operator << (FileLogger &logger, const logType l_type) {
		switch (l_type) {
			case FileLogger::logType::LOG_ERROR:
				logger.logFile << "[ERROR]: ";
				std::cout << "[ERROR]: " << std::endl;
				break;

			case FileLogger::logType::LOG_WARNING:
				logger.logFile << "[WARNING]: ";
				std::cout << "[WARNING]: " << std::endl;
				break;

			default:
				logger.logFile << "[INFO]: ";
				std::cout << "[INFO]: " << std::endl;
				break;
		}

		return logger;
	}

	friend FileLogger &operator << (FileLogger &logger, const char *text) {
		logger.logFile << FileLogger::getTimesamp() << text << std::endl;
		std::cout << text << std::endl;
		return logger;

	}

	FileLogger(const FileLogger &) = delete;
	FileLogger &operator= (const FileLogger &) = delete;

  private:
	std::ofstream logFile;

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
};

#endif // LOGGER_H
