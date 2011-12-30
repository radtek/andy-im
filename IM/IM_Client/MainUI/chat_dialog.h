#ifndef CHATDIALOG_HPP
#define CHATDIALOG_HPP

#include "skin_change_event.h"
#include "UIFriends.h"
class WindowImplBase;
class MainFrame;
class ChatDialog : public WindowImplBase, public SkinChangedReceiver
{
public:

	ChatDialog(MainFrame * frame_wnd, const tString& bgimage, DWORD bkcolor,const FriendListItemInfo& friend_info);
	~ChatDialog();

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

    int SendMsg(CStdString name,CStdString sText);

	tString GetCurrentTimeString();

protected:	

	void Notify(TNotifyUI& msg);
	void OnPrepare(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
	void OnTimer(TNotifyUI& msg);

private:
	void FontStyleChanged();

	int SetInputEditImg(CStdString strimg);

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
	//FriendListItemInfo myselft_;
	FriendListItemInfo friend_;

	MainFrame* frame_wnd_;

	CStdString m_strFriendip;

	int m_iFriendPort;

};

#endif // CHARTDIALOG_HPP