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
	//서버에 파일 리스트를 요청한다.
	MYCMD cmd = { CMD_GET_LIST, 0 };
	::send(hSocket, (const char*)&cmd, sizeof(cmd), 0); // you can check it in wireshark

	//서버로부터 파일 리스트를 수신한다.
	::recv(hSocket, (char*)&cmd, sizeof(cmd), 0);
	if (cmd.nCode != CMD_SND_FILELIST)
		ErrorHandler("서버에서 파일 리스트를 수신하지 못했습니다.");

	SEND_FILELIST filelist;
	::recv(hSocket, (char*)&filelist, sizeof(filelist), 0);

	//수신한 리스트 정보를 화면에 출력한다.
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
	printf("수신할 파일의 인덱스(0~2)를 입력하세요.: ");
	fflush(stdin);
	scanf_s("%d", &nIndex);

	//1. 서버에 파일 전송을 요청
	BYTE* pCommand = new BYTE[sizeof(MYCMD) + sizeof(GETFILE)]; // dynamic allocation
	memset(pCommand, 0, sizeof(MYCMD)+sizeof(GETFILE));

	MYCMD *pCmd = (MYCMD*)pCommand;
	pCmd->nCode = CMD_GET_FILE;
	pCmd->nSize = sizeof(GETFILE);

	GETFILE *pFile = (GETFILE*)(pCommand + sizeof(MYCMD));
	pFile->nIndex = nIndex;
	//두 헤더를 한 메모리에 묶어서 전송한다!
	::send(hSocket,
		(const char*)pCommand, sizeof(MYCMD) + sizeof(GETFILE), 0);
	delete [] pCommand;

	//2. 전송받을 파일에 대한 상세 정보 수신.
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

	//3. 파일을 수신한다.
	printf("%s 파일 수신을 시작합니다!\n", fInfo.szFileName);
	FILE *fp = NULL;
	errno_t nResult = fopen_s(&fp, fInfo.szFileName, "wb");
	if (nResult != 0)
		ErrorHandler("파일을 생성 할 수 없습니다.");

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
			puts("파일 수신완료!");
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
		ErrorHandler("윈속을 초기화 할 수 없습니다.");

	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandler("소켓을 생성할 수 없습니다.");

	//포트 바인딩 및 연결
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (::connect(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
		ErrorHandler("서버에 연결할 수 없습니다.");

	//서버로부터 파일 리스트를 수신한다.
	GetFileList(hSocket);

	//전송받을 파일을 선택하고 수신한다.
	GetFile(hSocket);

	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}