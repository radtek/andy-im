// ServerLog.h: interface for the CServerLog class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SERVER_LOG_
#define _SERVER_LOG_

#include "../StdAfx.h"
#include "BaseLog.h"
#define UM_SHOWLOG  WM_USER+1022

class CServerLog : public CBaseLog  
{
public:
	void AddRunLog(enum LOGLEVEL ll,const char* log,...);
	CServerLog();
	virtual ~CServerLog();
	virtual void AddTextLog(LOGLEVEL ll,const char* log);

protected:
};

#endif // !defined(AFX_SERVERLOG_H__AEC47861_4795_4A72_9100_42125F0618B9__INCLUDED_)
