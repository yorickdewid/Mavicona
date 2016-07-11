#include <iostream>
#include <memory>
#include <errno.h>
#include <unistd.h>

#include "client.h"
#include "header.h"
#include "server.h"
#include "utils.h"

#define MAX_THREAD_COUNT	40

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
	std::unique_ptr<CClient> Client(static_cast<CClient *>(client));
	bool res;

	/* Thread loop */
	while (1) {
		CHeader Header;
		std::string path, type, data;

		/* Receive request */
		res = Client->RecvRequest(Header, data);
		if (!res)
			break;

		/* Get request type/path */
		Header.GetType(type);
		Header.GetPath(path);

		/* Show request info */
		(*Client->logger) << "Request " << Client->Remote() << " [" << type << "] " << path << FileLogger::endl();

		/* Check request type */
		if (!type.compare("GET") || !type.compare("POST") || !type.compare("PUT") || !type.compare("DELETE")) {

			/* Check path */
			if (!path.compare("/")) {
				res = Client->SendRedirect("/core::webledge::");
				continue;
			}

			/* Match internal calls */
			res = Client->ParseUri(path, type, data);
			if (res)
				continue;

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

	/* Disconnect client */
	Client->Disconnect();

	return NULL;
}

bool CServer::Start() {
	bool res;

	/* Create socket */
	res = Socket.Create();
	if (!res) {
		(*logger) << FileLogger::error() << "Socket creation failed" << FileLogger::endl();
		return false;
	}

	/* Bind address */
	res = Socket.Bind(port);
	if (!res) {
		(*logger) << FileLogger::error() << "Bind address failed" << FileLogger::endl();
		return false;
	}

	/* Listen for connections */
	res = Socket.Listen(20);
	if (!res) {
		(*logger) << FileLogger::error() << "Listen failed" << FileLogger::endl();
		return false;
	}

	if (chdir("wwwroot")) {
		(*logger) << FileLogger::error() << "Cannot change directory to wwwroot/" << FileLogger::endl();
		return false;
	}

	return true;
}

void CServer::Stop(bool cleanup) {
	/* Destroy threads */
	for (unsigned int i = 0; i < (cleanup ? threads.size() / 2 : threads.size()); ++i) {
		CThread *Thread = threads[i];

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

bool CServer::CreateThread(CSocket *Socket, FileLogger *log) {
	CClient *Client;
	CThread *Thread;

	bool res;

	/* Clean the pool */
	if (threads.size() > MAX_THREAD_COUNT)
		Stop(true);

	/* Create client object */
	Client = new CClient(Socket, log);
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
		(*logger) << FileLogger::error() << "Accept connection failed" << FileLogger::endl();
		return false;
	}

	/* Create thread */
	return CreateThread(Client, logger);
}

