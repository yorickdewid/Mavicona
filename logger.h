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

	explicit FileLogger(const char *engine_version, const char *fname = "mavicona_main.log") {
		logFile.open(fname);

		if (logFile.is_open()) {
			logFile << getTimesamp() << "Logger started" << std::endl;
			logFile << getTimesamp() << "Version " << engine_version << std::endl;
		}
	}

	~FileLogger() {
		if (logFile.is_open()) {
			logFile << getTimesamp() << "Logger stopped" << std::endl;
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
		std::stringstream ss;
		time_t t = time(0);
		struct tm * now = localtime(& t);
		ss << (now->tm_year + 1900) << '-'
		   << (now->tm_mon + 1) << '-'
		   <<  now->tm_mday << ' ';

		return ss.str();
	}
};

#endif // LOGGER_H
