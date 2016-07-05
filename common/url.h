#ifndef URL_H
#define URL_H
#include <string>

struct url {
	url(const std::string& url_s) {
		parse(url_s);
	}

	inline std::string protocol() const {
		return protocol_;
	}

	inline std::string host() const {
		return host_;
	}

	inline std::string query() const {
		return query_;
	}

  private:
	void parse(const std::string& url_s);

	std::string protocol_, host_, path_, query_;
};

#endif // URL_H