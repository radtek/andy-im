#ifndef _EMOTION_LIST_
#define _EMOTION_LIST_
class CEmotion;
class Emotionlist :
	public WindowImplBase
{
public:
	Emotionlist(CEmotion * emotion_wnd,RECT rcparentWindow);
	virtual ~Emotionlist(void);

	LPCTSTR GetWindowClassName() const;

	virtual void OnFinalMessage(HWND hWnd);

	void Notify(TNotifyUI& msg);

	void Init();

	virtual tString GetSkinFile();

	virtual tString GetSkinFolder();

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	void OnPrepare(TNotifyUI& msg);
public:
	//void OnTimer(TNotifyUI& msg);

private:
	RECT parent_window_rect_;

	CEmotion* emotion_wnd_;


};





#endif