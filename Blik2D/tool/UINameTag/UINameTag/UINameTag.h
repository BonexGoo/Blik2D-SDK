
// UINameTag.h : UINameTag ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CUINameTagApp:
// �� Ŭ������ ������ ���ؼ��� UINameTag.cpp�� �����Ͻʽÿ�.
//

class CUINameTagApp : public CWinApp
{
public:
	CUINameTagApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CUINameTagApp theApp;
