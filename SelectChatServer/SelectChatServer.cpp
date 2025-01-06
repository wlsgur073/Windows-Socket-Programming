#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include <windows.h>
#include <list>
#include <iterator>

/////////////////////////////////////////////////////////////////////////
SOCKET				g_hSocket;		//������ ���� ����.
std::list<SOCKET>	g_listClient;	//����� Ŭ���̾�Ʈ ���� ����Ʈ.

/////////////////////////////////////////////////////////////////////////
void SendMessageAll(char *pszMessage, int nSize)
{
	std::list<SOCKET>::iterator it;
	for (it = g_listClient.begin(), ++it; it != g_listClient.end(); ++it)
		send(*it, pszMessage, nSize, 0);

}

/////////////////////////////////////////////////////////////////////////
void CloseAll(void)
{
	std::list<SOCKET>::iterator it;
	for (it = g_listClient.begin(), ++it; it != g_listClient.end(); ++it)
	{
		::shutdown(*it, SD_BOTH);
		::closesocket(*it);
	}
}

/////////////////////////////////////////////////////////////////////////
BOOL CtrlHandler(DWORD dwType)
{
	if (dwType == CTRL_C_EVENT)
	{
		::closesocket(g_hSocket);
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
	if (::bind(g_hSocket, (SOCKADDR*)&svraddr, sizeof(svraddr)) ==
		SOCKET_ERROR)
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

	//4. ����� Ŭ���̾�Ʈ ������ ���� ����Ʈ �����̳� ����
	g_listClient.push_back(g_hSocket);

	//5. ������ ��ȭ�� ���ø� ���� �ݺ���
	UINT nCount;
	FD_SET fdRead; // file descriptor set, array of SOCKET
	std::list<SOCKET>::iterator it;

	puts("I/O ��Ƽ�÷��� ä�� ������ �����մϴ�.");
	do
	{
		//5-1. Ŭ���̾�Ʈ ���� �� �������� ��ȭ ���ü� �ʱ�ȭ
		FD_ZERO(&fdRead); // init fdRead
		for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
			FD_SET(*it, &fdRead);

		//5-2. ��ȭ�� �߻��� ������ ��� // ��ȭ�� ������ִ� ��ü�� OS
		::select(0, &fdRead, NULL, NULL, NULL); // it will be blocked until any change is detected.

		//5-3. ��ȭ�� ������ ���� �� Ȯ��
		nCount = fdRead.fd_count;
		for (int nIndex = 0; nIndex < nCount; ++nIndex)
		{
			//���Ͽ� ��ȭ ���� �÷��װ� ��Ʈ �Ǿ��°�?
			if (!FD_ISSET(fdRead.fd_array[nIndex], &fdRead))
				continue;

			//5-3-1. ������ listen ������ ��Ʈ�Ǿ��°�?
			//��, ������ ������ �õ��ߴ°�?
			if (fdRead.fd_array[nIndex] == g_hSocket)
			{
				//�� Ŭ���̾�Ʈ�� ������ �޴´�.
				SOCKADDR_IN clientaddr = { 0 };
				int nAddrLen = sizeof(clientaddr);
				SOCKET hClient = ::accept(g_hSocket,
									(SOCKADDR*)&clientaddr, &nAddrLen);
				if (hClient != INVALID_SOCKET)
				{
					FD_SET(hClient, &fdRead);
					g_listClient.push_back(hClient);
				}
			}

			//5-3-2 Ŭ���̾�Ʈ�� ������ �����Ͱ� �ִ� ���.
			else
			{
				char szBuffer[1024] = { 0 };
				int nReceive = ::recv(fdRead.fd_array[nIndex],
								(char*)szBuffer, sizeof(szBuffer), 0);
				if (nReceive <= 0)
				{
					//���� ����.
					::closesocket(fdRead.fd_array[nIndex]);
					FD_CLR(fdRead.fd_array[nIndex], &fdRead);
					g_listClient.remove(fdRead.fd_array[nIndex]);
					puts("Ŭ���̾�Ʈ�� ������ �������ϴ�.");
				}
				else
				{
					//ä�� �޽��� ����
					SendMessageAll(szBuffer, nReceive);
				}
			}
		} //for()
	} while (g_hSocket != NULL);

	//6. ��� ������ �ݰ� �����Ѵ�.
	CloseAll();

	puts("������ �����մϴ�.");
	return 0;
}
