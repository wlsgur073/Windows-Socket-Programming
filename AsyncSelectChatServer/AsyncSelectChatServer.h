
// AsyncSelectChatServer.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CAsyncSelectChatServerApp:
// �� Ŭ������ ������ ���ؼ��� AsyncSelectChatServer.cpp�� �����Ͻʽÿ�.
//

class CAsyncSelectChatServerApp : public CWinApp
{
public:
	CAsyncSelectChatServerApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CAsyncSelectChatServerApp theApp;