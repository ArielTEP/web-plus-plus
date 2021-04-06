#include "TCPServer.h"

TCPServer::TCPServer() : 
	m_winSocketErr(false), 
	m_listenSocket(INVALID_SOCKET),
	m_clientSocket(INVALID_SOCKET),
	result(nullptr),
	ptr(nullptr),
	m_port(DEFAULT_PORT)
{
	initializeWinsock();
}

TCPServer::~TCPServer()
{
	// cleanup
	WSACleanup();
}

void TCPServer::start()
{
	createListenSocket();
	bindListenSocket();

	// mark the socket for listening
	// ready for only ONE connection
	// TODO: Figure it out for n connections.
	listenOnSocket();

	// Wait, accept and process connection 
	// (blocking here until done...)
	onConnection();

	// Once done, shutdown the connection.
	shutdownConnection();
}

void TCPServer::initializeWinsock()
{
	// Windows socket implementation info.
	WSADATA wsaData;

	// Initialize Winsock
	m_winSocketErr = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if ( m_winSocketErr ) {
		std::cerr << "WSAStartup failed: " << m_winSocketErr << std::endl;
		return;
	}
}

void TCPServer::createListenSocket()
{
	// Do not create a tcp soclet unless
	// winsock is already initialized successfully
	assert( !m_winSocketErr );

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	int ret = getaddrinfo(NULL, m_port, &hints, &result);
	if (0 != ret) {
		std::cerr << "getaddrinfo failed: " << ret << std::endl;
		return;
	}

	// Create a SOCKET for the server to listen for client connections
	m_listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (m_listenSocket == INVALID_SOCKET) {
		std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		return;
	}
}

void TCPServer::bindListenSocket()
{
	assert( m_listenSocket != INVALID_SOCKET );

	int ret = bind(m_listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (ret == SOCKET_ERROR) {
		std::cerr << "Binding failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(m_listenSocket);
		return;
	}
	// Once the bind function is called, the address information 
	// returned by the getaddrinfo function is no longer needed.
	freeaddrinfo(result);
}

void TCPServer::listenOnSocket()
{
	assert(m_listenSocket != INVALID_SOCKET);

	// Passing as parameters the created socket and a value for 
	// the backlog, maximum length of the queue of pending 
	// connections to accept. 
	if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(m_listenSocket);
		return;
	}
}

void TCPServer::onConnection()
{
	assert(m_listenSocket != INVALID_SOCKET);

	// Accept a client socket
	m_clientSocket = accept(m_listenSocket, NULL, NULL);

	if (m_clientSocket == INVALID_SOCKET) {
		std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
		closesocket(m_clientSocket);
		return;
	}

	// We are done listening
	closesocket(m_listenSocket);
	std::cout << "*New client accepted!\n\n";

	// TODO: If the client socket is valid, then it should be passed
	// to a worker thread or an I/O completion port and continue
	// accepting additional connections.

	char recvbuf[DEFAULT_BUFLEN + 1];
	int bytesRcv, bytesSnt;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {
		ZeroMemory(recvbuf, recvbuflen);
		bytesRcv = recv(m_clientSocket, recvbuf, recvbuflen, 0);
		if (bytesRcv > 0) {
#ifdef DEBUG
			std::cout << "Bytes received:\n";
			LOG_TRACE(recvbuf, bytesRcv);
#endif
			break;
			// unless we get a "\r\n"
			//std::string req(recvbuf);
			//size_t reqLen= req.length();
			//if (req.compare(reqLen - 2, reqLen, ENDL) == 0) break;

			// TCP echo response
			//bytesSnt = send(m_clientSocket, recvbuf, bytesRcv, 0);

			//if (bytesSnt == SOCKET_ERROR) {
			//	std::cout << "send failed: " << WSAGetLastError() << std::endl;
			//	closesocket(m_clientSocket);
			//	return;
			//}
#ifdef DEBUG
			// std::cout << "Bytes sent: ";
			// LOG_TRACE(recvbuf, bytesRcv);
#endif
		}
		else if (bytesRcv == 0)
			std::cout << "Done receiving...\n" << std::endl;
		else {
			std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
			closesocket(m_clientSocket);
			return;
		}

	} while (bytesRcv > 0);

	// Done receiving
	// now send a response

	// Preparing HTML content
	std::string httpHeader = "HTTP/1.1 200 OK";
	std::string httpBody = "<html><body><h1>Hello from the application server!</h1><p>Written in C++...</p></body></html>";
	std::string httpResponseMsg = httpHeader + ENDL + ENDL + httpBody;

	std::string scgi_response = "Status: 200 OK";
	scgi_response += ENDL;
	scgi_response += "Content-Type: text/html";
	scgi_response += ENDL;
	scgi_response += ENDL;
	scgi_response += httpBody;

	bytesSnt = send(m_clientSocket, scgi_response.c_str(), scgi_response.length(), 0);
	if (bytesSnt == SOCKET_ERROR) {
		std::cout << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(m_clientSocket);
		return;
	}
	// sent, now continue and close the socket
}

void TCPServer::shutdownConnection()
{
	assert(m_clientSocket != INVALID_SOCKET);
	// shutdown the send half of the connection since no more data will be sent
	int ret = shutdown(m_clientSocket, SD_SEND);
	if (ret == SOCKET_ERROR) {
		std::cerr << "shutdown failed: " << WSAGetLastError() << std::endl;
		closesocket(m_clientSocket);
		return;
	}

	closesocket(m_clientSocket);
}


