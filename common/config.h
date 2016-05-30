#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <typeinfo>
#include <exception>

enum ConfigErrors {
	KEY_NOT_EXIST = 10,
	INVALID_SYNTAX,
	INVALID_CAST,
	FILE_NOT_FOUND,
};

class error: public std::exception {
	int line;
	enum ConfigErrors errorCode;

  public:
	error(enum ConfigErrors err, int errline = 0) : errorCode(err), line(errline) {}

	virtual const char *what() const throw() {
		switch(errorCode) {
			case KEY_NOT_EXIST:
				return "Key does not exist";
			case INVALID_SYNTAX:
				return ("Invalid syntax on line " + std::to_string(line)).c_str();
			case INVALID_CAST:
				return "Cannot convert type";
			case FILE_NOT_FOUND:
				return "Config file not found";
		}
	}

};

class Convert {
  public:
	template <typename T>
	static std::string T_to_string(T const &val) {
		std::ostringstream ostr;
		ostr << val;

		return ostr.str();
	}

	template <typename T>
	static T string_to_T(std::string const &val) {
		std::istringstream istr(val);
		T returnVal;
		if (!(istr >> returnVal))
			throw error(INVALID_CAST);

		return returnVal;
	}
};

class ConfigFile {
	std::multimap<std::string, std::string> contents;

	void removeComment(std::string &line) const {
		if (line.find(';') != line.npos)
			line.erase(line.find(';'));

		if (line.find('#') != line.npos)
			line.erase(line.find('#'));
	}

	bool onlyWhitespace(const std::string &line) const {
		return (line.find_first_not_of(' ') == line.npos);
	}

	bool validLine(const std::string &line) const {
		std::string temp = line;
		temp.erase(0, temp.find_first_not_of("\t "));
		if (temp[0] == '=')
			return false;

		for (size_t i = temp.find('=') + 1; i < temp.length(); i++)
			if (temp[i] != ' ')
				return true;

		return false;
	}

	void extractKey(std::string &key, size_t const &sepPos, const std::string &line) const {
		key = line.substr(0, sepPos);
		if (key.find('\t') != line.npos || key.find(' ') != line.npos)
			key.erase(key.find_first_of("\t "));
	}
	void extractValue(std::string &value, size_t const &sepPos, const std::string &line) const {
		value = line.substr(sepPos + 1);
		value.erase(0, value.find_first_not_of("\t "));
		value.erase(value.find_last_not_of("\t ") + 1);
	}

	void extractContents(const std::string &line) {
		std::string temp = line;
		temp.erase(0, temp.find_first_not_of("\t "));
		size_t sepPos = temp.find('=');

		std::string key, value;
		extractKey(key, sepPos, temp);
		extractValue(value, sepPos, temp);

		contents.insert(std::pair<std::string, std::string>(key, value));
	}

	void parseLine(const std::string &line, size_t const lineNo) {
		if (line.find('=') == line.npos)
			throw error(INVALID_SYNTAX, lineNo);

		if (!validLine(line))
			throw error(INVALID_SYNTAX, lineNo);

		extractContents(line);
	}

	void extractKeys(const std::string& filename) {
		std::ifstream file;
		file.open(filename.c_str());
		if (!file)
			throw error(FILE_NOT_FOUND);

		std::string line;
		size_t lineNo = 0;
		while (std::getline(file, line)) {
			lineNo++;
			std::string temp = line;

			if (temp.empty())
				continue;

			removeComment(temp);
			if (onlyWhitespace(temp))
				continue;

			parseLine(temp, lineNo);
		}

		file.close();
	}
  public:
	ConfigFile(const std::string& filename) {
		extractKeys(filename);
	}

	ConfigFile(const char *filename) : ConfigFile(std::string(filename)) {}

	typedef size_t size_type;
	typedef std::multimap<std::string, std::string>::iterator iterator;
	typedef std::multimap<std::string, std::string>::const_iterator const_iterator;
	typedef std::pair<iterator, iterator> range;

	bool exist(const std::string &key) const {
		return contents.find(key) != contents.end();
	}

	template <typename T>
	T get(const std::string& key, T const &defaultValue = T()) const {
		if (!exist(key))
			return defaultValue;

		return Convert::string_to_T<T>(contents.find(key)->second);
	}

	inline size_type size() const {
		return contents.size();
	}

	range find(const std::string& key) {
		contents.equal_range(key);
	}

	iterator begin() {
		return contents.begin();
	}

	iterator end() {
		return contents.end();
	}

	const_iterator begin() const {
		return contents.begin();
	}

	const_iterator end() const {
		return contents.end();
	}

	template <typename T>
	T operator[](const std::string& key) {
		return get<T>(key);
	}
};
