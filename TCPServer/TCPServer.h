#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <assert.h>
#include <sstream>

#include "Utils.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27018"
#define DEFAULT_BUFLEN 2048
#define ENDL "\r\n"


// Discomment this line to see debug outputs
#define DEBUG

class TCPServer
{
public:

	TCPServer();
	~TCPServer();

	void start();

	int getLastWinSockError() 
		const { return WSAGetLastError(); }

private:

	int m_winSocketErr;
	SOCKET m_listenSocket;
	SOCKET m_clientSocket;
	const char * m_port;

	// addrinfo used by getaddrinfo
	struct addrinfo* result;
	struct addrinfo* ptr;
	struct addrinfo hints;

	// Winsock2 initialization required
	// by the library.
	void initializeWinsock();

	// Listen socket for the server to listen 
	// incoming connections from a tcp client.
	void createListenSocket();

	// Bind the server with a network address.
	void bindListenSocket();

	// The server must then listen on that IP address 
	// and port for incoming connection requests.
	void listenOnSocket();

	// Blocking method to wait for incoming connections.
	// In other words, accept a connection on a socket.
	// If a connection request occurs, the application
	// calls accept().
	// Then send and recv.
	void onConnection();

	// When the server is done sending data to the client, 
	// the shutdown function can be called specifying SD_SEND 
	// to shutdown the sending side of the socket. This allows 
	// the client to release some of the resources for this socket. 
	// The server application can still receive data on the socket.
	void shutdownConnection();

};