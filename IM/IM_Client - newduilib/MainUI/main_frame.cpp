#include "../stdafx.h"
#include <windows.h>
#if !defined(UNDER_CE)
#include <shellapi.h>
#endif

#include "UIGroups.h"
#include "UIMicroBlog.h"

#include "win_impl_base.h"
#include "main_frame.h"

#include "color_skin.h"
#include "chat_dialog.h"

#include "../TcpNet/ServerLog.h"
#include "../AndyLib/Andylib.h"


const TCHAR* const kTitleControlName = _T("apptitle");
const TCHAR* const kCloseButtonControlName = _T("closebtn");
const TCHAR* const kMinButtonControlName = _T("minbtn");
const TCHAR* const kMaxButtonControlName = _T("maxbtn");
const TCHAR* const kRestoreButtonControlName = _T("restorebtn");

const TCHAR* const kTabControlName = _T("tabs");

const TCHAR* const kFriendButtonControlName = _T("friendbtn");
const TCHAR* const kGroupButtonControlName = _T("groupbtn");
const TCHAR* const kMicroBlogButtonControlName = _T("microblogbtn");

const TCHAR* const kFriendsListControlName = _T("friends");
const TCHAR* const kGroupsListControlName = _T("groups");
const TCHAR* const kMicroBlogListControlName = _T("microblog");

const TCHAR* const kHideLeftMainPannelControlName = _T("btnleft");
const TCHAR* const kShowLeftMainPannelControlName = _T("btnright");
const TCHAR* const kLeftMainPannelControlName = _T("LeftMainPanel");

const TCHAR* const kSignatureTipsControlName = _T("signaturetip");
const TCHAR* const kSignatureControlName = _T("signature");

const TCHAR* const kSearchEditTipControlName = _T("search_tip");
const TCHAR* const kSearchEditControlName = _T("search_edit");

const TCHAR* const kChangeBkSkinControlName = _T("bkskinbtn");
const TCHAR* const kChangeColorSkinControlName = _T("colorskinbtn");

const TCHAR* const kBackgroundControlName = _T("bg");

const int kBackgroundSkinImageCount = 3;

HANDLE g_hMutex = NULL;


int g_ClientID;

CServerLog*		g_pSvrLog;

FriendListItemInfo g_myself_info;

//全局对象
MainFrame::MainFrame()
: bk_image_index_(0)
,m_p_login(NULL)
,m_pTcpCommunication(NULL)
{
	root_parent_myfriend= NULL;
	root_parent_company= NULL;
	root_parent_stranger=NULL;
	root_parent_blacklist= NULL;
	root_parent_group = NULL;
	//设置文件过滤标志
	m_dwFilter = FILTER_FOLDER|FILTER_PIC|FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE;
    m_mChatDlg.clear();
	vec_friends.clear();
	m_p_login = new UILogin(this);

	InitializeCriticalSection(&m_sectionlist);

}

MainFrame::~MainFrame()
{

	PostQuitMessage(0);
}

LPCTSTR MainFrame::GetWindowClassName() const
{
	return _T("TXGuiFoundation");
}
HANDLE MainFrame::RegOnlyWindow(LPCTSTR szMutex)
{
	HANDLE hImHere = CreateMutex(NULL,TRUE,szMutex);

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hImHere);
		return NULL;
	}
	else
		//第一次运行，不关闭句柄，阻止第二副本执行。退出时系统自动释放
		return hImHere;
}
CControlUI* MainFrame::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("FriendList")) == 0)
	{
		//
		return new CFriendsUI(paint_manager_);
	}
	else if (_tcsicmp(pstrClass, _T("GroupList")) == 0)
	{
		return new CGroupsUI(paint_manager_);
	}
	else if (_tcsicmp(pstrClass, _T("MicroBlog")) == 0)
	{
		return new CMicroBlogUI(paint_manager_);
	}

	return NULL;
}

void MainFrame::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

tString MainFrame::GetSkinFile()
{
	return _T("main_frame.xml");
}

tString MainFrame::GetSkinFolder()
{
	return tString(CPaintManagerUI::GetInstancePath()) + _T("skin\\QQRes\\");
}

LRESULT MainFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT MainFrame::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT MainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT MainFrame::ResponseDefaultKeyEvent(WPARAM wParam)
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

void MainFrame::OnTimer(TNotifyUI& msg)
{

}

void MainFrame::OnExit(TNotifyUI& msg)
{
    DeleteCriticalSection(&m_sectionlist);
	Close();
}

void MainFrame::Init()
{
	//监听线程启动

	CreateFriendsList();
	CreateGroupsList();
	CreateMicroBlogList();


}
int MainFrame::InitTcp()
{
	// 创建日志
	g_pSvrLog = new CServerLog(); 
	if (g_pSvrLog== NULL)
	{
		DbgPrint("new ServerLog error!\n");
		return -1;
	}
	m_pTcpCommunication = new CTcpCommunication(this);
	if (m_pTcpCommunication == NULL)
	{
		DbgPrint("new tcp thread error!\n");
		return -1;
	}
	return 0;
}
void MainFrame::ReleaseTCP()
{
	if (g_pSvrLog)
	{
		delete g_pSvrLog;
		g_pSvrLog = NULL;
	}
	if (m_pTcpCommunication)
	{
		delete m_pTcpCommunication;
		m_pTcpCommunication = NULL;
	}
}
DWORD MainFrame::GetBkColor()
{
	CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
	if (background != NULL)
		return background->GetBkColor();

	return -1;
}

void MainFrame::SetBkColor(DWORD dwBackColor)
{
	CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
	if (background != NULL)
	{
		background->SetBkImage(_T(""));
		background->SetBkColor(dwBackColor);
		background->NeedUpdate();

		SkinChangedParam param;
		param.bkcolor = background->GetBkColor();
		param.bgimage = background->GetBkImage();
		skin_changed_observer_.Broadcast(param);
	}
}
int MainFrame::AddUIList(int parent,const FriendListItemInfo list)
{

	CFriendsUI* pFriendsList = static_cast<CFriendsUI*>(paint_manager_.FindControl(kFriendsListControlName));
	//CDuiString str=CPaintManagerUI::GetInstancePath();
//	CDuiString strface,strfaceout;
	//strface.Format("..\\face\\user_on.bmp",str);
	//strfaceout.Format("..\\face\\user_off.bmp",str);
	switch (parent)
	{
	case ADD_LIST_FRIEND:
		{
			if (pFriendsList && root_parent_myfriend)
			{
	
				if (list.type == 1)
				{
	

					_stprintf((TCHAR*)list.logo,_T("%s"),_T("..\\face\\user_on.bmp"));
	
				}
				else
				{
					CreateOffBmp(_T("user_on.bmp"),_T("user_off.bmp"));
					_stprintf((TCHAR*)list.logo,_T("%s"),_T("..\\face\\user_off.bmp"));
				}
			     vec_friends.push_back(list);

			}
			else
			{
				DbgPrint("add list friend error!\n");
				return -1;
			}

		}
		break;
	case ADD_LIST_COMPANY:
		{

			if (pFriendsList && root_parent_company)
			{
				
					vec_friends.push_back(list);

			}
			else
			{
				DbgPrint("add list company error!\n");
				return -1;
			}
		}
		break;
	case ADD_LIST_STRANGER:
		{
			if (pFriendsList && root_parent_stranger)
			{
			
					vec_friends.push_back(list);

			}
			else
			{
				DbgPrint("add list stranger error!\n");
				return -1;
			}
		}
		break;
	case ADD_LIST_BLACKLIST:
		{
			if (pFriendsList && root_parent_blacklist)
			{
			
					vec_friends.push_back(list);

			}
			else
			{
				DbgPrint("add list blacklist error!\n");
				return -1;
			}
		}
		break;
	default:
		break;
	}
	UpdateFriendList();

	return 0;
}
int MainFrame::UpdateFriendList()
{
	CFriendsUI* pFriendsList = static_cast<CFriendsUI*>(paint_manager_.FindControl(kFriendsListControlName));
	if (pFriendsList != NULL)
	{
		EnterCriticalSection(&m_sectionlist);
		pFriendsList->RemoveChildByparent(root_parent_myfriend);
		int size = vec_friends.size();
		if(size > 0)
		{
			//首先构造在线的节点
			for (int i = 0; i<size;i++)
			{			
				//在线
				if(vec_friends[i].type == 1 && vec_friends[i].folder ==false )
					pFriendsList->AddNode(vec_friends[i],root_parent_myfriend);

			}
			//然后不在线节点
			for (int i = 0; i<size;i++)
			{			
				//在线
				if(vec_friends[i].type == 0 && vec_friends[i].folder ==false)
					pFriendsList->AddNode(vec_friends[i],root_parent_myfriend);

			}

		}
		LeaveCriticalSection(&m_sectionlist);
	}
	else
	{
		return -1;
	}
	return 0;

}
int MainFrame::UpdateFriend(unsigned int imid)
{
	int size = vec_friends.size();
	if(size > 0)
	EnterCriticalSection(&m_sectionlist);
	for (int i = 0; i<size;i++)
	{			
			if(vec_friends[i].id == imid)
			{
				_stprintf((TCHAR*)(vec_friends[i].logo),_T("%s"),_T("user_on.bmp"));
				vec_friends[i].type = 1;

			}

	}
	LeaveCriticalSection(&m_sectionlist);
	UpdateFriendList();
	return 0;

}
int MainFrame::CreateFriendsList()
{
	CFriendsUI* pFriendsList = static_cast<CFriendsUI*>(paint_manager_.FindControl(kFriendsListControlName));
	if (pFriendsList != NULL)
	{
		if (!vec_friends.empty())
			vec_friends.clear();
		if (pFriendsList->GetCount() > 0)
			pFriendsList->RemoveAll();

		FriendListItemInfo item;
        //根目录
		item.id = 0;
		item.folder = true;
		item.empty = false;
		_stprintf(item.nick_name,_T("%s"),_T("我的好友"));
		//item.nick_name = _T("我的好友");
		root_parent_myfriend = pFriendsList->AddNode(item, NULL);
		vec_friends.push_back(item);

		item.id = 1;
		//item.nick_name = _T("企业好友");
		_stprintf(item.nick_name,_T("%s"),_T("企业好友"));
		root_parent_company = pFriendsList->AddNode(item, NULL);
		vec_friends.push_back(item);

		item.id = 2;
		//item.nick_name = _T("陌生人");
		_stprintf(item.nick_name,_T("%s"),_T("陌生人"));
		root_parent_stranger = pFriendsList->AddNode(item, NULL);
		vec_friends.push_back(item);

		item.id = 3;
		//item.nick_name = _T("黑名单");
		_stprintf(item.nick_name,_T("%s"),_T("黑名单"));
		root_parent_blacklist= pFriendsList->AddNode(item, NULL);
		vec_friends.push_back(item);


/////test use/////////////
         //添加好友
		//item.id = 676;
		//item.folder = false;
		//item.logo = _T("default.png");
		//item.nick_name = _T("lay");
		//item.description = _T("lay@hotmail.com");

		//pFriendsList->AddNode(item, root_parent_myfriend);
		//vec_friends.push_back(item);

  //      item.id = 56756;
  //      item.folder = false;
  //      item.logo = _T("default.png");
  //      item.nick_name = _T("andy");
  //      item.description = _T("andywei001@gmail.com");
		//pFriendsList->AddNode(item, root_parent_myfriend);
		//vec_friends.push_back(item);

		//item.id = 222222;
		//item.folder = false;
		//item.logo = _T("default.png");
		//item.nick_name = _T("chunchun");
		//item.description = _T("chunchun@gmail.com");
		//AddUIList(ADD_LIST_FRIEND,item);


		//item.id = 4;
		//item.folder = false;
		//item.logo = _T("icon_home.png");
		//item.nick_name = _T("腾讯企业QQ的官方展示号");
		//item.description = _T("");
		//AddUIList(ADD_LIST_COMPANY,item);;


	}
	else
	{
		return -1;
	}
	return 0;
}
int MainFrame::AddGroupList(GroupsListItemInfo group)
{
	CGroupsUI* pGroupsList = static_cast<CGroupsUI*>(paint_manager_.FindControl(kGroupsListControlName));
	if (pGroupsList != NULL && root_parent_group !=NULL)
	{
		group.folder = false;
		group.empty =false;
	    _stprintf(group.logo,_T("%s"),_T("groups.png"));
	    pGroupsList->AddNode(group, root_parent_group);
		vec_group.push_back(group);
	}
	else
	{
		return -1;
	}
	return 0;
}
int MainFrame::CreateGroupsList()
{
	CGroupsUI* pGroupsList = static_cast<CGroupsUI*>(paint_manager_.FindControl(kGroupsListControlName));
	if (pGroupsList != NULL)
	{
		if (pGroupsList->GetCount() > 0)
			pGroupsList->RemoveAll();

		GroupsListItemInfo item;

		item.folder = true;
		item.empty = false;
		//item.nick_name = _T("我的QQ群");
		_stprintf(item.nick_name,_T("%s"),_T("我的QQ群"));

		root_parent_group = pGroupsList->AddNode(item, NULL);

		item.folder = false;
		item.id = 11535348;
		//item.logo = _T("groups.png");
		_stprintf(item.logo,_T("%s"),_T("groups.png"));
		//item.nick_name = _T("官方交流群");
		_stprintf(item.nick_name,_T("%s"),_T("官方交流群"));
		//item.description = _T("153787916");
		_stprintf(item.description,_T("%s"),_T("153787916"));
		pGroupsList->AddNode(item, root_parent_group);

		item.folder = false;
		//item.logo = _T("groups.png");
		//item.nick_name = _T("官方交流群2");
		//item.description = _T("79145400");
		_stprintf(item.logo,_T("%s"),_T("groups.png"));
		_stprintf(item.nick_name,_T("%s"),_T("官方交流群2"));
		_stprintf(item.description,_T("%s"),_T("79145400"));
		pGroupsList->AddNode(item, root_parent_group);

		return 0;
	}
	else
	{
		return -1;
	}
}

int MainFrame::CreateMicroBlogList()
{
	CMicroBlogUI* pMicroBlogList = static_cast<CMicroBlogUI*>(paint_manager_.FindControl(kMicroBlogListControlName));
	if (pMicroBlogList != NULL)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

void MainFrame::OnPrepare(TNotifyUI& msg)
{
	CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
	if (background != NULL)
	{
		TCHAR szBuf[MAX_PATH] = {0};
#if defined(UNDER_WINCE)
		_stprintf(szBuf, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#else
		_stprintf_s(szBuf, MAX_PATH - 1, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#endif
		background->SetBkImage(szBuf);
	}

	/*UpdateFriendsList();

	UpdateGroupsList();

	UpdateMicroBlogList();*/
}

void MainFrame::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("windowinit")) == 0)
	{
		OnPrepare(msg);
	}
	else if (_tcsicmp(msg.sType, _T("killfocus")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kSignatureControlName) == 0)
		{
			msg.pSender->SetVisible(false);
			CControlUI* signature_tip = paint_manager_.FindControl(kSignatureTipsControlName);
			if (signature_tip != NULL)
			{
				CRichEditUI* signature = static_cast<CRichEditUI*>(msg.pSender);
				if (signature != NULL)
					signature_tip->SetText(signature->GetText());
				signature_tip->SetVisible(true);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kSearchEditControlName) == 0)
		{
			msg.pSender->SetVisible(false);
			CControlUI* search_tip = static_cast<CRichEditUI*>(paint_manager_.FindControl(kSearchEditTipControlName));
			if (search_tip != NULL)
			{
				CRichEditUI* search_edit = static_cast<CRichEditUI*>(msg.pSender);
				if (search_edit != NULL)
					search_tip->SetText(search_edit->GetText());
				search_tip->SetVisible(true);
			}
		}
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
		else if (_tcsicmp(msg.pSender->GetName(), kHideLeftMainPannelControlName) == 0)
		{
			CControlUI* left_main_pannel = paint_manager_.FindControl(kLeftMainPannelControlName);
			CControlUI* hide_left_main_pannel = paint_manager_.FindControl(kHideLeftMainPannelControlName);
			CControlUI* show_left_main_pannel = paint_manager_.FindControl(kShowLeftMainPannelControlName);
			if ((left_main_pannel != NULL) && (show_left_main_pannel != NULL) && (hide_left_main_pannel != NULL))
			{
				hide_left_main_pannel->SetVisible(false);
				left_main_pannel->SetVisible(false);
				show_left_main_pannel->SetVisible(true);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kShowLeftMainPannelControlName) == 0)
		{
			CControlUI* left_main_pannel = paint_manager_.FindControl(kLeftMainPannelControlName);
			CControlUI* hide_left_main_pannel = paint_manager_.FindControl(kHideLeftMainPannelControlName);
			CControlUI* show_left_main_pannel = paint_manager_.FindControl(kShowLeftMainPannelControlName);
			if ((left_main_pannel != NULL) && (show_left_main_pannel != NULL) && (hide_left_main_pannel != NULL))
			{
				hide_left_main_pannel->SetVisible(true);
				left_main_pannel->SetVisible(true);
				show_left_main_pannel->SetVisible(false);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kSignatureTipsControlName) == 0)
		{
			msg.pSender->SetVisible(false);
			CRichEditUI* signature = static_cast<CRichEditUI*>(paint_manager_.FindControl(kSignatureControlName));
			if (signature != NULL)
			{
				signature->SetText(msg.pSender->GetText());
				signature->SetVisible(true);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kSearchEditTipControlName) == 0)
		{
			msg.pSender->SetVisible(false);
			CRichEditUI* search_edit = static_cast<CRichEditUI*>(paint_manager_.FindControl(kSearchEditControlName));
			if (search_edit != NULL)
			{
				search_edit->SetText(msg.pSender->GetText());
				search_edit->SetVisible(true);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kChangeBkSkinControlName) == 0)
		{
			CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
			if (background != NULL)
			{
				TCHAR szBuf[MAX_PATH] = {0};
				++bk_image_index_;
				if (kBackgroundSkinImageCount < bk_image_index_)
					bk_image_index_ = 0;

#if defined(UNDER_WINCE)
				_stprintf(szBuf, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#else
				_stprintf_s(szBuf, MAX_PATH - 1, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#endif
				background->SetBkImage(szBuf);

				SkinChangedParam param;
				CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
				if (background != NULL)
				{
					param.bkcolor = background->GetBkColor();
					if (_tcslen(background->GetBkImage()) > 0)
					{
#if defined(UNDER_WINCE)
						_stprintf(szBuf, _T("bg%d.png"), bk_image_index_);
#else
						_stprintf_s(szBuf, MAX_PATH - 1, _T("bg%d.png"), bk_image_index_);
#endif
					}

					param.bgimage = szBuf;
				}
				skin_changed_observer_.Broadcast(param);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kChangeColorSkinControlName) == 0)
		{
			CDuiRect rcWindow;
			GetWindowRect(m_hWnd, &rcWindow);
			rcWindow.top = rcWindow.top + msg.pSender->GetPos().bottom;
			new ColorSkinWindow(this, rcWindow);
		}
	}
	else if (_tcsicmp(msg.sType, _T("timer")) == 0)
	{
		return OnTimer(msg);
	}
	else if (_tcsicmp(msg.sType, _T("selectchanged")) == 0)
	{
		CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(kTabControlName));
		if (_tcsicmp(msg.pSender->GetName(), kFriendButtonControlName) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 0)
			{
				pTabControl->SelectItem(0);
				//UpdateFriendsList();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kGroupButtonControlName) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 1)
			{
				pTabControl->SelectItem(1);
				//UpdateGroupsList();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMicroBlogButtonControlName) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 2)
			{
				pTabControl->SelectItem(2);
				//UpdateMicroBlogList();
			}//
		}
	}
	else if (_tcsicmp(msg.sType, _T("itemactivate")) == 0)
	{
		CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(kTabControlName));
		if (pTabControl != NULL)
		{
			//friendlist
			if (pTabControl->GetCurSel() == 0)
			{
				CFriendsUI* pFriendsList = static_cast<CFriendsUI*>(paint_manager_.FindControl(kFriendsListControlName));
				if ((pFriendsList != NULL) &&  pFriendsList->GetItemIndex(msg.pSender) != -1)
				{
					if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
					{
						Node* node = (Node*)msg.pSender->GetTag();

						CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
						if (!pFriendsList->CanExpand(node) && (background != NULL))
						{

							for (std::vector<FriendListItemInfo>::const_iterator citer = vec_friends.begin(); citer != vec_friends.end(); ++citer)
							{
								if (citer->id==node->data().value)
								{
									m_friend_info = *citer;
									SHowChatDlg(m_friend_info.id,_T(""));
									return;
								}
							}
							DbgPrint("can not find friend in friend list\n");
					
						}
					}
				}
			}
			//grouplist
			else if (pTabControl->GetCurSel() == 1)
			{
				CFriendsUI* pGrpupList = static_cast<CFriendsUI*>(paint_manager_.FindControl(kGroupsListControlName));
				if ((pGrpupList != NULL) &&  pGrpupList->GetItemIndex(msg.pSender) != -1)
				{
					if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
					{
						Node* node = (Node*)msg.pSender->GetTag();

						CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
						if (!pGrpupList->CanExpand(node) && (background != NULL))
						{

							for (std::vector<GroupsListItemInfo>::const_iterator citer = vec_group.begin(); citer != vec_group.end(); ++citer)
							{
								if (citer->id==node->data().value)
								{
									m_group_info = *citer;
									ShowGroupDlg(m_group_info.id,-1,_T(""));
									break;
								}
							}
							DbgPrint("can not find group in group list\n");
			
							/*CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
							if ((background != NULL))
							{
								if (_tcslen(background->GetBkImage()) > 0)
								{

									_stprintf_s(m_szbakimage, MAX_PATH - 1, _T("bg%d.png"), bk_image_index_);

								}
							}

							ChatGroupDialog* pChatDialog = new ChatGroupDialog(this,m_szbakimage, background->GetBkColor(), m_myself_info);
							if( pChatDialog == NULL )
								return;
							pChatDialog->Create(NULL,(LPCTSTR)m_myself_info.nick_name, UI_WNDSTYLE_FRAME | WS_POPUP,  NULL, 0, 0, 0, 0);
							skin_changed_observer_.AddReceiver(pChatDialog);

							pChatDialog->CenterWindow();
							pChatDialog->ShowWindow(SW_SHOW);*/

						}
					}
				}  
			}
		}
	}
	else if (_tcsicmp(msg.sType, _T("itemclick")) == 0)
	{
		CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(paint_manager_.FindControl(kTabControlName));
		if (pTabControl != NULL)
		{
			if (pTabControl->GetCurSel() == 0)
			{
				CFriendsUI* pFriendsList = static_cast<CFriendsUI*>(paint_manager_.FindControl(kFriendsListControlName));
				if ((pFriendsList != NULL) &&  pFriendsList->GetItemIndex(msg.pSender) != -1)
				{
					if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
					{
						Node* node = (Node*)msg.pSender->GetTag();

						if (pFriendsList->CanExpand(node))
						{
							pFriendsList->SetChildVisible(node, !node->data().child_visible_);
						}
					}
				}
			}
			else if (pTabControl->GetCurSel() == 1)
			{
				CGroupsUI* pGroupsList = static_cast<CGroupsUI*>(paint_manager_.FindControl(kGroupsListControlName));
				if ((pGroupsList != NULL) &&  pGroupsList->GetItemIndex(msg.pSender) != -1)
				{
					if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
					{
						Node* node = (Node*)msg.pSender->GetTag();

						if (pGroupsList->CanExpand(node))
						{
							pGroupsList->SetChildVisible(node, !node->data().child_visible_);
						}
					}
				}
			}

		}
	}
}
//自定义消息处理
LRESULT MainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == MSG_HAVEDATA)
	{

		MSGBODY *body = (MSGBODY*)wParam;
		
		SHowChatDlg(body->imid,(TCHAR*)body->msg);	
	
	}
	else if (uMsg == MSG_HAVEDATA_GROUP)
	{
		MSGBODY *body = (MSGBODY*)wParam;
		ShowGroupDlg(body->groupid,body->imid,(TCHAR*)body->msg);
	}
	else if (uMsg == MSG_GETFRIEND)
	{
	    FriendListItemInfo *friendinfo = (FriendListItemInfo*)wParam;

		AddUIList(ADD_LIST_FRIEND,*friendinfo);

	}
	else if (uMsg == MSG_GET_GROUP)
	{
		GroupsListItemInfo *group = (GroupsListItemInfo*)wParam;
        AddGroupList(*group);
	}
	else if (uMsg == MSG_GET_GROUP_USER)
	{
		GroupsListItemInfo *user = (GroupsListItemInfo*)wParam;
		int *group_id = (int*)lParam;

		map<unsigned int, ChatGroupDialog*>::iterator mapit;
		mapit=m_mChatgroupDlg.find((*group_id));
		if (mapit != m_mChatgroupDlg.end())
		{
			ChatGroupDialog *pdlg = mapit->second;
			if (pdlg->GetHWND())
			{
		        pdlg->AddGroupItem((*user));

				//此时如果有消息则发送
				if(m_create_group_msg.groupid == (*group_id) && 
					m_create_group_msg.flag ==true          &&
					m_create_group_msg.imid == (*user).id)
				{
                     pdlg->SendMsg((*user).nick_name,m_create_group_msg.msg);
					 m_create_group_msg.flag=false;
				}

			}
		}

        
	}
	return 0;
}
int MainFrame::SetUser(LPCTSTR strName,LPCTSTR sign)
{
	CLabelUI *pUser = static_cast<CLabelUI *>(paint_manager_.FindControl(_T("TextName")));
	CRichEditUI *psign = static_cast<CRichEditUI*>(paint_manager_.FindControl(_T("signaturetip")));

	if (pUser)
	{
		pUser->SetText(strName);

		_stprintf((TCHAR*)m_strUser.c_str(),_T("%s"),strName);
		//m_strUser = strName;
	}
	else
	{
		DbgPrint("get user control error!\n");
		return -1;
	}

	if (psign)
	{
		psign->SetText(sign);
		psign->Invalidate();
	}
	else
	{
		DbgPrint("get sign control error!\n");
		return -1;
	}
	_stprintf((TCHAR*)g_myself_info.nick_name,_T("%s"),strName);
	_stprintf((TCHAR*)g_myself_info.description,_T("%s"),sign);
	g_myself_info.id = g_ClientID;

	//现在获取好友信息了.如果用户名不为空哈
	if(_tcscmp(strName,_T(""))!=0)
	{

		m_pTcpCommunication->SendBuffer(CMD_GET_FRIEND,_T("3"),1);
	}
	return 0;
}
int MainFrame::FindFriend(unsigned int imid)
{
	for (std::vector<FriendListItemInfo>::const_iterator citer = vec_friends.begin(); citer != vec_friends.end(); ++citer)
	{
		if (citer->id == imid)
		{
			m_friend_info = *citer;
			//break;
			return 0;
		}
	}
	return -1;
}
int MainFrame::FindGroup(unsigned int groupid)
{
	for (std::vector<GroupsListItemInfo>::const_iterator citer = vec_group.begin(); citer != vec_group.end(); ++citer)
	{
		if (citer->id == groupid)
		{
			m_group_info = *citer;
			//break;
			return 0;
		}
	}
	return -1;
}
int MainFrame::SHowChatDlg(unsigned int imid,TCHAR *strmsg)
{
	CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
	if ((background != NULL))
	{
		if (_tcslen(background->GetBkImage()) > 0)
		{

			_stprintf_s(m_szbakimage, MAX_PATH - 1, _T("bg%d.png"), bk_image_index_);

		}
	}
	if (-1==FindFriend(imid))
	{
		//no find
		return -1;
	}
	 //找到
	map<unsigned int, ChatDialog*>::iterator mapit;
	mapit=m_mChatDlg.find(imid);
	if (mapit != m_mChatDlg.end())
	{
		ChatDialog *pdlg = mapit->second;
		pdlg->ShowWindow(SW_SHOW);
		if (_tcslen(strmsg)>0)
		{

			pdlg->SendMsg(m_friend_info.nick_name,strmsg);
		}

	}
	else
	{
		ChatDialog* pChatDialog = new ChatDialog(this,m_szbakimage, background->GetBkColor(), m_friend_info);
		if( pChatDialog == NULL )
		{
			DbgPrint("new chat dlg error!\n");
			return -1;
		}
		pChatDialog->Create(NULL,(LPCTSTR)m_friend_info.nick_name, UI_WNDSTYLE_FRAME | WS_POPUP,  NULL, 0, 0, 0, 0);
		skin_changed_observer_.AddReceiver(pChatDialog);

		pChatDialog->CenterWindow();
		pChatDialog->ShowWindow(SW_SHOW);
	    m_mChatDlg.insert( pair<int, ChatDialog*>(m_friend_info.id,pChatDialog));
	
		if (_tcslen(strmsg)>0)
		{	
             //from who
			pChatDialog->SendMsg(m_friend_info.nick_name,strmsg);
		}

	}

	return 0;

}
int MainFrame::ShowGroupDlg(unsigned int groupid,unsigned int imno,TCHAR* strmsg)
{
	CControlUI* background = paint_manager_.FindControl(kBackgroundControlName);
	if ((background != NULL))
	{
		if (_tcslen(background->GetBkImage()) > 0)
		{

			_stprintf_s(m_szbakimage, MAX_PATH - 1, _T("bg%d.png"), bk_image_index_);

		}
	}
	if (-1 == FindGroup(groupid))
	{
		//no find 
		return -1;
	}
	//找到
	map<unsigned int, ChatGroupDialog*>::iterator mapit;
	mapit=m_mChatgroupDlg.find(groupid);
	if (mapit != m_mChatgroupDlg.end())
	{


		ChatGroupDialog *pdlg = mapit->second;
		pdlg->ShowWindow(SW_SHOW);
		if (_tcslen(strmsg)>0)
		{

			//pdlg->SendMsg(imno,strmsg);
			CDuiString name;
			if (0 == pdlg->FindUser(imno,name))
			{
				if (!name.IsEmpty())
				{
					pdlg->SendMsg(name,strmsg);
				}
			}
		}

	}
	else
	{
		ChatGroupDialog* pChatGroupDialog = new ChatGroupDialog(this,m_szbakimage, background->GetBkColor(), m_group_info);
		if( pChatGroupDialog == NULL )
		{
			DbgPrint("new chat group dlg error!\n");
			return -1;
		}
		pChatGroupDialog->Create(NULL,(LPCTSTR)m_group_info.nick_name, UI_WNDSTYLE_FRAME | WS_POPUP,  NULL, 0, 0, 0, 0);

		//send msg to get group user, only new group dlg need to send this msg
		//获取群信息
		if (m_pTcpCommunication)
		{
			m_pTcpCommunication->SendGetGroupUserMsg(m_group_info.id);
		}
		skin_changed_observer_.AddReceiver(pChatGroupDialog);
		//Sleep(100);
		pChatGroupDialog->CenterWindow();
		pChatGroupDialog->ShowWindow(SW_SHOW);
		m_mChatgroupDlg.insert( pair<int, ChatGroupDialog*>(m_group_info.id,pChatGroupDialog));
		if (_tcslen(strmsg)>0)
		{
			m_create_group_msg.flag=true;
			m_create_group_msg.groupid = groupid;
			m_create_group_msg.imid=imno;
			m_create_group_msg.msg.Format(_T("%s"),strmsg);
		}


		//if (_tcslen(strmsg)>0)
		//{	
		//	//from who
		//	/*for (int i = 0; i < 10 ;i++)
		//	{
		//		CDuiString name;
		//		if (0 == pChatGroupDialog->FindUser(imno,name))
		//		{
		//			if (!name.IsEmpty())
		//			{
		//				pChatGroupDialog->SendMsg(name,strmsg);
		//			}
		//		}
		//		Sleep(100);*/

		//	MSGBODY  msgbody;
		//	msgbody.imid = imno;
		//	msgbody.groupid = groupid;
		//	_stprintf(msgbody.msg,_T("%s"),strmsg);
		//	PostMessage(MSG_HAVEDATA_GROUP,(WPARAM)&msgbody,0);
	

		//}

	}
	return 0;
}
bool MainFrame::CreateOffBmp(LPCTSTR onBmp,LPCTSTR offbmp)
{
	CDuiString str=CPaintManagerUI::GetInstancePath();
	//CDuiString strface,strfaceout;
	TCHAR strface[256]={0};
	TCHAR strfaceout[256]={0};
	_stprintf(strface,_T("..\\skin\\face\\%s"),onBmp);
	_stprintf(strfaceout,_T("..\\skin\\face\\%s"),offbmp);
	andyFile file((LPCTSTR)strfaceout);
	if (file.isExist())
	{
		return TRUE;
	}
	HBITMAP hBitmap = (HBITMAP)LoadImage(CPaintManagerUI::GetInstance(), (LPCTSTR)strface, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	SaveGrayBmp(paint_manager_.GetPaintDC(),hBitmap,(LPCTSTR)strfaceout);
	//int err =GetLastError();
	DeleteObject(hBitmap);
	return TRUE;

}

int MainFrame::find_gif_in_folder(CDuiString strfigname,CDuiString strFolder)
{
	//获取选择的目录后列出图象文件
	andyDir dir(strFolder.GetData());
	andyFile rf;
	andyStr str;
	char szFile[MAX_PATH],szSize[40];
	int index,nSubIdx,nSize,nData;
	FILETIME fTime;
	DWORD dwAttr,dwCheck;
	int nFileCnt=0,nTotalSize=0;
	if(dir.BeginFindFile())
	{
		SHFILEINFO sfi;
		while(dir.FindNext(szFile))
		{
			if(strcmp(szFile,"..") == 0)
				continue;
			rf.SetFile("%s\\%s",dir.Getdir(),szFile);
			str = rf.Extname();
			str.toLower();
			dwAttr = rf.GetAttrib() & (FILTER_FOLDER|FILTER_HIDDEN|FILE_ATTRIBUTE_ARCHIVE);
			dwCheck = m_dwFilter & dwAttr;
			if((dwAttr != 0xFFFFFFFF ) && (str == "bmp" || str == "jpg" || 
				str == "jpeg" || str == "gif" || str == "pcx" || str == "png"))
			{				
				LPCTSTR filename = rf.Filename();
			//	CDuiString file;
				//file.Format(_T("%s"),filename);
				if (0==_tcscmp(strfigname.GetData(),filename))
				{
					return 0;
				}

			}
		}

	}
	return -1;
}