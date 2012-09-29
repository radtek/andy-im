#pragma once
#include "win_impl_base.h"
//#include "../DBInterface/DBInterface.h"
#include "../AndyLib/Andylib.h"
class MainFrame;
class UILogin :
	public WindowImplBase
{
public:
	UILogin(MainFrame * frame_wnd);
	virtual ~UILogin(void);

	LPCTSTR GetWindowClassName() const;

	virtual void OnFinalMessage(HWND hWnd);

	void Notify(TNotifyUI& msg);

	void Init();

	virtual tString GetSkinFile();

	virtual tString GetSkinFolder();

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	void OnPrepare(TNotifyUI& msg);
public:
	void OnTimer(TNotifyUI& msg);


	int login();
	

private:
	RECT parent_window_rect_;
	MainFrame* frame_wnd_;

	CControlUI* app_loadgif;
	CLabelUI* app_show ;

	int image_index;

	HANDLE m_threadHandle;	
	HANDLE m_threadListen;
		//	发送线程
	static DWORD WINAPI thrad_login(LPVOID lpParameter);					//	发送线程
	//线程函数
	static DWORD WINAPI thread_listen(LPVOID lpParameter);


public:
	int SetLogin(int status);
	void SetLoginInfo(LPCTSTR strinfo);
	int m_loginState;

};
