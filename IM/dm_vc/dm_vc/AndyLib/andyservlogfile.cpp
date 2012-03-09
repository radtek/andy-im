/**********************************************************************
//
//

//
//							AndySDK类库 andyservice.lib
//作者：
//
//版本：1.0
//

//
//
//原文件名：andyservlogfile.cpp
//
//说明：服务器日志文件类的封装实现
//
**********************************************************************/
#define MAKE_SELF_LIB

#include "andylib.h"

AndyServLogFile::AndyServLogFile()
{
	m_thread = NULL;
	m_nMaxFileSize = 0xFFFFFFFF;
	m_bNewLine = TRUE;
	m_nCurrDay = 0;
	m_lpszErr = NULL;
	m_rf = new andyFile;
}

AndyServLogFile::~AndyServLogFile()
{
	delete m_thread;	
	Del(m_lpszErr);
	delete m_rf;
}
	
void AndyServLogFile::SetLogFile(LPCTSTR lpszLogfile,int nMaxSize,BOOL bWithDate/*=FALSE*/)
{
	if(m_rf)
	{
		m_ctl.Enter();

		try
		{
			if(lpszLogfile)
				m_rf->SetFile(lpszLogfile);
			
			if(nMaxSize > 0)
				m_nMaxFileSize = nMaxSize;
			
			if(bWithDate)
			{
				memset(m_szFilePrefix,0,MAX_PATH);
				lstrcpyn(m_szFilePrefix,m_rf->Pathname(),MAX_PATH);
				lstrcat(m_szFilePrefix,_T("\\"));
				lstrcat(m_szFilePrefix,m_rf->Titlename());

				m_nCurrDay = 0;

				if(m_thread == NULL)
					m_thread = new AndyThread;
				if(m_thread)
				{
					//线程函数循环并不死等，wait的作用是看是否超时
					m_thread->Wait();
					m_thread->Start(this,(THREADFUNC)&AndyServLogFile::MonDateProc);
				}
			}
			else
				m_rf->Create(RF_APPEND);
		}
		catch(...)
		{
		}
		m_ctl.Leave();
	}
}

BOOL AndyServLogFile::WriteLog(LPCTSTR lpBuf,LPCTSTR lpBufNext)
{
	if(lpBuf)
		return WriteLog((LPTSTR)lpBuf,_tcslen(lpBuf),(LPTSTR)lpBufNext,
								lpBufNext?_tcslen(lpBufNext):0,FALSE,FALSE);
	else
		return FALSE;
}

BOOL AndyServLogFile::WriteLog(LPVOID lpBuf,int nSize)
{
	if(lpBuf)
		return WriteLog(lpBuf,nSize,NULL,0,FALSE,FALSE);
	else
		return FALSE;
}

BOOL AndyServLogFile::WriteError(DWORD dwErrCode)
{
	BOOL bOK = FALSE;

	if(m_rf)
	{
		m_ctl.Enter();
		if(m_lpszErr == NULL)
			m_lpszErr = (LPTSTR)New(1024*sizeof(TCHAR));
		
		if(m_lpszErr)
			if(GetErrMessage(dwErrCode,m_lpszErr,1024))
			{
				if(m_rf->Size() > m_nMaxFileSize)
				{
					m_rf->SeekToBegin();
					m_rf->SetEndOfFile();
				}
				bOK = m_rf->Write(m_lpszErr,_tcslen(m_lpszErr));
				wsprintf(m_lpszErr,_T("[%d]\0"),dwErrCode);
				m_rf->WriteLine(m_lpszErr);
				m_bNewLine = TRUE;
			}
		m_ctl.Leave();
	}
	return bOK;
}

BOOL AndyServLogFile::WriteLog(LPVOID lpBuf,int nSize1,LPVOID lpBufNext,int nSize2,
										 BOOL bSetNewLine,BOOL bAddWrap)
{
	BOOL bOK;
	m_ctl.Enter();
	if(m_rf->Size() > m_nMaxFileSize)
	{
		m_rf->SeekToBegin();
		m_rf->SetEndOfFile();
	}

	if(m_bNewLine)
	{
		wsprintf(m_szDate,_T("[%d-%02d-%02d %02d:%02d:%02d.%03d]\0"),m_rdt.year(),m_rdt.month(),
				m_rdt.day(),m_rdt.hour(),m_rdt.minute(),m_rdt.second(),m_rdt.ms());
		m_rf->Write(m_szDate,_tcslen(m_szDate));
	}
	
	bOK = m_rf->Write(lpBuf,nSize1);
	if(lpBufNext)
		bOK = m_rf->Write(lpBufNext,nSize2);

	m_bNewLine = bSetNewLine;
	if(bAddWrap)
		m_rf->Write(_T("\r\n"),2);

	m_ctl.Leave();
	return bOK;
}

BOOL AndyServLogFile::WriteLogLine(LPCTSTR lpBuf,LPCTSTR lpBufNext)
{
	if(lpBuf)
		return WriteLog((LPTSTR)lpBuf,_tcslen(lpBuf),(LPTSTR)lpBufNext,
								lpBufNext?_tcslen(lpBufNext):0,TRUE,TRUE);
	else
		return FALSE;
}
	
void AndyServLogFile::OnDateChange()
{
	m_ctl.Enter();
	try
	{
		TCHAR ext[40];
		memset(ext,0,40);
		wsprintf(ext,m_rf->Extname());
		m_rf->SetFile(_T("%s%d%02d%02d.%s\0"),m_szFilePrefix,m_rdt.year(),m_rdt.month(),m_rdt.day(),ext);
		m_rf->Create(RF_APPEND);
	}
	catch(...)
	{
	}
	m_ctl.Leave();
}

DWORD AndyServLogFile::MonDateProc(LONG)
{
	while(m_thread->IsRunning())
	{
		if(m_rdt.day() != m_nCurrDay)
		{
			m_nCurrDay = m_rdt.day();
			OnDateChange();
		}
		if(m_thread->CanContinue(20000))
			break;
	}
	return 0xDead;
}
