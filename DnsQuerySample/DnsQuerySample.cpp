#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	//윈속 초기화
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: 윈속을 초기화 할 수 없습니다.");
		return 0;
	}

	//현재 DNS 설정을 근거로 Naver의 IP주소를 질의한다.
	addrinfo hints = { 0 };
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo* result = NULL;
	int ret = ::getaddrinfo("www.naver.com", NULL, &hints, &result);
	if (ret != 0)
	{
		puts("ERROR: Naver의 IP주소를 알 수 없습니다.");
		::WSACleanup();
		return 0;
	}

	for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		sockaddr_in* sockaddr_ipv4 = (sockaddr_in*)ptr->ai_addr;
		char ipstr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipstr, INET_ADDRSTRLEN);
		printf("\tIP주소: %s\n", ipstr);
	}

	::freeaddrinfo(result);
	//윈속 해제
	::WSACleanup();
	return 0;
}
