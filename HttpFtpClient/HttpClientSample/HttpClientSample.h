
// HttpClientSample.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CHttpClientSampleApp:
// �� Ŭ������ ������ ���ؼ��� HttpClientSample.cpp�� �����Ͻʽÿ�.
//

class CHttpClientSampleApp : public CWinApp
{
public:
	CHttpClientSampleApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CHttpClientSampleApp theApp;