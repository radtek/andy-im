#include "../stdafx.h"
#include <windows.h>
#include <shellapi.h>

#include "win_impl_base.h"
#include "chat_dialog.h"
#include "ColorPicker.h"
#include "main_frame.h"
#include <atltime.h>

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

ChatDialog::ChatDialog(MainFrame * frame_wnd, const tString& bgimage, DWORD bkcolor, const FriendListItemInfo& myselft_info, const FriendListItemInfo& friend_info)
: frame_wnd_(frame_wnd)
, bgimage_(bgimage)
, bkcolor_(bkcolor)
, myselft_(myselft_info)
, friend_(friend_info)
, emotion_timer_start_(false)
, text_color_(0xFF000000)
, bold_(false)
, italic_(false)
, underline_(false)
, font_size_(12)
, font_face_name_(_T("΢���ź�"))
, m_iFriendPort(0)
{}

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
		m_strFriendip.Format("%s",strIp.c_str());
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
	_stprintf( szTime, _T("%04d��%02d��%02d�� %02d:%02d:%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
#else
	_stprintf_s( szTime, MAX_PATH, _T("%04d��%02d��%02d�� %02d:%02d:%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
#endif

	return szTime;
}
//name ��ʾ�����֣�sText����
void ChatDialog::SendMsg(CStdString name,CStdString sText)
{
  
    CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kViewRichEditControlName));
    if( pRichEdit == NULL ) return;
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
	CStdString sendTime = time.Format("%H:%M:%S");
	CStdString sendtimestr;
	sendtimestr.Format(_T("˵:\t%s\n"),sendTime);
	pRichEdit->ReplaceSel(sendtimestr, false);
    //pRichEdit->ReplaceSel(_T("˵:\t2011-07-30 08:30:12\n"), false);
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

    pRichEdit->SetSel(-1, -1);
    pRichEdit->ReplaceSel(sText.GetData(), false);

    pRichEdit->SetSel(-1, -1);
    pRichEdit->ReplaceSel(_T("\n"), false);

    cf.crTextColor = RGB(0, 0, 0);
    lSelEnd = pRichEdit->GetTextLength();
    pRichEdit->SetSel(lSelBegin, lSelEnd);
    pRichEdit->SetSelectionCharFormat(cf);

    ZeroMemory(&pf, sizeof(PARAFORMAT2));
    pf.cbSize = sizeof(pf);
    pf.dwMask = PFM_STARTINDENT;
    pf.dxStartIndent = 220;
    pRichEdit->SetParaFormat(pf);

    pRichEdit->EndDown();
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
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_MINIMIZE);
#else
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMaxButtonControlName) == 0)
		{
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(true);
#else
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kRestoreButtonControlName) == 0)
		{
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_RESTORE);
			CControlUI* pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(paint_manager_.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(false);
#else
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
#endif
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
			CRect rcEmotionBtn = msg.pSender->GetPos();
			CRect rcWindow;
			GetWindowRect(m_hWnd, &rcWindow);

			pt.y = rcWindow.top + rcEmotionBtn.top;
			pt.x = rcWindow.left + rcEmotionBtn.left;
			//emotion_list_window_.SelectEmotion(pt);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kSendButtonControlName) == 0)
        {
			CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kInputRichEditControlName));
			CRichEditUI* pViewEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kViewRichEditControlName));
			if( pRichEdit == NULL ) return;
			pRichEdit->SetFocus();
			CStdString sText = pRichEdit->GetTextRange(0, pRichEdit->GetTextLength());
			if( sText.IsEmpty() ) 
				return;

			int textlen = sText.GetLength();

			if (textlen > 512)
			{  
				pViewEdit->SetTextColor(RGB(255,0,0,));
				pViewEdit->SetText("���󣡷������ݵĳ��Ȳ��ܳ���512�ֽڡ�"); 
			}
			else
			{
				int imNum = friend_.id;
				char sendmesg[513] = {0};


				sprintf_s(sendmesg,512,"%s",sText.GetData());
				int len = strlen(sendmesg);

				if (len !=textlen)
				{
					pViewEdit->SetTextColor(RGB(255,0,0,));
					pViewEdit->SetText("���棡�������ݵĳ��Ⱥͼ���Ĳ�ͬ��"); 
				}
				frame_wnd_->m_pTcpCommunication->SendMsg(imNum,sendmesg,textlen);
				//��ʾ��
				SendMsg(myselft_.nick_name,sText);
				pRichEdit->SetText(_T(""));
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
			CStdString sText = pRichEdit->GetTextRange(0, pRichEdit->GetTextLength());
			if( sText.IsEmpty() ) 
				return;

			int textlen = sText.GetLength();

			if (textlen > 512)
			{  
				pViewEdit->SetTextColor(RGB(255,0,0,));
				pViewEdit->SetText("���󣡷������ݵĳ��Ȳ��ܳ���512�ֽڡ�"); 
			}
			else
			{
				int imNum = friend_.id;
				char sendmesg[513] = {0};
                 
				
				sprintf_s(sendmesg,512,"%s",sText.GetData());
                int len = strlen(sendmesg);

				if (len !=textlen)
				{
					pViewEdit->SetTextColor(RGB(255,0,0,));
					pViewEdit->SetText("���棡�������ݵĳ��Ⱥͼ���Ĳ�ͬ��"); 
				}
				frame_wnd_->m_pTcpCommunication->SendMsg(imNum,sendmesg,textlen);
				//��ʾ��
				SendMsg(myselft_.nick_name,sText);
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
				CRect rcFontbar = pFontbar->GetPos();
				CRect rcColorBtn = msg.pSender->GetPos();
				CRect rcWindow;
				GetWindowRect(m_hWnd, &rcWindow);

				pt.y = rcWindow.top + rcFontbar.top;
				pt.x = rcWindow.left + rcColorBtn.left + static_cast<LONG>(rcColorBtn.right - rcColorBtn.left / 2);
				new CColorPicker(this, pt);
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