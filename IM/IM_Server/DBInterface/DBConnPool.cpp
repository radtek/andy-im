// DBConnPool.cpp: implementation of the DBConnPool class.
//
//////////////////////////////////////////////////////////////////////

#include "DBConnPool.h"
#include "../LogFile/CommLog.h"
#include "../LogFile/ServerLog.h"
#include"../INIFile/INIFile.h"


DBConnPool* DBConnPool::m_pInstanse = NULL;
extern CServerLog * g_pSvrLog;//日志文件全局变量
extern CINIFile *g_pIniFile;//配置文件全局变量

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DBConnPool::DBConnPool()
{
	// 读取INI文件中数据库信息
	g_pIniFile->GetDBIinInfo(m_strServerName, m_strDBName, m_strUser, m_strPass,m_strLanguage);
				
	// 读取最大、最小数据库连接信息
	g_pIniFile->GetDBPoolInfo(m_nMaxCount, m_nMinCount);
	m_hMaintanceThread = INVALID_HANDLE_VALUE;
				
	// 线程控制
	m_bNeedStop = FALSE;
	m_bNeedConnection = FALSE;
	m_hHaveData = CreateEvent (NULL, TRUE, FALSE, "DataConnPool");
				
	InitializeCriticalSection(&m_csIdleConnList);
	InitializeCriticalSection(&m_csBusyConnList);
}

DBConnPool::DBConnPool(LPCTSTR dbservername,LPCTSTR dbname,LPCTSTR dbuser,LPCTSTR dbpass,LPCTSTR dblanguage)
{
    m_strServerName = dbservername;
	m_strDBName = dbname;
	m_strUser = dbuser;
	m_strPass = dbpass;
	m_strLanguage = dblanguage;
	m_hMaintanceThread = INVALID_HANDLE_VALUE;

	// 线程控制
	m_bNeedStop = FALSE;
	m_bNeedConnection = FALSE;
	m_hHaveData = CreateEvent (NULL, TRUE, FALSE, "DataConnPool");


	InitializeCriticalSection(&m_csIdleConnList);
	InitializeCriticalSection(&m_csBusyConnList);
}

DBConnPool::~DBConnPool()
{
	m_hMaintanceThread = INVALID_HANDLE_VALUE;
	m_bNeedStop = TRUE;
	CloseHandle(m_hHaveData);
	CloseHandle(m_hMaintanceThread);

    DeleteCriticalSection(&m_csIdleConnList);
	DeleteCriticalSection(&m_csBusyConnList);
}

DBConnPool *DBConnPool::Instanse()
{
	if (NULL == m_pInstanse)
	{
        m_pInstanse = new DBConnPool;
	}

	return m_pInstanse;
}

int DBConnPool::InitializeAllDBConnections()
{
	// 先七公里现有的数据
	DestroyAllDBConnections();

	// 开始按照最小数量开始创建
	DBEngine * pDBEngine = NULL;
	int nCount = 0;
	for (int i = 0; i < m_nMinCount; i++)
	{
		nCount = InitializeAConnection();
	}

	// 创建一个工作线程，用来进行一些后台维护工作
	if (INVALID_HANDLE_VALUE == m_hMaintanceThread)
	{
		m_hMaintanceThread = CreateThread(NULL, NULL, thread_run, (LPVOID)this, 0, NULL);
	}

	return nCount;
}

void DBConnPool::DestroyAllDBConnections()
{
	// 销毁数据库连接可以使用大锁
	EnterCriticalSection(&m_csIdleConnList);
	DBConnList::iterator itIdle = m_listIdleConnection.begin();
	DBConnList::iterator itIdleEnd = m_listIdleConnection.end();
	while (itIdle != itIdleEnd)
	{
		if (NULL != (*itIdle))
		{
			(*itIdle)->CloseDatabase();
			delete (*itIdle);
		}
		// erase本身就会把跌代器指向下一个
		itIdle = m_listIdleConnection.erase(itIdle);

	}
	LeaveCriticalSection(&m_csIdleConnList);

	// 还有使用中的连接
	EnterCriticalSection(&m_csBusyConnList);
	DBConnList::iterator itBusy = m_listBusyConnection.begin();
    DBConnList::iterator itBusyEnd = m_listBusyConnection.end();
	while (itBusy != itBusyEnd)
	{
		if (NULL != (*itBusy))
		{
			(*itBusy)->CloseDatabase();
			delete (*itBusy);
		}
		// erase本身就会把跌代器指向下一个
		itBusy = m_listBusyConnection.erase(itBusy);
	}
	LeaveCriticalSection(&m_csBusyConnList);
}

int DBConnPool::InitializeAConnection()
{
	BOOL bSuccess = FALSE;
	DBEngine * pDBEngine = new DBEngine((LPCTSTR)m_strServerName.c_str(), (LPCTSTR)m_strDBName.c_str(), (LPCTSTR)m_strUser.c_str(), (LPCTSTR)m_strPass.c_str(), (LPCTSTR)m_strLanguage.c_str(),bSuccess);
	if (bSuccess)
	{
		m_bNeedConnection = FALSE;
		return RestoreAConnection(pDBEngine);
	}
	else
	{
		delete pDBEngine;
		return m_listIdleConnection.size();
	}
}

void DBConnPool::CloseAConnection(DBEngine* pDBEngine)
{
    pDBEngine->CloseDatabase();
	// 从空闲队列将其删除
	EnterCriticalSection(&m_csIdleConnList);
	m_listIdleConnection.remove(pDBEngine);
	LeaveCriticalSection(&m_csIdleConnList);
}

DBEngine * DBConnPool::GetAConnection()
{
	DBEngine * pDBEngine = NULL;

	// 做一个循环，反复尝试五次取连接，每次间隔1秒钟
	int nTimes = 0;
	while ((m_listIdleConnection.size() <= 0) && (nTimes < 5))
	{
		Sleep(1000);
		nTimes++;
	}

	if (5 == nTimes)
	{
		// 这样狼狈的进来肯定是没有可用连接了，记录日志退出
		g_pSvrLog->AddRunLog(LL_ERROR, "Waiting for a connection for a long time, but failed.");
		return pDBEngine;
	}

	// 从空闲队列中取出，并且加入到使用队列中
	EnterCriticalSection(&m_csIdleConnList);
	if (m_listIdleConnection.size() > 0)
	{
		pDBEngine = m_listIdleConnection.front();
		m_listIdleConnection.pop_front();

		// 加入使用的连接队列
		EnterCriticalSection(&m_csBusyConnList);
		m_listBusyConnection.push_back(pDBEngine);
		LeaveCriticalSection(&m_csBusyConnList);
	}
	LeaveCriticalSection(&m_csIdleConnList);

	if (m_listIdleConnection.size() <= 1)
	{
		// 剩余空闲连接的数目小于等于1个时候需要检查开始创建
		if ((m_listIdleConnection.size() + m_listBusyConnection.size()) < m_nMaxCount)
		{
			// 还小于最大限制，可以创建
			SetEvent(m_hHaveData);
			m_bNeedConnection = TRUE;
		}
		else
		{
			// 超出限制了，做个记录吧
			g_pSvrLog->AddRunLog(LL_ERROR, "Database connection reached max count.");
		}
	}

	return pDBEngine;
}

int DBConnPool::RestoreAConnection(DBEngine* pDBEngine)
{
	if (NULL != pDBEngine)
	{
		// 从使用中的队列取出
		EnterCriticalSection(&m_csBusyConnList);
		m_listBusyConnection.remove(pDBEngine);
		LeaveCriticalSection(&m_csBusyConnList);

		// 加入到空闲队列中
		EnterCriticalSection(&m_csIdleConnList);
		m_listIdleConnection.push_back(pDBEngine);
		LeaveCriticalSection(&m_csIdleConnList);
	}

	EnterCriticalSection(&m_csIdleConnList);
	int nCount = m_listIdleConnection.size();
	LeaveCriticalSection(&m_csIdleConnList);

	return nCount;
}

void DBConnPool::StopThread()
{
	m_bNeedStop = TRUE;
	// 因为线程是无限制等待信号的，所以这里先把标志位置为停止，然后发信号让线程检测
	SetEvent(m_hHaveData);

	// 等待退出
	WaitForSingleObject(m_hMaintanceThread, INFINITE);
	CloseHandle(m_hMaintanceThread);
}

BOOL DBConnPool::IsNeedStop()
{
	return m_bNeedStop;
}

BOOL DBConnPool::IsNeedConnection()
{
	return m_bNeedConnection;
}
/************************************************************************/
/* 维护线程                                                             */
/************************************************************************/
DWORD WINAPI DBConnPool::thread_run( LPVOID pdata)
{
	DBConnPool * pConPool = (DBConnPool *) pdata;

	while (!pConPool->IsNeedStop())
	{
		// 设置事件为无信号, 并且无限制等待
		ResetEvent(pConPool->m_hHaveData);		
		WaitForSingleObject(pConPool->m_hHaveData, INFINITE);

		if (pConPool->IsNeedConnection())
		{
			g_pSvrLog->AddRunLog(LL_DEBUG, "Create a new DB connection.");
			pConPool->InitializeAConnection();
		}
	}
	return 0;
}