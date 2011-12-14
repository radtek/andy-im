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

	// 获取实例指针
	static DBConnPool * Instanse();

	// 初始化所有连接
	int InitializeAllDBConnections();
	
	// 关闭所有连接
	void DestroyAllDBConnections();

private:
	// 默认无参数构造函数
	DBConnPool();

	// 单实例让连接池唯一化
	DBConnPool(LPCTSTR dbservername, LPCTSTR dbname, LPCTSTR dbuser, LPCTSTR dbpass, LPCTSTR dblanguage);

	// 创建一个连接
	int InitializeAConnection();

	// 关闭一个连接
	void CloseAConnection(DBEngine* pDBEngine);

	// 获取一个空闲连接
	DBEngine* GetAConnection();
	
	// 交还连接给空闲队列
	int RestoreAConnection(DBEngine* pDBEngine);

	// 停止工作线程
	void StopThread();

	// 判断是否需要停止
	BOOL IsNeedStop();
	BOOL IsNeedConnection();

	// 将守卫类作为连接池类的友元类
	friend class ConnGuard;

	// 唯一实例
	static DBConnPool *m_pInstanse;

	// 空闲数据库连接队列
	DBConnList m_listIdleConnection;
	// 在使用的数据库连接
	DBConnList m_listBusyConnection;

	// 队列保护的临界区
	CRITICAL_SECTION m_csIdleConnList;
	CRITICAL_SECTION m_csBusyConnList;

	// 可用连接总数的三个指标：最大、最小
	int m_nMaxCount;
	int m_nMinCount;

	// 数据库信息
	string m_strServerName;
	string m_strDBName;
	string m_strUser;
	string m_strPass;
	string m_strLanguage;
	string m_strModuleName;

	// 维护线程
	HANDLE m_hMaintanceThread; // 线程句柄
	HANDLE m_hHaveData; // 信号

	BOOL m_bNeedStop; // 管理线程起停的标志位
	BOOL m_bNeedConnection; // 需要创建连接的标志
	static	DWORD	WINAPI	thread_run( LPVOID pdata);
};

// 守卫类，利用构造和析构函数保证连接取出和归还必须成对，防止资源泄露
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
