#include <iostream>
#include <errno.h>

#include "client.h"
#include "header.h"
#include "log.h"
#include "server.h"
#include "utils.h"

CServer::CServer(unsigned short port) {
	/* Set port number */
	this->port = port;
}

CServer::~CServer(void) {
	/* Stop server */
	Stop();
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
		CLog::Print("Request type: %s\n"
		            "Request path: %s", type.c_str(), path.c_str());

		/* Check request type */
		if (!type.compare("GET")) {
			/* Check path */
			if (!path.compare("/"))
				path = "/index.html";

			/* Send file */
			res = Client->SendFile(path);
		} else {
			/* Bad request */
			res = Client->HandleError(REPLY_BADREQ);
		}

		/* Disconnect if error */
		if (!res)
			break;
	}

	CLog::Print("Disconnecting...");

	/* Disconnect client */
	Client->Disconnect();

	/* Delete client */
	delete Client;

	return NULL;
}

bool CServer::Start(void) {
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

	return true;
}

void CServer::Stop(void) {
	std::vector<CThread *>::iterator it;

	/* Destroy socket */
	Socket.Destroy();

	/* Destroy threads */
	foreach (threads, it) {
		CThread *Thread = *it;

		CLog::Print("Destroying thread...");

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

bool CServer::Accept(void) {
	CSocket *Client;

	CLog::Print("Waiting for incoming connection...");

	/* Accept incoming connection */
	Client = Socket.Accept();
	if (!Client) {
		CLog::PrintErr("ERROR: Accept connection failed!");
		return false;
	}

	/* Show connection info */
	//cout << "Connection received!" << endl;
	//cout << "   IP Address: " << inet_ntoa(sockAddr.sin_addr) << endl;
	//cout << "   Port: " << htons(sockAddr.sin_port) << endl;
	//cout << endl;

	/* Create thread */
	return CreateThread(Client);
}

