#include "stdafx.h"

#include <winsock2.h>
//IP Helper API ����� ���� ��� �� ���̺귯�� ����
#include <iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")

#include <stdio.h>
#include <stdlib.h>

int _tmain(int argc, _TCHAR* argv[])
{
	//�� NIC�� ���� ������ ����� �� �ִ� �޸� Ȯ��.
	ULONG uBufferLength = sizeof (IP_ADAPTER_INFO);
	PIP_ADAPTER_INFO pNicInfo = (IP_ADAPTER_INFO*)malloc(uBufferLength);

	//���� ������ Ȯ���Ѵ�.
	DWORD dwResult = ::GetAdaptersInfo(pNicInfo, &uBufferLength); // get adapter info
	if (dwResult == ERROR_BUFFER_OVERFLOW)
	{
		//NIC�� 2�� �̻��̶� ��� ������ ���� �� ���� ����̹Ƿ�
		//�޸𸮸� �ٽ� �Ҵ� �Ѵ�.
		free(pNicInfo);
		pNicInfo = (IP_ADAPTER_INFO *)malloc(uBufferLength);

		//���� ũ�� ���� �� �ٽ� ���� ������ Ȯ�� �Ѵ�.
		if (::GetAdaptersInfo(pNicInfo, &uBufferLength) != NO_ERROR)
		{
			free(pNicInfo);
			puts("ERROR: ����� ������ �� �� �����ϴ�.");
			return 0;
		}
	}

	//���� ����Ʈ ���·� ���� ������ �����Ѵ�.
	PIP_ADAPTER_INFO pNicInfoTmp = pNicInfo;
	while (pNicInfoTmp != NULL)
	{
		printf("����: %s\n", pNicInfoTmp->Description);
		printf("MAC �ּ�: %02X-%02X-%02X-%02X-%02X-%02X\n",
			pNicInfoTmp->Address[0], pNicInfoTmp->Address[1],
			pNicInfoTmp->Address[2], pNicInfoTmp->Address[3],
			pNicInfoTmp->Address[4], pNicInfoTmp->Address[5]);
		printf("IP �ּ�: %s\n\n",
			pNicInfo->IpAddressList.IpAddress.String);

		pNicInfoTmp = pNicInfoTmp->Next;
	}

	free(pNicInfo);
	return 0;
}

