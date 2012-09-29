// ServerLog.cpp: implementation of the CServerLog class.
//
//////////////////////////////////////////////////////////////////////

#include "../StdAfx.h"
#include "ServerLog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerLog::CServerLog()
{

}

CServerLog::~CServerLog()
{

}

void CServerLog::AddTextLog(LOGLEVEL ll,const char* log)
{
	//if( m_pSynList )
	//{
	//	SYSTEMTIME st;
	//	GetSystemTime(&st);
	//	GetLocalTime(&st);

	//	char* pLogBuff = new char[strlen(log)+32];
	//	//首先加入时标等信息
	//	switch( ll )
	//	{
	//	case LL_DEBUG:
	//	//	sprintf(pLogBuff,"%02d:%02d:%02d>[%04x,D]: ",
	//		//	st.wHour,st.wMinute,st.wSecond,GetCurrentThreadId());
	//		sprintf(pLogBuff,"%02d:%02d:%02d: ",
	//			st.wHour,st.wMinute,st.wSecond,GetCurrentThreadId());
	//		break;
	//		
	//	case LL_WARNING:
	//		sprintf(pLogBuff,"%02d:%02d:%02d: ",
	//			st.wHour,st.wMinute,st.wSecond,GetCurrentThreadId());
	//		break;
	//		
	//	case LL_ERROR:
	//		sprintf(pLogBuff,"%02d:%02d:%02d: ",
	//			st.wHour,st.wMinute,st.wSecond,GetCurrentThreadId());
	//		break;		
	//		
	//	default:	
	//		sprintf(pLogBuff,"%02d:%02d:%02d: ",
	//			st.wHour,st.wMinute,st.wSecond,GetCurrentThreadId());
	//	}
	//	strcat(pLogBuff,log);

	//	m_pSynList->AddTail( pLogBuff);
	

	//}
}

void CServerLog::AddRunLog(enum LOGLEVEL ll, const char *log, ... )
{
	
	//处理格式化参数
	int nBuf=0;
	char acBuf[1024];
	va_list args;		
	va_start(args, log);		
	nBuf = _vsnprintf(acBuf, 1024, log, args);
	va_end(args);
	
	AddTextLog(ll,acBuf);
	
}
