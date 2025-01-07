#include "stdafx.h"

#include <winsock2.h>
#pragma comment(lib, "ws2_32")
//::TranmitFile() �Լ��� ����ϱ� ���� ����� ���̺귯�� ����
#include <Mswsock.h>
#pragma comment(lib, "Mswsock")

//������ ���Ͽ� ���� ������ ������� ����ü
typedef struct MY_FILE_DATA
{
	// _MAX_FNAME: This size is sufficient for file names in the Windows file system.
    char szName[_MAX_FNAME]; 
	DWORD dwSize;
} MY_FILE_DATA;

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

	//������ ���� ����
	HANDLE hFile = ::CreateFile(_T("Sleep Away.zip"),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		ErrorHandler("������ ������ ������ �� �����ϴ�.");

	//���Ӵ�� ���� ����
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandler("���� ��� ������ ������ �� �����ϴ�.");

	//��Ʈ ���ε�
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (::bind(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
		ErrorHandler("���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.");

	//���Ӵ�� ���·� ��ȯ
	if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR)
		ErrorHandler("���� ���·� ��ȯ�� �� �����ϴ�.");
	puts("���ϼ۽ż����� �����մϴ�.");

	//Ŭ���̾�Ʈ ������ �޾Ƶ��̰� ���ο� ���� ����(����)
	SOCKADDR_IN clientaddr = { 0 };
	int nAddrLen = sizeof(clientaddr);
	SOCKET hClient = ::accept(hSocket,
		(SOCKADDR*)&clientaddr, &nAddrLen);
	if (hClient == INVALID_SOCKET)
		ErrorHandler("Ŭ���̾�Ʈ ��� ������ ������ �� �����ϴ�.");
	puts("Ŭ���̾�Ʈ�� ����Ǿ����ϴ�.");

	//������ ���Ͽ� ���� ������ �ۼ��Ѵ�.
	MY_FILE_DATA fData = { "Sleep Away.zip", 0 };
	fData.dwSize = ::GetFileSize(hFile, NULL);
	TRANSMIT_FILE_BUFFERS tfb = { 0 };
	tfb.Head = &fData;
	tfb.HeadLength = sizeof(fData);

	//���ϼ۽�
	if ( ::TransmitFile(
			hClient,	//������ ������ ���� �ڵ�.
			hFile,		//������ ���� �ڵ�.
			0,			//������ ũ��. 0�̸� ��ü.
			65536,		//�� ���� ������ ���� ũ��.
			NULL,		//*�񵿱� ��/��¿� ���� OVERLAPPED ����ü.
			&tfb,		//���� ���ۿ� �ռ� ���� ������ ������.
			0			//��Ÿ �ɼ�.
			) == FALSE )
		ErrorHandler("������ ������ �� �����ϴ�.");

	//Ŭ���̾�Ʈ�� ������ ���� ���⸦ ����Ѵ�.
	::recv(hClient, NULL, 0, 0);
	puts("Ŭ���̾�Ʈ�� ������ �������ϴ�.");

	::closesocket(hClient);
	::closesocket(hSocket);
	::CloseHandle(hFile);
	::WSACleanup();
	return 0;
}

