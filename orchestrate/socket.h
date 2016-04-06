#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

/*!
	Socket object class.
	\author Miguel Boton Campo
*/
class CSocket {
	int sockFd;	//!< Socket descriptor.

  public:
	/*!
		Socket object constructor.
	*/
	CSocket(void);

	/*!
		Socket object constructor.
		\param fd the socket descriptor.
	*/
	CSocket(int fd);

	/*!
		Socket object destructor.
	*/
	~CSocket(void);

	/*!
		Create a socket.
		\returns true if success, false otherwise.
	*/
	bool Create(void);

	/*!
		Destroy the socket.
	*/
	void Destroy(void);

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
	CSocket *Accept(void);

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
