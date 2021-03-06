#include "../stdafx.h"
#include <windows.h>
#include <shellapi.h>

#include "win_impl_base.h"
#include "chat_dialog.h"
#include "ColorPicker.h"
#include "main_frame.h"
#include <atltime.h>
#include "Emotion.h"
#if USE(ZIP_SKIN)
static const TCHAR* const kResourceSkinZipFileName = _T("QQRes.zip");
#endif

const TCHAR* const kTitleControlName = _T("apptitle");
const TCHAR* const kCloseButtonControlName = _T("closebtn");
const TCHAR* const kMinButtonControlName = _T("minbtn");
const TCHAR* const kMaxButtonControlName = _T("maxbtn");
const TCHAR* const kRestoreButtonControlName = _T("restorebtn");

const TCHAR* const kBackgroundControlName = _T("bg");

const TCHAR* const kLogoButtonControlName = _T("logo");
const TCHAR* const kNickNameControlName = _T("nickname");
const TCHAR* const kDescriptionControlName = _T("description");

const TCHAR* const kFontButtonControlName = _T("fontbtn");
const TCHAR* const kFontbarControlName = _T("fontbar");
const TCHAR* const kFontTypeControlName = _T("font_type");
const TCHAR* const kFontSizeControlName = _T("font_size");
const TCHAR* const kBoldButtonControlName = _T("boldbtn");
const TCHAR* const kItalicButtonControlName = _T("italicbtn");
const TCHAR* const KUnderlineButtonControlName = _T("underlinebtn");
const TCHAR* const kColorButtonControlName = _T("colorbtn");

const TCHAR* const kInputRichEditControlName = _T("input_richedit");
const TCHAR* const kViewRichEditControlName = _T("view_richedit");

const TCHAR* const kEmotionButtonControlName = _T("emotionbtn");

const TCHAR* const kSendButtonControlName = _T("sendbtn");

const int kEmotionRefreshTimerId = 1001;
const int kEmotionRefreshInterval = 150;


extern FriendListItemInfo g_myself_info;
ChatDialog::ChatDialog(MainFrame * frame_wnd, const tString& bgimage, DWORD bkcolor,const FriendListItemInfo& friend_info)
: frame_wnd_(frame_wnd)
, bgimage_(bgimage)
, bkcolor_(bkcolor)
, friend_(friend_info)
, emotion_timer_start_(false)
, text_color_(0xFF000000)
, bold_(false)
, italic_(false)
, underline_(false)
, font_size_(12)
, font_face_name_(_T("微软雅黑"))
, m_iFriendPort(0)
{
}

ChatDialog::~ChatDialog()
{
	//frame_wnd_->DeleteChatDlg(friend_.nick_name);
	return;
}

LPCTSTR ChatDialog::GetWindowClassName() const
{
	return _T("ChatDialog");
}

CControlUI* ChatDialog::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

void ChatDialog::OnFinalMessage(HWND hWnd)
{
	RemoveObserver();
	WindowImplBase::OnFinalMessage(hWnd);	
	delete this;
}

BOOL ChatDialog::Receive(SkinChangedParam param)
{
	bgimage_ = param.bgimage;
	bkcolor_ = param.bkcolor;
	CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
	if (background != NULL)
	{
		if (!param.bgimage.empty())
		{
			TCHAR szBuf[MAX_PATH] = {0};
#if defined(UNDER_WINCE)
			_stprintf(szBuf, _T("file='%s' corner='600,200,1,1'"), param.bgimage.c_str());
#else
			_stprintf_s(szBuf, MAX_PATH - 1, _T("file='%s' corner='600,200,1,1'"), param.bgimage.c_str());
#endif
			background->SetBkImage(szBuf);
		}
		else
			background->SetBkImage(_T(""));

		background->SetBkColor(param.bkcolor);
	}

	return TRUE;
}

tString ChatDialog::GetSkinFile()
{
	return _T("chatbox.xml");
}

tString ChatDialog::GetSkinFolder()
{
	return tString(CPaintManagerUI::GetInstancePath()) + _T("skin\\QQRes\\");
}

LRESULT ChatDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT ChatDialog::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT ChatDialog::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if (::IsZoomed(m_hWnd) != bZoomed)
	{
		if (!bZoomed)
		{
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(true);
		}
		else 
		{
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(false);
		}
	}
#else
	return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

	return 0;
}

LRESULT ChatDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT ChatDialog::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return FALSE;
}

void ChatDialog::OnTimer(TNotifyUI& msg)
{}

void ChatDialog::OnExit(TNotifyUI& msg)
{
	ShowWindow(SW_HIDE);
	//Close();
}

void ChatDialog::Init()
{

    if (frame_wnd_)
    {
		string strIp;

		//frame_wnd_->m_pDBLib->GetIpByName(friend_.nick_name,strIp,m_iFriendPort);
		m_strFriendip.Format(_T("%s"),strIp.c_str());
    }
}

void ChatDialog::OnPrepare(TNotifyUI& msg)
{
	TCHAR szBuf[MAX_PATH] = {0};

	CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
	if (background != NULL)
	{
#if defined(UNDER_WINCE)
		_stprintf(szBuf, _T("file='%s' corner='600,200,1,1'"), bgimage_.c_str());
#else
		_stprintf_s(szBuf, MAX_PATH - 1, _T("file='%s' corner='600,200,1,1'"), bgimage_.c_str());
#endif
		background->SetBkImage(szBuf);
		background->SetBkColor(bkcolor_);
	}

	CButtonUI* log_button = static_cast<CButtonUI*>(paint_manager_.FindControl(kLogoButtonControlName));
	if (log_button != NULL)
	{
#if defined(UNDER_WINCE)
		_stprintf(szBuf, _T("%s"), friend_.logo.c_str());
#else
		_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), friend_.logo);
#endif
		log_button->SetNormalImage(szBuf);
	}

	CControlUI* nick_name = paint_manager_.FindControl(kNickNameControlName);
	if (nick_name != NULL)
		nick_name->SetText(friend_.nick_name);

	CControlUI* desciption = paint_manager_.FindControl(kDescriptionControlName);
	if (desciption != NULL)
		desciption->SetText((LPCTSTR)friend_.description);

	CContainerUI* pFontbar = static_cast<CContainerUI*>(paint_manager_.FindControl(kFontbarControlName));
	if (pFontbar != NULL)
		pFontbar->SetVisible(!pFontbar->IsVisible());
}

tString ChatDialog::GetCurrentTimeString()
{
	SYSTEMTIME time = {0};
	TCHAR szTime[MAX_PATH] = {0};
	::GetLocalTime( &time );
#if defined(UNDER_CE)
	_stprintf( szTime, _T("%04d年%02d月%02d日 %02d:%02d:%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
#else
	_stprintf_s( szTime, MAX_PATH, _T("%04d年%02d月%02d日 %02d:%02d:%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
#endif

	return szTime;
}
//name 显示的名字，sText内容
int ChatDialog::SendMsg(CDuiString name,CDuiString sText)
{
  
    CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kViewRichEditControlName));
    if( pRichEdit == NULL ) 
	{
		DbgPrint("can not find kViewRichEditControlName\n");
		return -1;
	}
	CDuiString strFolder=CPaintManagerUI::GetInstancePath();
    long lSelBegin = 0, lSelEnd = 0;
    CHARFORMAT2 cf;
    ZeroMemory(&cf, sizeof(CHARFORMAT2));
    cf.cbSize = sizeof(cf);
    cf.dwReserved = 0;
    cf.dwMask = CFM_COLOR | CFM_LINK | CFM_UNDERLINE | CFM_UNDERLINETYPE;
    cf.dwEffects = CFE_LINK;
    cf.bUnderlineType = CFU_UNDERLINE;
    cf.crTextColor = RGB(220, 0, 0);

    lSelEnd = lSelBegin = pRichEdit->GetTextLength();
    pRichEdit->SetSel(lSelEnd, lSelEnd);
    pRichEdit->ReplaceSel((LPCTSTR)name, false);

    lSelEnd = pRichEdit->GetTextLength();
    pRichEdit->SetSel(lSelBegin, lSelEnd);
    pRichEdit->SetSelectionCharFormat(cf);

    lSelEnd = lSelBegin = pRichEdit->GetTextLength();
    pRichEdit->SetSel(lSelEnd, lSelEnd);
	CTime time = CTime::GetCurrentTime();
	CDuiString sendTime = time.Format(_T("%H:%M:%S"));
	CDuiString sendtimestr;
	sendtimestr.Format(_T("说:\t%s\n"),sendTime);
	pRichEdit->ReplaceSel(sendtimestr, false);
    //pRichEdit->ReplaceSel(_T("说:\t2011-07-30 08:30:12\n"), false);
    cf.dwMask = CFM_COLOR;
    cf.crTextColor = RGB(0, 0, 0);
    cf.dwEffects = 0;
    lSelEnd = pRichEdit->GetTextLength();
    pRichEdit->SetSel(lSelBegin, lSelEnd);
    pRichEdit->SetSelectionCharFormat(cf);

    PARAFORMAT2 pf;
    ZeroMemory(&pf, sizeof(PARAFORMAT2));
    pf.cbSize = sizeof(pf);
    pf.dwMask = PFM_STARTINDENT;
    pf.dxStartIndent = 0;
    pRichEdit->SetParaFormat(pf);
    lSelEnd = lSelBegin = pRichEdit->GetTextLength();
	//插入字符串，需转移表情
    //pRichEdit->SetSel(-1, -1);
    //pRichEdit->ReplaceSel(sText.GetData(), false);
	pRichEdit->SetSel(-1,-1);
	int m,begin;
	begin=0;
	CDuiString temp;
	char buff[2];
	while(1)
	{
		m=sText.Find(_T("#"),begin);
		if(-1!=m)
		{

			temp=sText.Mid(begin,m-begin);
			pRichEdit->ReplaceSel(temp.GetData(),false);
            int n=sText.Find(_T(".gif"),m);
           //find .gif
			if (n!=-1)
			{
               temp = sText.Mid(m+1,n-m+3);
			   if (frame_wnd_)
			   {
				   CDuiString strfacefolder,strface;
				   strfacefolder.Format(_T("%s\\skin\\Faces\\"),strFolder.GetData());
				   strface.Format(_T("%s\\skin\\Faces\\%s"),strFolder.GetData(),temp.GetData());
				  if (0==frame_wnd_->find_gif_in_folder(temp,strfacefolder))
				  {
                //     pRichEdit->InsertGif(strface);
			
				  }
				  begin=n+4;
				  m=n+4;

			   }
			   else
			   {
				   return 0;
			   }
			}
			else
			{
				pRichEdit->ReplaceSel(temp,false);
				begin=n+1;
			}

		}
		else
		{
			temp=sText.Mid(begin);
			pRichEdit->SetSel(-1, -1);
			pRichEdit->ReplaceSel(temp,false);
			//pRichEdit->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);//滚动条自动到最后一行
			break;
		}
	}

    pRichEdit->SetSel(-1, -1);
    pRichEdit->ReplaceSel(_T("\n"), false);

    cf.crTextColor = RGB(0, 0, 0);
    lSelEnd = pRichEdit->GetTextLength();
   // pRichEdit->SetSel(lSelBegin, lSelEnd);
    pRichEdit->SetSelectionCharFormat(cf);

    ZeroMemory(&pf, sizeof(PARAFORMAT2));
    pf.cbSize = sizeof(pf);
    pf.dwMask = PFM_STARTINDENT;
    pf.dxStartIndent = 220;
    pRichEdit->SetParaFormat(pf);

    pRichEdit->EndDown();

	return 0;
}

void ChatDialog::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("windowinit")) == 0)
	{
		OnPrepare(msg);
	}
	else if (_tcsicmp(msg.sType, _T("killfocus")) == 0)
	{
	}
	else if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kCloseButtonControlName) == 0)
		{
			OnExit(msg);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMinButtonControlName) == 0)
		{

			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);

		}
		else if (_tcsicmp(msg.pSender->GetName(), kMaxButtonControlName) == 0)
		{

			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

		}
		else if (_tcsicmp(msg.pSender->GetName(), kRestoreButtonControlName) == 0)
		{

			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);

		}
		else if (_tcsicmp(msg.pSender->GetName(), kFontButtonControlName) == 0)
		{
			CContainerUI* pFontbar = static_cast<CContainerUI*>(paint_manager_.FindControl(kFontbarControlName));
			if (pFontbar != NULL)
				pFontbar->SetVisible(!pFontbar->IsVisible());
		}
		else if (_tcsicmp(msg.pSender->GetName(), kEmotionButtonControlName) == 0)
		{
			POINT pt = {0};
			CDuiRect rcEmotionBtn = msg.pSender->GetPos();
			CDuiRect rcWindow;
			GetWindowRect(m_hWnd, &rcWindow);

			pt.y = rcWindow.top + rcEmotionBtn.top;
			pt.x = rcWindow.left + rcEmotionBtn.left;
			//emotion_list_window_.SelectEmotion(pt);
			new CEmotion(this,pt);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kSendButtonControlName) == 0)
        {
			CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kInputRichEditControlName));
			CRichEditUI* pViewEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kViewRichEditControlName));
			if( pRichEdit == NULL ) return;
			pRichEdit->SetFocus();
			CDuiString sText = pRichEdit->GetTextRange(0, pRichEdit->GetTextLength());
			//CDuiString sText=pRichEdit->GetEditGifStr();
		    if( sText.IsEmpty() ) 
				return;

			int textlen = sText.GetLength();

			if (textlen > 512)
			{  
				pViewEdit->SetTextColor(RGB(255,0,0));
				pViewEdit->SetText(_T("错误！发送数据的长度不能超过512字节。")); 
			}
			else
			{
				int imNum = friend_.id;
				TCHAR sendmesg[513] = {0};


				_stprintf_s(sendmesg,512,_T("%s"),sText.GetData());
				int len = strlen(sendmesg);

				if (len !=textlen)
				{
					pViewEdit->SetTextColor(RGB(255,0,0));
					pViewEdit->SetText(_T("警告！发送数据的长度和计算的不同。")); 
				}
				frame_wnd_->m_pTcpCommunication->SendMsg(MSG_TYPE_FRIEND,imNum,sendmesg,textlen);
				//显示在
				SendMsg(g_myself_info.nick_name,sText);
				pRichEdit->SetText(_T(""));
                //test
				//CDuiString strbmp;
				//strbmp.Format("c:\\1.gif");
				//pRichEdit->InsertGif(strbmp);
			}

		}
	}
    else if( _tcsicmp(msg.sType, _T("return")) == 0 ) 
    {
        if (_tcsicmp(msg.pSender->GetName(), kInputRichEditControlName) == 0)
        {
			CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kInputRichEditControlName));
			CRichEditUI* pViewEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kViewRichEditControlName));
			if( pRichEdit == NULL ) return;
			pRichEdit->SetFocus();
			CDuiString sText = pRichEdit->GetTextRange(0, pRichEdit->GetTextLength());
			if( sText.IsEmpty() ) 
				return;

			int textlen = sText.GetLength();

			if (textlen > 512)
			{  
				pViewEdit->SetTextColor(RGB(255,0,0));
				pViewEdit->SetText(_T("错误！发送数据的长度不能超过512字节。")); 
			}
			else
			{
				int imNum = friend_.id;
				TCHAR sendmesg[513] = {0};
                 
				
				_stprintf_s(sendmesg,512,_T("%s"),sText.GetData());
                int len = strlen(sendmesg);

				if (len !=textlen)
				{
					pViewEdit->SetTextColor(RGB(255,0,0));
					pViewEdit->SetText(_T("警告！发送数据的长度和计算的不同。")); 
				}
				frame_wnd_->m_pTcpCommunication->SendMsg(MSG_TYPE_FRIEND,imNum,sendmesg,textlen);
				//显示在
				SendMsg(g_myself_info.nick_name,sText);
				pRichEdit->SetText(_T(""));
			}
        }
    }
	else if (_tcsicmp(msg.sType, _T("timer")) == 0)
	{
		return OnTimer(msg);
	}
	else if (_tcsicmp(msg.sType, _T("selectchanged")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kColorButtonControlName) == 0)
		{
			CContainerUI* pFontbar = static_cast<CContainerUI*>(paint_manager_.FindControl(kFontbarControlName));
			if (pFontbar != NULL)
			{
				POINT pt = {0};
				CDuiRect rcFontbar = pFontbar->GetPos();
				CDuiRect rcColorBtn = msg.pSender->GetPos();
				CDuiRect rcWindow;
				GetWindowRect(m_hWnd, &rcWindow);

				pt.y = rcWindow.top + rcFontbar.top;
				pt.x = rcWindow.left + rcColorBtn.left + static_cast<LONG>(rcColorBtn.right - rcColorBtn.left / 2);
				
			     new CColorPicker((ChatDialog*)this, pt);
			
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kBoldButtonControlName) == 0)
		{
			COptionUI* bold_button = static_cast<COptionUI*>(msg.pSender);
			if (bold_button != NULL)
			{
				bold_ = bold_button->IsSelected();
				FontStyleChanged();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kItalicButtonControlName) == 0)
		{
			COptionUI* italic_button = static_cast<COptionUI*>(msg.pSender);
			if (italic_button != NULL)
			{
				italic_ = italic_button->IsSelected();
				FontStyleChanged();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), KUnderlineButtonControlName) == 0)
		{
			COptionUI* underline_button = static_cast<COptionUI*>(msg.pSender);
			if (underline_button != NULL)
			{
				underline_ = underline_button->IsSelected();
				FontStyleChanged();
			}
		}
	}
	else if (_tcsicmp(msg.sType, _T("itemselect")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kFontTypeControlName) == 0)
		{
			CComboUI* font_type = static_cast<CComboUI*>(msg.pSender);
			if (font_type != NULL)
			{
				font_face_name_ = font_type->GetText();
				FontStyleChanged();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kFontSizeControlName) == 0)
		{
			CComboUI* font_size = static_cast<CComboUI*>(msg.pSender);
			if (font_size != NULL)
			{
				font_size_ = _ttoi(font_size->GetText());
				FontStyleChanged();
			}
		}
	}
}

void ChatDialog::SetTextColor(DWORD dwColor)
{
	COptionUI* color_button = static_cast<COptionUI*>(paint_manager_.FindControl(kColorButtonControlName));
	if (color_button != NULL)
	{
		color_button->Selected(false);
		if (dwColor != 0)
		{
			text_color_ = dwColor;
			FontStyleChanged();
		}
	}
}

LRESULT ChatDialog::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

void ChatDialog::FontStyleChanged()
{}
int ChatDialog::SetInputEditImg(CDuiString strimg)
{
	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kInputRichEditControlName));
	if( pRichEdit == NULL ) 
		return -1;
	pRichEdit->SetFocus();
	//pRichEdit->InsertGif(strimg);
	//pRichEdit->SetBkColor()
	return 0;
}