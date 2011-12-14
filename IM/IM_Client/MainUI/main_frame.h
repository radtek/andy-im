#ifndef _MAIN_FRAME_
#define _MAIN_FRAME_
#include "skin_change_event.h"
#include "UIFriends.h"
#include "UILogin.h"
//#include "TcpConn.h"
#include "chat_dialog.h"
#include "chat_group_dialog.h"
#include "../TcpNet/TcpCommunication.h"


#define  ADD_LIST_FRIEND    1
#define  ADD_LIST_COMPANY   2
#define  ADD_LIST_STRANGER  3
#define  ADD_LIST_BLACKLIST 4


typedef struct tag_chatdlg_info
{
	string strfriendName;
	ChatDialog *pchat;
	tag_chatdlg_info()
	{
		strfriendName = _T("");

		pchat = NULL;

	}
}chatdlg_info;

class WindowImplBase;
class MainFrame : public WindowImplBase
{
public:

	MainFrame();
	~MainFrame();

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

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    DWORD GetBkColor();
    void SetBkColor(DWORD dwBackColor);

protected:	

	void Notify(TNotifyUI& msg);
	void OnPrepare(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
	void OnTimer(TNotifyUI& msg);

public:

	void CreateFriendsList();

	void CreateGroupsList();

	void CreateMicroBlogList();
 //ͨ�����ߵ�bmp��������״̬��ͼƬ
	bool CreateOffBmp(LPCTSTR OnBm,LPCTSTR offBmp);

private:
	int bk_image_index_;
	string m_strUser;

	TCHAR m_szbakimage[MAX_PATH];
    DWORD m_back_clolr;
	FriendListItemInfo m_myself_info;
	FriendListItemInfo  m_friend_info;

	SkinChangedObserver skin_changed_observer_;


	Node* root_parent_myfriend;
	Node* root_parent_company;
	Node* root_parent_stranger;
	Node* root_parent_blacklist;
public:

	UILogin *m_p_login;
	std::vector<FriendListItemInfo> vec_friends;
	map<unsigned int, ChatDialog*> m_mChatDlg;//���촰��


	CTcpCommunication	*m_pTcpCommunication;
	//  �����ٽ���
	CRITICAL_SECTION    m_sectionlist;   
	


public:
	HANDLE RegOnlyWindow(LPCTSTR szMutex);
	void SetUser(LPCTSTR strName,LPCTSTR sign);
    //imidΪIM���룬findflagָʾ�Ƿ����imid����friendlist
	void SHowChatDlg(unsigned int imid,char* strmsg);


	void AddUIList(int parent,const FriendListItemInfo listitem);

	void InitTcp();
	void ReleaseTCP();


	void FindFriend(unsigned int imid);
   //���º���״̬������ʱ�õ�
	void UpdateFriend(unsigned int imid);
	//����״̬���¹�������list
	void UpdateFriendList();

	//void StartListenTHread(tcpPara  &tcppara);
};

#endif // MAINFRAME_HPP