#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
		return 0;
	}

	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ���� ��� ������ ������ �� �����ϴ�.");
		return 0;
	}

	//�� ���ε� ���� IP�ּҿ� ��Ʈ�� �����ϵ��� ���� �ɼ��� �����Ѵ�.
	BOOL bOption = TRUE;
	if (::setsockopt(hSocket, SOL_SOCKET
		, SO_REUSEADDR // you can open the same server again and again
		, (char*)&bOption, sizeof(BOOL)) == SOCKET_ERROR)
	{
		puts("ERROR: ���� �ɼ��� ������ �� �����ϴ�.");
		return 0;
	}

	//���ε�
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	// svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//svraddr.sin_addr.S_un.S_addr = inet_addr("198.61.12.3"); // it is not the same address IPv4 and loopback
	svraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (::bind(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: ���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.");
		return 0;
	}

	if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		puts("ERROR: ���� ���·� ��ȯ�� �� �����ϴ�.");
		return 0;
	}

	puts("Echo ������ �����մϴ�.");

	SOCKADDR_IN clientaddr = { 0 };
	int nAddrLen = sizeof(clientaddr);
	SOCKET hClient = 0;
	char szBuffer[128] = { 0 };
	int nReceive = 0;

	while ((hClient = ::accept(hSocket,
		(SOCKADDR*)&clientaddr, &nAddrLen)) != INVALID_SOCKET)
	{
		puts("�� Ŭ���̾�Ʈ�� ����Ǿ����ϴ�.");
		while ((nReceive = ::recv(hClient,
			szBuffer, sizeof(szBuffer), 0)) > 0)
		{
			::send(hClient, szBuffer, sizeof(szBuffer), 0);
			puts(szBuffer);
			memset(szBuffer, 0, sizeof(szBuffer));
		}

		::closesocket(hClient);
		puts("Ŭ���̾�Ʈ ������ ������ϴ�.");
	}

	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}


