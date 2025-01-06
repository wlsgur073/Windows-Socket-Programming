#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

//������ ���� �޽����� �����ϰ� ȭ�鿡 ����ϴ� ������ �Լ�.
DWORD WINAPI ThreadReceive(LPVOID pParam)
{
	SOCKET hSocket = (SOCKET)pParam;
	char szBuffer[128] = { 0 };
	while (::recv(hSocket, szBuffer, sizeof(szBuffer), 0) > 0)
	{
		printf("-> %s\n", szBuffer);
		memset(szBuffer, 0, sizeof(szBuffer));
	}

	puts("���� �����尡 �������ϴ�.");
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//���� �ʱ�ȭ
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
		return 0;
	}

	//������ ������ ���� ����
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ������ ������ �� �����ϴ�.");
		return 0;
	}

	//��Ʈ ���ε� �� ����
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

	//ä�� �޽��� ���� ������ ����
	DWORD dwThreadID = 0;
	HANDLE hThread = ::CreateThread(NULL,	//���ȼӼ� ���
		0,					//���� �޸𸮴� �⺻ũ��(1MB)
		ThreadReceive,		//������� ������ �Լ��̸�
		(LPVOID)hSocket,	//���� �ڵ��� �Ű������� �ѱ�
		0,					//���� �÷��״� �⺻�� ���
		&dwThreadID);		//������ ������ID�� ����� �����ּ�
	::CloseHandle(hThread);

	//ä�� �޽��� �۽�
	char szBuffer[128];
	puts("ä���� �����մϴ�. �޽����� �Է��ϼ���.");
	while (1)
	{
		//����ڷκ��� ���ڿ��� �Է� �޴´�.
		memset(szBuffer, 0, sizeof(szBuffer));
		gets_s(szBuffer);
		if (strcmp(szBuffer, "EXIT") == 0)		break;

		//����ڰ� �Է��� ���ڿ��� ������ �����Ѵ�.
		::send(hSocket, szBuffer, strlen(szBuffer) + 1, 0);

	}

	//������ �ݰ� ����.
	::closesocket(hSocket);
	//�����尡 ����� �� �ֵ��� ��� ��ٷ��ش�.
	::Sleep(100);
	//���� ����
	::WSACleanup();
	return 0;
}
