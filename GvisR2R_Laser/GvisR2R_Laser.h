
// GvisR2R_Laser.h : GvisR2R_Laser 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.

#include "SingleInstance.h"

// CGvisR2R_LaserApp:
// 이 클래스의 구현에 대해서는 GvisR2R_Laser.cpp을 참조하십시오.
//

class CGvisR2R_LaserApp : public CWinApp
{
	CString m_strHostAddress, m_strHostPort;

	void wsaStartup();
	void wsaEndup();

public:
	CGvisR2R_LaserApp();
	~CGvisR2R_LaserApp();

	CSingleInstance m_singleInstance;

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.
	afx_msg void OnAppAbout();

	CString GetHostAddress();
	CString GetHostPort();
	void SetHostAddress(CString sAddr);
	void SetHostPort(CString sPort);


	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CGvisR2R_LaserApp theApp;
