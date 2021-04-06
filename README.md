# Web Server in C++

Simple C++ Web Server that at this point only handles one request/response at a time. It was intended to demo the SCGI functionality. 

Hopefully this will be changed to accept multiple connections simultaneously.

### Prerequisite

Make sure that the following code dependencies compile successfully on your build environment:

```
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

int main()
{
	return 0;
}

```

### Build and run

VS2019 configuration: Debug | x64

Build->Build Solution and press F5.


### More about TCP server and clients using WinSock
https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application