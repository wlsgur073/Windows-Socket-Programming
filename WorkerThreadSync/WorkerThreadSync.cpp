#include "stdafx.h"
#include <Windows.h>

// Worker thread function
DWORD WINAPI ThreadFunction(LPVOID pParam)
{
	puts("*** Begin Thread ****");
	for (int i = 0; i < 5; ++i)
	{
		printf("[Worker thread] %d\n", i);
		::Sleep(1);
	}

    // Set the event before the thread ends.
    puts("Before setting the event");
    // Calling this function will cause
	// the WaitForSingleObject() function in _tmain() to return!
	::SetEvent((HANDLE)pParam);
	puts("After setting the event");
	puts("**** End Thread ****");
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
    // Create an event object.
    HANDLE hEvent = ::CreateEvent(
        NULL,   // Apply default security attributes.
        FALSE,  // Automatically reset the state.
        FALSE,  // Initial state is FALSE.
        NULL);  // No name.

    // Create and start the worker thread.
	DWORD dwThreadID = 0;
	HANDLE hThread = ::CreateThread(NULL, 0,
		ThreadFunction,
		hEvent,		// Pass the event handle to the thread function.
		0, &dwThreadID);

    // Start of code region that runs concurrently with the worker thread.
	for (int i = 0; i < 5; ++i)
	{
		printf("[Main thread] %d\n", i);
        // If `i` is 3, wait indefinitely for the event to be set!
		if (i == 3 &&
			::WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0)
		{
            puts("Detected the termination event!");
			::CloseHandle(hEvent);
			hEvent = NULL;
		}
	}

	::CloseHandle(hThread);
	return 0;
}
