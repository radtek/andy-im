#include "../stdafx.h"
#include <windows.h>
#include <shellapi.h>

#include "win_impl_base.h"
#include "chat_group_dialog.h"
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

const TCHAR* const kLogoButtonControlName = _T("logogroup");
const TCHAR* const kNickNameControlName = _T("nicknamegroup");
const TCHAR* const kDescriptionControlName = _T("descriptiongroup");

const TCHAR* const kFontButtonControlName = _T("fontbtngroup");
const TCHAR* const kFontbarControlName = _T("fontbargroup");
const TCHAR* const kFontTypeControlName = _T("font_typegroup");
const TCHAR* const kFontSizeControlName = _T("font_size");
const TCHAR* const kBoldButtonControlName = _T("boldbtngroup");
const TCHAR* const kItalicButtonControlName = _T("italicbtngroup");
const TCHAR* const KUnderlineButtonControlName = _T("underlinebtngroup");
const TCHAR* const kColorButtonControlName = _T("colorbtngroup");

const TCHAR* const kInputRichEditControlName = _T("input_richedit");
const TCHAR* const kViewRichEditControlName = _T("view_richedit");

const TCHAR* const kEmotionButtonControlName = _T("emotionbtngroup");

const TCHAR* const kSendButtonControlName = _T("sendbtn");

const TCHAR* const kGroupsListControlName = _T("groups");

const int kEmotionRefreshTimerId = 1001;
const int kEmotionRefreshInterval = 150;


extern FriendListItemInfo g_myself_info;

ChatGroupDialog::ChatGroupDialog(MainFrame * frame_wnd, const tString& bgimage, DWORD bkcolor,const GroupsListItemInfo& group_info)
: frame_wnd_(frame_wnd)
, bgimage_(bgimage)
, bkcolor_(bkcolor)
, group_info_(group_info)
, emotion_timer_start_(false)
, text_color_(0xFF000000)
, bold_(false)
, italic_(false)
, underline_(false)
, font_size_(12)
, font_face_name_(_T("微软雅黑"))
{

}

ChatGroupDialog::~ChatGroupDialog()
{
	//frame_wnd_->DeleteChatDlg(friend_.nick_name);
	return;
}

LPCTSTR ChatGroupDialog::GetWindowClassName() const
{
	return _T("ChatGroupDialog");
}

CControlUI* ChatGroupDialog::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("FriendList")) == 0)
	{
		return new CGroupsUI(paint_manager_);
	}
	return NULL;
}

void ChatGroupDialog::OnFinalMessage(HWND hWnd)
{
	RemoveObserver();
	WindowImplBase::OnFinalMessage(hWnd);	
	delete this;
}

BOOL ChatGroupDialog::Receive(SkinChangedParam param)
{
	bgimage_ = param.bgimage;
	bkcolor_ = param.bkcolor;
	CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
	if (background != NULL)
	{
		if (!param.bgimage.empty())
		{
			TCHAR szBuf[MAX_PATH] = {0};
			_stprintf_s(szBuf, MAX_PATH - 1, _T("file='%s' corner='600,200,1,1'"), param.bgimage.c_str());
			background->SetBkImage(szBuf);
		}
		else
			background->SetBkImage(_T(""));

		background->SetBkColor(param.bkcolor);
	}

	return TRUE;
}

tString ChatGroupDialog::GetSkinFile()
{
	return _T("chatbox_group.xml");
}

tString ChatGroupDialog::GetSkinFolder()
{
	return tString(CPaintManagerUI::GetInstancePath()) + _T("skin\\QQRes\\");
}

LRESULT ChatGroupDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT ChatGroupDialog::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT ChatGroupDialog::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT ChatGroupDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT ChatGroupDialog::ResponseDefaultKeyEvent(WPARAM wParam)
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

void ChatGroupDialog::OnTimer(TNotifyUI& msg)
{}

void ChatGroupDialog::OnExit(TNotifyUI& msg)
{
	ShowWindow(SW_HIDE);
	//Close();
}

void ChatGroupDialog::Init()
{

	CGroupsUI* pGroupList = static_cast<CGroupsUI*>(paint_manager_.FindControl(kGroupsListControlName));
	if (pGroupList != NULL)
	{
	
		if (pGroupList->GetCount() > 0)
			pGroupList->RemoveAll();
        pGroupList->SetFixHight(25);

		//GroupsListItemInfo item;


		///////test use/////////////
		////添加好友
		//item.id = 676;
		//item.folder = false;
		//sprintf(item.logo,"%s",_T("default.png"));
		//sprintf(item.nick_name,"%s",_T("lay"));
		//sprintf(item.description,"%s",_T("lay@hotmail.com"));


		//pGroupList->AddNode(item);

	}
}
void ChatGroupDialog::UpdateGroupList()
{
	CGroupsUI* pGroupList = static_cast<CGroupsUI*>(paint_manager_.FindControl(kGroupsListControlName));
	if (pGroupList != NULL)
	{

		if (pGroupList->GetCount() > 0)
			pGroupList->RemoveAll();
		pGroupList->SetFixHight(25);

		int size = m_vec_group.size();
		if(size > 0)
		{
			//首先构造在线的节点
			for (int i = 0; i<size;i++)
			{			
				//在线
				if(m_vec_group[i].type == 1 )
					pGroupList->AddNode(m_vec_group[i]);

			}
			//然后不在线节点
			for (int i = 0; i<size;i++)
			{			
				//在线
				if(m_vec_group[i].type == 0 && m_vec_group[i].folder ==false)
					pGroupList->AddNode(m_vec_group[i]);

			}

		}
	}
}
void ChatGroupDialog::AddGroupItem(const GroupsListItemInfo listitem)
{
   
	m_vec_group.push_back(listitem);
	UpdateGroupList();

}
void ChatGroupDialog::OnPrepare(TNotifyUI& msg)
{
	TCHAR szBuf[MAX_PATH] = {0};

	CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
	if (background != NULL)
	{

		_stprintf_s(szBuf, MAX_PATH - 1, _T("file='%s' corner='600,200,1,1'"), bgimage_.c_str());
		background->SetBkImage(szBuf);
		background->SetBkColor(bkcolor_);
	}

	CButtonUI* log_button = static_cast<CButtonUI*>(paint_manager_.FindControl(kLogoButtonControlName));
	if (log_button != NULL)
	{

		_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), group_info_.logo);
		log_button->SetNormalImage(szBuf);
	}

	CControlUI* nick_name = paint_manager_.FindControl(kNickNameControlName);
	if (nick_name != NULL)
		nick_name->SetText(group_info_.nick_name);

	CControlUI* desciption = paint_manager_.FindControl(kDescriptionControlName);
	if (desciption != NULL)
		desciption->SetText((LPCTSTR)group_info_.description);

	CContainerUI* pFontbar = static_cast<CContainerUI*>(paint_manager_.FindControl(kFontbarControlName));
	if (pFontbar != NULL)
		pFontbar->SetVisible(!pFontbar->IsVisible());
}

tString ChatGroupDialog::GetCurrentTimeString()
{
	SYSTEMTIME time = {0};
	TCHAR szTime[MAX_PATH] = {0};
	::GetLocalTime( &time );
	_stprintf_s( szTime, MAX_PATH, _T("%04d年%02d月%02d日 %02d:%02d:%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	return szTime;
}
//name 显示的名字，sText内容
int ChatGroupDialog::SendMsg(CStdString name,CStdString sText)
{

	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kViewRichEditControlName));
	if( pRichEdit == NULL ) 
	{
		DbgPrint("can not find kViewRichEditControlName\n");
		return -1;
	}
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
	CStdString sendTime = time.Format(_T("%H:%M:%S"));
	CStdString sendtimestr;
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

	return 0;
}

void ChatGroupDialog::Notify(TNotifyUI& msg)
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
			CRect rcEmotionBtn = msg.pSender->GetPos();
			CRect rcWindow;
			GetWindowRect(m_hWnd, &rcWindow);

			pt.y = rcWindow.top + rcEmotionBtn.top;
			pt.x = rcWindow.left + rcEmotionBtn.left;
			//emotion_list_window_.SelectEmotion(pt);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kSendButtonControlName) == 0)
		{
			/*
			CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kInputRichEditControlName));
			if( pRichEdit == NULL ) return;
			pRichEdit->SetFocus();
			CStdString sText = pRichEdit->GetTextRange(0, pRichEdit->GetTextLength());
			if( sText.IsEmpty() ) return;
			pRichEdit->SetText(_T(""));
			//显示在
			SendMsg(group_info_.nick_name,sText);
			//int imNum = friend_.id;
			//frame_wnd_->m_pTcpCommunication->SendMsg(imNum,(char*)sText.GetData());
			*/

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
				pViewEdit->SetTextColor(RGB(255,0,0));
				pViewEdit->SetText(_T("错误！发送数据的长度不能超过512字节。")); 
			}
			else
			{
				//int imNum = friend_.id;
				TCHAR sendmesg[513] = {0};


				_stprintf_s(sendmesg,512,_T("%s"),sText.GetData());
				int len = strlen(sendmesg);

				if (len !=textlen)
				{
					pViewEdit->SetTextColor(RGB(255,0,0));
					pViewEdit->SetText(_T("警告！发送数据的长度和计算的不同。")); 
				}
				//显示在
				SendMsg(g_myself_info.nick_name,sText);
				pRichEdit->SetText(_T(""));

				int size =0;
				size = m_vec_group.size();

				if (size > 0)
				{
					for (int i = 0; i < size; i++)
					{
						if (g_myself_info.id != m_vec_group[i].id)
						{
							int imNum = m_vec_group[i].id;
							frame_wnd_->m_pTcpCommunication->SendGroupMsg(group_info_.id,imNum,sendmesg,textlen);
							Sleep(100);
						}
					}
				}

			}

		}
	}
	else if( _tcsicmp(msg.sType, _T("return")) == 0 ) 
	{
		if (_tcsicmp(msg.pSender->GetName(), kInputRichEditControlName) == 0)
		{
			//CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kInputRichEditControlName));
			//if( pRichEdit == NULL ) return;
			//pRichEdit->SetFocus();
			//CStdString sText = pRichEdit->GetTextRange(0, pRichEdit->GetTextLength());
			//if( sText.IsEmpty() ) return;
			//pRichEdit->SetText(_T(""));
			//SendMsg(group_info_.nick_name,sText);
			//int imNum = friend_.id;
			//frame_wnd_->m_pTcpCommunication->SendMsg(imNum,(char*)sText.GetData());

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
				pViewEdit->SetTextColor(RGB(255,0,0));
				pViewEdit->SetText(_T("错误！发送数据的长度不能超过512字节。")); 
			}
			else
			{
				//int imNum = friend_.id;
				TCHAR sendmesg[513] = {0};


				_stprintf_s(sendmesg,512,_T("%s"),sText.GetData());
				int len = strlen(sendmesg);

				if (len !=textlen)
				{
					pViewEdit->SetTextColor(RGB(255,0,0));
					pViewEdit->SetText(_T("警告！发送数据的长度和计算的不同。")); 
				}
				//显示在
				SendMsg(g_myself_info.nick_name,sText);
				pRichEdit->SetText(_T(""));
				//text code
//                 CStdString strbmp;
//				strbmp.Format("c:\\1.gif");
//// 				HBITMAP bmp = NULL;
//// 				bmp = (HBITMAP)::LoadImage(NULL, strbmp.GetData(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
//// 
//// 				if(bmp==NULL)
//// 				{
//// 					throw "无效的HBITMAP类型。\r\n\r\n可能的原因是:\r\n位图文件的路径不正确。";
//// 				}
//// 
// 				pRichEdit->InsertGif(strbmp);

				int size =0;
				size = m_vec_group.size();

				if (size > 0)
				{
					for (int i = 0; i < size; i++)
					{
				
						if (g_myself_info.id != m_vec_group[i].id)
						{
							int imNum = m_vec_group[i].id;
							frame_wnd_->m_pTcpCommunication->SendGroupMsg(group_info_.id,imNum,sendmesg,textlen);
							Sleep(100);
						}
						
					}
				}

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
			//	new CColorPicker(this, pt);
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

void ChatGroupDialog::SetTextColor(DWORD dwColor)
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

LRESULT ChatGroupDialog::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

void ChatGroupDialog::FontStyleChanged()
{}

int ChatGroupDialog::FindUser(int immo,CStdString &name)
{
	int size = m_vec_group.size();
	if (size > 0)
	{
		for (int i = 0; i <size;i++)
		{
			if (immo == m_vec_group[i].id)
			{
				name.Format(_T("%s"),m_vec_group[i].nick_name);
				return 0;
			}
		}
	}
	return -1;
}