#ifndef UIGROUPS_HPP
#define UIGROUPS_HPP

#include "UIListCommonDefine.h"

namespace DuiLib
{
struct GroupsListItemInfo
{
	bool folder;//
	bool empty;
	unsigned int id; //im no
	char logo[100];//bmp
	char nick_name[100];//name
	char description[250];//
	int type;//online offline
	GroupsListItemInfo()
	{
		 folder=false;
		 empty= false;
		 id= -1;
		 type =-1;
		 memset(logo,0,100);
		 memset(nick_name,0,100);
		 memset(description,0,250);
	}
};

class CGroupsUI : public CListUI
{
public:
	enum {SCROLL_TIMERID = 10};

	CGroupsUI(CPaintManagerUI& paint_manager);

	~CGroupsUI();

	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int iIndex);

	bool Remove(CControlUI* pControl);

	bool RemoveAt(int iIndex);

	void RemoveAll();

	void DoEvent(TEventUI& event);

	Node* GetRoot();

	Node* AddNode(const GroupsListItemInfo& item, Node* parent = NULL);

	bool RemoveNode(Node* node);

	void SetChildVisible(Node* node, bool visible);

	bool CanExpand(Node* node) const;

	void SetFixHight(int height);

private:
	Node*	root_node_;
	LONG	delay_deltaY_;
	DWORD	delay_number_;
	DWORD	delay_left_;
	CDuiRect	text_padding_;
	int level_text_start_pos_;
	tString level_expand_image_;
	tString level_collapse_image_;
	CPaintManagerUI& paint_manager_;

	int m_iListHeight;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UIGROUPS_HPP