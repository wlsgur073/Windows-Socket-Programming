#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")


char g_szRemoteAddress[32];
int g_nRemotePort;
int g_nLocalPort;

void ErrorHandler(const char *pszMessage)
{
	printf("ERROR: %s\n", pszMessage);
	::WSACleanup();
	exit(1);
}

//�������� �޽����� �����ϴ� ������ �Լ�.
DWORD WINAPI ThreadSendto(LPVOID pParam)
{
	//�۽��� ���� UDP ������ �ϳ� �� �����Ѵ�.
	SOCKET hSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (hSocket == INVALID_SOCKET)
		ErrorHandler("UDP ������ ������ �� �����ϴ�.");

	char szBuffer[128];
	SOCKADDR_IN	remoteaddr = { 0 };
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_port = htons(g_nRemotePort);
	remoteaddr.sin_addr.S_un.S_addr = inet_addr(g_szRemoteAddress);
	while (1)
	{
		gets_s(szBuffer);
		if (strcmp(szBuffer, "EXIT") == 0)
			break;

		//����ڰ� �Է��� �޽����� �������� �����Ѵ�.
		::sendto(
			hSocket,
			szBuffer,
			strlen(szBuffer) + 1,
			0,
			(sockaddr*)&remoteaddr, // The client's address must be specified each time.
			sizeof(remoteaddr)
		);
	}

	//������ ���� UDP ������ �ݴ´�. _tmain() �Լ��� while���� ������.
	::closesocket((SOCKET)pParam);
	::closesocket(hSocket);
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//���� �ʱ�ȭ
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		ErrorHandler("������ �ʱ�ȭ �� �� �����ϴ�.");

	//���� ����. SOCK_DGRAM Ÿ���� ����Ѵ�!
	SOCKET hSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (hSocket == INVALID_SOCKET)
		ErrorHandler("UDP ������ ������ �� �����ϴ�.");

	//�ּҿ� ��Ʈ��ȣ�� �Է� �޴´�.
	printf("������ IP�ּҸ� �Է��ϼ���.: ");
	gets_s(g_szRemoteAddress);
	fflush(stdin);
	printf("������ ��Ʈ��ȣ�� �Է��ϼ���.: ");
	scanf_s("%d", &g_nRemotePort);
	fflush(stdin);
	printf("���� ��Ʈ��ȣ�� �Է��ϼ���.: ");
	scanf_s("%d", &g_nLocalPort);

	//��Ʈ ���ε�
	SOCKADDR_IN	addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(g_nLocalPort);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (::bind(hSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
		ErrorHandler("���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.");

	//�޽��� �۽� ������ ����.
	DWORD dwThreadID = 0;
	HANDLE hThread = ::CreateThread(NULL,
		0,					//���� �޸𸮴� �⺻ũ��(1MB)
		ThreadSendto,		//������� ������ �Լ��̸� // only send	
		(LPVOID)hSocket,
		0,					//���� �÷��״� �⺻�� ���
		&dwThreadID);		//������ ������ID�� ����� �����ּ�
	::CloseHandle(hThread);

	//�޽��� ���� �� ���.
	char szBuffer[128];
	SOCKADDR_IN	remoteaddr; // client address
	int nLenSock = sizeof(remoteaddr), nResult;
	while ((nResult = ::recvfrom(hSocket, szBuffer, sizeof(szBuffer), 0,
		(sockaddr*)&remoteaddr, &nLenSock)) > 0)
	{
		printf("-> %s\n", szBuffer);
		memset(szBuffer, 0, sizeof(szBuffer));
	}

	puts("UDP ��� ����.");
	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}
