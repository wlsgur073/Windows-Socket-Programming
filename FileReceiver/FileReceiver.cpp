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
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandler("������ ������ �� �����ϴ�.");

	//��Ʈ ���ε� �� ����
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (::connect(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
		ErrorHandler("������ ������ �� �����ϴ�.");

	//������ ������ �����Ѵ�.
	puts("*** ���� ������ �����մϴ�. ***");
	FILE *fp = NULL;
	errno_t nResult = fopen_s(&fp, "Sleep away.zip", "wb"); // crc32.zip
	if (nResult != 0)
		ErrorHandler("������ ���� �� �� �����ϴ�.");

	//������ �����ϴ� �����͸� �ݺ��� ���Ͽ� �ٿ� �ִ´�.
	char byBuffer[65536];		//64KB
	int nRecv;
	while ((nRecv = ::recv(hSocket, byBuffer, 65536, 0)) > 0) // copy data of socket buffer to byBuffer
	{
		//�������� ���� ũ�⸸ŭ �����͸� ���Ͽ� ����.
		fwrite(byBuffer, nRecv, 1, fp);
		putchar('#');
	}

	fclose(fp);
	printf("\n*** ���ϼ����� �������ϴ�. ***\n");

	//������ �ݰ� ���α׷� ����.
	//������ ���� ���� ���� '����'��.
	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}
