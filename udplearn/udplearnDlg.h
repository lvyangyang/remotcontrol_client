
// udplearnDlg.h : 头文件
//

#pragma once
#define WM_UDPMESSAGE WM_USER + 106
#define WM_TCPMESSAGE WM_USER+107

// CudplearnDlg 对话框
class CudplearnDlg : public CDialogEx
{
// 构造
public:
	
	CudplearnDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UDPLEARN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT udpmessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT tcpmessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	double got_value;
	afx_msg void OnEnChangeEditValue();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	double got_lparam;
};
