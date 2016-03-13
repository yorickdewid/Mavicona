#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>

class FileLogger {
  public:
	enum class logType {
		LOG_ERROR,
		LOG_WARNING,
		LOG_INFO
	};

	explicit FileLogger(const char *engine_version, const char *fname = "mavicona_main.log")
		:   numWarnings(0U),
		    numErrors(0U) {

		myFile.open(fname);

		if (myFile.is_open()) {
			myFile << "My Game Engine, version " << engine_version << std::endl;
			myFile << "Log file created" << std::endl << std::endl;
		}
	}

	~FileLogger() {
		if (myFile.is_open()) {
			myFile << std::endl << std::endl;

			myFile << numWarnings << " warnings" << std::endl;
			myFile << numErrors << " errors" << std::endl;

			myFile.close();
		}
	}

	friend FileLogger &operator << (FileLogger &logger, const logType l_type) {
		switch (l_type) {
			case FileLogger::logType::LOG_ERROR:
				logger.myFile << "[ERROR]: ";
				++logger.numErrors;
				break;

			case FileLogger::logType::LOG_WARNING:
				logger.myFile << "[WARNING]: ";
				++logger.numWarnings;
				break;

			default:
				logger.myFile << "[INFO]: ";
				break;
		}

		return logger;
	}

	friend FileLogger &operator << (FileLogger &logger, const char *text) {
		logger.myFile << text << std::endl;
		return logger;

	}

	FileLogger(const FileLogger &) = delete;
	FileLogger &operator= (const FileLogger &) = delete;

  private:
	std::ofstream           myFile;

	unsigned int            numWarnings;
	unsigned int            numErrors;

};

#endif // LOGGER_H
