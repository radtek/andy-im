#include "../stdafx.h"
#include "UIFriends.h"
#include "../AndyLib/Andylib.h"

namespace DuiLib
{

const int kFriendListItemNormalHeight = 32;
const int kFriendListItemSelectedHeight = 52;


CFriendsUI::CFriendsUI(CPaintManagerUI& paint_manager)
	: root_node_(NULL)
	, delay_deltaY_(0)
	, delay_number_(0)
	, delay_left_(0)
	, level_expand_image_(_T("<i list_icon_b.png>"))
	, level_collapse_image_(_T("<i list_icon_a.png>"))
	, level_text_start_pos_(10)
	, text_padding_(10, 0, 0, 0)
	, paint_manager_(paint_manager)
{
	SetItemShowHtml(true);

	root_node_ = new Node;
	root_node_->data().level_ = -1;
	root_node_->data().child_visible_ = true;
	root_node_->data().has_child_ = true;
	root_node_->data().list_elment_ = NULL;

}

CFriendsUI::~CFriendsUI()
{
	if (root_node_)
		delete root_node_;

	root_node_ = NULL;
}

bool CFriendsUI::Add(CControlUI* pControl)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::Add(pControl);
}

bool CFriendsUI::AddAt(CControlUI* pControl, int iIndex)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::AddAt(pControl, iIndex);
}

bool CFriendsUI::Remove(CControlUI* pControl)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	if (reinterpret_cast<Node*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()) == NULL)
		return CListUI::Remove(pControl);
	else
		return RemoveNode(reinterpret_cast<Node*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()));
}

bool CFriendsUI::RemoveAt(int iIndex)
{
	CControlUI* pControl = GetItemAt(iIndex);
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	if (reinterpret_cast<Node*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()) == NULL)
		return CListUI::RemoveAt(iIndex);
	else
		return RemoveNode(reinterpret_cast<Node*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()));
}

void CFriendsUI::RemoveAll()
{
	CListUI::RemoveAll();
	for (int i = 0; i < root_node_->num_children(); ++i)
	{
		Node* child = root_node_->child(i);
		RemoveNode(child);
	}
	delete root_node_;

	root_node_ = new Node;
	root_node_->data().level_ = -1;
	root_node_->data().child_visible_ = true;
	root_node_->data().has_child_ = true;
	root_node_->data().list_elment_ = NULL;
}

void CFriendsUI::DoEvent(TEventUI& event) 
{
	if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
	{
		if (m_pParent != NULL)
			m_pParent->DoEvent(event);
		else
			CVerticalLayoutUI::DoEvent(event);
		return;
	}

	if (event.Type == UIEVENT_TIMER && event.wParam == SCROLL_TIMERID)
	{
		if (delay_left_ > 0)
		{
			--delay_left_;
			SIZE sz = GetScrollPos();
			LONG lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
			if ((lDeltaY > 0 && sz.cy != 0)  || (lDeltaY < 0 && sz.cy != GetScrollRange().cy ))
			{
				sz.cy -= lDeltaY;
				SetScrollPos(sz);
				return;
			}
		}
		delay_deltaY_ = 0;
		delay_number_ = 0;
		delay_left_ = 0;
		m_pManager->KillTimer(this, SCROLL_TIMERID);
		return;
	}
	if (event.Type == UIEVENT_SCROLLWHEEL)
	{
		LONG lDeltaY = 0;
		if (delay_number_ > 0)
			lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
		switch (LOWORD(event.wParam))
		{
		case SB_LINEUP:
			if (delay_deltaY_ >= 0)
				delay_deltaY_ = lDeltaY + 8;
			else
				delay_deltaY_ = lDeltaY + 12;
			break;
		case SB_LINEDOWN:
			if (delay_deltaY_ <= 0)
				delay_deltaY_ = lDeltaY - 8;
			else
				delay_deltaY_ = lDeltaY - 12;
			break;
		}
		if
			(delay_deltaY_ > 100) delay_deltaY_ = 100;
		else if
			(delay_deltaY_ < -100) delay_deltaY_ = -100;

		delay_number_ = (DWORD)sqrt((double)abs(delay_deltaY_)) * 5;
		delay_left_ = delay_number_;
		m_pManager->SetTimer(this, SCROLL_TIMERID, 50U);
		return;
	}

	CListUI::DoEvent(event);
}

Node* CFriendsUI::GetRoot()
{
	return root_node_;
}

const TCHAR* const kLogoButtonControlName = _T("logo");
const TCHAR* const kLogoContainerControlName = _T("logo_container");
const TCHAR* const kNickNameControlName = _T("nickname");
const TCHAR* const kDescriptionControlName = _T("description");
const TCHAR* const kOperatorPannelControlName = _T("operation");
const TCHAR* const kHeadPannelControlName = _T("head");

static bool OnLogoButtonEvent(void* event) {
    if( ((TEventUI*)event)->Type == UIEVENT_BUTTONDOWN ) {
        CControlUI* pButton = ((TEventUI*)event)->pSender;
        if( pButton != NULL ) {
            CListContainerElementUI* pListElement = (CListContainerElementUI*)(pButton->GetTag());
            if( pListElement != NULL ) pListElement->DoEvent(*(TEventUI*)event);
        }
    }
    return true;
}
Node* CFriendsUI::AddNode(const FriendListItemInfo& item,Node* parent)
{
	if (!parent)
		parent = root_node_;

	TCHAR szBuf[MAX_PATH] = {0};

    CListContainerElementUI* pListElement = NULL;
    if( !m_dlgBuilder.GetMarkup()->IsValid() ) {
        pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("friend_list_item.xml"), (UINT)0, NULL, &paint_manager_));
    }
    else {
        pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create((UINT)0, &paint_manager_));
    }
    if (pListElement == NULL)
        return NULL;

	Node* node = new Node;

	node->data().level_ = parent->data().level_ + 1;
	if (item.folder)
		node->data().has_child_ = !item.empty;
	else
		node->data().has_child_ = false;

	node->data().folder_ = item.folder;

	node->data().child_visible_ = (node->data().level_ == 0);
	node->data().child_visible_ = false;

	node->data().text_ = item.nick_name;
	node->data().value = item.id;
	node->data().list_elment_ = pListElement;

	if (!parent->data().child_visible_)
		pListElement->SetVisible(false);

	if (parent != root_node_ && !parent->data().list_elment_->IsVisible())
		pListElement->SetVisible(false);

	CRect rcPadding = text_padding_;
	for (int i = 0; i < node->data().level_; ++i)
	{
		rcPadding.left += level_text_start_pos_;		
	}
	pListElement->SetPadding(rcPadding);

	CButtonUI* log_button = static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, kLogoButtonControlName));
	if (log_button != NULL)
	{
		if (!item.folder && strlen(item.logo)>0)
		{
                //"file='user_on.bmp' mask='#000000'"
			    //"file='tree_top.png' corner='2,1,2,1' fade='100'"
			_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item.logo);



			//LPCTSTR dir =getcurrent
			//_stprintf_s(szBuf, MAX_PATH - 1, "file='..\\face\\user_on.bmp' mask='#0000FF'", item.logo);
			log_button->SetNormalImage(szBuf);
		}
		else
		{
			CContainerUI* logo_container = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, kLogoContainerControlName));
			if (logo_container != NULL)
				logo_container->SetVisible(false);
		}
        log_button->SetTag((UINT_PTR)pListElement);
        log_button->OnEvent += MakeDelegate(&OnLogoButtonEvent);
	}

	tString html_text;
	if (node->data().has_child_)
	{
		if (node->data().child_visible_)
			html_text += level_expand_image_;
		else
			html_text += level_collapse_image_;

		_stprintf_s(szBuf, MAX_PATH - 1, _T("<x %d>"), level_text_start_pos_);

		html_text += szBuf;
	}

	if (item.folder)
	{
		html_text += node->data().text_;
	}
	else
	{

		_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item.nick_name);
		html_text += szBuf;
	}

	CLabelUI* nick_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kNickNameControlName));
	if (nick_name != NULL)
	{
		if (item.folder)
			nick_name->SetFixedWidth(0);

		nick_name->SetShowHtml(true);
		nick_name->SetText(html_text.c_str());
	}

	if (!item.folder && strlen((char*)item.description)>0)
	{
		CLabelUI* description = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kDescriptionControlName));
		if (description != NULL)
		{

			_stprintf_s(szBuf, MAX_PATH - 1, _T("<x 0><c #808080>%s</c>"), item.description);
			description->SetShowHtml(true);
			description->SetText(szBuf);
		}
	}

	pListElement->SetFixedHeight(kFriendListItemNormalHeight);
	pListElement->SetTag((UINT_PTR)node);
	int index = 0;
	//有子节点
	if (parent->has_children())
	{
		Node* prev = parent->get_last_child();

		index = prev->data().list_elment_->GetIndex() + 1;
        //交换节点信息

	}
	else //无子节点
	{
		if (parent == root_node_)
			index = 0;
		else  //
			index = parent->data().list_elment_->GetIndex() + 1;
	}
	if (!CListUI::AddAt(pListElement, index))
	{
		delete pListElement;
		delete node;
		node = NULL;
	}

	parent->add_child(node);
	return node;
}
void CFriendsUI::FrontAddNode(const FriendListItemInfo& item,Node* parent)
{

	//有子节点
	if (parent->has_children())
	{

		Node* prev = parent->get_last_child();
		//交换节点信息
		FriendListItemInfo itemprev;

		itemprev.folder = false;
		itemprev.empty =false;
		itemprev.id = prev->data().value;
		CButtonUI* log_button = static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(prev->data().list_elment_, kLogoButtonControlName));
		if (log_button != NULL)
		{

			sprintf(itemprev.logo,"%s",log_button->GetNormalImage());

		}
		sprintf(itemprev.nick_name,"%s",prev->data().text_.data());
		CLabelUI* description = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(prev->data().list_elment_, kDescriptionControlName));
		if (description)
		{
			char strdescripte[200]={0};
			sprintf(strdescripte,"%s",description->GetText().GetData());

			int len = strlen(strdescripte);
			//<x 20><c #808080> </c>
			memcpy(itemprev.description,strdescripte+17,len -17-4);
		}
		RemoveNode(prev);
		AddNode(item,parent);
		AddNode(itemprev,parent);
	}
	else
	{
		AddNode(item,parent);
	}
}
bool CFriendsUI::RemoveNode(Node* node)
{
	if (!node || node == root_node_) return false;

	for (int i = 0; i < node->num_children(); ++i)
	{
		Node* child = node->child(i);
		RemoveNode(child);
	}

	CListUI::Remove(node->data().list_elment_);
	node->parent()->remove_child(node);
	delete node;

	return true;
}
bool CFriendsUI::RemoveChildByparent(Node *parent)
{
	
	while (parent->num_children())
	{
		for (int i = 0; i < parent->num_children(); ++i)
		{
			Node* child = parent->child(i);
			RemoveNode(child);
		}
	}
	//int size = parent->num_children();
	return false;
}
bool CFriendsUI::RemoveNodeByImid(Node* parent,int imid)
{
     Node* node= NULL;

	 if (parent->has_children())
	 {

		 for (int i = 0; i < parent->num_children(); ++i)
		 {
			 Node* child = parent->child(i);
			 if (child->data().value==imid)
			 {
                 RemoveNode(child); 
				 return TRUE;
			 }
	
		 }
	 }
	 return false;
	
   
}

void CFriendsUI::SetChildVisible(Node* node, bool visible)
{
	if (!node || node == root_node_)
		return;

	if (node->data().child_visible_ == visible)
		return;

	node->data().child_visible_ = visible;

	TCHAR szBuf[MAX_PATH] = {0};
	tString html_text;
	if (node->data().has_child_)
	{
		if (node->data().child_visible_)
			html_text += level_expand_image_;
		else
			html_text += level_collapse_image_;

#if defined(UNDER_WINCE)
		_stprintf(szBuf, _T("<x %d>"), level_text_start_pos_);
#else
		_stprintf_s(szBuf, MAX_PATH - 1, _T("<x %d>"), level_text_start_pos_);
#endif
		html_text += szBuf;

		html_text += node->data().text_;

		CLabelUI* nick_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(node->data().list_elment_, kNickNameControlName));
		if (nick_name != NULL)
		{
			nick_name->SetShowHtml(true);
			nick_name->SetText(html_text.c_str());
		}
	}

	if (!node->data().list_elment_->IsVisible())
		return;

	if (!node->has_children())
		return;

	Node* begin = node->child(0);
	Node* end = node->get_last_child();
	for (int i = begin->data().list_elment_->GetIndex(); i <= end->data().list_elment_->GetIndex(); ++i)
	{
		CControlUI* control = GetItemAt(i);
		if (_tcsicmp(control->GetClass(), _T("ListContainerElementUI")) == 0)
		{
			if (visible) 
			{
				Node* local_parent = ((Node*)control->GetTag())->parent();
				if (local_parent->data().child_visible_ && local_parent->data().list_elment_->IsVisible())
				{
					control->SetVisible(true);
				}
			}
			else
			{
				control->SetVisible(false);
			}
		}
	}
}

bool CFriendsUI::CanExpand(Node* node) const
{
	if (!node || node == root_node_)
		return false;

	return node->data().has_child_;
}

bool CFriendsUI::SelectItem(int iIndex, bool bTakeFocus)
{
    if( iIndex == m_iCurSel ) return true;

    // We should first unselect the currently selected item
    if( m_iCurSel >= 0 ) 
	{
        CControlUI* pControl = GetItemAt(m_iCurSel);
        if( pControl != NULL) {
            IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
            if( pListItem != NULL )
			{
				CListContainerElementUI* pFriendListItem = static_cast<CListContainerElementUI*>(pControl);
				Node* node = (Node*)pControl->GetTag();
				if ((pFriendListItem != NULL) && (node != NULL) && !node->folder())
				{
					pFriendListItem->SetFixedHeight(40);
					CContainerUI* pOperatorPannel = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pFriendListItem, kOperatorPannelControlName));
					if (pOperatorPannel != NULL)
					{
						pOperatorPannel->SetVisible(false);
					}
					CButtonUI* log_button = static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pFriendListItem, kLogoButtonControlName));
					if (log_button != NULL)
					{

						CContainerUI* logo_container = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pFriendListItem, kLogoContainerControlName));
						if (logo_container != NULL)
						{
							logo_container->SetPadding(CRect(0,0,0,0));
							log_button->SetPadding(CRect(0,0,0,0));
							log_button->SetFixedHeight(32);
							log_button->SetFixedWidth(32);
							log_button->Invalidate();
						}
					}
		
				}
				pListItem->Select(false);
			}
        }

        m_iCurSel = -1;
    }

    if( iIndex < 0 )
		return false;

	if (!__super::SelectItem(iIndex, bTakeFocus))
		return false;


	CControlUI* pControl = GetItemAt(m_iCurSel);
	if( pControl != NULL) {
		CListContainerElementUI* pFriendListItem = static_cast<CListContainerElementUI*>(pControl);
		Node* node = (Node*)pControl->GetTag();
		if ((pFriendListItem != NULL) && (node != NULL) && !node->folder())
		{

	
			pFriendListItem->SetFixedHeight(kFriendListItemSelectedHeight);
			CContainerUI* pOperatorPannel = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pFriendListItem, kOperatorPannelControlName));
			if (pOperatorPannel != NULL)
			{
				pOperatorPannel->SetVisible(true);
			}
		

			CButtonUI* log_button = static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pFriendListItem, kLogoButtonControlName));
			if (log_button != NULL)
			{
	
					CContainerUI* logo_container = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pFriendListItem, kLogoContainerControlName));
					if (logo_container != NULL)
					{
					
					
						 //padding离内边距位移。5表示离上边框下移5
						logo_container->SetPadding(CRect(0,5,0,0));
						log_button->SetPadding(CRect(0,5,0,0));
						//大小
						log_button->SetFixedHeight(40);
						log_button->SetFixedWidth(40);
						log_button->Invalidate();
					}
		    }
			
	   }
	}
	return true;
}

} // namespace DuiLib