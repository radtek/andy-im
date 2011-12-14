// BaseLog.h: interface for the CBaseLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_BASELIST_H__)
#define _BASELIST_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

struct CSimpleLock
{
public:
	CSimpleLock(CRITICAL_SECTION& lock): m_Lock(lock) 
	{  
		EnterCriticalSection(&m_Lock) ; 
	}
	~CSimpleLock() 
	{
		LeaveCriticalSection(&m_Lock);
	}
protected:
	CRITICAL_SECTION& m_Lock;
};

#endif // !defined(_BASELIST_H__)
