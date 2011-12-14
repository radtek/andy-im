//***********************************************************************
// * Module:  DBDevice.h
// * Author:  
// * Modified:
// * Purpose: 日志类实现文件
// **********************************************************************

#include "../HeaderFiles/stdafx.h"
#include "../LogFile/BaseList.h"
#include <memory> 
#include <io.h>
#include<winsock.h>
#include "../LogFile/ServerLog.h"

void MyReportEvent(const char *szMsg, int nLevel)
{
	/************************************************************************/
	/* #define EVENTLOG_SUCCESS                0X0000
	   #define EVENTLOG_ERROR_TYPE             0x0001
	   #define EVENTLOG_WARNING_TYPE           0x0002
    #define EVENTLOG_INFORMATION_TYPE       0x0004*/
	/************************************************************************/
	HANDLE hRptEvnt;	
	
	hRptEvnt = RegisterEventSource(NULL, "MyServiceReport");
	
	if (NULL == hRptEvnt)
	{
		return;
	}
	
    if (!ReportEvent(hRptEvnt, nLevel, 0, 0, NULL, 1, 0, &szMsg, NULL))
	{
		return;
    }
	
	DeregisterEventSource(hRptEvnt);
}

CServerLog::CServerLog()
{
#if _SERVICE
	m_bNeedStop = FALSE;
	m_bConnected = FALSE;
	std::string strNamePipe = "\\\\.\\Pipe\\JXTPipe";
    m_NamePipeInstence.hPipe = CreateNamedPipe ( strNamePipe.c_str(), PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED, \
		PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT, 1, 0, 0, 1000, NULL ) ;
	if ( m_NamePipeInstence.hPipe == INVALID_HANDLE_VALUE )
	{
		DWORD dwErrorCode = GetLastError () ;
		char szLog[256] = {0};
		sprintf(szLog, "CServerLog:OpenNamePipe Failed! Error code: %d", dwErrorCode);
		MyReportEvent(szLog, EVENTLOG_ERROR_TYPE);
	}
	m_NamePipeInstence.hEvent = CreateEvent ( NULL, false, false, false );
	m_NamePipeInstence.hTread = CreateThread( NULL,	NULL, ServerThread,	(LPVOID)this, 0, NULL);

#endif
	m_strLogName = "";
}

void CServerLog::CreateLogFile(std::string strLogName)
{
	// 将日志文件名保存
	m_strLogName = strLogName;
	// 开始检查日志文件大小
	if(GetFileSize(strLogName) >  MAX_LOG_SIZE)
	{
		string newfilename ;
		SetNewFilename(newfilename);//设置备份文件名称
		BackFile(strLogName, newfilename);//备份文件
	}
	//把原来隐藏的文件可见
	SetFileAttributes(strLogName.c_str(),FILE_ATTRIBUTE_NORMAL);	

	m_osOutput.open(strLogName.c_str(),ios::app);
}
CServerLog::~CServerLog()
{
	m_osOutput.close();
#ifdef _SERVICE
	CloseHandle(m_NamePipeInstence.hEvent);
	CloseHandle(m_NamePipeInstence.hPipe);
	CloseHandle(m_NamePipeInstence.hTread);		
#endif
}

void CServerLog::AddTextLog(LOGLEVEL ll,const char* log)
{
	if( m_osOutput )
	{
		SYSTEMTIME st;
		GetSystemTime(&st);
		GetLocalTime(&st);
		
		char* pLogBuff = new char[strlen(log)+50];
		int error  = WSAGetLastError();
		//首先加入时标等信息
		switch( ll )
		{
		case LL_DEBUG:
			sprintf(pLogBuff,"%02d%02d(%02d:%02d:%02d.%03d): ",
				st.wMonth,st.wDay, st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
			break;
			
		case LL_WARNING:
			sprintf(pLogBuff,"%02d%02d(%02d:%02d:%02d.%03d): ",
				st.wMonth,st.wDay, st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
			break;
			
		case LL_ERROR:
			sprintf(pLogBuff,"%02d%02d(%02d:%02d:%02d.%03d): ",
				st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
			break;		
			
		default:	
			sprintf(pLogBuff,"%02d%02d(%02d:%02d:%02d.%03d): ",
				st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
		}
		strcat(pLogBuff,log);
		
		m_osOutput<< pLogBuff <<std::endl;
		
		delete [] pLogBuff;
		
	}
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

long CServerLog::GetFileSize(std::string &filename)
{
	int handle;
	handle = open(filename.c_str(), 0x0100); //open file for read
	long MaxSize = filelength(handle); //get length of file
	close(handle);
	return MaxSize;
}

bool CServerLog::BackFile(std::string &oldfilename,std::string &newfilename)
{

	string strNewFilepath = ".\\BackLog";
	string strNewFile =strNewFilepath + "\\" + newfilename ;

     if( CreateDirectory( strNewFilepath.c_str(),NULL))//如果返回错误，则说明文件存在
	 {
        MoveFile(oldfilename.c_str(), strNewFile.c_str());
	 }
	 else
	 {
		MoveFile(oldfilename.c_str(), strNewFile.c_str());
	 }
	 return true;
}
//设置备份文件的名称，以年，月，日期，小时，分钟组成的字符串命名
void  CServerLog::SetNewFilename(string &newfilename)
{
	char* pFilename = new char[50];
	SYSTEMTIME st;
	GetSystemTime(&st);
	GetLocalTime(&st);
	sprintf( pFilename,"%4d%02d%02d%02d%02d.txt",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
	newfilename = pFilename;
    delete [] pFilename;
}

void CServerLog::CheckIfBackupLog()
{
	CreateLogFile(m_strLogName);
}
void CServerLog::LogPrintf(const char * log, ...)
{
	//处理格式化参数
	int nBuf=0;
	char acBuf[1024];
	va_list args;		
	va_start(args, log);		
	nBuf = _vsnprintf(acBuf, 1024, log, args);
	va_end(args);
#if _SERVICE
	if (m_bConnected)
	{
		DWORD dwBytes = strlen(acBuf);
		DWORD dwTransfered = 0;
		int iRet = WriteFile(m_NamePipeInstence.hPipe, acBuf, dwBytes, &dwTransfered, NULL);
		if (0 == iRet)
		{
			DisConnectNamePipe();
		}
	}
#else	
	printf(acBuf);
#endif

}
#if _SERVICE

BOOL CServerLog::IsNeedStop()
{
	return m_bNeedStop;
}

void CServerLog::NotifyStop()
{
	m_bNeedStop = TRUE;
}

void CServerLog::Stop()
{
	NotifyStop();
	WaitForSingleObject(m_NamePipeInstence.hTread, INFINITE);
}

BOOL CServerLog::IsConnected()
{
	return m_bConnected;
}

void CServerLog::DisConnectNamePipe()
{
	DisconnectNamedPipe(m_NamePipeInstence.hPipe);
	m_bConnected = FALSE;
}

void CServerLog::SetConnected()
{
	m_bConnected = TRUE;
}

DWORD WINAPI CServerLog::ServerThread(LPVOID pdata)
{
	DWORD	nReadByte = 0, nWriteByte = 0, dwByte = 0 ;	
	CServerLog* pServerLog = (CServerLog*)pdata;
	OVERLAPPED OverLapStruct = { 0, 0, 0, 0, pServerLog->m_NamePipeInstence.hEvent };
	while ( !pServerLog->IsNeedStop())
	{
		if (pServerLog->IsConnected())
		{
			Sleep(1000);
			continue;
		}
		else
		{
			ConnectNamedPipe(pServerLog->m_NamePipeInstence.hPipe, &OverLapStruct);
			int iRet = WaitForSingleObject(pServerLog->m_NamePipeInstence.hEvent, 1000);
			if (WAIT_OBJECT_0 == iRet)
			{
                pServerLog->SetConnected();
			}            
		}
	}
	
	return 0;
}

#endif