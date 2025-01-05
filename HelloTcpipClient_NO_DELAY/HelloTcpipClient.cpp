#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")


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
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ������ ������ �� �����ϴ�.");
		return 0;
	}

	//2. ��Ʈ ���ε� �� ����
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (::connect(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: ������ ������ �� �����ϴ�.");
		return 0;
	}

	//2.1
	int nOpt = 1;
	::setsockopt(hSocket, IPPROTO_TCP
		, TCP_NODELAY // no buffering 
		, (char*)&nOpt, sizeof(nOpt));

	//3. ä�� �޽��� ��/����
	char szBuffer[128] = { 0 };
	while (1)
	{
		//����ڷκ��� ���ڿ��� �Է� �޴´�.
		gets_s(szBuffer);
		if (strcmp(szBuffer, "EXIT") == 0)		break;

		//����ڰ� �Է��� ���ڿ��� ������ �����Ѵ�.
		//::send(hSocket, szBuffer, strlen(szBuffer) + 1, 0);
		int nLength = strlen(szBuffer);
		for (int i = 0; i < nLength; ++i)
			::send(hSocket, szBuffer + i, 1, 0);

		//�����κ��� ��� ���� ���ڿ��� ���� ���� �޽����� �����Ѵ�.
		memset(szBuffer, 0, sizeof(szBuffer));
		::recv(hSocket, szBuffer, sizeof(szBuffer), 0);
		printf("From server: %s\n", szBuffer);
	}

	//4. ������ �ݰ� ����.
	::shutdown(hSocket, SD_BOTH);
	::closesocket(hSocket);
	//������ ����
	::WSACleanup();
	return 0;
}
