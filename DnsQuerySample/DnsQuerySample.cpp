#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	//���� �ʱ�ȭ
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
		return 0;
	}

	//���� DNS ������ �ٰŷ� Naver�� IP�ּҸ� �����Ѵ�.
	addrinfo hints = { 0 };
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo* result = NULL;
	int ret = ::getaddrinfo("www.naver.com", NULL, &hints, &result);
	if (ret != 0)
	{
		puts("ERROR: Naver�� IP�ּҸ� �� �� �����ϴ�.");
		::WSACleanup();
		return 0;
	}

	for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		sockaddr_in* sockaddr_ipv4 = (sockaddr_in*)ptr->ai_addr;
		char ipstr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipstr, INET_ADDRSTRLEN);
		printf("\tIP�ּ�: %s\n", ipstr);
	}

	::freeaddrinfo(result);
	//���� ����
	::WSACleanup();
	return 0;
}
