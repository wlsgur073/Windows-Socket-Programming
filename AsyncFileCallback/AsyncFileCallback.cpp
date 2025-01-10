#include "stdafx.h"
#include <Windows.h>

// https://learn.microsoft.com/en-us/windows/win32/fileio/alertable-i-o
// https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleepex
/////////////////////////////////////////////////////////////////////////
//���� ���Ⱑ �Ϸ�Ǹ� ��ȣ��Ǵ� �Լ�.
void CALLBACK FileIoComplete( // LPOVERLAPPED_COMPLETION_ROUTINE
	DWORD dwError,		//�����ڵ�
	DWORD dwTransfered,	//��/����� �Ϸ�� ������ ũ��
	LPOVERLAPPED pOl)	//OVERLAPPED ����ü
{
	printf("FileIoComplete() Callback - [%d ����Ʈ] ����Ϸ� - %s\n",
		dwTransfered, (char*)pOl->hEvent);

	//hEvent ����� �����ͷ� ���������Ƿ� ����Ű�� ��� �޸� �����Ѵ�.
	//�� �޸𸮴� IoThreadFunction() �Լ����� ���� �Ҵ��� �͵��̴�! // important!
	delete[] pOl->hEvent;
	delete pOl;
	puts("FileIoComplete() - return\n");
}

/////////////////////////////////////////////////////////////////////////
DWORD WINAPI IoThreadFunction(LPVOID pParam)
{
	//�޸𸮸� ���� �Ҵ��ϰ� ���� ä���.
	//���� �޸𸮴� �Ϸ� �Լ����� �����Ѵ�.
	char *pszBuffer = new char[16];
	memset(pszBuffer, 0, sizeof(char) * 16);
	strcpy_s(pszBuffer, sizeof(char) * 16, "Hello IOCP");

	//��OVERLAPPED ����ü�� hEvent ����� ������ ������ �����Ѵ�!
	LPOVERLAPPED pOverlapped = NULL;
	pOverlapped = new OVERLAPPED; // dynamic allocation
	memset(pOverlapped, NULL, sizeof(OVERLAPPED)); 

	pOverlapped->Offset = 1024 * 1024 * 512; // too large offset will slow down the process
	pOverlapped->hEvent = pszBuffer; // hEvent is used as a pointer

	//�񵿱� ���⸦ �õ��Ѵ�. ���Ⱑ �Ϸ�Ǹ� �Ϸ� �Լ��� ��ȣ��ȴ�.
	puts("IoThreadFunction() - ��ø�� ���� �õ�");
	::WriteFileEx((HANDLE)pParam,
		pszBuffer,
		sizeof(char) * 16,
		pOverlapped,
		FileIoComplete // address of the callback function
	);

	//�񵿱� ���� �õ��� ���� Alertable wait ���·� ����Ѵ�.
	for (; ::SleepEx(1, TRUE) != WAIT_IO_COMPLETION; ); // alertable wait
    // Even if you set a time for SleepEx(), the wait is released when the OS calls the callback function.
	puts("IoThreadFunction() - return");
	return 0;
}

/////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	//��ø�� ���� �Ӽ��� �ο��ϰ� ������ �����Ѵ�.
	HANDLE hFile = ::CreateFile(
		_T("TestFile.txt"),
		GENERIC_WRITE,		//���� ���
		0,					//�������� ����.
		NULL,
		CREATE_ALWAYS,		//������ ����.
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //��ø�� ���� // for async
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		puts("ERROR: ��� ������ �� �� �����ϴ�.");
		return 0;
	}

	//�񵿱� ���⸦ ���� �����带 �����Ѵ�.
	HANDLE hThread = NULL;
	DWORD dwThreadID = 0;

	dwThreadID = 0;
	hThread = ::CreateThread(
			NULL,	//���ȼӼ� ���.
			0,		//���� �޸𸮴� �⺻ũ��(1MB).
			IoThreadFunction,	//������� ������ �Լ��̸�.
			hFile,	//�Լ��� ������ �Ű�����.
			0,		//���� �÷��״� �⺻�� ���.
			&dwThreadID);	//������ ������ID ����.

	//�۾��� �����尡 ����� ������ ��ٸ���.
	::WaitForSingleObject(hThread, INFINITE);
	
	//������ �ݰ� �����Ѵ�.
	puts("main() thread ����");
	::CloseHandle(hFile);
	return 0;
}

