/**********************************************************************
//

//
//
//							AndySDK类库 andylib.lib
//作者：
//
//版本：1.0
//

//
//
//原文件名：andymsg.cpp
//
//说明：
//    信息显示/记录类 andyMsg 实现
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "andylib.h"

AndyMsg::AndyMsg()
{
	m_msgMode = 0;
	m_lprf = NULL;;
	m_nMaxFileSize = -1;
	m_hWnd = NULL;
	m_msg = MYWM_ERROR;
	m_str = NULL;
	m_dwLastErrorCode = ERROR_SUCCESS;

	InitializeCriticalSection(&msg_critical);
}

AndyMsg::~AndyMsg()
{
	delete m_lprf;
	m_str = (LPTSTR)Del(m_str);
	DeleteCriticalSection(&msg_critical);
}

void AndyMsg::SetMsgMode(DWORD mode,BOOL bOverWrite/*=FALSE*/)
{
	if(bOverWrite)
		m_msgMode = mode;
	else
		m_msgMode |= mode;
}

BOOL AndyMsg::SetMsgFile(LPCTSTR szfilename,int maxSize/*=-1*/)
{
	if(m_lprf == NULL)
		m_lprf = new andyFile;

	if(m_lprf)
	{
		if(szfilename != NULL)
			m_lprf->SetFile(szfilename);

		SetMsgFileSize(maxSize);
		
		m_lprf->Create(RF_APPEND);

		m_msgMode |= ANDYMSG_LOGFILE;
		return TRUE;
	}
	else
		return FALSE;
}

void AndyMsg::SetMsgFileSize(int maxSize/*=-1*/)
{
	if(maxSize > 0)
		m_nMaxFileSize = maxSize;
	else if(m_nMaxFileSize == -1)
		m_nMaxFileSize = 64000;
}

BOOL AndyMsg::ClearMsgFile()
{
	EnterCriticalSection(&msg_critical);
	if(m_lprf)
		m_lprf->SetEndOfFile(0);
	LeaveCriticalSection(&msg_critical);
	return (BOOL)m_lprf;
}

void AndyMsg::SetMsgMessage(HWND hWnd,UINT msg,BOOL bWithLine/*=TRUE*/)
{
	if(hWnd)
	{
		m_hWnd = hWnd;
		m_msg = msg;
		m_msgMode |= ANDYMSG_SENDMESSAGE;
		if(!bWithLine)
			m_msgMode |= ANDYMSG_NORETURN;
	}
}

BOOL AndyMsg::DataOut(LPVOID lpBuf,DWORD dwSize)
{
	m_dwLastErrorCode = ERROR_SUCCESS;
	if((m_msgMode & ANDYMSG_LOGFILE) && m_lprf)
	{
		EnterCriticalSection(&msg_critical);

		if((int)m_lprf->Size() > m_nMaxFileSize)
		{
			m_lprf->Seek(0);
			SetEndOfFile(m_lprf->Handle());
		}
		
		if((m_msgMode & ANDYMSG_RECTIME))
		{
			TCHAR szTime[32];
			
			wsprintf(szTime,_T("[%04d-%02d-%02d %02d:%02d:%02d.%03d]\0"),
				m_rdt.year(),m_rdt.month(),m_rdt.day(),m_rdt.hour(),m_rdt.minute(),m_rdt.second(),m_rdt.ms());
			
			m_lprf->Write(szTime,_tcslen(szTime));
		}
		if(m_lprf->Write(lpBuf,dwSize))
			m_lprf->Write(_T("\r\n"),2);

		LeaveCriticalSection(&msg_critical);
		return TRUE;
	}
	return FALSE;
}

void AndyMsg::MsgOut(LPCTSTR format,...)
{	
	m_dwLastErrorCode = ERROR_SUCCESS;
	if(LOWORD(m_msgMode) == ANDYMSG_NONE || format == NULL)
		return;

	EnterCriticalSection(&msg_critical);

	if(m_str == NULL)
		m_str = (LPTSTR)New(sizeof(TCHAR)*2048);

	if(m_str)
	{
		RtlZeroMemory(m_str,sizeof(TCHAR)*2048);

		va_list ap;
		
		if(format)
		{
			va_start(ap, format);
			_vsntprintf(m_str,2046,format,ap);
			//vsprintf(m_str,format,ap);
			va_end(ap);
		}

		OutFile(m_str);
		
		if((m_msgMode & ANDYMSG_DEBUGOUT))
			OutputDebugString(m_str);

		SendMsgMessage(m_str,(m_msgMode & ANDYMSG_NORETURN));
		
		if((m_msgMode & ANDYMSG_ALERT))
			MessageBox(GetFocus(),m_str,_T("错误"),MB_OK | MB_ICONWARNING);
	}
	LeaveCriticalSection(&msg_critical);
}

void AndyMsg::MsgOutFast(LPCTSTR format)
{
	m_dwLastErrorCode = ERROR_SUCCESS;
	if(LOWORD(m_msgMode) == ANDYMSG_NONE || format == NULL)
		return;
	
	EnterCriticalSection(&msg_critical);
	
	OutFile(format);
	
	if((m_msgMode & ANDYMSG_DEBUGOUT))
		OutputDebugString(format);
	
	SendMsgMessage(format,(m_msgMode & ANDYMSG_NORETURN));
	
	if((m_msgMode & ANDYMSG_ALERT))
		MessageBox(GetFocus(),format,_T("错误"),MB_OK | MB_ICONWARNING);
	
	LeaveCriticalSection(&msg_critical);
}

BOOL AndyMsg::MsgFormatOut(DWORD dwMsgId)
{
	m_dwLastErrorCode = dwMsgId;
	if(m_str == NULL)
		m_str = (LPTSTR)New(sizeof(TCHAR)*2048);
	else
		RtlZeroMemory(m_str,sizeof(TCHAR)*2048);

	if(GetErrMessage(dwMsgId,m_str,2048))
	{
		m_dwLastErrorCode = dwMsgId;
		MsgOutFast(m_str);
		return TRUE;
	}
	else
		return FALSE;
}

void AndyMsg::SendMsgMessage(LPCTSTR lpMsg,int nErr,BOOL bError)
{
	if((m_msgMode & ANDYMSG_SENDMESSAGE) && lpMsg)		
	{
		if(m_msg == EM_REPLACESEL)
		{
			int Pos = SendMessage(m_hWnd, WM_GETTEXTLENGTH, 0, 0);
			
			if(Pos + _tcslen(lpMsg) >= 16384)
			{
				Pos = SendMessage(m_hWnd, EM_LINEFROMCHAR, 1638, 0) + 1;
				Pos = SendMessage(m_hWnd, EM_LINEINDEX, Pos, 0);
				SendMessage(m_hWnd, EM_SETSEL, 0, Pos);
				SendMessage(m_hWnd, EM_REPLACESEL, FALSE, (LPARAM)"");
				
				Pos = SendMessage(m_hWnd, WM_GETTEXTLENGTH, 0, 0);				
			}
			SendMessage(m_hWnd, EM_SETSEL, Pos, Pos);

			if(bError)
			{
				TCHAR szMsg[32];
				wsprintf(szMsg,_T("错误，代码%d:"),nErr);
				m_dwLastErrorCode = (DWORD)nErr;
				SendMessage(m_hWnd,EM_REPLACESEL,0,(LPARAM)szMsg);
			}			
			SendMessage(m_hWnd,EM_REPLACESEL,0,(LPARAM)lpMsg);
			if(nErr != ANDYMSG_NORETURN)
				SendMessage(m_hWnd,EM_REPLACESEL,0,(LPARAM)"\r\n");			
		}
		else
			SendMessage(m_hWnd,m_msg,nErr,(LPARAM)lpMsg);
	}
}

void AndyMsg::PostMsgMessage(LPCTSTR lpMsg,int nErr,BOOL bError)
{
	if((m_msgMode & ANDYMSG_SENDMESSAGE) && lpMsg)		
	{
		if(m_msg == EM_REPLACESEL)
		{
			static int btCnt = 0;
			btCnt += _tcslen(lpMsg);
			if(btCnt > 3800)
			{
				char tmp[4];
				int cnt = SendMessage(m_hWnd,EM_GETLINE,0,(LPARAM)tmp);
				SendMessage(m_hWnd,EM_SETSEL,0,cnt);
				SendMessage(m_hWnd,EM_REPLACESEL,0,(LPARAM)"");
				SendMessage(m_hWnd,EM_SETSEL,btCnt,btCnt);
			}
			if(bError)
			{
				TCHAR szMsg[32];
				wsprintf(szMsg,_T("错误，代码%d:"),nErr);
				m_dwLastErrorCode = (DWORD)nErr;
				SendMessage(m_hWnd,EM_REPLACESEL,0,(LPARAM)szMsg);
			}
			SendMessage(m_hWnd,EM_REPLACESEL,0,(LPARAM)lpMsg);
			if(nErr != ANDYMSG_NORETURN)
				SendMessage(m_hWnd,EM_REPLACESEL,0,(LPARAM)"\r\n");			
		}
		else
			PostMessage(m_hWnd,m_msg,nErr,(LPARAM)lpMsg);
	}
}

void AndyMsg::OutFile(LPCTSTR lpMsg)
{
	if((m_msgMode & ANDYMSG_LOGFILE) && m_lprf && lpMsg)
	{
		if((int)m_lprf->Size() > m_nMaxFileSize)
		{
			m_lprf->SeekToBegin();
			m_lprf->SetEndOfFile();
		}
			
		if((m_msgMode & ANDYMSG_RECTIME))
		{
			TCHAR szTime[32];
		
			wsprintf(szTime,_T("[%04d-%02d-%02d %02d:%02d:%02d.%03d]\0"),
				m_rdt.year(),m_rdt.month(),m_rdt.day(),m_rdt.hour(),m_rdt.minute(),m_rdt.second(),m_rdt.ms());
		
			m_lprf->Write(szTime,_tcslen(szTime));
		}
		
		m_lprf->Write((LPTSTR)lpMsg,_tcslen(lpMsg));
		m_lprf->Write(_T("\r\n"),2);
	}
}

BOOL AndyMsg::DeleteMsgFile()
{
	EnterCriticalSection(&msg_critical);
	BOOL bOK = FALSE;

	if(m_lprf)
		if(m_lprf->delIt())
		{
			delete m_lprf;
			m_lprf = NULL;
			bOK = TRUE;
		}
	LeaveCriticalSection(&msg_critical);
	return bOK;
}

int AndyMsg::GetMsgFileSize()
{
	EnterCriticalSection(&msg_critical);
	
	int nSize = 0;

	if(m_lprf)
		nSize	= m_lprf->Size();

	LeaveCriticalSection(&msg_critical);
	return nSize;
}

void AndyMsg::ErrOut(int errCode,LPCTSTR szErr)
{
	m_dwLastErrorCode = (DWORD)errCode;
	if(LOWORD(m_msgMode) == ANDYMSG_NONE || szErr == NULL)
		return;
	
	EnterCriticalSection(&msg_critical);
	
	SendMsgMessage(szErr,errCode,TRUE);

	if(m_str == NULL)
		m_str = (LPTSTR)New(sizeof(TCHAR)*1024);
	
	wsprintf(m_str,_T("错误，代码%d:%s"),errCode,szErr);
	OutFile(m_str);
		
	if((m_msgMode & ANDYMSG_DEBUGOUT))
		OutputDebugString(m_str);
		
	if((m_msgMode & ANDYMSG_ALERT))
		MessageBox(GetFocus(),m_str,_T("错误"),MB_OK | MB_ICONWARNING);
	
	LeaveCriticalSection(&msg_critical);
}
/*
void AndyMsgClass::MsgOut(LPCTSTR format,...)
{
	if(m_msg == NULL)
		return;
	
	if(m_msg->m_msgMode == ANDYMSG_NONE || format == NULL)
		return;
	
	EnterCriticalSection(&m_msg->msg_critical);
	
	if(m_msg->m_str == NULL)
		m_msg->m_str = (LPSTR)New(1024);
	
	if(m_msg->m_str)
	{
		memset(m_msg->m_str,0,1024);
		
		va_list ap;
		
		va_start(ap, format);
		vsprintf(m_msg->m_str,format,ap);
		va_end(ap);
		
		m_msg->OutFile(m_msg->m_str);
		
		if((m_msg->m_msgMode & ANDYMSG_DEBUGOUT))
			OutputDebugString(m_msg->m_str);
		
		m_msg->SendMsgMessage(m_msg->m_str,0);
		
		if((m_msg->m_msgMode & ANDYMSG_ALERT))
			MessageBox(GetFocus(),m_msg->m_str,"错误",MB_OK | MB_ICONWARNING);
	}
	LeaveCriticalSection(&m_msg->msg_critical);
}
*/
