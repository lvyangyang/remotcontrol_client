
// udplearn.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CudplearnApp: 
// �йش����ʵ�֣������ udplearn.cpp
//

class CudplearnApp : public CWinApp
{
public:
	CudplearnApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CudplearnApp theApp;