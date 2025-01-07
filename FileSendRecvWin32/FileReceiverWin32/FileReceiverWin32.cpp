#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")


typedef struct MY_FILE_DATA
{
	char szName[_MAX_FNAME];
	DWORD dwSize;
} MY_FILE_DATA; // will be flexible in the future if extended to insert more data fields like original file path, key, MD5 etc.

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
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandler("������ ������ �� �����ϴ�.");

	//��Ʈ ���ε� �� ����
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (::connect(hSocket, // connect TCP socket
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
		ErrorHandler("������ ������ �� �����ϴ�.");

	//������ ���ϸ�, ũ�� ������ ���� �޴´�.
	MY_FILE_DATA fData = { 0 };
	if (::recv(hSocket, (char*)&fData, sizeof(fData), 0) < sizeof(fData))
		ErrorHandler("���� ������ �������� ���߽��ϴ�.");

	//������ ������ �����Ѵ�.
	puts("*** ���� ������ �����մϴ�. ***");
	HANDLE hFile = ::CreateFileA(
		fData.szName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,	//������ �����Ѵ�.
		0,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		ErrorHandler("������ ������ ������ �� �����ϴ�.");

	//������ �����ϴ� �����͸� �ݺ��� ���Ͽ� �ٿ� �ִ´�.
	char byBuffer[65536];		//64KB
	int nRecv;
	DWORD dwTotalRecv = 0, dwRead = 0;
	while (dwTotalRecv < fData.dwSize)
	{
		if ((nRecv = ::recv(hSocket, byBuffer, 65536, 0)) > 0)
		{
			dwTotalRecv += nRecv;
			//�������� ���� ũ�⸸ŭ �����͸� ���Ͽ� ����.
			::WriteFile(hFile, byBuffer, nRecv, &dwRead, NULL);
			printf("Receive: %d/%d\n", dwTotalRecv, fData.dwSize);
			fflush(stdout);
		}
		else
		{
			puts("ERROR: ���� ���� �߿� ������ �߻��߽��ϴ�.");
			break;
		}
	}

	::CloseHandle(hFile);
	//���� ���� �Ϸ� �� Ŭ���̾�Ʈ�� ���� ������ �ݴ´�!
	::closesocket(hSocket);
	printf("*** ���ϼ����� �������ϴ�. ***\n");

	::WSACleanup();
	return 0;
}

