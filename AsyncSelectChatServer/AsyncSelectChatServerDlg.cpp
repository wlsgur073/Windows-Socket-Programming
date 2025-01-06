
// AsyncSelectChatServerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "AsyncSelectChatServer.h"
#include "AsyncSelectChatServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////
//���� ���� ���Ͽ� ���� ����� ���� �޽���.
#define UM_SOCKET_SERVER		WM_USER + 10
//Ŭ���̾�Ʈ ��� ���Ͽ� ���� ����� ���� �޽���.
#define UM_SOCKET_CLIENT		WM_USER + 20


/////////////////////////////////////////////////////////////////////////
// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAsyncSelectChatServerDlg ��ȭ ����



CAsyncSelectChatServerDlg::CAsyncSelectChatServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAsyncSelectChatServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAsyncSelectChatServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAsyncSelectChatServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//����� ���� �޽��� �ڵ鷯
	ON_MESSAGE(UM_SOCKET_SERVER,
					&CAsyncSelectChatServerDlg::OnConnectServerSocket)
	ON_MESSAGE(UM_SOCKET_CLIENT,
					&CAsyncSelectChatServerDlg::OnClientSocket)
	//��ư �̺�Ʈ ó����
	ON_BN_CLICKED(IDC_BUTTON_START,
					&CAsyncSelectChatServerDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP,
					&CAsyncSelectChatServerDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// CAsyncSelectChatServerDlg �޽��� ó����

BOOL CAsyncSelectChatServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CAsyncSelectChatServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CAsyncSelectChatServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CAsyncSelectChatServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////
//���� ���� ����
SOCKET g_hSocket = NULL;
//����� Ŭ���̾�Ʈ ���ϵ��� �����ϱ� ���� ���� ����Ʈ 
CPtrList g_listClient;

/////////////////////////////////////////////////////////////////////////
void CAsyncSelectChatServerDlg::OnBnClickedButtonStart()
{
	//���Ӵ�� ���� ����
	g_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (g_hSocket == INVALID_SOCKET)
	{
		AfxMessageBox(_T("ERROR: ���� ��� ������ ������ �� �����ϴ�."));
		return;
	}

	//��Ʈ ���ε�
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (::bind(g_hSocket, (SOCKADDR*)&svraddr, sizeof(svraddr)) ==
		SOCKET_ERROR)
	{
		AfxMessageBox(_T("ERROR: ���Ͽ� �ּҸ� ���ε� �� �� �����ϴ�."));
		return;
	}

	//���Ӵ�� g_hSocket ��ȯ
	if (::listen(g_hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("ERROR: ���� ���·� ��ȯ�� �� �����ϴ�."));
		return;
	}

	//�񵿱� ���� �޽��� ������ ���� ���� �̺�Ʈ ���.
	//���� ���Ͽ� ��ȭ�� �߻��ϸ� ����� ���� �޽����� ���޵ȴ�!
	if (::WSAAsyncSelect(g_hSocket, m_hWnd, UM_SOCKET_SERVER,
		FD_ACCEPT) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("ERROR: WSAAsyncSelect()"));
		return;
	}
}

/////////////////////////////////////////////////////////////////////////
void CAsyncSelectChatServerDlg::OnBnClickedButtonStop()
{
	//���� ������ ����.
	if (g_hSocket != NULL)
	{
		::closesocket(g_hSocket);
		g_hSocket = NULL;
	}

	//��� Ŭ���̾�Ʈ ������ ������.
	POSITION pos = g_listClient.GetHeadPosition();
	SOCKET hClient;
	while (pos != NULL)
	{
		hClient = (SOCKET)g_listClient.GetNext(pos);
		if (hClient != NULL)
		{
			::shutdown(hClient, SD_BOTH);
			::closesocket(hClient);
		}
	}

	g_listClient.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////
//Ŭ���̾�Ʈ�� ������ �õ��� ��� ȣ��Ǵ� ����� ���� �޽��� �ڵ鷯.
LRESULT CAsyncSelectChatServerDlg::OnConnectServerSocket(
										WPARAM wParam, LPARAM lParam)
{
	//lParam�� High word���� �������� �˻��Ѵ�.
	if (WSAGETSELECTERROR(lParam))
	{
		AfxMessageBox(_T("ERROR: WSAGETSELECTERROR()"));
		return 0L;
	}

	//lParam�� Low word���� �߻��� �̺�Ʈ�� ��ü���� ������ Ȯ���Ѵ�.
	if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT)
	{
		//���ο� Ŭ���̾�Ʈ�� ������ �õ���.
		SOCKADDR_IN clientaddr = { 0 };
		int nAddrLen = sizeof(clientaddr);
		SOCKET hClient = ::accept(g_hSocket, 
								(SOCKADDR*)&clientaddr, &nAddrLen);

		if (hClient != INVALID_SOCKET)
		{
			g_listClient.AddTail((LPVOID)hClient);
			//�� Ŭ���̾�Ʈ�� ���� ���� �̺�Ʈ�� �����.
			//���� �����Ͱ� �ְų� ������ ������ ��쿡 ���� �̺�Ʈ�� ����
			//����� ���� �޽����� �����Ѵ�.
			if (::WSAAsyncSelect(hClient,
				m_hWnd,				//�޽����� ���� ������ �ڵ�
				UM_SOCKET_CLIENT,	//��ȭ �߻��� ���޵� �޽���.
				FD_READ | FD_CLOSE) == SOCKET_ERROR)
			{
				AfxMessageBox(_T("ERROR:WSAAsyncSelect()-accept()"));
			}
		}
	}

	return 0L;
}

/////////////////////////////////////////////////////////////////////////
//Ŭ���̾�Ʈ�� �����͸� �����ϰų�
//������ �����ϴ� ��쿡 ȣ��Ǵ� ����� ���� �޽��� �ڵ鷯.
LRESULT CAsyncSelectChatServerDlg::OnClientSocket(
										WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		AfxMessageBox(_T("ERROR: WSAGETSELECTERROR()"));
		return 0L;
	}

	TCHAR szBuffer[1024] = { 0 };
	POSITION pos;
	SOCKET hSockTmp;
	int nReceive;

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
		//Ŭ���̾�Ʈ�κ��� �����ؾ� �� �����Ͱ� �ִ� ���.
		nReceive = ::recv((SOCKET)wParam,
						(char*)szBuffer, sizeof(szBuffer), 0);
		//ä�� �޽��� ����
		pos = g_listClient.GetHeadPosition();
		while (pos != NULL)
		{
			hSockTmp = (SOCKET)g_listClient.GetNext(pos);
			::send(hSockTmp, (char*)szBuffer, sizeof(szBuffer), 0);
		}
		break;

	case FD_CLOSE:
		//Ŭ���̾�Ʈ�� ������ ������ ���.
		::closesocket((SOCKET)wParam);
		pos = g_listClient.Find((LPVOID)wParam);
		if (pos != NULL)
			g_listClient.RemoveAt(pos);

		AfxMessageBox(_T("Ŭ���̾�Ʈ�� ������ �������ϴ�."));
		break;
	}

	return 0L;
}




