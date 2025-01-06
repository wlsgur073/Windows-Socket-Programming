
// AsyncSelectChatServerDlg.h : ��� ����
//

#pragma once


// CAsyncSelectChatServerDlg ��ȭ ����
class CAsyncSelectChatServerDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CAsyncSelectChatServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ASYNCSELECTCHATSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	//����� ���� �޽��� �ڵ鷯
	LRESULT OnConnectServerSocket(WPARAM wParam, LPARAM lParam);
	LRESULT OnClientSocket(WPARAM wParam, LPARAM lParam);
};
