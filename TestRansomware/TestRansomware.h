
// TestRansomware.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CTestRansomwareApp:
// �� Ŭ������ ������ ���ؼ��� TestRansomware.cpp�� �����Ͻʽÿ�.
//

class CTestRansomwareApp : public CWinApp
{
public:
	CTestRansomwareApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CTestRansomwareApp theApp;