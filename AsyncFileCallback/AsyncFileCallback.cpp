#include "stdafx.h"
#include <Windows.h>

// https://learn.microsoft.com/en-us/windows/win32/fileio/alertable-i-o
// https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleepex
/////////////////////////////////////////////////////////////////////////
//파일 쓰기가 완료되면 역호출되는 함수.
void CALLBACK FileIoComplete( // LPOVERLAPPED_COMPLETION_ROUTINE
	DWORD dwError,		//에러코드
	DWORD dwTransfered,	//입/출력이 완료된 데이터 크기
	LPOVERLAPPED pOl)	//OVERLAPPED 구조체
{
	printf("FileIoComplete() Callback - [%d 바이트] 쓰기완료 - %s\n",
		dwTransfered, (char*)pOl->hEvent);

	//hEvent 멤버를 포인터로 전용했으므로 가리키는 대상 메모리 해제한다.
	//이 메모리는 IoThreadFunction() 함수에서 동적 할당한 것들이다! // important!
	delete[] pOl->hEvent;
	delete pOl;
	puts("FileIoComplete() - return\n");
}

/////////////////////////////////////////////////////////////////////////
DWORD WINAPI IoThreadFunction(LPVOID pParam)
{
	//메모리를 동적 할당하고 값을 채운다.
	//※이 메모리는 완료 함수에서 해제한다.
	char *pszBuffer = new char[16];
	memset(pszBuffer, 0, sizeof(char) * 16);
	strcpy_s(pszBuffer, sizeof(char) * 16, "Hello IOCP");

	//※OVERLAPPED 구조체의 hEvent 멤버를 포인터 변수로 전용한다!
	LPOVERLAPPED pOverlapped = NULL;
	pOverlapped = new OVERLAPPED; // dynamic allocation
	memset(pOverlapped, NULL, sizeof(OVERLAPPED)); 

	pOverlapped->Offset = 1024 * 1024 * 512; // too large offset will slow down the process
	pOverlapped->hEvent = pszBuffer; // hEvent is used as a pointer

	//비동기 쓰기를 시도한다. 쓰기가 완료되면 완료 함수가 역호출된다.
	puts("IoThreadFunction() - 중첩된 쓰기 시도");
	::WriteFileEx((HANDLE)pParam,
		pszBuffer,
		sizeof(char) * 16,
		pOverlapped,
		FileIoComplete // address of the callback function
	);

	//비동기 쓰기 시도에 대해 Alertable wait 상태로 대기한다.
	for (; ::SleepEx(1, TRUE) != WAIT_IO_COMPLETION; ); // alertable wait
    // Even if you set a time for SleepEx(), the wait is released when the OS calls the callback function.
	puts("IoThreadFunction() - return");
	return 0;
}

/////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	//중첩된 쓰기 속성을 부여하고 파일을 생성한다.
	HANDLE hFile = ::CreateFile(
		_T("TestFile.txt"),
		GENERIC_WRITE,		//쓰기 모드
		0,					//공유하지 않음.
		NULL,
		CREATE_ALWAYS,		//무조건 생성.
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //중첩된 쓰기 // for async
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		puts("ERROR: 대상 파일을 열 수 없습니다.");
		return 0;
	}

	//비동기 쓰기를 위한 스레드를 생성한다.
	HANDLE hThread = NULL;
	DWORD dwThreadID = 0;

	dwThreadID = 0;
	hThread = ::CreateThread(
			NULL,	//보안속성 상속.
			0,		//스택 메모리는 기본크기(1MB).
			IoThreadFunction,	//스래드로 실행할 함수이름.
			hFile,	//함수에 전달할 매개변수.
			0,		//생성 플래그는 기본값 사용.
			&dwThreadID);	//생성된 스레드ID 저장.

	//작업자 스레드가 종료될 때까지 기다린다.
	::WaitForSingleObject(hThread, INFINITE);
	
	//파일을 닫고 종료한다.
	puts("main() thread 종료");
	::CloseHandle(hFile);
	return 0;
}

