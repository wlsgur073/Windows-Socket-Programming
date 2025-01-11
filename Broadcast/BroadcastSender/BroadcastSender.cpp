#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")


void ErrorHandler(const char *pszMessage)
{
	printf("ERROR: %s\n", pszMessage);
	::WSACleanup();
	exit(1);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//���� �ʱ�ȭ
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		ErrorHandler("������ �ʱ�ȭ �� �� �����ϴ�.");

	//���� ����
	SOCKET hSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (hSocket == INVALID_SOCKET)
		ErrorHandler("UDP ������ ������ �� �����ϴ�.");

	//��Ʈ ���ε�
	SOCKADDR_IN	addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(25000);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);

	//����ּҷ� �����ϱ� ���� ���� �ɼ��� �����Ѵ�.
	int nOption = 1;
	::setsockopt(hSocket,
		SOL_SOCKET,
		SO_BROADCAST,			//����ּҷ� �����Ѵ�.
		(const char*)&nOption,
		sizeof(nOption));

	char szBuffer[128] = { 0 };
	while (1)
	{
		//����ڷκ��� ���ڿ��� �Է� �޴´�.
		putchar('>');
		gets_s(szBuffer);
		if (strcmp(szBuffer, "EXIT") == 0)		break;

		//����ּҷ� �޽����� �����Ѵ�.
		//���� ��� Peer���� ���ÿ� �޽����� �����Ѵ�.
		::sendto(hSocket, szBuffer, sizeof(szBuffer), 0,
			(const SOCKADDR*)&addr, sizeof(addr));
	}

	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}