//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// DBInterface.h : 数据库接口定义文件
//
// Author: ziweic
// Date: 08/04/2008
// Modify: Leo
// Date: 11/23/2008
//
//////////////////////////////////////////////////////////////////////
#ifndef __DBINTERFACE_H__
#define __DBINTERFACE_H__


#include <string>
using std::string;
#include "../HeaderFiles/common.h"
#include "DBDervice.h"
#include <windows.h>
//#define  DBNTWIN32
//#include <sqlfront.h>
//#include <sqldb.h>


class DBInterface  
{
public:
	DBInterface();
	DBInterface(string dbservername,string dbname,string dbuser,string dbpass,string dblanguage);
	virtual ~DBInterface();

	int	ExecSQL(const char *format,...);	//执行SQL语句(没有返回结果集)

public:
	/////////////////////////////////////接口/////////////////////////////

	int UserLogin(string imnum,string password);

	int SetLoginState(int imno,int state);

	int GetLoginState(int imno,LOGININFO &loginfo);

	int GetUserInfo(USER_INFO &user);

	int GetFriendInfo(vector<USER_INFO> &verfriend,int imid);

	int GetGroupInfo(vector<GROUP_INFO> &vecgroup,int imid);

	//获得群中的用户信息
	int GetGroupUserInfo(vector<USER_INFO> &verfriend,int groupno);

	int GetOnlineFriend(vector<int> &vecfriend,int imid);
public:
/////////////////////////存储过程///////////////////////////////////////////
	//创建超过时间删除表的存储过程
	int				createprocessdeletetable( );
	//如果指定的存储过程存在则删除
	int             ifexitdeletprocess( string &processname);
	//把Unicom_check表中的内容复制到newtable表中
	int				backupUnicomCheck(string &newtablename);
	//清空指定表中的内容
    int				truncatetable( string &tablename);
	//执行存储过程删除超过90天的表
    int             DeletebackTable(string &tablename);
	int             Getbacktablename();
public:		
	vector<string>  m_vbacktablename;                   //存放备份表的名称
protected:
	UINT mr;
};

#endif
