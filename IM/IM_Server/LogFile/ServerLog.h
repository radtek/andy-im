//***********************************************************************
// * Module:  DBDevice.h
// * Author:  
// * Modified:
// * Purpose: 日志类定义文件
// **********************************************************************

#ifndef __SERVERLOG_H__
#define __SERVERLOG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include"../HeaderFiles/common.h"
#include "../LogFile/CommLog.h"
#include <string>
#include <fstream>

using namespace std;
typedef struct {
	HANDLE	hTread ;
	HANDLE	hPipe ;
	HANDLE	hEvent ;
} PIPE_INSTRUCT;

class CServerLog : public CCommLog
{
public:
	//CServerLog();
	CServerLog();
	virtual ~CServerLog();

public:
	void        CreateLogFile(std::string strLogName);
	void		AddRunLog(enum LOGLEVEL ll,const char* log,...);
	virtual		void AddTextLog(LOGLEVEL ll,const char* log);
	long		GetFileSize(std::string &filename);//获得文件大小
	bool        BackFile(std::string &oldfilename,std::string &newfilename);//back file 
	void		SetNewFilename(string &newfilename);
	void        LogPrintf(const char *log, ...);
	void        CheckIfBackupLog();

private:
	std::ofstream m_osOutput;
	std::string m_strLogName;

#if _SERVICE
	BOOL m_bNeedStop;
	BOOL m_bConnected;
public:
    BOOL IsNeedStop();
	void NotifyStop();
	void Stop();
	BOOL IsConnected();
	void DisConnectNamePipe();
	void SetConnected();
	static DWORD WINAPI ServerThread(LPVOID pdata);

	PIPE_INSTRUCT m_NamePipeInstence;
#endif

};

#endif
