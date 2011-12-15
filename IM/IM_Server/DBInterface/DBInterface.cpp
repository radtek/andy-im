//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// DBInterface.cpp : ���ݿ�ӿ�ʵ���ļ�
//
// Author: ziweic
// Date: 08/04/2008
//
//////////////////////////////////////////////////////////////////////
#include "../HeaderFiles/stdafx.h"
#include "DBInterface.h"
#include "../HeaderFiles/common.h"
#include "../LogFile/CommLog.h"
#include <memory>
#include "../LogFile/ServerLog.h"
#include"../INIFile/INIFile.h"
#include "DBConnPool.h"
#include "DBEngine.h"
#include <mmsystem.h>
#pragma  comment(lib,"Winmm.lib.")

/*
#define   _CRTDBG_MAP_ALLOC   
#include   <stdlib.h>   
#include   <crtdbg.h>   

class   memchecker{public:memchecker(   void   )
{              
	_CrtSetDbgFlag(   	
		_CRTDBG_ALLOC_MEM_DF|_CRTDBG_DELAY_FREE_MEM_DF|_CRTDBG_DELAY_FREE_MEM_DF   |   
		_CRTDBG_CHECK_ALWAYS_DF   |_CRTDBG_LEAK_CHECK_DF);}}   memcheckerinst;   

*/
extern CServerLog * g_pSvrLog;//��־�ļ�ȫ�ֱ���
extern CINIFile *g_pIniFile;//�����ļ�ȫ�ֱ���


void CALLBACK TimerbackProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

DBInterface::DBInterface()
{
    // ��ʼ�����ݿ����ӳ�
	DBConnPool::Instanse()->InitializeAllDBConnections();
	// ����ɾ����Ĵ洢����
	//createprocessdeletetable();
	//�ص�����һСʱִ��һ��3600000
	//mr = timeSetEvent(1000000, 1000,TimerbackProc,(unsigned long)this,TIME_PERIODIC);
}
//�������Ĺ��캯��
DBInterface::DBInterface(string dbservername,string dbname,string dbuser,string dbpass,string dblanguage)
{
	// ��ʼ�����ݿ����ӳ�
	DBConnPool::Instanse()->InitializeAllDBConnections();
    //����ɾ����Ĵ洢����
	//createprocessdeletetable();
	//�ص�����һСʱִ��һ��3600000
	//mr = timeSetEvent(1000000,1000000,TimerbackProc,(unsigned long)this,TIME_PERIODIC);
}
DBInterface::~DBInterface()
{
   
   //timeKillEvent(mr);
}
//ִ�в���Ҫ���ؽ������SQL���
int DBInterface::ExecSQL(const char *format,...)
{
	DBEngine *pDB = NULL;
	ConnGuard oConnGuard(pDB);
	if (NULL != pDB)
	{
		int sql_len=0;
		char szSQL[STRSQLMAX] = {0};

		va_list args;
		va_start(args, format);
		sql_len = _vsnprintf(szSQL, STRSQLMAX, format, args);
		va_end(args);

		if (sql_len>=STRSQLMAX - 1 || sql_len<=0)
		{
			return ERR_MAX_SQLSTR;
		}

		return pDB->ExecSQL(szSQL);
	}

	return -1;
}
int DBInterface::UserLogin(string imnum,string password)
{
		DBEngine *pDB = NULL;
		ConnGuard oConnGuard(pDB);
		if (NULL != pDB)
		{
			return pDB->UserLogin(imnum, password);
		}
	
		return -1;
}
int DBInterface::SetLoginState(int imno,int state)
{
	DBEngine *pDB = NULL;
	ConnGuard oConnGuard(pDB);
	if (NULL != pDB)
	{
		return pDB->SetLoginState(imno,state);
	}

	return -1;
}

int DBInterface::GetUserInfo(USER_INFO &user)
{
	DBEngine *pDB = NULL;
	ConnGuard oConnGuard(pDB);
	if (NULL != pDB)
	{
		return pDB->GetUserInfo(user);
	}

	return -1;
}
int DBInterface::GetLoginState(int imno,LOGININFO &loginfo)
{
	DBEngine *pDB = NULL;
	ConnGuard oConnGuard(pDB);
	if (NULL != pDB)
	{
		return pDB->GetLoginState(imno,loginfo);
	}

	return -1;
}
int DBInterface::GetFriendInfo(vector<USER_INFO> &verfriend,int imid)
{
	DBEngine *pDB = NULL;
	ConnGuard oConnGuard(pDB);
	if (NULL != pDB)
	{
		return pDB->GetFriendInfo(verfriend,imid);
	}

	return -1;
}
int DBInterface::GetGroupInfo(vector<GROUP_INFO> &vecgroup,int imid)
{
	DBEngine *pDB = NULL;
	ConnGuard oConnGuard(pDB);
	if (NULL != pDB)
	{
		return pDB->GetGroupInfo(vecgroup,imid);
	}

	return -1;
}
//���Ⱥ�е��û���Ϣ
int DBInterface::GetGroupUserInfo(vector<USER_INFO> &verfriend,int groupno)
{
	DBEngine *pDB = NULL;
	ConnGuard oConnGuard(pDB);
	if (NULL != pDB)
	{
		return pDB->GetGroupUserInfo(verfriend,groupno);
	}

	return -1;
}
int DBInterface::GetOnlineFriend(vector<int> &vecfriend,int imid)
{
	DBEngine *pDB = NULL;
	ConnGuard oConnGuard(pDB);
	if (NULL != pDB)
	{
		return pDB->GetOnlineFriend(vecfriend,imid);
	}

	return -1;
}
//��������ʱ��ɾ����Ĵ洢����
int DBInterface::createprocessdeletetable()
{
	string processname = "pro_deletable";
	string strSQL = "";
	ifexitdeletprocess(processname);
	strSQL = "create proc pro_deletable @killtablename varchar(128),@killdate int ";  
	strSQL+= " as";   
	strSQL+= " declare @t varchar(100),@cnt int";   
	strSQL+= " select @cnt=count(*) from sysobjects where name=''+@killtablename+ ''and xtype='U' and datediff(day,crdate,getdate())>=''+@killdate+''";   
	strSQL+= " if @cnt>0";     
	strSQL+= " begin";     
	strSQL+= " exec('drop table '+@killtablename)";   
	strSQL+= " end";
    return ExecSQL(	strSQL.c_str());
}
//���ָ���Ĵ洢���̴�����ɾ��
int DBInterface::ifexitdeletprocess(string &processname)
{
	string strSQL = "";
	strSQL = "if Exists(select name from sysobjects where name='%s' and type='P') drop procedure %s";
	return ExecSQL(	strSQL.c_str(),processname.c_str(),processname.c_str());
}

//����������
int DBInterface::truncatetable(string &tablename)
{
	string strSQL = "";
	strSQL = "TRUNCATE TABLE %s";
	return ExecSQL(strSQL.c_str(),tablename.c_str() );
}
//���Ʊ�����ݵ��ƶ���
int DBInterface::backupUnicomCheck(string &newtablename)
{
	string strSQL = "";
	strSQL="select * into %s from Unicom_Check"; 	
    return ExecSQL(strSQL.c_str(), newtablename.c_str());
}
//ִ�д洢����ɾ������90��ı�
int DBInterface::DeletebackTable(string &tablename)
{
	string strSQL = "";
	strSQL = " exec   pro_deletable  '%s','90'";
	return ExecSQL(	strSQL.c_str(),tablename.c_str());
}

int DBInterface::Getbacktablename()
{
	DBEngine *pDB = NULL;
	ConnGuard oConnGuard(pDB);
	if (NULL != pDB)
	{
		//return pDB->Getbacktablename(m_vbacktablename);
	}
	
	return -1;
}
void CALLBACK TimerbackProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	DBInterface *pDB=(DBInterface*)dwUser;
	time_t tt;
 	struct tm *now;
	time( &tt);
	now = localtime( &tt);
	char cnewtablename[20];
	memset(cnewtablename,0,20);
	string newtablename = "";
	string oldtable = "Unicom_Check";
	int weekday = now->tm_wday;//���ڼ�
	int nowhour = now->tm_hour;//���㣬ȡ����
	int nowminute = now->tm_min;// ������
	sprintf(cnewtablename,"back_Unicom_Check_%d%d",now->tm_mon+1,now->tm_mday);
	newtablename = cnewtablename;
	if( (weekday == 6) && (2<nowhour && nowhour <4 )&& (0 <= nowminute && nowminute <= 20))//ÿ������6������3�㵽4��֮��
	{
		pDB->backupUnicomCheck(newtablename);//��Unicom_Check�����ݿ����±�.�±�����Ϊback_Unicom_Check_����
		pDB->truncatetable(oldtable);//����ϱ�����
	}		
	
	if( (2<nowhour &&  nowhour <4) && (0 <= nowminute && nowminute <= 20) )//ÿ�������3�㵽4��
	{
		pDB->Getbacktablename();
		if(pDB->m_vbacktablename.size() !=0 )
		{
			vector<string>::iterator it = pDB->m_vbacktablename.begin();
		
		    for( ; it !=pDB->m_vbacktablename.end();it++ )
			{
				pDB->DeletebackTable(*it);//���ÿ����ִ�д洢����ɾ��
			}
		}
	}
}
