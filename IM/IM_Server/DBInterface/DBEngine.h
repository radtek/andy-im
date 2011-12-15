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
	int	ExecSQL(char *szSQL);	//执行SQL语句(没有返回结果集)
    // Interface
public:
	/////////////////////////////////////接口//////////////////////////////
    //用户登录
	int          UserLogin(string imnum,string password);
   //设置登录状态
	int          SetLoginState(int imid,int state); 
	//获取用户名称
    int          GetUserInfo(USER_INFO &user);

    //获取在线状态
	int			GetLoginState(int imno,LOGININFO &loginfo);
    //获取好友信息
	int			GetFriendInfo(vector<USER_INFO> &verfriend,int imid);
	//获得群信息
	int			GetGroupInfo(vector<GROUP_INFO> &vecgroup,int imid);
	//获得群中的用户信息
	int         GetGroupUserInfo(vector<USER_INFO> &verfriend,int groupno);
  
    //获得在线好友
	int         GetOnlineFriend(vector<int> &vecfriend,int imid);



	int          GetIpByName(string  name, string &ip,int &port);

	int          UpdateIpByName(string name,string ip,int port);

	//int           GetFriendinfoByNmae(LPCTSTR name,vector<friend_info> &vecfriend);

private:
	ADODB::_ConnectionPtr           _connection_ptr;   //ADO的数据库连接智能指针
	bool                            _isAdoValid;       //ADO环境是否已经初化成功标志量
	bool							m_bDBOpen;

	LPCSTR							_strDstAddress;    //数据源地址或服务名
	LPCSTR							_strUsername;      //数据库用户名
	LPCSTR							_strPassword;      //数据库密码
	LPCSTR							_strDBName;        //数据库名称

	void                           VarientToString(_variant_t var, string& str);

};

#endif // !defined(AFX_DBENGINE_H__F0F8EC3C_7879_47EB_BFA9_5D0AC34AE1DC__INCLUDED_)
