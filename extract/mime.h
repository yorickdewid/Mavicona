#ifndef MIME_H
#define MIME_H

#include <cstddef>
#include <iostream>

class Mime {
  public:
	Mime(const std::string name, const std::string type, const std::string extension) :
		m_Name(name), m_MimeType(type), m_Extension(extension) {
	}

	Mime(const std::string name, const std::string type) :
		m_Name(name), m_MimeType(type) {
	}

	inline std::string name() {
		return m_Name;
	}

	inline std::string type() {
		return m_MimeType;
	}

	inline std::string extension() {
		return m_Extension;
	}

	inline std::string category() {
		std::size_t pos = m_MimeType.find('/');
		if (pos != std::string::npos) {
			return m_MimeType.substr(0, pos);
		}

		return m_MimeType;
	}

  private:
	std::string m_Name;
	std::string m_MimeType;
	std::string m_Extension;

	const char *parseMagic(const char *magicrs);
};

#endif // MIME_H
