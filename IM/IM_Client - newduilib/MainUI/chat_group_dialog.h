#ifndef CHAT_GROUP_DIALOG_
#define CHAT_GROUP_DIALOG_
#include "skin_change_event.h"
#include "UIFriends.h"
#include "UIGroups.h"

class WindowImplBase;
class MainFrame;
class ChatGroupDialog : public WindowImplBase, public SkinChangedReceiver
{
public:

	ChatGroupDialog(MainFrame * frame_wnd, const tString& bgimage, DWORD bkcolor,const GroupsListItemInfo& group_info);
	~ChatGroupDialog();

public:

	LPCTSTR GetWindowClassName() const;	

	virtual void OnFinalMessage(HWND hWnd);

	virtual void Init();

	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);

	virtual tString GetSkinFile();

	virtual tString GetSkinFolder();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual BOOL Receive(SkinChangedParam param);

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void SetTextColor(DWORD dwColor);

	int SendMsg(CDuiString name,CDuiString sText);

	tString GetCurrentTimeString();

protected:	

	void Notify(TNotifyUI& msg);
	void OnPrepare(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
	void OnTimer(TNotifyUI& msg);

private:
	void FontStyleChanged();

private:	
	bool emotion_timer_start_;

	bool bold_;
	bool italic_;
	bool underline_;
	DWORD text_color_;
	DWORD font_size_;
	tString font_face_name_;

	tString bgimage_;
	DWORD bkcolor_;
	GroupsListItemInfo group_info_;
	//FriendListItemInfo myselft_info_;

	std::vector<GroupsListItemInfo> m_vec_group;

	MainFrame* frame_wnd_;

public:
	//按照状态重新构造整个list
	void UpdateGroupList();
	//添加一个group item
	void AddGroupItem(const GroupsListItemInfo listitem);

	int  FindUser(int immo,CDuiString &name);

};










#endif