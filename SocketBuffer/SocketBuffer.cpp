#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

// https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-getsockopt
int _tmain(int argc, _TCHAR* argv[])
{
    // Initialize Winsock
    WSADATA wsa = { 0 };
    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        puts("ERROR: Unable to initialize Winsock.");
        return 0;
    }

    // Create socket
    SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
    {
        puts("ERROR: Unable to create socket.");
        return 0;
    }

    // Check and print the size of the socket's send buffer
    int nBufSize = 0, nLen = sizeof(nBufSize);
    if (::getsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR)
        printf("Send buffer size: %d\n", nBufSize);

    // Check and print the size of the socket's receive buffer
    nBufSize = 0;
    nLen = sizeof(nBufSize);
    if (::getsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR)
        printf("Receive buffer size: %d\n", nBufSize);

    // Close socket and cleanup
    ::closesocket(hSocket);
    ::WSACleanup();
    return 0;
}
