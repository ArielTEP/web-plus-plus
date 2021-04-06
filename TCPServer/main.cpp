// This implementation is based on the official Microsoft Docs.
// For more info:
// https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application
// 

#include "TCPServer.h"


int main() {

	{
		std::unique_ptr<TCPServer> upTecpServer = std::make_unique<TCPServer>();
		upTecpServer->start();
	}

	return 0;
}