#include <cstdio>
#include <unistd.h>
#include <stdlib.h>

#include "socket.h"

CSocket::CSocket(void) {
	/* Reset descriptor */
	sockFd = -1;
}

CSocket::CSocket(int fd, struct sockaddr_in sockAddr) {
	/* Set descriptor */
	sockFd = fd;
	this->sockAddr = sockAddr;
}

CSocket::~CSocket(void) {
	/* Destroy socket */
	Destroy();
}

bool CSocket::Create(void) {
	/* Destroy socket */
	Destroy();

	/* Create socket */
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFd < 0)
		return false;

	return true;
}

void CSocket::Destroy(void) {
	/* Shutdown socket */
	if (sockFd >= 0)
		shutdown(sockFd, SHUT_RDWR);

	/* Reset descriptor */
	sockFd = -1;
}

std::string CSocket::RemoteAddr() {
	/*std::cout << "Connection received!" << std::endl;
	std::cout << "   IP Address: " << inet_ntoa(sockAddr.sin_addr) << std::endl;
	std::cout << "   Port: " << htons(sockAddr.sin_port) << std::endl;
	std::cout << std::endl;*/
	return inet_ntoa(sockAddr.sin_addr);
}

std::string CSocket::RemotePort() {
	return std::to_string(htons(sockAddr.sin_port));
}

bool CSocket::Bind(unsigned short port) {
	struct sockaddr_in sockAddr;

	int res;

	/* Socket address */
	sockAddr.sin_family      = AF_INET;
	sockAddr.sin_port        = htons(port);
	sockAddr.sin_addr.s_addr = INADDR_ANY;

	/* Bind address to socket */
	res = bind(sockFd, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
	if (res == -1)
		return false;

	return true;
}

bool CSocket::Listen(unsigned int max) {
	int res;

	/* Listen for connections */
	res = listen(sockFd, max);
	if (res == -1)
		return false;

	return true;
}
//#include <iostream>
CSocket *CSocket::Accept() {
	struct sockaddr_in sockAddr;
	socklen_t sockLen;

	int fd;

	/* Socket address length */
	sockLen = sizeof(sockAddr);

	/* Accept incoming connection */
	fd = accept(sockFd, (struct sockaddr *)&sockAddr, &sockLen);
	if (fd == -1)
		return NULL;

	/* Create socket */
	return (new CSocket(fd, sockAddr));
}

int CSocket::Send(const void *buffer, size_t len) {
	/* Send buffer */
	return send(sockFd, buffer, len, MSG_NOSIGNAL);
}

int CSocket::Receive(void *buffer, size_t len) {
	/* Receive buffer */
	return recv(sockFd, buffer, len, 0);
}
