#include "../stdafx.h"
#include <windows.h>
#if !defined(UNDER_CE)
#include <shellapi.h>
#endif

#include "win_impl_base.h"
#include "main_frame.h"
#include "Emotion.h"
#include "Emotionlist.h"

Emotionlist::Emotionlist(CEmotion * emotion_wnd,RECT rcparentWindow)
:emotion_wnd_(emotion_wnd)
,parent_window_rect_(rcparentWindow)
{
	Create(emotion_wnd_->GetHWND(), _T("Emotionlist"), WS_CHILD, WS_EX_TOOLWINDOW, 0, 0);
	ShowWindow(true);
}

Emotionlist::~Emotionlist(void)
{
}
LPCTSTR Emotionlist::GetWindowClassName() const 
{ 
	return _T("Emotionlist");
}

void Emotionlist::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void Emotionlist::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("windowinit") ) 
		OnPrepare(msg);
	else if (_tcsicmp(msg.sType, _T("click")) == 0)
	{

		if(msg.pSender->GetName() == _T("ButtonOK"))
		{





		}




	}
	//else if (_tcsicmp(msg.sType, _T("timer")) == 0)
		//return OnTimer(msg);
}

void Emotionlist::Init()
{
	SIZE size = paint_manager_.GetInitSize();
	MoveWindow(m_hWnd, parent_window_rect_.left, parent_window_rect_.top,  parent_window_rect_.right-parent_window_rect_.left, parent_window_rect_.bottom-parent_window_rect_.top, FALSE);

}
void Emotionlist::OnPrepare(TNotifyUI& msg)
{

// 	app_gif =static_cast<CControlUI*>(paint_manager_.FindControl(_T("LabelUI1")));
// 	if (app_gif)
// 	{
// 		app_gif->SetBkImage("connect1.bmp");
// 		paint_manager_.SetTimer(app_gif,1,3000);
// 	}
	//app_show =static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LabeSHowInfo")));
	//btn_ok =static_cast<CButtonUI*>(paint_manager_.FindControl(_T("ButtonOK")));




}
tString Emotionlist::GetSkinFile()
{
	return _T("emotion.xml");
}

tString Emotionlist::GetSkinFolder()
{
	return tString(CPaintManagerUI::GetInstancePath()) + _T("skin\\QQRes\\");
}

LRESULT Emotionlist::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//Close();
	//CControlUI* app_load = paint_manager_.FindControl(_T("LabelUI3"));
	return 0;
}