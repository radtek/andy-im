#ifndef _EMOTION_H_
#define _EMOTION_H_
class WindowImplBase;
class ChatDialog;
class ChatGroupDialog;
class CEmotion : public WindowImplBase
{
public:
	CEmotion(ChatDialog* chat_dialog, POINT ptMouse);
	LPCTSTR GetWindowClassName() const;

	virtual void OnFinalMessage(HWND hWnd);

	void Notify(TNotifyUI& msg);

	void Init();

	virtual tString GetSkinFile();

	virtual tString GetSkinFolder();

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	ChatDialog *m_chat_dialog_;
	POINT based_point_;

	//设置文件过滤标志
	DWORD m_dwFilter;

	std::vector<CDuiString> m_vec_FileName;

	int initFileName(CDuiString folder);

};







#endif