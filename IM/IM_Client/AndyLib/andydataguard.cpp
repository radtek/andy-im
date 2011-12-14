/**********************************************************************
//
//

//
//
//							AndySDK类库 andylib.lib
//作者：
//
//版本：1.0
//

//
//原文件名：andydataguard.cpp
//
//说明：同步操作类实现代码
//
**********************************************************************/
#define MAKE_SELF_LIB

#if (!defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0400))
#define _WIN32_WINNT 0x0400
#endif

#include "andylib.h"

BOOL AndyDataGuard::TryEnter()
{
	return TryEnterCriticalSection(&m_crs);
}

AndyDataGuardEx::AndyDataGuardEx()
{
	m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_info = (LPDGEX_SHAREINFO)New(sizeof(DGEX_SHAREINFO));
	m_hFileMap = NULL;
}

AndyDataGuardEx::AndyDataGuardEx(LPCTSTR szName)
{
	if(szName && *szName != '\0')
	{
		TCHAR szTmp[200];
		wsprintf(szTmp,_T("AndyDataGuardEx_%s\0"),szName);
		m_hEvent = CreateEvent(NULL,FALSE,FALSE,szTmp);
		
		wsprintf(szTmp,_T("AndyDGEX_FileMap_%s\0"),szName);
		m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(DGEX_SHAREINFO),szTmp);
		m_info = (LPDGEX_SHAREINFO)MapViewOfFile(m_hFileMap,FILE_MAP_WRITE,0,0,0);
	}
}

AndyDataGuardEx::~AndyDataGuardEx()
{
	CloseHandle(m_hEvent);
	
   if(m_hFileMap == NULL)
	   Del(m_info);
   else
	{
      UnmapViewOfFile(m_info);
      CloseHandle(m_hFileMap);
   }
}

void AndyDataGuardEx::Enter()
{
	DWORD dwThreadId = GetCurrentThreadId();

	if(InterlockedIncrement((LPLONG)&m_info->m_nLockCount) == 1)
	{
		//未进入，进入	
		m_info->m_dwThreadId = dwThreadId;
		m_info->m_nEnterCount = 1;
	}
	else
	{
		if(m_info->m_dwThreadId == dwThreadId)
		{
			//再次进入，增加计数
			m_info->m_nEnterCount++;
		}
		else
		{
			//等待
			WaitForSingleObject(m_hEvent,INFINITE);
			//进入
			m_info->m_dwThreadId = dwThreadId;
			m_info->m_nEnterCount = 1;
		}
	}
}

void AndyDataGuardEx::Leave()
{
	if(m_info->m_dwThreadId == GetCurrentThreadId()) 
   {
		if(--m_info->m_nEnterCount > 0)
			InterlockedDecrement((LPLONG)&m_info->m_nLockCount);
		else
		{
			m_info->m_dwThreadId = 0;
			
			if(InterlockedDecrement((LPLONG)&m_info->m_nLockCount) > 0)
				SetEvent(m_hEvent);
		}
	}
}

BOOL AndyDataGuardEx::TryEnter(DWORD dwWaitMS)
{
	DWORD dwThreadId = GetCurrentThreadId();
   BOOL bEnter = FALSE;
   
   do
	{
      bEnter = (InterlockedCompareExchange((LPLONG)&m_info->m_nLockCount,1,0) == 0); 
		
      if(bEnter)
		{
         //进入
			m_info->m_dwThreadId = dwThreadId;
         m_info->m_nEnterCount = 1;
      }
		else
		{
         if(m_info->m_dwThreadId == dwThreadId)
			{
				//已进入，增加计数
            InterlockedIncrement((LPLONG)&m_info->m_nLockCount);
            m_info->m_nEnterCount++;
            bEnter = TRUE;
         }
      }
		Sleep(1);
   }while(!bEnter && (dwWaitMS-- > 0));
	
   return bEnter;
}
