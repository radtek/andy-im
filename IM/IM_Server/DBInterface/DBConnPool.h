// DBConnPool.h: interface for the DBConnPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBCONNPOOL_H__FE67E781_AD8A_4561_9519_3C2C2D0B63AF__INCLUDED_)
#define AFX_DBCONNPOOL_H__FE67E781_AD8A_4561_9519_3C2C2D0B63AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <list>
#include <string>
#include "DBEngine.h"
#include "../BaseThread/commThread.H"


using std::string;
typedef std::list<DBEngine*> DBConnList;

class ConnGuard;

// 
class DBConnPool
{
public:
	virtual ~DBConnPool();

	// ��ȡʵ��ָ��
	static DBConnPool * Instanse();

	// ��ʼ����������
	int InitializeAllDBConnections();
	
	// �ر���������
	void DestroyAllDBConnections();

private:
	// Ĭ���޲������캯��
	DBConnPool();

	// ��ʵ�������ӳ�Ψһ��
	DBConnPool(LPCTSTR dbservername, LPCTSTR dbname, LPCTSTR dbuser, LPCTSTR dbpass, LPCTSTR dblanguage);

	// ����һ������
	int InitializeAConnection();

	// �ر�һ������
	void CloseAConnection(DBEngine* pDBEngine);

	// ��ȡһ����������
	DBEngine* GetAConnection();
	
	// �������Ӹ����ж���
	int RestoreAConnection(DBEngine* pDBEngine);

	// ֹͣ�����߳�
	void StopThread();

	// �ж��Ƿ���Ҫֹͣ
	BOOL IsNeedStop();
	BOOL IsNeedConnection();

	// ����������Ϊ���ӳ������Ԫ��
	friend class ConnGuard;

	// Ψһʵ��
	static DBConnPool *m_pInstanse;

	// �������ݿ����Ӷ���
	DBConnList m_listIdleConnection;
	// ��ʹ�õ����ݿ�����
	DBConnList m_listBusyConnection;

	// ���б������ٽ���
	CRITICAL_SECTION m_csIdleConnList;
	CRITICAL_SECTION m_csBusyConnList;

	// ������������������ָ�꣺�����С
	int m_nMaxCount;
	int m_nMinCount;

	// ���ݿ���Ϣ
	string m_strServerName;
	string m_strDBName;
	string m_strUser;
	string m_strPass;
	string m_strLanguage;
	string m_strModuleName;

	// ά���߳�
	HANDLE m_hMaintanceThread; // �߳̾��
	HANDLE m_hHaveData; // �ź�

	BOOL m_bNeedStop; // �����߳���ͣ�ı�־λ
	BOOL m_bNeedConnection; // ��Ҫ�������ӵı�־
	static	DWORD	WINAPI	thread_run( LPVOID pdata);
};

// �����࣬���ù��������������֤����ȡ���͹黹����ɶԣ���ֹ��Դй¶
class ConnGuard
{
public:
	ConnGuard(DBEngine*& dbEngine)
	{
        dbEngine = DBConnPool::Instanse()->GetAConnection();
		m_pEngine = dbEngine;
	}
	virtual ~ConnGuard()
	{
		DBConnPool::Instanse()->RestoreAConnection(m_pEngine);
	}
	
private:
	DBEngine *m_pEngine;
};


#endif // !defined(AFX_DBCONNPOOL_H__FE67E781_AD8A_4561_9519_3C2C2D0B63AF__INCLUDED_)
