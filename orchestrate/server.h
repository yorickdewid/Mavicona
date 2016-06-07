#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <vector>

#include "common/logger.h"
#include "socket.h"
#include "thread.h"

/*!
	Server class.
	\author Miguel Boton Campo
*/
class CServer {
	CSocket Socket;			//!< Socket object.
	FileLogger *logger = new FileLogger("access");

	std::vector<CThread *> threads;	//!< Thread array.
	unsigned short port;		//!< Server port number.

  private:
	/*!
		Client connection handler.
		\param client the client object pointer.
	*/
	static void *Handler(void *client);

	/*!
		Create a client thread.
		\param Socket the client socket object.
		\returns true if sucess, otherwise false.
	*/
	bool CreateThread(CSocket *Socket, FileLogger *log);

  public:
	/*!
		Server object constructor.
		\param port the port to bind to the server (80 default).
	*/
	CServer(unsigned short port = 80);

	/*!
		Server object destructor.
	*/
	~CServer();

	/*!
		Start the server.
		\returns true if success, otherwise false.
	*/
	bool Start();

	/*!
		Stop the server.
	*/
	void Stop(bool cleanup = false);

	/*!
		Accept an incoming connection.
		\returns true if success, otherwise false.
	*/
	bool Accept();
};

#endif	/* _HTTP_HPP */
