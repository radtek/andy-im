//
// win_impl_base.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 achellies (achellies at 163 dot com), wangchyz (wangchyz at gmail dot com)
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#include "stdafx.h"
#include "mainui/win_impl_base.h"
#include "mainui/main_frame.h"
#include "mainui/UILogin.h"

#include <atlbase.h>
CComModule _Module;
#include <atlwin.h>

#if defined(WIN32) && !defined(UNDER_CE)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int nCmdShow)
#endif
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

	//HINSTANCE hInstRich = ::LoadLibrary(_T("Riched20.dll"));

	//::CoInitialize(NULL);
	::OleInitialize(NULL);



	_Module.Init( 0, hInstance );

	WSADATA wsaData;
	WORD wVersionRequested;
	/* Ask for Winsock 2.2 functionality */
	wVersionRequested = MAKEWORD( 2, 2 );
	int sta = WSAStartup( wVersionRequested, &wsaData );

#if defined(WIN32) && !defined(UNDER_CE)
	HRESULT Hr = ::CoInitialize(NULL);
#else
	HRESULT Hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif
	if( FAILED(Hr) ) return 0;
	//CDuiString strrgv;
	//strrgv.Format(_T("regSvr32 /s %s\\DLL\\GifOle.dll"),CPaintManagerUI::GetInstancePath());

	//WinExec(strrgv,SW_HIDE);
	//WinExec("regsvr32 /DLL/GifOle.dll",SW_SHOW);
	int err = GetLastError();

	MainFrame* pFrame = new MainFrame();
	if( pFrame == NULL ) return 0;
#if defined(WIN32) && !defined(UNDER_CE)
	pFrame->Create(NULL, _T("IM2001"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);
#else
	pFrame->Create(NULL, _T("QQ2011"), UI_WNDSTYLE_FRAME, WS_EX_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#endif
	pFrame->CenterWindow();
   // UILogin *m_plogin = new UILogin(pFrame);

  //  ::ShowWindow(*pFrame, SW_HIDE);

	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();

	_Module.Term();

	WindowImplBase::Cleanup();

	::OleUninitialize();
	::CoUninitialize();

	//::FreeLibrary(hInstRich);

	return 0;
}