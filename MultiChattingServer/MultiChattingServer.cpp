#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include <windows.h>
#include <list>
#include <iterator>

/////////////////////////////////////////////////////////////////////////
CRITICAL_SECTION	g_cs;			//������ ����ȭ ��ü.
SOCKET				g_hSocket;		//������ ���� ����.
std::list<SOCKET>	g_listClient;	//����� Ŭ���̾�Ʈ ���� ����Ʈ. //Linked List

/////////////////////////////////////////////////////////////////////////
//���� ����� Ŭ���̾�Ʈ�� ������ ����Ʈ�� �����Ѵ�.
BOOL AddUser(SOCKET hSocket)
{
	::EnterCriticalSection(&g_cs);		// ciritical section start
	//�� �� �ڵ�� ���� �� �����常 �����Ѵٴ� ���� ����ȴ�.
	g_listClient.push_back( hSocket );
	::LeaveCriticalSection(&g_cs);		// ciritical section end

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
//����� Ŭ���̾�Ʈ ��ο��� �޽����� �����Ѵ�.
void SendChattingMessage(char *pszParam)
{
	int nLength = strlen(pszParam);
	std::list<SOCKET>::iterator it;

	::EnterCriticalSection(&g_cs);		// ciritical section start
	//����� ��� Ŭ���̾�Ʈ�鿡�� ���� �޽����� �����Ѵ�.
	for (it = g_listClient.begin(); it != g_listClient.end(); ++it) // send to all clients
		::send(*it, pszParam, sizeof(char) * (nLength + 1), 0); // O(1) latency
	::LeaveCriticalSection(&g_cs);		// ciritical section end
}

/////////////////////////////////////////////////////////////////////////
//Ctrl+C �̺�Ʈ�� �����ϰ� ���α׷��� �����Ѵ�.
BOOL CtrlHandler(DWORD dwType)
{
	if (dwType == CTRL_C_EVENT)
	{
		std::list<SOCKET>::iterator it;

		//����� ��� Ŭ���̾�Ʈ �� ���� ������ �ݰ� ���α׷��� �����Ѵ�.
		::shutdown(g_hSocket, SD_BOTH);

		::EnterCriticalSection(&g_cs);		//�Ӱ迵�� ����
		for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
			::closesocket(*it);
		//���� ����Ʈ�� ��ϵ� ��� ������ �����Ѵ�.
		g_listClient.clear();
		::LeaveCriticalSection(&g_cs);		//�Ӱ迵�� ��

		puts("��� Ŭ���̾�Ʈ ������ �����߽��ϴ�.");
		//Ŭ���̾�Ʈ�� ����ϴ� ��������� ����Ǳ⸦ ��ٸ���.
		::Sleep(100);
		::DeleteCriticalSection(&g_cs);
		::closesocket(g_hSocket);

		//���� ����
		::WSACleanup();
		exit(0);
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////
//Ŭ���̾�Ʈ���� ä�� �޽��� ���񽺸� �����ϴ� ������ �Լ�.
//����� ������ Ŭ���̾�Ʈ���� �� �����尡 �����ȴ�.
DWORD WINAPI ThreadFunction(LPVOID pParam)
{
	char szBuffer[128] = { 0 };
	int nReceive = 0;
	SOCKET hClient = (SOCKET)pParam;

	puts("�� Ŭ���̾�Ʈ�� ����Ǿ����ϴ�.");
	while ((nReceive = ::recv(hClient,
							szBuffer, sizeof(szBuffer), 0)) > 0)
	{
		puts(szBuffer);
		//������ ���ڿ��� ����� ��ü Ŭ���̾�Ʈ�鿡�� ����
		SendChattingMessage(szBuffer);
		memset(szBuffer, 0, sizeof(szBuffer));
	}

	puts("Ŭ���̾�Ʈ�� ������ �������ϴ�."); // if client disconnects, to prevent concurrency problem
	::EnterCriticalSection(&g_cs);		//�Ӱ迵�� ����
	g_listClient.remove(hClient);
	::LeaveCriticalSection(&g_cs);		//�Ӱ迵�� ��

	::closesocket(hClient);
	return 0;
}

/////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	//���� �ʱ�ȭ
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
		return 0;
	}

	//�Ӱ迵�� ��ü�� �����Ѵ�.
	::InitializeCriticalSection(&g_cs);

	//Ctrl+C Ű�� ������ �� �̸� �����ϰ� ó���� �Լ��� ����Ѵ�.
	if ( ::SetConsoleCtrlHandler(
				(PHANDLER_ROUTINE)CtrlHandler, TRUE) == FALSE )
		puts("ERROR: Ctrl+C ó���⸦ ����� �� �����ϴ�.");

	//���Ӵ�� ���� ����
	g_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (g_hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ���� ��� ������ ������ �� �����ϴ�.");
		return 0;
	}

	//��Ʈ ���ε�
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (::bind(g_hSocket,
			(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: ���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.");
		return 0;
	}

	//���Ӵ�� ���·� ��ȯ
	if (::listen(g_hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		puts("ERROR: ���� ���·� ��ȯ�� �� �����ϴ�.");
		return 0;
	}
	puts("*** ä�ü����� �����մϴ�. ***");

	//Ŭ���̾�Ʈ ���� ó�� �� ����
	SOCKADDR_IN clientaddr = { 0 };
	int nAddrLen = sizeof(clientaddr);
	SOCKET hClient = 0;
	DWORD dwThreadID = 0;
	HANDLE hThread;

	//Ŭ���̾�Ʈ ������ �޾Ƶ��̰� ���ο� ���� ����(����)
	while ((hClient = ::accept(g_hSocket,
		(SOCKADDR*)&clientaddr, &nAddrLen)) != INVALID_SOCKET)
	{
		if (AddUser(hClient) == FALSE)
		{
			puts("ERROR: �� �̻� Ŭ���̾�Ʈ ������ ó���� �� �����ϴ�.");
			CtrlHandler(CTRL_C_EVENT); // event handling for Ctrl+C
			break;
		}

		//Ŭ���̾�Ʈ�κ��� ���ڿ��� ������.
		hThread = ::CreateThread(NULL,	//���ȼӼ� ���
			0,				//���� �޸𸮴� �⺻ũ��(1MB)
			ThreadFunction,	//������� ������ �Լ��̸�
			(LPVOID)hClient,	//���� ������ Ŭ���̾�Ʈ ����
			0,				//���� �÷��״� �⺻�� ���
			&dwThreadID);	//������ ������ID�� ����� �����ּ�

		::CloseHandle(hThread);
	}

	puts("*** ä�ü����� �����մϴ�. ***");
	return 0;
}
