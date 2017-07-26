
// udplearnDlg.cpp : 实现文件
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



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CudplearnDlg 对话框



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


// CudplearnDlg 消息处理程序

BOOL CudplearnDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CudplearnDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CudplearnDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CudplearnDlg::OnEnChangeEditValue()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CudplearnDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(FALSE);
	 
}




void CudplearnDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
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