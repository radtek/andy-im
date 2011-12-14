// DBConnPool.cpp: implementation of the DBConnPool class.
//
//////////////////////////////////////////////////////////////////////

#include "DBConnPool.h"
#include "../LogFile/CommLog.h"
#include "../LogFile/ServerLog.h"
#include"../INIFile/INIFile.h"


DBConnPool* DBConnPool::m_pInstanse = NULL;
extern CServerLog * g_pSvrLog;//��־�ļ�ȫ�ֱ���
extern CINIFile *g_pIniFile;//�����ļ�ȫ�ֱ���

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DBConnPool::DBConnPool()
{
	// ��ȡINI�ļ������ݿ���Ϣ
	g_pIniFile->GetDBIinInfo(m_strServerName, m_strDBName, m_strUser, m_strPass,m_strLanguage);
				
	// ��ȡ�����С���ݿ�������Ϣ
	g_pIniFile->GetDBPoolInfo(m_nMaxCount, m_nMinCount);
	m_hMaintanceThread = INVALID_HANDLE_VALUE;
				
	// �߳̿���
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

	// �߳̿���
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
	// ���߹������е�����
	DestroyAllDBConnections();

	// ��ʼ������С������ʼ����
	DBEngine * pDBEngine = NULL;
	int nCount = 0;
	for (int i = 0; i < m_nMinCount; i++)
	{
		nCount = InitializeAConnection();
	}

	// ����һ�������̣߳���������һЩ��̨ά������
	if (INVALID_HANDLE_VALUE == m_hMaintanceThread)
	{
		m_hMaintanceThread = CreateThread(NULL, NULL, thread_run, (LPVOID)this, 0, NULL);
	}

	return nCount;
}

void DBConnPool::DestroyAllDBConnections()
{
	// �������ݿ����ӿ���ʹ�ô���
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
		// erase����ͻ�ѵ�����ָ����һ��
		itIdle = m_listIdleConnection.erase(itIdle);

	}
	LeaveCriticalSection(&m_csIdleConnList);

	// ����ʹ���е�����
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
		// erase����ͻ�ѵ�����ָ����һ��
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
	// �ӿ��ж��н���ɾ��
	EnterCriticalSection(&m_csIdleConnList);
	m_listIdleConnection.remove(pDBEngine);
	LeaveCriticalSection(&m_csIdleConnList);
}

DBEngine * DBConnPool::GetAConnection()
{
	DBEngine * pDBEngine = NULL;

	// ��һ��ѭ���������������ȡ���ӣ�ÿ�μ��1����
	int nTimes = 0;
	while ((m_listIdleConnection.size() <= 0) && (nTimes < 5))
	{
		Sleep(1000);
		nTimes++;
	}

	if (5 == nTimes)
	{
		// �����Ǳ��Ľ����϶���û�п��������ˣ���¼��־�˳�
		g_pSvrLog->AddRunLog(LL_ERROR, "Waiting for a connection for a long time, but failed.");
		return pDBEngine;
	}

	// �ӿ��ж�����ȡ�������Ҽ��뵽ʹ�ö�����
	EnterCriticalSection(&m_csIdleConnList);
	if (m_listIdleConnection.size() > 0)
	{
		pDBEngine = m_listIdleConnection.front();
		m_listIdleConnection.pop_front();

		// ����ʹ�õ����Ӷ���
		EnterCriticalSection(&m_csBusyConnList);
		m_listBusyConnection.push_back(pDBEngine);
		LeaveCriticalSection(&m_csBusyConnList);
	}
	LeaveCriticalSection(&m_csIdleConnList);

	if (m_listIdleConnection.size() <= 1)
	{
		// ʣ��������ӵ���ĿС�ڵ���1��ʱ����Ҫ��鿪ʼ����
		if ((m_listIdleConnection.size() + m_listBusyConnection.size()) < m_nMaxCount)
		{
			// ��С��������ƣ����Դ���
			SetEvent(m_hHaveData);
			m_bNeedConnection = TRUE;
		}
		else
		{
			// ���������ˣ�������¼��
			g_pSvrLog->AddRunLog(LL_ERROR, "Database connection reached max count.");
		}
	}

	return pDBEngine;
}

int DBConnPool::RestoreAConnection(DBEngine* pDBEngine)
{
	if (NULL != pDBEngine)
	{
		// ��ʹ���еĶ���ȡ��
		EnterCriticalSection(&m_csBusyConnList);
		m_listBusyConnection.remove(pDBEngine);
		LeaveCriticalSection(&m_csBusyConnList);

		// ���뵽���ж�����
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
	// ��Ϊ�߳��������Ƶȴ��źŵģ����������Ȱѱ�־λ��Ϊֹͣ��Ȼ���ź����̼߳��
	SetEvent(m_hHaveData);

	// �ȴ��˳�
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
/* ά���߳�                                                             */
/************************************************************************/
DWORD WINAPI DBConnPool::thread_run( LPVOID pdata)
{
	DBConnPool * pConPool = (DBConnPool *) pdata;

	while (!pConPool->IsNeedStop())
	{
		// �����¼�Ϊ���ź�, ���������Ƶȴ�
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