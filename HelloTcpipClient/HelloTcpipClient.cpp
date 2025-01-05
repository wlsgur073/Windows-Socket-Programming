#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

// Keep in mind. to terminating a TCP connection lies with the client.
// The side that initiates the disconnection will enter the TIME_WAIT state.
// Therefore, the server should not be in the TIME_WAIT state.
int _tmain(int argc, _TCHAR* argv[])
{
	//������ �ʱ�ȭ
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
		return 0;
	}

	//1. ���Ӵ�� ���� ����
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0); // client also has a socket
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ������ ������ �� �����ϴ�.");
		return 0;
	}

	//2. ��Ʈ ���ε� �� ����
	// where set the client socket's address ? -> OS will assign the client address
	SOCKADDR_IN	svraddr = { 0 }; // server socket address
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (::connect(hSocket, // client socket must to know server socket ip and port
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: ������ ������ �� �����ϴ�.");
		return 0;
	}

	//3. ä�� �޽��� ��/����
	char szBuffer[128] = { 0 };
	while (1)
	{
		//����ڷκ��� ���ڿ��� �Է� �޴´�.
		gets_s(szBuffer);
		if (strcmp(szBuffer, "EXIT") == 0) break;

		//����ڰ� �Է��� ���ڿ��� ������ �����Ѵ�.
		::send(hSocket, szBuffer
			, strlen(szBuffer) + 1 // why +1 ? -> include `null` character
			, 0);

		// Test : send and recv mappped one by one? -> Nope.
		//int nLen = strlen(szBuffer);
		//for (int i = 0; i < nLen; i++) // send one character at a time
		//	::send(hSocket, szBuffer + i, 1, 0);


		//�����κ��� ��� ���� ���ڿ��� ���� ���� �޽����� �����Ѵ�.
		memset(szBuffer, 0, sizeof(szBuffer));
		::recv(hSocket, szBuffer, sizeof(szBuffer), 0);
		printf("From server: %s\n", szBuffer);
	}

	//4. ������ �ݰ� ����.
	//::shutdown(hSocket, SD_BOTH); // means stop I/O
	::closesocket(hSocket);
	//������ ����
	::WSACleanup();
	return 0;
}
