#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <vector>

#include "socket.h"
#include "thread.h"

/*!
	Server class.
	\author Miguel Boton Campo
*/
class CServer {
	CSocket Socket;			//!< Socket object.

	std::vector <CThread *> threads;	//!< Thread array.
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
	bool CreateThread(CSocket *Socket);

  public:
	/*!
		Server object constructor.
		\param port the port to bind to the server (80 default).
	*/
	CServer(unsigned short port = 80);

	/*!
		Server object destructor.
	*/
	~CServer(void);

	/*!
		Start the server.
		\returns true if success, otherwise false.
	*/
	bool Start(void);

	/*!
		Stop the server.
	*/
	void Stop(void);

	/*!
		Accept an incoming connection.
		\returns true if success, otherwise false.
	*/
	bool Accept(void);
};

#endif	/* _HTTP_HPP */
