//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// DBInterface.h : ���ݿ�ӿڶ����ļ�
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

	int	ExecSQL(const char *format,...);	//ִ��SQL���(û�з��ؽ����)

public:
	/////////////////////////////////////�ӿ�/////////////////////////////

	int UserLogin(string imnum,string password);

	int SetLoginState(int imno,int state);

	int GetLoginState(int imno,LOGININFO &loginfo);

	int GetUserInfo(USER_INFO &user);

	int GetFriendInfo(vector<USER_INFO> &verfriend,int imid);

	int GetGroupInfo(vector<GROUP_INFO> &vecgroup,int imid);

	//���Ⱥ�е��û���Ϣ
	int GetGroupUserInfo(vector<USER_INFO> &verfriend,int groupno);

	int GetOnlineFriend(vector<int> &vecfriend,int imid);
public:
/////////////////////////�洢����///////////////////////////////////////////
	//��������ʱ��ɾ����Ĵ洢����
	int				createprocessdeletetable( );
	//���ָ���Ĵ洢���̴�����ɾ��
	int             ifexitdeletprocess( string &processname);
	//��Unicom_check���е����ݸ��Ƶ�newtable����
	int				backupUnicomCheck(string &newtablename);
	//���ָ�����е�����
    int				truncatetable( string &tablename);
	//ִ�д洢����ɾ������90��ı�
    int             DeletebackTable(string &tablename);
	int             Getbacktablename();
public:		
	vector<string>  m_vbacktablename;                   //��ű��ݱ������
protected:
	UINT mr;
};

#endif
