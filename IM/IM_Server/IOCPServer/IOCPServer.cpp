//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// MyClass.h: ��ɶ˿ڹ����ʵ����
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


extern CServerLog * g_pSvrLog;  // ��־�ļ�ȫ�ֱ���
extern CClientMgr * g_pClientMgr;  // ȫ�ֿͻ��˹���ģ��ָ��
extern CINIFile *g_pIniFile;    //�����ļ�ȫ�ֱ���


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

	// ��ѯ������CPU����������ȷ����ɶ˿���Ҫ���е��߳���Ŀ
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
	// �������һ��API�ķ���ֵ
	int iResult = -1;
	// ����������������

	
	// ����ȡ�õ�ǰ��Socketʵ�ֵ���ϸ��Ϣ�Ľṹ
	WSADATA wsd;

	// ��ʼ��Socket��̬��
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		g_pSvrLog->AddTextLog(LL_DEBUG, "unable to load Winsock!");
		return FALSE;
	}

	// ������ɶ˿�
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)NULL, 0);
	if (NULL == m_hCompletionPort)
	{
        g_pSvrLog->AddRunLog(LL_DEBUG, "CIOCPServer::InitializeService:CreateIoCompletionPort, error code : %d", GetLastError());
		return FALSE;
	}

	// ��ʼ���������׽���
	SOCKADDR_IN addr;
	m_skListener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons((short)m_wServerPort);
	
	// ���׽�����ָ����ַ��
	iResult = bind( m_skListener, (SOCKADDR *)&addr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == iResult)
	{
        g_pSvrLog->AddRunLog(LL_DEBUG, "CIOCPServer::InitializeService:bind, error code : %d", WSAGetLastError());
		return FALSE;
	}
	
	// ���ø��׽���Ϊ����ģʽ
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

	// �����趨������Ԥ�ȷ���OverLapped�ṹ��AcceptEx����������������
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
	// �������׽����봴���õ���ɶ˿ڰ�
	HANDLE hResult = CreateIoCompletionPort( (HANDLE) m_skListener, m_hCompletionPort, 0, 0);
	if (NULL == hResult)
	{
		g_pSvrLog->AddRunLog(LL_DEBUG, "CIOCPServer::InitializeService:BindIoCompletionPort, error code : %d", GetLastError());
		return FALSE;
	}


	// ������ɶ˿ڵĹ����߳�, Ĭ����CPU�������߳������ɣ�����ʵ��������̵߳Ĺ������������̵߳ĵȴ���Ϊ�˴ﵽ��CPU��Դ�ĳ������
	// �������Ϊ(CPU���� * 2 +2)��
	for(int j = 0; j < m_iCompThreadCount; j++)
	{
		//���������̣߳������뵽һ���߳�����
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
                //   a denial of service attack. (DoS �������������������Ӻ�ը�������Ϸ������)
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
	
	// ����������¼����
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