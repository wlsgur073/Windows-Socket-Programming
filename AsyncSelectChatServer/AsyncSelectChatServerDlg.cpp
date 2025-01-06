
// AsyncSelectChatServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "AsyncSelectChatServer.h"
#include "AsyncSelectChatServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////
//서버 리슨 소켓에 대한 사용자 정의 메시지.
#define UM_SOCKET_SERVER		WM_USER + 10
//클라이언트 통신 소켓에 대한 사용자 정의 메시지.
#define UM_SOCKET_CLIENT		WM_USER + 20


/////////////////////////////////////////////////////////////////////////
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CAsyncSelectChatServerDlg 대화 상자



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
	//사용자 정의 메시지 핸들러
	ON_MESSAGE(UM_SOCKET_SERVER,
					&CAsyncSelectChatServerDlg::OnConnectServerSocket)
	ON_MESSAGE(UM_SOCKET_CLIENT,
					&CAsyncSelectChatServerDlg::OnClientSocket)
	//버튼 이벤트 처리기
	ON_BN_CLICKED(IDC_BUTTON_START,
					&CAsyncSelectChatServerDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP,
					&CAsyncSelectChatServerDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// CAsyncSelectChatServerDlg 메시지 처리기

BOOL CAsyncSelectChatServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CAsyncSelectChatServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CAsyncSelectChatServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////
//서버 리슨 소켓
SOCKET g_hSocket = NULL;
//연결된 클라이언트 소켓들을 관리하기 위한 연결 리스트 
CPtrList g_listClient;

/////////////////////////////////////////////////////////////////////////
void CAsyncSelectChatServerDlg::OnBnClickedButtonStart()
{
	//접속대기 소켓 생성
	g_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (g_hSocket == INVALID_SOCKET)
	{
		AfxMessageBox(_T("ERROR: 접속 대기 소켓을 생성할 수 없습니다."));
		return;
	}

	//포트 바인딩
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (::bind(g_hSocket, (SOCKADDR*)&svraddr, sizeof(svraddr)) ==
		SOCKET_ERROR)
	{
		AfxMessageBox(_T("ERROR: 소켓에 주소를 바인드 할 수 없습니다."));
		return;
	}

	//접속대기 g_hSocket 전환
	if (::listen(g_hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("ERROR: 리슨 상태로 전환할 수 없습니다."));
		return;
	}

	//비동기 통지 메시지 수신을 위한 소켓 이벤트 등록.
	//서버 소켓에 변화가 발생하면 사용자 정의 메시지가 전달된다!
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
	//서버 소켓을 닫음.
	if (g_hSocket != NULL)
	{
		::closesocket(g_hSocket);
		g_hSocket = NULL;
	}

	//모든 클라이언트 연결을 종료함.
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
//클라이언트가 연결을 시도할 경우 호출되는 사용자 정의 메시지 핸들러.
LRESULT CAsyncSelectChatServerDlg::OnConnectServerSocket(
										WPARAM wParam, LPARAM lParam)
{
	//lParam의 High word에서 오류값을 검사한다.
	if (WSAGETSELECTERROR(lParam))
	{
		AfxMessageBox(_T("ERROR: WSAGETSELECTERROR()"));
		return 0L;
	}

	//lParam의 Low word에서 발생한 이벤트의 구체적인 내용을 확인한다.
	if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT)
	{
		//새로운 클라이언트가 연결을 시도함.
		SOCKADDR_IN clientaddr = { 0 };
		int nAddrLen = sizeof(clientaddr);
		SOCKET hClient = ::accept(g_hSocket, 
								(SOCKADDR*)&clientaddr, &nAddrLen);

		if (hClient != INVALID_SOCKET)
		{
			g_listClient.AddTail((LPVOID)hClient);
			//새 클라이언트에 대한 소켓 이벤트를 등록함.
			//읽을 데이터가 있거나 소켓이 닫히는 경우에 대한 이벤트에 대해
			//사용자 정의 메시지를 수신한다.
			if (::WSAAsyncSelect(hClient,
				m_hWnd,				//메시지를 받을 윈도우 핸들
				UM_SOCKET_CLIENT,	//변화 발생시 전달될 메시지.
				FD_READ | FD_CLOSE) == SOCKET_ERROR)
			{
				AfxMessageBox(_T("ERROR:WSAAsyncSelect()-accept()"));
			}
		}
	}

	return 0L;
}

/////////////////////////////////////////////////////////////////////////
//클라이언트가 데이터를 전송하거나
//연결을 종료하는 경우에 호출되는 사용자 정의 메시지 핸들러.
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
		//클라이언트로부터 수신해야 할 데이터가 있는 경우.
		nReceive = ::recv((SOCKET)wParam,
						(char*)szBuffer, sizeof(szBuffer), 0);
		//채팅 메시지 전송
		pos = g_listClient.GetHeadPosition();
		while (pos != NULL)
		{
			hSockTmp = (SOCKET)g_listClient.GetNext(pos);
			::send(hSockTmp, (char*)szBuffer, sizeof(szBuffer), 0);
		}
		break;

	case FD_CLOSE:
		//클라이언트가 연결을 종료한 경우.
		::closesocket((SOCKET)wParam);
		pos = g_listClient.Find((LPVOID)wParam);
		if (pos != NULL)
			g_listClient.RemoveAt(pos);

		AfxMessageBox(_T("클라이언트가 연결을 끊었습니다."));
		break;
	}

	return 0L;
}




