#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include "AppProtocol.h"


void ErrorHandler(const char *pszMessage)
{
	printf("ERROR: %s\n", pszMessage);
	::WSACleanup();
	exit(1);
}

/////////////////////////////////////////////////////////////////////////
void GetFileList(SOCKET hSocket)
{
	//������ ���� ����Ʈ�� ��û�Ѵ�.
	MYCMD cmd = { CMD_GET_LIST, 0 };
	::send(hSocket, (const char*)&cmd, sizeof(cmd), 0); // you can check it in wireshark

	//�����κ��� ���� ����Ʈ�� �����Ѵ�.
	::recv(hSocket, (char*)&cmd, sizeof(cmd), 0);
	if (cmd.nCode != CMD_SND_FILELIST)
		ErrorHandler("�������� ���� ����Ʈ�� �������� ���߽��ϴ�.");

	SEND_FILELIST filelist;
	::recv(hSocket, (char*)&filelist, sizeof(filelist), 0);

	//������ ����Ʈ ������ ȭ�鿡 ����Ѵ�.
	FILEINFO fInfo;
	for (unsigned int i = 0; i < filelist.nCount; ++i)
	{
		::recv(hSocket, (char*)&fInfo, sizeof(fInfo), 0);
		printf("%d\t%s\t%d\n",
			fInfo.nIndex, fInfo.szFileName, fInfo.dwFileSize);
	}
}

/////////////////////////////////////////////////////////////////////////
void GetFile(SOCKET hSocket)
{
	int nIndex;
	printf("������ ������ �ε���(0~2)�� �Է��ϼ���.: ");
	fflush(stdin);
	scanf_s("%d", &nIndex);

	//1. ������ ���� ������ ��û
	BYTE* pCommand = new BYTE[sizeof(MYCMD) + sizeof(GETFILE)]; // dynamic allocation
	memset(pCommand, 0, sizeof(MYCMD)+sizeof(GETFILE));

	MYCMD *pCmd = (MYCMD*)pCommand;
	pCmd->nCode = CMD_GET_FILE;
	pCmd->nSize = sizeof(GETFILE);

	GETFILE *pFile = (GETFILE*)(pCommand + sizeof(MYCMD));
	pFile->nIndex = nIndex;
	//�� ����� �� �޸𸮿� ��� �����Ѵ�!
	::send(hSocket,
		(const char*)pCommand, sizeof(MYCMD) + sizeof(GETFILE), 0);
	delete [] pCommand;

	//2. ���۹��� ���Ͽ� ���� �� ���� ����.
	MYCMD cmd = { 0 };
	FILEINFO fInfo = { 0 };
	::recv(hSocket, (char*)&cmd, sizeof(cmd), 0);
	if (cmd.nCode == CMD_ERROR)
	{
		ERRORDATA err = { 0 };
		::recv(hSocket, (char*)&err, sizeof(err), 0);
		ErrorHandler(err.szDesc);
	}
	else
		::recv(hSocket, (char*)&fInfo, sizeof(fInfo), 0);

	//3. ������ �����Ѵ�.
	printf("%s ���� ������ �����մϴ�!\n", fInfo.szFileName);
	FILE *fp = NULL;
	errno_t nResult = fopen_s(&fp, fInfo.szFileName, "wb");
	if (nResult != 0)
		ErrorHandler("������ ���� �� �� �����ϴ�.");

	char byBuffer[65536];		//64KB
	int nRecv;
	DWORD dwTotalRecv = 0;
	while ((nRecv = ::recv(hSocket, byBuffer, 65536, 0)) > 0)
	{
		fwrite(byBuffer, nRecv, 1, fp);
		dwTotalRecv += nRecv;
		putchar('#');

		if (dwTotalRecv >= fInfo.dwFileSize)
		{
			putchar('\n');
			puts("���� ���ſϷ�!");
			break;
		}
	}

	fclose(fp);
}

/////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		ErrorHandler("������ �ʱ�ȭ �� �� �����ϴ�.");

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

	//�����κ��� ���� ����Ʈ�� �����Ѵ�.
	GetFileList(hSocket);

	//���۹��� ������ �����ϰ� �����Ѵ�.
	GetFile(hSocket);

	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}