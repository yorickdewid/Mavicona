#include <iostream>
#include <fstream>

#include "client.h"
#include "error_page.h"
#include "file.h"

/* Constants */
#define BUF_MAXLEN	1024


CClient::CClient(CSocket *Socket) {
	/* Set the client socket */
	this->Socket = Socket;
}

CClient::~CClient(void) {
	/* Disconnect client */
	Disconnect();

	/* Delete socket */
	delete Socket;
}

void CClient::Disconnect(void) {
	/* Destroy socket */
	Socket->Destroy();
}

int CClient::Read(void *buffer, size_t len) {
	/* Receive data */
	return Socket->Receive(buffer, len);
}

int CClient::Write(const void *buffer, size_t len) {
	unsigned char *ptr = (unsigned char *)buffer;

	size_t  cnt = 0;
	ssize_t res;

	/* Write loop */
	while (cnt < len) {
		/* Send data */
		res = Socket->Send(ptr + cnt, len - cnt);

		/* Check result */
		if (res == -1)
			return res;
		if (res == 0)
			break;

		/* Increment counter */
		cnt += res;
	}

	return cnt;
}

int CClient::ReadLoop(void *buffer, size_t len) {
	unsigned char *ptr = (unsigned char *)buffer;

	size_t  cnt = 0;
	ssize_t res;

	/* Read loop */
	while (cnt < len) {
		/* Receive data */
		res = Socket->Receive(ptr + cnt, len - cnt);

		/* Check result */
		if (res == -1)
			return res;
		if (res == 0)
			break;

		/* Increment counter */
		cnt += res;
	}

	return cnt;
}

bool CClient::Send(const void *buffer, size_t len, int reply) {
	CHeader Header;

	ssize_t res;
	bool    ret;

	/* Setup header */
	Header.AddReply(reply);
	Header.AddDate();
	Header.AddServer();
	if (len > 0)
		Header.AddLength(len);
	Header.AddEnd();

	/* Send header */
	ret = SendHeader(Header);
	if (!ret)
		return false;

	/* Send data */
	if (len > 0) {
		res = Write(buffer, len);
		if (res <= 0)
			return false;
	}

	return true;
}

bool CClient::SendHeader(CHeader &Header) {
	std::string  hdr;
	int len, res;

	/* Generate header */
	hdr = Header.Generate();
	len = hdr.length();

	/* Send header */
	res = Write(hdr.c_str(), len);
	if (res != len)
		return false;

	return true;
}

bool CClient::SendFile(std::string filepath) {
	const char *s = filepath.c_str();

	/* Send file */
	return SendFile(s);
}

bool CClient::SendFile(const char *filepath) {
	CFile   File;
	CHeader Header;

	char   buffer[BUF_MAXLEN];
	size_t len;
	bool   res;

	/* Open file */
	res = File.Open(filepath);

	/* File not found */
	if (!res) {
		/* Handle error */
		HandleError(REPLY_NOENT);
		goto out;
	}

	/* Get file size */
	len = File.Size();

	/* No contents? */
	if (!len) {
		/* Handle error */
		HandleError(REPLY_INTERR);

		res = false;
		goto out;
	}

	/* Setup header */
	Header.AddReply(REPLY_OK);
	Header.AddDate();
	Header.AddServer();
	Header.AddLength(len);
	Header.AddEnd();

	/* Send header */
	res = SendHeader(Header);
	if (!res)
		goto out;

	/* Read loop */
	while (len) {
		ssize_t ret, size;

		/* Block size */
		size = (len > BUF_MAXLEN) ? BUF_MAXLEN : len;

		/* Read block */
		res = File.Read(buffer, size);
		if (!res)
			break;

		/* Send block */
		ret = Write(buffer, size);
		if (ret != size) {
			res = false;
			break;
		}

		/* Update length */
		len -= size;
	}

out:
	/* Close file */
	File.Close();

	return res;
}

bool CClient::RecvRequest(CHeader &Header) {
	char   *buffer;
	ssize_t res;

	/* Allocate buffer */
	buffer = new char[HDR_MAXLEN];
	if (!buffer)
		return false;

	/* Read data */
	res = Read(buffer, HDR_MAXLEN);

	/* Set header string */
	if (res > 0)
		Header.Set(buffer);

	/* Delete buffer */
	delete buffer;

	return (res > 0) ? true : false;
}

bool CClient::HandleError(int reply) {
	const char *buffer;
	size_t      len;

	std::string data;

	/* Get the error page */
	data = CErrorPage::Get(reply);

	/* Set buffer */
	buffer = data.c_str();
	len    = data.length();

	/* Send reply */
	return Send(buffer, len, reply);
}
