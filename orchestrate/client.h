#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <string>

#include "common/logger.h"
#include "header.h"
#include "http.h"
#include "socket.h"

/*!
	Client class.
	\author Miguel Boton Campo.
*/
class CClient {
	CSocket *Socket;			//!< Client socket object.

  private:
	/*!
		Send a HTTP header to the client.
		\param header the header object.
		\returns true if success, otherwise false.
	*/
	bool SendHeader(CHeader& Header);

  public:
	FileLogger *logger;

	/*!
		Client object constructor.
		\param fd the client socket descriptor.
	*/
	CClient(CSocket *Socket, FileLogger *log);

	/*!
		Client object destructor.
	*/
	~CClient(void);

	/*!
		Disconnect the client.
	*/
	void Disconnect();

	std::string Remote();

	/*!
		Read data from the client.
		\param buffer the data buffer pointer.
		\param len the data buffer length.
		\returns the number of bytes read.
	*/
	int Read(void *buffer, size_t len);

	/*!
		Write data to the client.
		\param buffer the data buffer pointer.
		\param len the data buffer length.
		\returns the number of bytes written.
	*/
	int Write(const void *buffer, size_t len);

	/*!
		Read data from the client.
		The loop finishes when the buffer is filled.
		\param buffer the data buffer pointer.
		\param len the data buffer length.
		\returns the number of bytes read.
	*/
	int ReadLoop(void *buffer, size_t len);

	/*!
		Send data to the client.
		\param buffer the data buffer pointer.
		\param len the data buffer length.
		\returns true if success, otherwise false.
	*/
	bool SendHtml(const void *buffer, size_t len, int reply = REPLY_OK);


	bool SendRedirect(const std::string& location);
	/*!
		Send file to the client.
		\param filepath the file to send.
		\returns true if success, otherwise false.
	*/
	bool SendFile(std::string filepath);
	bool SendFile(const char *filepath);

	/*!
		Send file to the client.
		\param filepath the file to send.
		\returns true if success, otherwise false.
	*/
	bool ParseUri(std::string filepath, const std::string& type, const std::string& data);

	/*!
		Receive request from the client.
		\param header the header object.
		\returns true if success, otherwise false.
	*/
	bool RecvRequest(CHeader& Header, std::string& data);

	/*!
		Handle a request error.
		\param reply the request reply number.
	*/
	bool HandleError(int reply);
};

#endif	/* _CLIENT_H_ */
