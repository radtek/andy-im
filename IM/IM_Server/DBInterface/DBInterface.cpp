//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// DBInterface.cpp : 数据库接口实现文件
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
extern CServerLog * g_pSvrLog;//日志文件全局变量
extern CINIFile *g_pIniFile;//配置文件全局变量


void CALLBACK TimerbackProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

DBInterface::DBInterface()
{
    // 初始化数据库连接池
	DBConnPool::Instanse()->InitializeAllDBConnections();
	// 创建删除表的存储过程
	//createprocessdeletetable();
	//回调函数一小时执行一次3600000
	//mr = timeSetEvent(1000000, 1000,TimerbackProc,(unsigned long)this,TIME_PERIODIC);
}
//带参数的构造函数
DBInterface::DBInterface(string dbservername,string dbname,string dbuser,string dbpass,string dblanguage)
{
	// 初始化数据库连接池
	DBConnPool::Instanse()->InitializeAllDBConnections();
    //创建删除表的存储过程
	//createprocessdeletetable();
	//回调函数一小时执行一次3600000
	//mr = timeSetEvent(1000000,1000000,TimerbackProc,(unsigned long)this,TIME_PERIODIC);
}
DBInterface::~DBInterface()
{
   
   //timeKillEvent(mr);
}
//执行不需要返回结果集的SQL语句
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
//获得群中的用户信息
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
//创建超过时间删除表的存储过程
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
//如果指定的存储过程存在则删除
int DBInterface::ifexitdeletprocess(string &processname)
{
	string strSQL = "";
	strSQL = "if Exists(select name from sysobjects where name='%s' and type='P') drop procedure %s";
	return ExecSQL(	strSQL.c_str(),processname.c_str(),processname.c_str());
}

//清除表的内容
int DBInterface::truncatetable(string &tablename)
{
	string strSQL = "";
	strSQL = "TRUNCATE TABLE %s";
	return ExecSQL(strSQL.c_str(),tablename.c_str() );
}
//复制表的内容到制定表
int DBInterface::backupUnicomCheck(string &newtablename)
{
	string strSQL = "";
	strSQL="select * into %s from Unicom_Check"; 	
    return ExecSQL(strSQL.c_str(), newtablename.c_str());
}
//执行存储过程删除超过90天的表
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
	int weekday = now->tm_wday;//星期几
	int nowhour = now->tm_hour;//几点，取整数
	int nowminute = now->tm_min;// 几分钟
	sprintf(cnewtablename,"back_Unicom_Check_%d%d",now->tm_mon+1,now->tm_mday);
	newtablename = cnewtablename;
	if( (weekday == 6) && (2<nowhour && nowhour <4 )&& (0 <= nowminute && nowminute <= 20))//每个星期6的早上3点到4点之间
	{
		pDB->backupUnicomCheck(newtablename);//把Unicom_Check的内容考到新表.新表命名为back_Unicom_Check_月天
		pDB->truncatetable(oldtable);//清空老表内容
	}		
	
	if( (2<nowhour &&  nowhour <4) && (0 <= nowminute && nowminute <= 20) )//每天的早上3点到4点
	{
		pDB->Getbacktablename();
		if(pDB->m_vbacktablename.size() !=0 )
		{
			vector<string>::iterator it = pDB->m_vbacktablename.begin();
		
		    for( ; it !=pDB->m_vbacktablename.end();it++ )
			{
				pDB->DeletebackTable(*it);//检查每个表并执行存储过程删除
			}
		}
	}
}
