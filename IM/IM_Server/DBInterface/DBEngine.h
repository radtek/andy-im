// DBEngine.h: interface for the DBEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBENGINE_H__F0F8EC3C_7879_47EB_BFA9_5D0AC34AE1DC__INCLUDED_)
#define AFX_DBENGINE_H__F0F8EC3C_7879_47EB_BFA9_5D0AC34AE1DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include "DBDervice.h"
#include "DBInterface.h"
#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename("EOF","ADOEOF")
using namespace ADODB;
using std::string;

#define  SQL_BUFFER_LEN 1024
class DBEngine  
{
public:
	DBEngine(LPCTSTR dbservername, LPCTSTR dbname, LPCTSTR dbuser, LPCTSTR dbpass, LPCTSTR dblanguage, BOOL& bStatus);
	virtual ~DBEngine();

	// Operate
    int	OpenDatabase(LPCTSTR dbservername, LPCTSTR dbname, LPCTSTR dbuser, LPCTSTR dbpass);	// open database
	int	CloseDatabase(); // close database
	int	ExecSQL(char *szSQL);	//ִ��SQL���(û�з��ؽ����)
    // Interface
public:
	/////////////////////////////////////�ӿ�//////////////////////////////
    //�û���¼
	int          UserLogin(string imnum,string password);
   //���õ�¼״̬
	int          SetLoginState(int imid,int state); 
	//��ȡ�û�����
    int          GetUserInfo(USER_INFO &user);

    //��ȡ����״̬
	int			GetLoginState(int imno,LOGININFO &loginfo);
    //��ȡ������Ϣ
	int			GetFriendInfo(vector<USER_INFO> &verfriend,int imid);
	//���Ⱥ��Ϣ
	int			GetGroupInfo(vector<GROUP_INFO> &vecgroup,int imid);
	//���Ⱥ�е��û���Ϣ
	int         GetGroupUserInfo(vector<USER_INFO> &verfriend,int groupno);
  
    //������ߺ���
	int         GetOnlineFriend(vector<int> &vecfriend,int imid);



	int          GetIpByName(string  name, string &ip,int &port);

	int          UpdateIpByName(string name,string ip,int port);

	//int           GetFriendinfoByNmae(LPCTSTR name,vector<friend_info> &vecfriend);

private:
	ADODB::_ConnectionPtr           _connection_ptr;   //ADO�����ݿ���������ָ��
	bool                            _isAdoValid;       //ADO�����Ƿ��Ѿ������ɹ���־��
	bool							m_bDBOpen;

	LPCSTR							_strDstAddress;    //����Դ��ַ�������
	LPCSTR							_strUsername;      //���ݿ��û���
	LPCSTR							_strPassword;      //���ݿ�����
	LPCSTR							_strDBName;        //���ݿ�����

	void                           VarientToString(_variant_t var, string& str);

};

#endif // !defined(AFX_DBENGINE_H__F0F8EC3C_7879_47EB_BFA9_5D0AC34AE1DC__INCLUDED_)
