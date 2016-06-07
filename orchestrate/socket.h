#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

/*!
	Socket object class.
	\author Miguel Boton Campo
*/
class CSocket {
	int sockFd;	//!< Socket descriptor.
	struct sockaddr_in6 sockAddr;

  public:
	/*!
		Socket object constructor.
	*/
	CSocket();

	/*!
		Socket object constructor.
		\param fd the socket descriptor.
	*/
	CSocket(int fd, struct sockaddr_in6 sockAddr);

	/*!
		Socket object destructor.
	*/
	~CSocket();

	/*!
		Create a socket.
		\returns true if success, false otherwise.
	*/
	bool Create();

	/*!
		Destroy the socket.
	*/
	void Destroy();

	std::string RemoteAddr();
	std::string RemotePort();

	/*!
		Bind the socket to a port.
		\param port the port to bind the socket.
		\returns true if success, otherwise false.
	*/
	bool Bind(unsigned short port);

	/*!
		Listen for incoming connections.
		\param max maximum number of simultaneously connections.
		\returns true if success, otherwise false.
	*/
	bool Listen(unsigned int max);

	/*!
		Accept incoming connection.
	*/
	CSocket *Accept();

	/*!
		Send data through the socket.
		\param buffer the buffer pointer.
		\param len the buffer length.
		\returns the number of bytes sent.
	*/
	int Send(const void *buffer, size_t len);

	/*!
		Receive data through the socket.
		\param buffer the buffer pointer.
		\param len the buffer length.
		\returns the number of bytes received.
	*/
	int Receive(void *buffer, size_t len);
};

#endif	/* _NETWORK_HPP_ */
