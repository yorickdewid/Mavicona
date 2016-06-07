#include <iostream>
#include <errno.h>
#include <unistd.h>

#include "client.h"
#include "header.h"
#include "log.h"
#include "server.h"
#include "utils.h"

#define MAX_THREAD_COUNT	20

CServer::CServer(unsigned short _port) {
	/* Set port number */
	port = _port;
}

CServer::~CServer() {
	/* Stop server */
	Stop();

	delete logger;

	/* Destroy socket */
	Socket.Destroy();
}

void *CServer::Handler(void *client) {
	CClient *Client = static_cast<CClient *>(client);
	bool     res;

	/* Thread loop */
	while (1) {
		CHeader Header;
		std::string path, type;

		/* Receive request */
		res = Client->RecvRequest(Header);
		if (!res)
			break;

		/* Get request type/path */
		Header.GetType(type);
		Header.GetPath(path);

		/* Show request info */
		CLog::Print("Request %s [%s] %s", Client->Remote().c_str(), type.c_str(), path.c_str());
		// (*logger) << "Request [" << type << "] " << path << FileLogger::endl();

		/* Check request type */
		if (!type.compare("GET")) {
			/* Check path */
			if (!path.compare("/")) {
				res = Client->SendRedirect("/core::webledge::");
				continue;
			}

			/* Match internal calls */
			res = Client->ParseUri(path);

			/* Send file */
			// res = Client->SendFile(path);
			res = Client->HandleError(REPLY_BADREQ);
		} else {
			/* Bad request */
			res = Client->HandleError(REPLY_BADREQ);
		}

		/* Disconnect if error */
		if (!res)
			break;
	}

	// CLog::Print("Disconnecting..."); 

	/* Disconnect client */
	Client->Disconnect();

	/* Delete client */
	delete Client;

	return NULL;
}

bool CServer::Start() {
	bool res;

	/* Create socket */
	res = Socket.Create();
	if (!res) {
		CLog::PrintErr("ERROR: Socket creation failed!");
		return false;
	}

	/* Bind address */
	res = Socket.Bind(port);
	if (!res) {
		CLog::PrintErr("ERROR: Bind address failed!");
		return false;
	}

	/* Listen for connections */
	res = Socket.Listen(10);
	if (!res) {
		CLog::PrintErr("ERROR: Listen failed!");
		return false;
	}

	if (chdir("wwwroot")) {
		CLog::PrintErr("ERROR: Cannot change directory!");
		return false;
	}

	return true;
}

void CServer::Stop() {
	std::vector<CThread *>::iterator it;

	/* Destroy threads */
	foreach(threads, it) {
		CThread *Thread = *it;

		/* Destroy thread */
		Thread->Destroy();

		/* Join thread */
		Thread->Join();

		/* Delete thread */
		delete Thread;
	}

	/* Clear threads array */
	threads.clear();
}

bool CServer::CreateThread(CSocket *Socket) {
	CClient *Client;
	CThread *Thread;

	bool res;

	/* Clean the pool */
	if (threads.size() > MAX_THREAD_COUNT)
		Stop();

	/* Create client object */
	Client = new CClient(Socket);
	if (!Client)
		return false;

	/* Create thread object */
	Thread = new CThread();
	if (!Thread)
		return false;

	/* Create thread */
	res = Thread->Create(Handler, Client);
	if (!res)
		return false;

	/* Push thread into the array */
	threads.push_back(Thread);

	return true;
}

bool CServer::Accept() {
	CSocket *Client;

	/* Accept incoming connection */
	Client = Socket.Accept();
	if (!Client) {
		CLog::PrintErr("ERROR: Accept connection failed!");
		return false;
	}

	/* Show connection info */
	// std::cout << "Connection received!" << endl;
	// std::cout << "   IP Address: " << inet_ntoa(sockAddr.sin_addr) << endl;
	// std::cout << "   Port: " << htons(sockAddr.sin_port) << endl;
	// std::cout << endl;

	/* Create thread */
	return CreateThread(Client);
}

