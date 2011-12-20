#ifndef UIFRIENDS_HPP
#define UIFRIENDS_HPP

#include "UIListCommonDefine.h"

namespace DuiLib
{
struct FriendListItemInfo
{
	bool folder;
	bool empty;
	unsigned int id;
	TCHAR logo[MAX_PATH];
	TCHAR nick_name[50];
	TCHAR description[100];
	int type;//在线状态
	FriendListItemInfo()
	{
		folder =false;
		empty = true;
		_stprintf_s(logo,MAX_PATH-1,_T("%s"),_T("user_off.bmp"));
		memset(nick_name,0,50);
		memset(description,0,100);
		type = -1;

	}
};


class CFriendsUI : public CListUI
{
public:
	enum {SCROLL_TIMERID = 10};

	CFriendsUI(CPaintManagerUI& paint_manager);

	~CFriendsUI();

	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int iIndex);

	bool Remove(CControlUI* pControl);

	bool RemoveAt(int iIndex);

	void RemoveAll();

	void DoEvent(TEventUI& event);

	Node* GetRoot();

	Node* AddNode(const FriendListItemInfo& item, Node* parent = NULL);
	void  FrontAddNode(const FriendListItemInfo& item, Node* parent);

	bool RemoveNode(Node* node);
   //按照imid删除好友信息
	bool RemoveNodeByImid(Node* parent,int imid);
	bool RemoveChildByparent(Node *parent);

	void SetChildVisible(Node* node, bool visible);

	bool CanExpand(Node* node) const;

	bool SelectItem(int iIndex, bool bTakeFocus = false);


private:
	Node*	root_node_;
	LONG	delay_deltaY_;
	DWORD	delay_number_;
	DWORD	delay_left_;
	CRect	text_padding_;
	int level_text_start_pos_;
	tString level_expand_image_;
	tString level_collapse_image_;
	CPaintManagerUI& paint_manager_;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UIFRIENDS_HPP