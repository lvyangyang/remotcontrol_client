
// udplearnDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "udplearn.h"
#include "udplearnDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct Mouse
{
	WPARAM wparam;
	MSLLHOOKSTRUCT position;

}*pmouse_data, mouse_data;
struct Keyboard
{
	WPARAM wparam;
	KBDLLHOOKSTRUCT code;
	char space[3];
}*pkeyboard_data, keyboard_data;

WSADATA wsad;
SOCKET s,tcp_s;
SOCKADDR_IN addr;
SOCKADDR_IN peeraddr,server_addr;
BOOL bBoardcast;

DWORD threadid;
HANDLE hthread;
LPVOID lparam;
bool beconscan;

char szBuf[100] = { 0 };
SOCKADDR_IN remote;
int len = sizeof(remote);

DWORD WINAPI udpproc(LPVOID lparam)
{
	beconscan = 1;
	

	while (1)
	{
		if (beconscan == 1)
		{
			if (recvfrom(s, szBuf, 100, 0, (LPSOCKADDR)&remote, &len) == SOCKET_ERROR)
			{
				//AfxMessageBox(_T("socket error"));
				break;
			}
			AfxMessageBox(_T("get into thread"));
			//int namelength;
			//getpeername(s, (LPSOCKADDR)&peeraddr, &namelength);
			peeraddr = *(SOCKADDR_IN*)szBuf;
			beconscan = 0;
			::SendMessage(*((HWND*)lparam), WM_UDPMESSAGE, 123, (LPARAM)&szBuf);
		}

			
			
	}
	return 0;
}

DWORD WINAPI tcpproc(LPVOID lparam)
{
	while (1)
	{
		if (SOCKET_ERROR == recv(tcp_s, szBuf, 30, 0))
			break;
		::SendMessage(*((HWND*)lparam), WM_TCPMESSAGE, 123, (LPARAM)&szBuf);
	}
	beconscan = 1;
	return 0;
}



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CudplearnDlg �Ի���



CudplearnDlg::CudplearnDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UDPLEARN_DIALOG, pParent)
	, got_value(0)
	, got_lparam(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CudplearnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VALUE, got_value);
	DDX_Text(pDX, IDC_EDIT1, got_lparam);
}

BEGIN_MESSAGE_MAP(CudplearnDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_VALUE, &CudplearnDlg::OnEnChangeEditValue)
	ON_BN_CLICKED(IDC_BUTTON1, &CudplearnDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CudplearnDlg::OnBnClickedButton2)
	ON_MESSAGE(WM_UDPMESSAGE, &CudplearnDlg::udpmessage)
	ON_MESSAGE(WM_TCPMESSAGE, &CudplearnDlg::tcpmessage)
END_MESSAGE_MAP()


// CudplearnDlg ��Ϣ�������

BOOL CudplearnDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CudplearnDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CudplearnDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CudplearnDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CudplearnDlg::OnEnChangeEditValue()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CudplearnDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(FALSE);
	 
}




void CudplearnDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	WSAStartup(MAKEWORD(2, 2), &wsad);
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(4677);
	bBoardcast = TRUE;
	AfxMessageBox(_T("open socket"));
	if (SOCKET_ERROR == bind(s, (LPSOCKADDR)&addr, sizeof(addr)))
	{	AfxMessageBox(_T("socket error"));

		/*if (INVALID_SOCKET != s)
		{
			
			closesocket(s);
			s = INVALID_SOCKET;
		*/
		WSACleanup();
	}
	memset(&remote, 0, sizeof(remote));
	//lparam = &m_hWnd;
	hthread = CreateThread(NULL, 0, udpproc, (LPVOID)&m_hWnd, 0, &threadid);
	
}

LRESULT CudplearnDlg::udpmessage(WPARAM wParam, LPARAM lParam)
{
	tcp_s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (tcp_s == INVALID_SOCKET)
	{
		WSACleanup();
		beconscan = 1;
		return 1;
	}
	server_addr.sin_family = peeraddr.sin_family;
	server_addr.sin_addr = peeraddr.sin_addr;
	server_addr.sin_port = peeraddr.sin_port;

	int iresult;
	iresult = connect(tcp_s, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (SOCKET_ERROR ==iresult )
	{
		WSACleanup();
		beconscan = 1;
		return 1;

	}
	CreateThread(NULL,0,tcpproc,(LPVOID)&m_hWnd,0,NULL);


}


LRESULT CudplearnDlg::tcpmessage(WPARAM wParam,LPARAM lParam)
{
	//got_value++;
	//AfxMessageBox(_T("get into tcpmessage"));
	UpdateData(FALSE);
	static MSLLHOOKSTRUCT mouseStruct;
	static KBDLLHOOKSTRUCT keyboardStruct;
	
	WPARAM button;
	button = ((Mouse*)lParam)->wparam;

	if ((button == WM_LBUTTONDOWN) || (button == WM_LBUTTONUP) || (button == WM_MOUSEMOVE) || (button == WM_MOUSEWHEEL) || (button == WM_MOUSEHWHEEL) || (button == WM_RBUTTONDOWN) || (button == WM_RBUTTONUP))
	{
		mouseStruct = ((Mouse*)lParam)->position;
		mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE,mouseStruct.pt.x*35,mouseStruct.pt.y*65,0,0);
		switch (button)
		{
		case WM_LBUTTONDOWN:
			mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
			break;
		case WM_LBUTTONUP:
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			break;
		case WM_RBUTTONDOWN:
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			break;
		case WM_RBUTTONUP:
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			break;
		default: break;

		}
		//got_value = button;
		//got_lparam = ((Mouse*)lParam)->position.pt.x;
		
	}
	if ((button == WM_KEYDOWN) || (button == WM_KEYUP) || (button == WM_SYSKEYDOWN) || (button == WM_SYSKEYUP))
	{
		keyboardStruct = ((Keyboard*)lParam)->code;
		if ((button == WM_KEYDOWN) || (button == WM_SYSKEYDOWN))
		{
			keybd_event(keyboardStruct.vkCode, keyboardStruct.scanCode,0,0);
		}
		else
		{
			keybd_event(keyboardStruct.vkCode, keyboardStruct.scanCode, KEYEVENTF_KEYUP, 0);
		}
		got_value = button;
		got_lparam = ((Keyboard*)lParam)->code.scanCode;
	} 
	return 1; 
}