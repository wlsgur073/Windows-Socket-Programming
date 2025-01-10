#include "stdafx.h"
#include <Windows.h>

// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
int _tmain(int argc, _TCHAR* argv[])
{
	//��ø�� ���� �Ӽ��� �ο��ϰ� ������ �����Ѵ�.
	HANDLE hFile = ::CreateFile(
		_T("TestFile.txt"),
		GENERIC_WRITE,		//���� ���
		0,					//���μ������� �������� ����.
		NULL,
		CREATE_ALWAYS,		//������ ����.
		FILE_ATTRIBUTE_NORMAL |	FILE_FLAG_OVERLAPPED,	//��ø�� ����
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		puts("ERROR: ��� ������ �� �� �����ϴ�.");
		return 0;
	}

	//�񵿱� ����� ������ OVERLAPPED ����ü �� �̺�Ʈ �ڵ��� ���� �Ѵ�.
	DWORD dwRead;
	OVERLAPPED aOl[3] = { 0 };
	HANDLE aEvt[3] = { 0 };

	//�� ���� �񵿱� ���� �ϷḦ Ȯ���ϱ� ���� �̺�Ʈ ��ü�� �����Ѵ�.
	for (int i = 0; i < 3; ++i)
	{
		aEvt[i] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		aOl[i].hEvent = aEvt[i];
	}

	//�񵿱� ���Ⱑ ���۵� ������ ����Ѵ�.
	//�� ��° ����� �� ��° ���⺸�� ���߿� �̷���� ���ɼ��� ����.
	aOl[0].Offset = 0;					//������ ����.
	aOl[1].Offset = 1024 * 1024 * 5;	//5MB
	aOl[2].Offset = 16;					//16����Ʈ

	//�� ���� �񵿱� ���⸦ ���������� �����Ѵ�.
	for (int i = 0; i < 3; ++i)
	{
		printf("%d��° ��ø�� ���� �õ�.\n", i);
		::WriteFile(hFile, "0123456789", 10, &dwRead, &aOl[i]);
		//�������� ��� ���� �õ��� ����(����)�ȴ�!
		if (::GetLastError() != ERROR_IO_PENDING)
			exit(0);
	}

	//�� ���� �񵿱� ���Ⱑ �Ϸ�Ǳ⸦ ����Ѵ�.
	DWORD dwResult = 0;
	for (int i = 0; i < 3; ++i)
	{
		dwResult = ::WaitForMultipleObjects(3, aEvt, FALSE, INFINITE);
		printf("-> %d��° ���� �Ϸ�.\n", dwResult - WAIT_OBJECT_0);
	}

	//�̺�Ʈ �ڵ�� ������ �ݴ´�.
	for (int i = 0; i < 3; ++i)
		::CloseHandle(aEvt[i]);
	::CloseHandle(hFile);
	return 0;
}