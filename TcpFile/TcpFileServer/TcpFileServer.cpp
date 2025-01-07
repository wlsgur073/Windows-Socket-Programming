#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
//�� ���� ���α׷� ��� �������� ���ǰ� ����ִ� �������
#include "AppProtocol.h"


/////////////////////////////////////////////////////////////////////////
//������ �������� ����
SEND_FILELIST g_flist = { 3 };

//Ŭ���̾�Ʈ�� �ٿ�ε� ������ ����
FILEINFO g_aFInfo[3] = { // �ϵ��ڵ�, �����δ� ���Ͻý����� �˻��ؾ� ��.
	{ 0, "Sleep Away.mp3", 4842585 },
	{ 1, "Kalimba.mp3", 8414449 },
	{ 2, "Maid with the Flaxen Hair.mp3", 4113874 }
};

/////////////////////////////////////////////////////////////////////////
void ErrorHandler(const char *pszMessage)
{
	printf("ERROR: %s\n", pszMessage);
	::WSACleanup();
	exit(1);
}

/////////////////////////////////////////////////////////////////////////
//���� ����Ʈ ������ Ŭ���̾�Ʈ�� �����ϴ� �Լ�.
void SendFileList(SOCKET hClient)
{
	MYCMD cmd;
	cmd.nCode = CMD_SND_FILELIST;
	cmd.nSize = sizeof(g_flist)+sizeof(g_aFInfo);
	//�⺻��� ����.
	send(hClient, (const char*)&cmd, sizeof(cmd), 0);
	//���� ����Ʈ ��� ����.
	send(hClient, (const char*)&g_flist, sizeof(g_flist), 0);
	//���� ������ ����.
	send(hClient, (const char*)g_aFInfo, sizeof(g_aFInfo), 0);
}

/////////////////////////////////////////////////////////////////////////
//�ε����� �ش��ϴ� ������ Ŭ���̾�Ʈ���� �۽��ϴ� �Լ�.
void SendFile(SOCKET hClient, int nIndex)
{
	MYCMD cmd;
	ERRORDATA err;
	//���� ����Ʈ���� �ε����� �´� ������ �˻��Ѵ�.
	if (nIndex < 0 || nIndex > 2)
	{
		cmd.nCode = CMD_ERROR;
		cmd.nSize = sizeof(err);
		err.nErrorCode = 0;
		strcpy_s(err.szDesc, "�߸��� ���� �ε��� �Դϴ�.");

		//���� ������ Ŭ���̾�Ʈ���� ����.
		send(hClient, (const char*)&cmd, sizeof(cmd), 0);
		send(hClient, (const char*)&err, sizeof(err), 0);
		return;
	}

	//���� �۽� ������ �˸��� ������ �����Ѵ�.
	cmd.nCode = CMD_BEGIN_FILE;
	cmd.nSize = sizeof(FILEINFO);
	send(hClient, (const char*)&cmd, sizeof(cmd), 0);
	//�۽��ϴ� ���Ͽ� ���� ������ �����Ѵ�.
	send(hClient, (const char*)&g_aFInfo[nIndex], sizeof(FILEINFO), 0);

	FILE *fp = NULL;
	errno_t nResult = fopen_s(&fp, g_aFInfo[nIndex].szFileName, "rb");
	if (nResult != 0)
		ErrorHandler("������ ������ ������ �� �����ϴ�.");

	//������ �۽��Ѵ�.
	char byBuffer[65536];		//64KB
	int nRead;
	while ((nRead = fread(byBuffer, sizeof(char), 65536, fp)) > 0)
		send(hClient, byBuffer, nRead, 0);

	fclose(fp);
}

/////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	//���� �ʱ�ȭ
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		ErrorHandler("������ �ʱ�ȭ �� �� �����ϴ�.");

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
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR )
		ErrorHandler("���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.");

	//���Ӵ�� ���·� ��ȯ
	if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR)
		ErrorHandler("���� ���·� ��ȯ�� �� �����ϴ�.");
	puts("���ϼ۽ż����� �����մϴ�.");

	//Ŭ���̾�Ʈ ������ �޾Ƶ��̰� ���ο� ���� ����(����)
	SOCKADDR_IN clientaddr = { 0 };
	int nAddrLen = sizeof(clientaddr);

	SOCKET hClient = ::accept(hSocket, (SOCKADDR*)&clientaddr, &nAddrLen);
	if (hClient == INVALID_SOCKET)
		ErrorHandler("Ŭ���̾�Ʈ ��� ������ ������ �� �����ϴ�.");
	puts("Ŭ���̾�Ʈ�� ����Ǿ����ϴ�.");

	//Ŭ���̾�Ʈ�κ��� ����� �����ϰ� �����ϴ� Event loop.
	MYCMD cmd;
	while (::recv(hClient, (char*)&cmd, sizeof(MYCMD), 0) > 0)
	{
		switch (cmd.nCode)
		{
		case CMD_GET_LIST:
			puts("Ŭ���̾�Ʈ�� ���ϸ���� �䱸��.");
			SendFileList(hClient);
			break;

		case CMD_GET_FILE:
			puts("Ŭ���̾�Ʈ�� ���������� �䱸��.");
			{
				GETFILE file;
				::recv(hClient, (char*)&file, sizeof(file), 0);
				SendFile(hClient, file.nIndex);
				break;
			}

		default:
			ErrorHandler("�� �� ���� ����� �����߽��ϴ�.");
			break;
		}
	}

	//Ŭ���̾�Ʈ�� ������ ������.
	::closesocket(hClient);
	puts("Ŭ���̾�Ʈ ������ ������ϴ�.");

	//���� ���� �ݱ�
	::closesocket(hSocket);
	//���� ����
	::WSACleanup();
	return 0;
}
