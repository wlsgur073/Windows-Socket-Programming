#include "stdafx.h"

#include <Windows.h>
#include <WinInet.h>
#pragma comment(lib, "Wininet.lib")


int _tmain(int argc, _TCHAR* argv[])
{
	//WinINet ���̺귯���� �ʱ�ȭ �Ѵ�.
	HINTERNET hInet = ::InternetOpen(_T("TEST FTP"),
		INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	//FTP ������ �����Ѵ�.
	HINTERNET hFtp = ::InternetConnect(	hInet,
		_T("127.0.0.1"), 21,
		NULL, NULL,
		INTERNET_SERVICE_FTP,
		0, 1234);
	if (hFtp == NULL)
	{
		::InternetCloseHandle(hInet);
		puts("ERROR: FTP ������ ������ �� �����ϴ�.");
		return 0;
	}

	//FTP ��Ʈ ���丮�� ����ִ� ��� ���ϵ��� �����Ѵ�.
	//ù ��° ���� ���.
	WIN32_FIND_DATA findData = { 0 };
	TCHAR szFirstFile[_MAX_FNAME];
	HINTERNET hFtpFile = ::FtpFindFirstFile( hFtp,
		_T("*.*"),
		&findData,
		INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_RELOAD,
		0);
	if (hFtpFile != NULL)
	{
		_putws(findData.cFileName);
		wsprintf(szFirstFile, _T("%s"), findData.cFileName);

		//ù ��°�� ������ ������ ���ϵ� ���.
		while (::InternetFindNextFile(hFtpFile, &findData))
			_putws(findData.cFileName);
	}

	//ù ��° ������ FTP�� �ٿ�ε� �ޱ� ����
	//FTP ������ ������ �����Ѵ�.
	HINTERNET hFtpFileFirst = FtpOpenFile(hFtp,
		szFirstFile,
		GENERIC_READ,
		FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD,
		0);
	if (hFtpFileFirst != NULL)
	{
		//���� ��ũ�� ������ �����Ѵ�.
		HANDLE hFile = ::CreateFile(szFirstFile,
			GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			BYTE byBuffer[8192];
			DWORD dwRead, dwWritten;

			//FTP �������� 8KB�� �о�� ���Ͽ� ����.
			puts("�ٿ�ε� ����");
			while (::InternetReadFile(hFtpFileFirst,
				byBuffer, sizeof(byBuffer), &dwRead))
			{
				//�� �̻� ���� �����Ͱ� �����Ƿ� �� ���� ���̴�.
				if (dwRead == 0)
				{
					puts("\n�ٿ�ε� �Ϸ�");
					break;
				}
				
				::WriteFile(hFile, byBuffer, dwRead, &dwWritten, NULL);
				putchar('#');
			}

			::CloseHandle(hFile);
		}

		::InternetCloseHandle(hFtpFileFirst);
	}

	::InternetCloseHandle(hFtpFile);
	::InternetCloseHandle(hFtp);
	::InternetCloseHandle(hInet);
	return 0;
}

