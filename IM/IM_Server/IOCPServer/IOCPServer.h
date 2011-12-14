//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// MyClass.h: 完成端口管理类，创建和关闭完成端口，提供主消息循环的入口
//
// Author: Leo
// Date: 08/02/2008
//
////////////////////////////////////////////////////////////////////// 
#ifndef _IOCP_SERVER_H_
#define _IOCP_SERVER_H_
#include <vector>
#include "../IOCPServer/CClientMgr.h"
#include "../BaseThread/commThread.H"
#include "../IOCPServer/IOCPThread.h"



#define MAX_COMPLETION_THREAD_COUNT 32
#define BURST_ACCEPT_COUNT          100
#define MAX_OVERLAPPED_ACCEPTS      5000

typedef std::vector<CIOCPThread*> CPThreadVec;

struct ThreadStopNotifier : public unary_function<CIOCPThread*, void>
{
	result_type operator()(argument_type pThread) const
	{
		if (pThread)
		{
			pThread->NotifyStop();
		}
	}
};

// struct ThreadStartNotifier : public unary_function<CRunTimeThread*, void>
// {
// 	result_type operator()(argument_type pThread) const
// 	{
// 		if (pThread)
// 		{
// 			pThread->Start();
// 		}
// 	}
// };

class CIOCPServer
{
public:
    CIOCPServer();
	virtual ~CIOCPServer();
	BOOL InitializeService();
	
	void DestroyService();
	
	void MainLoop();

	int PostAccept(CClient* pClient);
protected:
private:
	WORD m_wServerPort;
	HANDLE m_hCompletionPort;
	SOCKET m_skListener;
	int m_iOverLapperCount;
	CPThreadVec m_vecThread;
	HANDLE m_hRepostAccept;
	HANDLE m_hAcceptEvent;
	HANDLE m_hWaitEvents[MAX_COMPLETION_THREAD_COUNT];
	int m_iWatiCount;
	int m_iCompThreadCount;
	CRITICAL_SECTION m_csListenPort;

};

#endif