#ifndef _MAIN_FRAME_
#define _MAIN_FRAME_
#include "skin_change_event.h"
#include "UIFriends.h"
#include "UILogin.h"
//#include "TcpConn.h"
#include "chat_dialog.h"
#include "chat_group_dialog.h"
#include "../TcpNet/TcpCommunication.h"
#include "debug.h"


#define  ADD_LIST_FRIEND    1
#define  ADD_LIST_COMPANY   2
#define  ADD_LIST_STRANGER  3
#define  ADD_LIST_BLACKLIST 4
//globle var
typedef struct tag_Create_Group_Msg
{
	bool flag;
	int groupid;
	int imid;
	CStdString msg;
	tag_Create_Group_Msg()
	{
        flag = false;
		groupid = -1;
		imid = -1;
		msg="";
	}
}TAG_CREATE_GROUP_MSG;

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

	int CreateFriendsList();

	int CreateGroupsList();

	int CreateMicroBlogList();
 //ͨ�����ߵ�bmp��������״̬��ͼƬ
	bool CreateOffBmp(LPCTSTR OnBm,LPCTSTR offBmp);

private:
	int bk_image_index_;
	string m_strUser;

	TCHAR m_szbakimage[MAX_PATH];
    DWORD m_back_clolr;
	FriendListItemInfo  m_friend_info;

	GroupsListItemInfo m_group_info;

	SkinChangedObserver skin_changed_observer_;


	Node* root_parent_myfriend;
	Node* root_parent_company;
	Node* root_parent_stranger;
	Node* root_parent_blacklist;
	Node* root_parent_group;
public:

	UILogin *m_p_login;
	//�����ļ����˱�־
	DWORD m_dwFilter;
	std::vector<FriendListItemInfo> vec_friends;
	map<unsigned int, ChatDialog*> m_mChatDlg;//���촰��

	//group dialog
	std::vector<GroupsListItemInfo> vec_group;
	map<unsigned int, ChatGroupDialog*> m_mChatgroupDlg;//Ⱥ���촰��


	CTcpCommunication	*m_pTcpCommunication;
	//  �����ٽ���
	CRITICAL_SECTION    m_sectionlist;   

	//����create group dlg��ͬʱҪ������Ϣ
     TAG_CREATE_GROUP_MSG  m_create_group_msg;
	


public:
	HANDLE RegOnlyWindow(LPCTSTR szMutex);
	int SetUser(LPCTSTR strName,LPCTSTR sign);
    //imidΪIM���룬findflagָʾ�Ƿ����imid����friendlist
	int SHowChatDlg(unsigned int imid,TCHAR* strmsg);


	int ShowGroupDlg(unsigned int groupid,unsigned int imno,TCHAR* strmsg);

	int AddUIList(int parent,const FriendListItemInfo listitem);

	int AddGroupList(GroupsListItemInfo group);

	int  AddGroupUser(int groupid,const GroupsListItemInfo user);

	int InitTcp();
	void ReleaseTCP();


	int FindFriend(unsigned int imid);

	int FindGroup(unsigned int groupid);
   //���º���״̬������ʱ�õ�
	int UpdateFriend(unsigned int imid);
	//����״̬���¹�������list
	int UpdateFriendList();

    //��ָ���ļ����в���gif
	int find_gif_in_folder(CStdString strfigname,CStdString strFolder);
    //ת���gif����Ϣ
    int insert_gif_str(CRichEditUI *pRich,CStdString str);
	//void StartListenTHread(tcpPara  &tcppara);
};

#endif // MAINFRAME_HPP