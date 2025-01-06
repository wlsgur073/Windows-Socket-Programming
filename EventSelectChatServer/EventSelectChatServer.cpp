#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include <windows.h>
#include <list>
#include <iterator>

/////////////////////////////////////////////////////////////////////////
SOCKET		g_hSocket;
//�迭��ҿ� ���� �ε���. �� ���� WSA_MAXIMUM_WAIT_EVENTS���� �۴�.
int			g_nListIndex;
//���Ͽ� ���� �̺�Ʈ �ڵ��� �迭
WSAEVENT	g_aListEvent[WSA_MAXIMUM_WAIT_EVENTS]; // WSA_MAXIMUM_WAIT_EVENTS = 64
//������ �迭. �̺�Ʈ �ڵ� �迭�� ���� �̷��.
SOCKET		g_aListSocket[WSA_MAXIMUM_WAIT_EVENTS];

/////////////////////////////////////////////////////////////////////////
void CloseAll(void)
{
	//���� ���� �� ����� Ŭ���̾�Ʈ ������ ������.
	for (int i = 0; i < g_nListIndex; ++i)
	{
		::shutdown(g_aListSocket[i], SD_BOTH);
		::closesocket(g_aListSocket[i]);
		::WSACloseEvent(g_aListEvent[i]);
	}
}

/////////////////////////////////////////////////////////////////////////
BOOL CtrlHandler(DWORD dwType)
{
	if (dwType == CTRL_C_EVENT)
	{
		CloseAll();
		puts("��� Ŭ���̾�Ʈ ������ �����߽��ϴ�.");

		::WSACleanup();
		exit(0);
		return TRUE;
	}

	return FALSE;
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

	//Ctrl+C Ű�� ������ �� �̸� �����ϰ� ó���� �Լ��� ����Ѵ�.
	if (::SetConsoleCtrlHandler(
		(PHANDLER_ROUTINE)CtrlHandler, TRUE) == FALSE)
		puts("ERROR: Ctrl+C ó���⸦ ����� �� �����ϴ�.");

	//1. ���Ӵ�� ���� ����
	g_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (g_hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ���� ��� ������ ������ �� �����ϴ�.");
		return 0;
	}

	//2. ��Ʈ ���ε�
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (::bind(g_hSocket,
			(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: ���Ͽ� �ּҸ� ���ε� �� �� �����ϴ�.");
		return 0;
	}

	//3. ���Ӵ�� g_hSocket ��ȯ
	if (::listen(g_hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		puts("ERROR: ���� ���·� ��ȯ�� �� �����ϴ�.");
		return 0;
	}

	//4. ���� �̺�Ʈ ������ ���� ���� �̺�Ʈ ���.
	g_nListIndex = 0;
	g_aListSocket[g_nListIndex] = g_hSocket;
	g_aListEvent[g_nListIndex] = ::WSACreateEvent();
	//���� �����̹Ƿ� FD_ACCEPT �̺�Ʈ�� �����Ѵ�.
	if (::WSAEventSelect(g_hSocket, g_aListEvent[g_nListIndex],
			FD_ACCEPT) == SOCKET_ERROR)
	{
		puts("ERROR: WSAEventSelect()");
		return 0;
	}
	puts("*** ������ �����մϴ�. ***");

	//5. ������ ���鼭 �̺�Ʈ�� ó���Ѵ�.
	DWORD dwIndex;
	WSANETWORKEVENTS netEvent;
	while (TRUE)
	{
		/////////////////////////////////////////////////////////////////
		//���� �̺�Ʈ�� ��ٸ���.
		dwIndex = ::WSAWaitForMultipleEvents(
						g_nListIndex + 1,	//������ �̺�Ʈ ����
						g_aListEvent,		//�̺�Ʈ �迭
						FALSE,	//��ü�� ���� ��������� �ʴ´�.
						100,	//100ms ���� ����Ѵ�.
						FALSE);	//ȣ���� ������ ���¸� �������� �ʴ´�.
		if (dwIndex == WSA_WAIT_FAILED)
			continue;

		//�̺�Ʈ�� �߻��� ������ �ε��� �� �̺�Ʈ �߻� ������ Ȯ���Ѵ�.
		if (::WSAEnumNetworkEvents(g_aListSocket[dwIndex],
				g_aListEvent[dwIndex], &netEvent) == SOCKET_ERROR)
			continue;

		/////////////////////////////////////////////////////////////////
		//5-1. Ŭ���̾�Ʈ�� ������.
		if (netEvent.lNetworkEvents & FD_ACCEPT)
		{
			if (netEvent.iErrorCode[FD_ACCEPT_BIT] != 0)
				continue;

			if (g_nListIndex >= WSA_MAXIMUM_WAIT_EVENTS)
			{
				puts("ERROR: �ִ� ���ӻ���!");
				continue;
			}

			SOCKADDR_IN clientaddr = { 0 };
			int nAddrLen = sizeof(clientaddr);
			SOCKET hClient = ::accept(g_hSocket,
								(SOCKADDR*)&clientaddr, &nAddrLen);
			if (hClient != INVALID_SOCKET)
			{
				//�� Ŭ���̾�Ʈ ��� ���ϰ� �̺�Ʈ �ڵ��� �迭�� ����Ѵ�.
				++g_nListIndex;
				g_aListSocket[g_nListIndex] = hClient;
				g_aListEvent[g_nListIndex] = ::WSACreateEvent();
				puts("�� Ŭ���̾�Ʈ�� ����ƽ��ϴ�.");
			}

			//Ŭ���̾�Ʈ ���Ͽ� ���� �б�, �ݱ� �̺�Ʈ�� �����Ѵ�.
			::WSAEventSelect(hClient, g_aListEvent[g_nListIndex],
				FD_READ | FD_CLOSE);
		}

		/////////////////////////////////////////////////////////////////
		//5-2. Ŭ���̾�Ʈ�� ������ ����.
		else if (netEvent.lNetworkEvents & FD_CLOSE)
		{
			//Ŭ���̾�Ʈ ���� �� �̺�Ʈ �ڵ��� �ݴ´�.
			::WSACloseEvent(g_aListEvent[dwIndex]);
			::shutdown(g_aListSocket[dwIndex], SD_BOTH);
			::closesocket(g_aListSocket[dwIndex]);

			//���� ���� �� �ڵ������� �����ϰ� �迭�� �����Ѵ�.
			for (DWORD i = dwIndex; i < g_nListIndex; ++i)
			{
				g_aListEvent[i] = g_aListEvent[i + 1];
				g_aListSocket[i] = g_aListSocket[i + 1];
			}

			g_nListIndex--;
			printf("Ŭ���̾�Ʈ ���� ����. ���� ��: %d.\n", g_nListIndex);
		}

		/////////////////////////////////////////////////////////////////
		//5-3. Ŭ���̾�Ʈ�� �����͸� ������.
		else if (netEvent.lNetworkEvents & FD_READ)
		{
			TCHAR szBuffer[1024] = { 0 };
			int nRecv = ::recv(g_aListSocket[dwIndex],
				(char*)szBuffer, sizeof(szBuffer), 0);

			//����� ��� Ŭ���̾�Ʈ�� ���� �޽����� �����Ѵ�.
			for (DWORD i = 1; i < g_nListIndex + 1; ++i)
				::send(g_aListSocket[i], (char*)szBuffer, nRecv, 0);
		}
		/////////////////////////////////////////////////////////////////
	}

	return 0;
}