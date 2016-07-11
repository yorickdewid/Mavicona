#include <cstdio>
#include <memory.h>
#include <unistd.h>
#include <stdlib.h>

#include "socket.h"

CSocket::CSocket() {
	/* Reset descriptor */
	sockFd = -1;
}

CSocket::CSocket(int fd, struct sockaddr_in6 sockAddr) {
	/* Set descriptor */
	sockFd = fd;
	this->sockAddr = sockAddr;
}

CSocket::~CSocket(void) {
	/* Destroy socket */
	Destroy();
}

bool CSocket::Create() {
	/* Destroy socket */
	Destroy();

	/* Create socket */
	sockFd = socket(AF_INET6, SOCK_STREAM, 0);
	if (sockFd < 0)
		return false;

	int on = 1;
	if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
		return false;

	return true;
}

void CSocket::Destroy() {
	/* Shutdown socket */
	if (sockFd >= 0)
		shutdown(sockFd, SHUT_RDWR);

	/* Reset descriptor */
	sockFd = -1;
}

std::string CSocket::RemoteAddr() {
	char str[INET6_ADDRSTRLEN];

	inet_ntop(AF_INET6, &sockAddr.sin6_addr, str, sizeof(str));

	return str;
}

std::string CSocket::RemotePort() {
	return std::to_string(htons(sockAddr.sin6_port));
}

bool CSocket::Bind(unsigned short port) {
	struct sockaddr_in6 sockAddr;

	memset(&sockAddr, 0, sizeof(sockAddr));

	int res;

	/* Socket address */
	sockAddr.sin6_family    = AF_INET6;
	sockAddr.sin6_port      = htons(port);
	sockAddr.sin6_addr      = in6addr_any;

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

CSocket *CSocket::Accept() {
	struct sockaddr_in6 sockAddr;
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
