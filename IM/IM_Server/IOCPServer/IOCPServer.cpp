//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// MyClass.h: 完成端口管理的实现类
//
// Author: Leo
// Date: 08/02/2008
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
#include "../HeaderFiles/stdafx.h"
#include "IOCPServer.h"
#include "../LogFile/ServerLog.h"
#include "../INIFile/INIFile.h"
#include "../IOCPServer/IOCPThread.h"
#include <algorithm>
#include <mswsock.h>


extern CServerLog * g_pSvrLog;  // 日志文件全局变量
extern CClientMgr * g_pClientMgr;  // 全局客户端管理模块指针
extern CINIFile *g_pIniFile;    //配置文件全局变量


CIOCPServer::CIOCPServer()
{
	std::string strTempPort = "";
	g_pIniFile->GetLocalPort(strTempPort);

	m_wServerPort = (WORD)atoi(strTempPort.c_str());
	m_hCompletionPort = INVALID_HANDLE_VALUE;
	m_skListener = INVALID_SOCKET;

	strTempPort = "";
	g_pIniFile->GetDefaultOverlapped(strTempPort);
    m_iOverLapperCount = atoi(strTempPort.c_str());
	m_hRepostAccept = INVALID_HANDLE_VALUE;
	m_hAcceptEvent = INVALID_HANDLE_VALUE;
	m_iWatiCount = 0;

	// 查询机器的CPU个数，用来确定完成端口需要运行的线程数目
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	m_iCompThreadCount = (int)sysinfo.dwNumberOfProcessors * 2 + 2;

	InitializeCriticalSection(&m_csListenPort);
}

CIOCPServer::~CIOCPServer()
{
    DeleteCriticalSection(&m_csListenPort);
}

BOOL CIOCPServer::InitializeService()
{
	// 用来存放一般API的返回值
	int iResult = -1;
	// 用来组合日至的语句

	
	// 用来取得当前的Socket实现的详细信息的结构
	WSADATA wsd;

	// 初始化Socket动态库
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		g_pSvrLog->AddTextLog(LL_DEBUG, "unable to load Winsock!");
		return FALSE;
	}

	// 创建完成端口
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)NULL, 0);
	if (NULL == m_hCompletionPort)
	{
        g_pSvrLog->AddRunLog(LL_DEBUG, "CIOCPServer::InitializeService:CreateIoCompletionPort, error code : %d", GetLastError());
		return FALSE;
	}

	// 开始创建监听套接字
	SOCKADDR_IN addr;
	m_skListener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons((short)m_wServerPort);
	
	// 将套接字与指定地址绑定
	iResult = bind( m_skListener, (SOCKADDR *)&addr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == iResult)
	{
        g_pSvrLog->AddRunLog(LL_DEBUG, "CIOCPServer::InitializeService:bind, error code : %d", WSAGetLastError());
		return FALSE;
	}
	
	// 设置该套接字为监听模式
	iResult = listen( m_skListener, 20);
	if (SOCKET_ERROR == iResult)
	{
		g_pSvrLog->AddRunLog(LL_DEBUG, "CIOCPServer::InitializeService:listen, error code : %d", WSAGetLastError());
		return FALSE;
	}

	// Create an event to register for FD_ACCEPT events on
	m_hAcceptEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hAcceptEvent == NULL)
	{
		g_pSvrLog->AddRunLog(LL_DEBUG, "CreateEvent failed: %d", GetLastError());
		return FALSE;
	}
	
	m_hRepostAccept = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hRepostAccept == NULL)
	{
		g_pSvrLog->AddRunLog(LL_DEBUG, "CreateSemaphore failed: %d", GetLastError());
		return FALSE;
    }

	m_hWaitEvents[m_iWatiCount++] = m_hAcceptEvent;
	m_hWaitEvents[m_iWatiCount++] = m_hRepostAccept;

	// Register for FD_ACCEPT notification on listening socket
	iResult = WSAEventSelect(m_skListener, m_hAcceptEvent, FD_ACCEPT);
	if (iResult == SOCKET_ERROR)
	{
		g_pSvrLog->AddRunLog(LL_DEBUG, "WSAEventSelect failed: %d", WSAGetLastError());
		return FALSE;
	}

	// 根据设定的上限预先分配OverLapped结构，AcceptEx用来接收连接请求
	for (int i = 0; i < m_iOverLapperCount; ++ i)
	{
		CClient * pClient = new CClient;
		if (NULL != pClient)
		{
			pClient->SetAcceptEvent(m_hAcceptEvent);
			g_pClientMgr->InsertPendingAccept(pClient);
			
		    PostAccept(pClient);
		}
	}
	// 将监听套接字与创建好的完成端口绑定
	HANDLE hResult = CreateIoCompletionPort( (HANDLE) m_skListener, m_hCompletionPort, 0, 0);
	if (NULL == hResult)
	{
		g_pSvrLog->AddRunLog(LL_DEBUG, "CIOCPServer::InitializeService:BindIoCompletionPort, error code : %d", GetLastError());
		return FALSE;
	}


	// 创建完成端口的工作线程, 默认是CPU个数的线程数即可，但是实际情况下线程的工作可能引起线程的等待，为了达到对CPU资源的充分利用
	// 经验个数为(CPU个数 * 2 +2)个
	for(int j = 0; j < m_iCompThreadCount; j++)
	{
		//创建工作线程，并加入到一个线程数组
        m_vecThread.push_back(new CIOCPThread(m_hCompletionPort, m_skListener, m_hRepostAccept));
        m_vecThread[j]->Start();
	}
	//for_each(m_vecThread.end(),m_vecThread.begin(),ThreadStartNotifier());

	return TRUE;
}

void CIOCPServer::MainLoop()
{
	int iReturnCode = -1;
	int iInterval = 0;
	while (TRUE)
	{
		iReturnCode = WSAWaitForMultipleEvents(
			m_iWatiCount,
			m_hWaitEvents,
			FALSE,
			5000,
			FALSE
			);

        if (iReturnCode == WAIT_FAILED)
        {
			g_pSvrLog->LogPrintf("WSAWaitForMultipleEvents failed: %d", WSAGetLastError());
            continue;
        }
        else if (iReturnCode == WAIT_TIMEOUT)
        {
            iInterval++;

			g_pSvrLog->LogPrintf("\n");
			g_pSvrLog->LogPrintf("Current Client Count : %d\n", g_pClientMgr->m_lRegClientCount);
			g_pSvrLog->LogPrintf("Current Raw Client Count : %d\n", g_pClientMgr->m_lRawClientCount);
			g_pSvrLog->LogPrintf("Current Pending Accept Count :%d\n", g_pClientMgr->m_lPendingAcceptCount);
            if (iInterval == 36)
            {
                // For TCP, cycle through all the outstanding AcceptEx operations
                //   to see if any of the client sockets have been connected but
                //   haven't received any data. If so, close them as they could be
                //   a denial of service attack. (DoS 攻击，常见的网络连接轰炸，针对游戏服务器)
				g_pClientMgr->PollPendingAccept(iReturnCode);
                iInterval = 0;
            }
        }
        else
        {
            int index = 0;
			
            index = iReturnCode - WAIT_OBJECT_0;
			
            for( ; index < m_iWatiCount ; index++)
            {
                iReturnCode = WaitForSingleObject(m_hWaitEvents[index], 0);
                if (iReturnCode == WAIT_FAILED || iReturnCode == WAIT_TIMEOUT)
                {
                    continue;
                }
				
				WSANETWORKEVENTS ne;
				int              limit = 0;
				
				if (m_hAcceptEvent == m_hWaitEvents[index])
				{
					// EnumNetworkEvents to see if FD_ACCEPT was set
					iReturnCode = WSAEnumNetworkEvents(
						m_skListener,
						m_hAcceptEvent,
						&ne
						);
					
					if (iReturnCode == SOCKET_ERROR)
					{
						g_pSvrLog->LogPrintf("WSAEnumNetworkEvents failed: %d",	WSAGetLastError());
					}
					if ((ne.lNetworkEvents & FD_ACCEPT) == FD_ACCEPT)
					{
						// We got an FD_ACCEPT so post multiple accepts to 
						// cover the burst
						limit = BURST_ACCEPT_COUNT;
					}
				}
				else if (m_hRepostAccept == m_hWaitEvents[index])
				{
					// Semaphore is signaled
					limit = InterlockedExchange(&g_pClientMgr->m_lRepostCount, 0);
					
					ResetEvent(m_hRepostAccept);
				}
				
				int i = 0;
				while ( (i < limit) && (g_pClientMgr->m_lPendingAcceptCount < MAX_OVERLAPPED_ACCEPTS) )
				{
					CClient *pClient = NULL;
					pClient = g_pClientMgr->GetPendingAccept();

					if (NULL != pClient)
					{
						pClient->SetAcceptEvent(m_hAcceptEvent);
						g_pClientMgr->InsertPendingAccept(pClient);

						PostAccept(pClient);
						i++;
					}
					else
					{
                        g_pSvrLog->AddRunLog(LL_DEBUG, "CIOCPServer::MainLoop: insufficient memory , error %d", GetLastError());
						Sleep(1000);
					}
				}
            }
        }
    }
	
    DestroyService();
}

int CIOCPServer::PostAccept(CClient* pClient)
{
	DWORD   dwBytes = 0;
    int     iReturnCode = 0;
	const int iAddrlen = sizeof(sockaddr_in) + 16;

    pClient->m_eOperation = OP_ACCEPT;
	
    // Create the client socket for an incoming connection
    pClient->m_skClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (pClient->m_skClient == INVALID_SOCKET)
    {
		g_pSvrLog->LogPrintf("PostAccept: socket failed: %d", WSAGetLastError());
        return -1;
    }
	int zero = 0;
    setsockopt( pClient->m_skClient, IPPROTO_TCP, TCP_NODELAY, (char*)&zero, sizeof(int));

    iReturnCode = AcceptEx(
		m_skListener,
		pClient->m_skClient,
		pClient->m_szbuf,
		pClient->m_ulBuflen - ((sizeof(SOCKADDR_IN) + 16) * 2),
		iAddrlen,
		iAddrlen,
		&dwBytes,
		&pClient->m_wsaOvLap
		);
    if (iReturnCode == FALSE)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
			g_pSvrLog->AddRunLog(LL_DEBUG, "PostAccept: AcceptEx failed: %d",	WSAGetLastError());
            return SOCKET_ERROR;
        }
    }
	
	pClient->m_lOutstandingRecv = dwBytes;
    return NO_ERROR;
}

void CIOCPServer::DestroyService()
{
	// no more accepts, please
	closesocket( m_skListener );
	m_skListener = INVALID_SOCKET;
	
	// queue suicide packets
	for (int i = 0; i < m_iCompThreadCount; ++ i )
	{
		PostQueuedCompletionStatus( m_hCompletionPort, 0, COMPKEY_DIE, 0 );
	}
	
	// 清理掉两个事件句柄
	for (int j = 0; j < m_iWatiCount; j++)
	{
		CloseHandle(m_hWaitEvents[j]);
		m_hWaitEvents[j] = INVALID_HANDLE_VALUE;
	}

	// wait for thread terminations
	for_each(m_vecThread.end(),m_vecThread.begin(),ThreadStopNotifier());	
	m_vecThread.empty();
	
	// destroy the ClientMgr
    g_pClientMgr->DestroyAllClient();

	// shut down winsock
	WSACleanup();
	
	// close IOCP and suicide flag	
	CloseHandle( m_hCompletionPort );	

}