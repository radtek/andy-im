// ServerLog.h: interface for the CCommLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_COMMLOG_H__)
#define _COMMLOG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "../HeaderFiles/StdAfx.h"


//日志级别定义：
//LL_ERROR：  错误
//LL_WARNING：异常但不一定是错误
//LL_INFO：   应用程序运行信息，用于处理流程跟踪
//LL_DEBUG：  调试数据，用于在调试程序时进行数据跟踪
//LL_DEBUGDETAIL: 调试细节数据，比如命令的详细数据等
#define MAX_LOG_SIZE   3000  //1048576 //1M 以字节为单位
enum LOGLEVEL
{
	LL_DEBUGDETAIL=0,
		LL_DEBUG,
		LL_INFO,		
		LL_WARNING,
		LL_ERROR,
		LL_MAXLEVEL
};

class CCommLog  
{
public:
	CCommLog();
	virtual ~CCommLog();
	
public:
	//设置日志记录级别
	void  SetLogLevel(LOGLEVEL ll)
	{
		m_RecLogLevel=ll;
	}
	//获取日志记录级别
	LOGLEVEL GetLogLevel()
	{
		return m_RecLogLevel;
	}
	
	//添加一条带时标的日志记录，可带格式化参数
	virtual void  AddTextLog(enum LOGLEVEL ll,const char* log)=0;
	
protected:
	//日志记录级别		
	LOGLEVEL m_RecLogLevel;	
};

#endif // !defined(_COMMLOG_H__)
