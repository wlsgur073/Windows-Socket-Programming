#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

// https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket
int _tmain(int argc, _TCHAR* argv[])
{
	// Initialize WinSock, WSA(Windows Sockets API)
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
        puts("ERROR: Unable to initialize WinSock.");
		return 0;
	}

	// 1. Create a socket for listening
	SOCKET hSocket = ::socket(
		AF_INET // L3: Protocol Family - IPv4
		, SOCK_STREAM // L4: Protocol - TCP
		, 0 // L4 Protocol: TCP
	);
	if (hSocket == INVALID_SOCKET)
	{
        puts("ERROR: Unable to create a listening socket.");
		return 0;
	}

	// 2. port binding
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
    // IP addresses and ports are all numbers. The way computers handle numbers is called Endian.
	// CPUs like Intel use Little Endian. Networks use Big Endian.
	// socket APIs use Big Endian. So, we need to convert Little Endian to Big Endian.
	svraddr.sin_port = htons(25000); // standard port for 32bit
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // if server has multiple IP addresses, client can connect to any of them.
	if (::bind(hSocket, (SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
        puts("ERROR: Unable to bind IP address and port to the socket.");
		return 0;
	}

    // 3. Switch to listening state
	if (::listen(hSocket
		, SOMAXCONN // maximum number of clients that can be queued in OS
	) == SOCKET_ERROR)
	{
        puts("ERROR: Unable to switch to listening state.");
		return 0;
	}

    // 4. Client connection handling and response
	SOCKADDR_IN clientaddr = { 0 }; // address of the client
	int nAddrLen = sizeof(clientaddr);
	SOCKET hClient = 0; // client socket
	char szBuffer[128] = { 0 };
	int nReceive = 0; // number of bytes received

    // 4.1 Accept client connection and create a new socket
	while ((hClient = ::accept(
		hSocket
		, (SOCKADDR*)&clientaddr
		, &nAddrLen)) != INVALID_SOCKET)
	{
        puts("A new client has connected."); fflush(stdout);
        // 4.2 Receive a string from the client.
		while ((nReceive = ::recv(hClient
			, szBuffer
			, sizeof(szBuffer) // cut off byte if it is longer than 128 bytes
			, 0)) > 0) // if recv returns 0, the client has disconnected
		{
            // 4.3 Echo the received string back to the client.
			::send(hClient
				, szBuffer
				, sizeof(szBuffer) // even if nReceive is smaller than sizeof(szBuffer), we send the whole buffer
				, 0);
			puts(szBuffer); fflush(stdout);
			memset(szBuffer, 0, sizeof(szBuffer));
		}

        // 4.3 Client has disconnected.
		//::shutdown(hSocket, SD_BOTH); // do not shutdown the listening socket
		::shutdown(hClient, SD_BOTH);
		::closesocket(hClient);
        puts("Client connection has been terminated."); fflush(stdout);
	}

	// 5. close the listening socket
	::closesocket(hSocket);

    // Cleanup WinSock
	::WSACleanup();
	return 0;
}

