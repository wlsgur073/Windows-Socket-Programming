#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include <windows.h>
#include <list>
#include <iterator>

/////////////////////////////////////////////////////////////////////////
typedef struct _USERSESSION
{
	SOCKET	hSocket;
	char	buffer[8192];	//8KB
} USERSESSION;

/////////////////////////////////////////////////////////////////////////
//Ŭ���̾�Ʈ ó���� ���� �۾��� ������ ����. // thread pool
#define MAX_THREAD_CNT		4

CRITICAL_SECTION  g_cs;				//������ ����ȭ ��ü
std::list<SOCKET>	g_listClient;	//����� Ŭ���̾�Ʈ ���� ����Ʈ.
SOCKET	g_hSocket;					//������ ���� ����
HANDLE	g_hIocp;					//IOCP �ڵ�


/////////////////////////////////////////////////////////////////////////
//����� Ŭ���̾�Ʈ ��ο��� �޽����� �����Ѵ�.
void SendMessageAll(char *pszMessage, int nSize)
{
	std::list<SOCKET>::iterator it;

	::EnterCriticalSection(&g_cs);
	for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
		::send(*it, pszMessage, nSize, 0);
	::LeaveCriticalSection(&g_cs);
}

/////////////////////////////////////////////////////////////////////////
//����� ��� Ŭ���̾�Ʈ �� ���� ������ �ݴ´�.
void CloseAll()
{
	std::list<SOCKET>::iterator it;

	::EnterCriticalSection(&g_cs);
	for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
	{
		::shutdown(*it, SD_BOTH);
		::closesocket(*it);
	}
	::LeaveCriticalSection(&g_cs);
}

/////////////////////////////////////////////////////////////////////////
void CloseClient(SOCKET hSock)
{
	::shutdown(hSock, SD_BOTH);
	::closesocket(hSock);

	::EnterCriticalSection(&g_cs);
	g_listClient.remove(hSock);
	::LeaveCriticalSection(&g_cs);
}

/////////////////////////////////////////////////////////////////////////
void ReleaseServer(void)
{
	//Ŭ���̾�Ʈ ������ ��� �����Ѵ�.
	CloseAll();
	::Sleep(500);

	//Listen ������ �ݴ´�.
	::shutdown(g_hSocket, SD_BOTH);
	::closesocket(g_hSocket);
	g_hSocket = NULL;

	//IOCP �ڵ��� �ݴ´�. �̷��� �ϸ� GQCS() �Լ��� FALSE�� ��ȯ�ϸ�
	//:GetLastError() �Լ��� ERROR_ABANDONED_WAIT_0�� ��ȯ�Ѵ�.
	//IOCP ��������� ��� ����ȴ�.
	::CloseHandle(g_hIocp);
	g_hIocp = NULL;

	//IOCP ��������� ����Ǳ⸦ �����ð� ���� ��ٸ���.
	::Sleep(500);
	::DeleteCriticalSection(&g_cs);
}

/////////////////////////////////////////////////////////////////////////
//Ctrl+C �̺�Ʈ�� �����ϰ� ���α׷��� �����Ѵ�.
BOOL CtrlHandler(DWORD dwType)
{
	if (dwType == CTRL_C_EVENT)
	{
		ReleaseServer();

		puts("*** ä�ü����� �����մϴ�! ***");
		::WSACleanup();
		exit(0);
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////
DWORD WINAPI ThreadComplete(LPVOID pParam)
{
	DWORD			dwTransferredSize = 0;
	DWORD			dwFlag = 0;
	USERSESSION		*pSession = NULL;
	LPWSAOVERLAPPED	pWol = NULL;
	BOOL			bResult;

	puts("[IOCP �۾��� ������ ����]");
	while (1)
	{
		bResult = ::GetQueuedCompletionStatus( // GQCS, IOCP Queue���� �Ϸ� ��Ŷ�� ������.
			g_hIocp,				//Dequeue�� IOCP �ڵ�.
			&dwTransferredSize,		//������ ������ ũ��.
			(PULONG_PTR)&pSession,	//���ŵ� �����Ͱ� ����� �޸�
			&pWol,					//OVERLAPPED ����ü.
			INFINITE);				//�̺�Ʈ�� ������ ���.

		if (bResult == TRUE)
		{
			//�������� ���.

			/////////////////////////////////////////////////////////////
			//1. Ŭ���̾�Ʈ�� ������ ���������� �ݰ� ������ ���� ���.
			if (dwTransferredSize == 0)
			{
				
				CloseClient(pSession->hSocket);
				delete pWol;
				delete pSession;
				puts("\tGQCS: Ŭ���̾�Ʈ�� ���������� ������ ������.");
			}

			/////////////////////////////////////////////////////////////
			//2. Ŭ���̾�Ʈ�� ���� �����͸� ������ ���.
			else
			{
				SendMessageAll(pSession->buffer, dwTransferredSize);
				memset(pSession->buffer, 0, sizeof(pSession->buffer));

				//�ٽ� IOCP�� ���(�ɾ��شٰ� ǥ����).
				DWORD dwReceiveSize	= 0;
				DWORD dwFlag		= 0;
				WSABUF wsaBuf		= { 0 };
				wsaBuf.buf = pSession->buffer;
				wsaBuf.len = sizeof(pSession->buffer);

				::WSARecv(
					pSession->hSocket,	//Ŭ���̾�Ʈ ���� �ڵ�
					&wsaBuf,			//WSABUF ����ü �迭�� �ּ�
					1,					//�迭 ����� ����
					&dwReceiveSize,
					&dwFlag,
					pWol,
					NULL);
				if (::WSAGetLastError() != WSA_IO_PENDING)
					puts("\tGQCS: ERROR: WSARecv()");
			}
		}
		else
		{
			//���������� ���.

			/////////////////////////////////////////////////////////////
			//3. �Ϸ� ť���� �Ϸ� ��Ŷ�� ������ ���ϰ� ��ȯ�� ���.
			if (pWol == NULL)
			{
				//IOCP �ڵ��� ���� ���(������ �����ϴ� ���)�� �ش�ȴ�.
				puts("\tGQCS: IOCP �ڵ��� �������ϴ�.");
				break;
			}

			/////////////////////////////////////////////////////////////
			//4. Ŭ���̾�Ʈ�� ������������ ����ưų�
			//   ������ ���� ������ ������ ���.
			else
			{
				if (pSession != NULL)
				{
					CloseClient(pSession->hSocket);
					delete pWol;
					delete pSession;
				}

				puts("\tGQCS: ���� ���� Ȥ�� �������� ���� ����");
			}
		}
	}

	puts("[IOCP �۾��� ������ ����]");
	return 0;
}

/////////////////////////////////////////////////////////////////////////
DWORD WINAPI ThreadAcceptLoop(LPVOID pParam)
{
	LPWSAOVERLAPPED	pWol = NULL;
	DWORD			dwReceiveSize, dwFlag;
	USERSESSION		*pNewUser;
	int				nAddrSize = sizeof(SOCKADDR);
	WSABUF			wsaBuf;
	SOCKADDR		ClientAddr;
	SOCKET			hClient;
	int				nRecvResult = 0;

	while ((hClient = ::accept(g_hSocket,
						&ClientAddr, &nAddrSize)) != INVALID_SOCKET)
	{
		puts("�� Ŭ���̾�Ʈ�� ����ƽ��ϴ�.");
		::EnterCriticalSection(&g_cs);
		g_listClient.push_back(hClient);
		::LeaveCriticalSection(&g_cs);

		//�� Ŭ���̾�Ʈ�� ���� ���� ��ü ����
		pNewUser = new USERSESSION;
		::ZeroMemory(pNewUser, sizeof(USERSESSION));
		pNewUser->hSocket = hClient;

		//�񵿱� ���� ó���� ���� OVERLAPPED ����ü ����.
		pWol = new WSAOVERLAPPED;
		::ZeroMemory(pWol, sizeof(WSAOVERLAPPED));

		//(�����) Ŭ���̾�Ʈ ���� �ڵ��� IOCP�� ����.
		::CreateIoCompletionPort(
			(HANDLE)hClient,		// accept�� Ŭ���̾�Ʈ ���� �ڵ�
			g_hIocp,				// IOCP �ڵ�	
			(ULONG_PTR)pNewUser,	// KEY, ��ġ C����� mlloc() �Լ�ó�� new�� ������ �ּҰ��� �ٸ��⿡ �ĺ��ڷ� ����ϱ� �����ϴ�.
			0);

		dwReceiveSize = 0;
		dwFlag = 0;
		wsaBuf.buf = pNewUser->buffer;
		wsaBuf.len = sizeof(pNewUser->buffer);

		//Ŭ���̾�Ʈ�� ���� ������ �񵿱� �����Ѵ�. // ReadFileEx() �Լ��� ����ϴ�.
		nRecvResult = ::WSARecv(hClient, &wsaBuf, 1, &dwReceiveSize,
							&dwFlag, pWol, NULL);
		if (::WSAGetLastError() != WSA_IO_PENDING)
			puts("ERROR: WSARecv() != WSA_IO_PENDING");
	}

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

	//�Ӱ迵����ü�� �����Ѵ�.
	::InitializeCriticalSection(&g_cs);

	//Ctrl+C Ű�� ������ �� �̸� �����ϰ� ó���� �Լ��� ����Ѵ�.
	if (::SetConsoleCtrlHandler(
			(PHANDLER_ROUTINE)CtrlHandler, TRUE) == FALSE)
		puts("ERROR: Ctrl+C ó���⸦ ����� �� �����ϴ�.");

	//IOCP ����
	g_hIocp = ::CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,	//����� ���� ����.
		NULL,					//���� �ڵ� ����.
		0,						//�ĺ���(Key) �ش���� ����.
		0);						//������ ������ OS�� �ñ�.
	if (g_hIocp == NULL)
	{
		puts("ERORR: IOCP�� ������ �� �����ϴ�.");
		return 0;
	}

	//IOCP ������� ����
	HANDLE hThread;
	DWORD dwThreadID;
	for (int i = 0; i < MAX_THREAD_CNT; ++i)
	{
		dwThreadID = 0;
		//Ŭ���̾�Ʈ�κ��� ���ڿ��� ������.
		hThread = ::CreateThread(NULL,	//���ȼӼ� ���
			0,				//���� �޸𸮴� �⺻ũ��(1MB)
			ThreadComplete,	//������� ������ �Լ��̸�
			(LPVOID)NULL,	//
			0,				//���� �÷��״� �⺻�� ���
			&dwThreadID);	//������ ������ID�� ����� �����ּ�

		::CloseHandle(hThread);
	}

	//���� ���� ���� ����
	g_hSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
		NULL, 0, WSA_FLAG_OVERLAPPED); // for IOCP

	//bind()/listen()
	SOCKADDR_IN addrsvr;
	addrsvr.sin_family = AF_INET;
	addrsvr.sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);
	addrsvr.sin_port = ::htons(25000);

	if (::bind(g_hSocket,
			(SOCKADDR*)&addrsvr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		puts("ERROR: ��Ʈ�� �̹� ������Դϴ�.");
		ReleaseServer();
		return 0;
	}

	if (::listen(g_hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		puts("ERROR:  ���� ���·� ��ȯ�� �� �����ϴ�.");
		ReleaseServer();
		return 0;
	}

	//�ݺ��ؼ� Ŭ���̾�Ʈ�� ������ accept() �Ѵ�.
	hThread = ::CreateThread(NULL, 0, ThreadAcceptLoop,
					(LPVOID)NULL, 0, &dwThreadID);
    if (hThread != NULL) {
        ::CloseHandle(hThread);
    }
	//_tmain() �Լ��� ��ȯ���� �ʵ��� ����Ѵ�.
	puts("*** ä�ü����� �����մϴ�! ***");
	while (1) {
		if (getchar() == EOF) {
			// Handle error or end of input
			break;
		}
	}
	return 0;
}
