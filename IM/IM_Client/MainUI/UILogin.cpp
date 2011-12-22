#include "../StdAfx.h"
#include "UILogin.h"
#include "main_frame.h"

extern int g_ClientID;
UILogin::UILogin(MainFrame * frame_wnd)
:frame_wnd_(frame_wnd)
,image_index(1)
,m_loginState(-1)
{
	Create(NULL, _T("用户登录"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0);
	ShowWindow(true);


}

UILogin::~UILogin(void)
{

}

LPCTSTR UILogin::GetWindowClassName() const 
{ 
	return _T("UiLoginWindow");
}

void UILogin::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}
void UILogin::Init()
{
	SIZE size = paint_manager_.GetInitSize();
	this->CenterWindow();
	//MoveWindow(m_hWnd, parent_window_rect_.left, parent_window_rect_.top,  parent_window_rect_.right-parent_window_rect_.left, parent_window_rect_.bottom-parent_window_rect_.top, FALSE);

}
void UILogin::OnPrepare(TNotifyUI& msg)
{
	 CComboUI* pAccountCombo = static_cast<CComboUI*>(paint_manager_.FindControl(_T("accountcombo")));
	 CControlUI * plist  = new CListLabelElementUI();
	 plist->SetText(_T("666666"));
	 pAccountCombo->AddAt(plist,2);
     CControlUI * plist1  = new CListLabelElementUI();
	 plist1->SetText(_T("333333"));
	 pAccountCombo->AddAt(plist1,3);

	app_loadgif =static_cast<CControlUI*>(paint_manager_.FindControl(_T("LabelUI1")));
	if (app_loadgif)
	{
		//app_loadgif->SetBkImage("connect1.bmp");
		//paint_manager_.SetTimer(app_loadgif,1,3000);
	}


}
int UILogin::login()
{
	CControlUI *pedituser = static_cast<CControlUI*>(paint_manager_.FindControl(_T("accountedit")));
	CControlUI *ppsw = static_cast<CControlUI*>(paint_manager_.FindControl(_T("pwdedit")));
	CLabelUI *plable = static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LabelUI4")));
	CControlUI *pbton = static_cast<CControlUI*>(paint_manager_.FindControl(_T("loginBtn")));
	CStdString strUser,strPsw;
	if (pedituser && ppsw)
	{
		plable->SetText(_T("正在登录，请稍等madie..."));
	    strUser = pedituser->GetText();
		strPsw = ppsw->GetText();
		if (strUser.IsEmpty() || strPsw.IsEmpty())
		{
		    plable->SetText(_T("NND,快输入账号、密码。"));
			pbton->SetEnabled(TRUE);
			return -1;
		}
   
		g_ClientID = atoi(strUser.GetData());

		frame_wnd_->InitTcp();
		if (FALSE == frame_wnd_->m_pTcpCommunication->Initialize("10.96.20.12", 6666, 1, 10000))
		{
              	plable->SetText(_T("无法连接服务器,坑爹啊,快叫曹子伟开服务器。"));

				frame_wnd_->ReleaseTCP();
				pbton->SetEnabled(TRUE);
				return -1;


		}
		else
		{
		
			TCHAR longinbuf[100]={0};
			_stprintf(longinbuf,_T("USER=%s PSW=%s"),strUser.GetData(),strPsw.GetData());
			int len = strlen(longinbuf);
			//发送登录消息
			plable->SetText(_T("正在登录，请稍等。"));
			frame_wnd_->m_pTcpCommunication->SendBuffer(CMD_LOGIN,longinbuf,len);



		}

	}
	pbton->SetEnabled(TRUE);
	return -1;
}
void UILogin::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("windowinit") ) 
		OnPrepare(msg);
	else if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		if (msg.pSender->GetName()==_T("closebtn"))
		{
				PostQuitMessage(0); 
				return; 
			
		}

		else if(msg.pSender->GetName() == _T("loginBtn"))
		{

			CControlUI *pbton = static_cast<CControlUI*>(paint_manager_.FindControl(_T("loginBtn")));

			pbton->SetEnabled(FALSE);
			DWORD dwThreadID = 0;
			//登录线程
			m_threadHandle= CreateThread(NULL,0,thrad_login, (LPVOID)this,  0,&dwThreadID);
             //监听线程15还没登录则失败处理
		    m_threadListen = CreateThread(NULL,0,thread_listen, (LPVOID)this,  0,&dwThreadID);

		}

	}
	else if( msg.sType == _T("itemselect") ) {
		if( msg.pSender->GetName() == _T("accountcombo") ) {
			CEditUI* pAccountEdit = static_cast<CEditUI*>(paint_manager_.FindControl(_T("accountedit")));
			if( pAccountEdit ) pAccountEdit->SetText(msg.pSender->GetText());
		}
	}
	else if (_tcsicmp(msg.sType, _T("timer")) == 0)
		return OnTimer(msg);
}

tString UILogin::GetSkinFile()
{
	return _T("login.xml");
}

tString UILogin::GetSkinFolder()
{
	return tString(CPaintManagerUI::GetInstancePath()) + _T("skin\\QQRes");
}

LRESULT UILogin::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//Close();
	//CControlUI* app_load = paint_manager_.FindControl(_T("LabelUI3"));
	return 0;
}
void UILogin::OnTimer(TNotifyUI& msg)
{
	switch (msg.wParam)
	{
	case 1:
		{

			++image_index;

			if (image_index > 5)
				image_index = 1;
			if (app_loadgif !=  NULL)
			{
				TCHAR szBuf[MAX_PATH] = {0};

				_stprintf_s(szBuf, MAX_PATH - 1, _T("connect%d.bmp"), image_index);

				tString logo_image = app_loadgif->GetBkImage();
				if (_tcsicmp(logo_image.c_str(), szBuf) != 0)
				{
					app_loadgif->SetBkImage(szBuf);
					app_loadgif->Invalidate();

				}
			}
		}
	}
}
//获取用户信息
int UILogin::SetLogin(int status)
{
	CLabelUI *plable = static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LabelUI4")));
	if (status == 1)
	{
		m_loginState=1;
		//获取用户信息为2长度4个字节
		frame_wnd_->m_pTcpCommunication->SendBuffer(CMD_GET_USER,_T("2"),1);
		Sleep(500);
       //获取群信息
		frame_wnd_->m_pTcpCommunication->SendBuffer(CMD_GET_GROUP,_T("2"),1);
        //获取好友信息
		//登录成功
		ShowWindow(SW_HIDE);

	}
	else
	{
       plable->SetText(_T("登录失败，用户名或者密码错误"));
	}
	return 0;
}
void UILogin::SetLoginInfo(LPCTSTR strinfo)
{
	CLabelUI *plable = static_cast<CLabelUI*>(paint_manager_.FindControl(_T("LabelUI4")));
	if (plable)
	{
		
		plable->SetText(strinfo);
	}
}
DWORD WINAPI UILogin::thrad_login(LPVOID lpParameter)
{
	try
	{

		UILogin &plogin = *(UILogin*)lpParameter;
		if (plogin)
		{

			plogin.login();

		}
	}
	catch(...)
	{

	}
	return 0;
}
DWORD UILogin::thread_listen(LPVOID lpParameter)
{
	try
	{

		UILogin &plogin = *(UILogin*)lpParameter;
		
			for(int i = 0; i < 15; i++)
			{
				if (plogin)
				{
                    if (plogin.m_loginState ==1)
                    {
						return 0;
                    }
				}
				Sleep(1000);

			}
			if (plogin)
			{
				plogin.SetLoginInfo("登录超时！");
			}
			//plogin.login();

	}
	catch(...)
	{

	}
	return 0;
}
