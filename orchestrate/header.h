#ifndef _HEADER_HPP_
#define _HEADER_HPP_

#include <sstream>
#include <string>

/*!
	HTTP header parser class.
	\author Miguel Boton Campo
*/
class CHeader {
	std::stringstream header;	//!< Header string.

  public:
	/*!
		Header object constructor.
	*/
	CHeader(void);

	/*!
		Header object constructor.
		\param header the http header string.
	*/
	CHeader(std::string header);

	/*!
		Clear header string.
	*/
	void Clear(void);

	/*!
		Generate the HTTP header.
		\returns the generated HTTP header string.
	*/
	std::string Generate(void);

	/*!
		Parse the request type.
		\param type the string buffer to store the type string.
		\returns true if success, otherwise false.
	*/
	bool GetType(std::string &type);

	/*!
		Parse the request path.
		\param path the string buffer to store the path string.
		\returns true if success, otherwise false.
	*/
	bool GetPath(std::string &path);

	/*!
		Set the header string.
		\param header the header string.
	*/
	void Set(std::string header);
	void Set(const char *header);

	/*!
		Add the HTTP reply.
		\param reply the reply number.
	*/
	void AddReply(int reply);

	/*!
		Add the HTTP reply.
		\param reply the reply number.
	*/
	void AddType(const std::string& type);

	/*!
		Add the HTTP reply.
		\param reply the reply number.
	*/
	void AddLocation(const std::string& location);

	void AddConnection(bool close = true);

	/*!
		Add the content length string.
		\param len the content length.
	*/
	void AddLength(size_t len);
	void AddAdditional();

	/*!
		Add the date.
	*/
	void AddDate(void);

	/*!
		Add the server name string.
	*/
	void AddServer(void);

	/*!
		Add the ending char sequence.
	*/
	void AddEnd(void);
};

#endif	/* _HEADER_HPP_ */
