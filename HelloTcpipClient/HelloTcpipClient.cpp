#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

// Keep in mind. to terminating a TCP connection lies with the client.
// The side that initiates the disconnection will enter the TIME_WAIT state.
// Therefore, the server should not be in the TIME_WAIT state.
int _tmain(int argc, _TCHAR* argv[])
{
	//※윈속 초기화
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: 윈속을 초기화 할 수 없습니다.");
		return 0;
	}

	//1. 접속대기 소켓 생성
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0); // client also has a socket
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: 소켓을 생성할 수 없습니다.");
		return 0;
	}

	//2. 포트 바인딩 및 연결
	// where set the client socket's address ? -> OS will assign the client address
	SOCKADDR_IN	svraddr = { 0 }; // server socket address
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (::connect(hSocket, // client socket must to know server socket ip and port
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: 서버에 연결할 수 없습니다.");
		return 0;
	}

	//3. 채팅 메시지 송/수신
	char szBuffer[128] = { 0 };
	while (1)
	{
		//사용자로부터 문자열을 입력 받는다.
		gets_s(szBuffer);
		if (strcmp(szBuffer, "EXIT") == 0) break;

		//사용자가 입력한 문자열을 서버에 전송한다.
		::send(hSocket, szBuffer
			, strlen(szBuffer) + 1 // why +1 ? -> include `null` character
			, 0);

		// Test : send and recv mappped one by one? -> Nope.
		//int nLen = strlen(szBuffer);
		//for (int i = 0; i < nLen; i++) // send one character at a time
		//	::send(hSocket, szBuffer + i, 1, 0);


		//서버로부터 방금 보낸 문자열에 대한 에코 메시지를 수신한다.
		memset(szBuffer, 0, sizeof(szBuffer));
		::recv(hSocket, szBuffer, sizeof(szBuffer), 0);
		printf("From server: %s\n", szBuffer);
	}

	//4. 소켓을 닫고 종료.
	//::shutdown(hSocket, SD_BOTH); // means stop I/O
	::closesocket(hSocket);
	//※윈속 해제
	::WSACleanup();
	return 0;
}
