// BaseLog.h: interface for the CBaseLog class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BASE_LIST_
#define _BASE_LIST_
#include "../StdAfx.h"
#include <queue>
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
typedef struct tag_ImMsg
{
	int iLen;	  // 发送串的长度
	TCHAR* szSendBuf;  // 待发送的字符串
	tag_ImMsg()
	{
		iLen = -1;
		szSendBuf = NULL;
	}
}IMMsg;
typedef std::queue<IMMsg*>SendQueue;
class CSynList
{
protected:
	CRITICAL_SECTION m_lock;
	// 待发送数据队列
	SendQueue           m_SendQueue;

public:
	explicit CSynList() 
	{ 
		m_hSynListEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		InitializeCriticalSection(&m_lock); 
	}

	~CSynList()	
	{
		CloseHandle(m_hSynListEvent);
		DeleteCriticalSection(&m_lock); 
	}
	

	int GetCount() 
	{ 
		CSimpleLock sl(m_lock); 
		return m_SendQueue.size(); 
	}
	BOOL IsEmpty() 
	{ 
		CSimpleLock sl(m_lock); 
		return m_SendQueue.size();
	}

	// peek at head or tail
	IMMsg* GetHead() 
	{ 
		CSimpleLock sl(m_lock); 
		if( m_SendQueue.size()<1 )
			return NULL; 
		IMMsg* pHead = m_SendQueue.front();
		assert(NULL != pHead);
		m_SendQueue.pop();
		return pHead; 
	}

	void AddTail(IMMsg* newElement) 
	{
		SetEvent(m_hSynListEvent);
		CSimpleLock sl(m_lock); 
		//return CPtrList::AddTail(newElement); 
		return m_SendQueue.push(newElement);
	}

	// remove all elements
	void RemoveAll()  
	{
		IMMsg* pData = NULL;
		// 优先队列
		CSimpleLock sl(m_lock); 
		while (m_SendQueue.size())
		{
			pData = m_SendQueue.front();
			m_SendQueue.pop();
			if (NULL != pData)
			{
				delete pData;
				pData = NULL;
			}
		}
	}
	
	HANDLE m_hSynListEvent;
};


#endif // !defined(_BASELIST_H__)
