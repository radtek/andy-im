#ifndef		_RUNTIME_THREAD
#define		_RUNTIME_THREAD
#include "../StdAfx.h"
#include <string>
#include"ServerLog.h"
using namespace std;

extern int g_port;

extern  CServerLog*		g_pSvrLog;

struct CLock
{
public:
	CLock(CRITICAL_SECTION& lock): m_Lock(lock) 
	{  
		EnterCriticalSection(&m_Lock) ; 
	}
	~CLock() 
	{
		LeaveCriticalSection(&m_Lock);
	}
protected:
	CRITICAL_SECTION& m_Lock;
};

class	CRunTimeThread
{
public:
	explicit CRunTimeThread(const char *pchName) 
		: m_pThreadHandle(NULL), m_bNeedStop(FALSE)
	{
		
		if (pchName)
		{
			m_threadName = pchName;
		}
	}

	explicit CRunTimeThread()
		: m_pThreadHandle(NULL), m_bNeedStop(FALSE)
	{
		  InitializeCriticalSection(&m_lock); 
	}

	virtual ~CRunTimeThread()
	{
		m_bNeedStop = TRUE;
		m_pThreadHandle = NULL;
		m_threadName = "";
		DeleteCriticalSection(&m_lock);
	}
	
	void NotifyStop()
	{
		m_bNeedStop = TRUE; 
	}

	
	void Start()

	{
		DWORD	dwthreadId;
		m_pThreadHandle = CreateThread( 0,
										0,
										ThreadFunc, 
										this, 
										0, 
										&dwthreadId);
	}
	
	BOOL	Wait(DWORD dwMillSecond)
	{
		return WaitForSingleObject(m_pThreadHandle, dwMillSecond);
	}

	const char * GetThreadName() const
	{
		return m_threadName.c_str();
	}

	const HANDLE  GetThreadId() const
	{
		return	m_pThreadHandle;
	}

protected:
	BOOL IsNeedStop()
	{
		return m_bNeedStop;
	}

	virtual void Run() = 0;

	HANDLE m_pThreadHandle; // The thread object's pointer.

private:
	static DWORD WINAPI ThreadFunc(void *lpParam)
	{
		CRunTimeThread *pObj = reinterpret_cast<CRunTimeThread*>(lpParam);
		pObj->Run();
		return 0;
	}
	
	CRITICAL_SECTION m_lock;
	volatile BOOL m_bNeedStop;
	string m_threadName; // the name for the thread.

};

#endif