//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// MyClass.h: ��ɶ˿ڵĹ����߳����ʵ���ļ�
//
// Author: Leo
// Date: 08/02/2008
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
#include "IOCPThread.h"
#include "../HeaderFiles/stdafx.h"
#include "../HeaderFiles/common.h"
#include "../LogFile/ServerLog.h"
#include "../DBInterface/DBInterface.h"
#include "../MainFlow/CommonQueue.h"

#include <mswsock.h>
#include <assert.h>

extern CServerLog * g_pSvrLog;  // ��־�ļ�ȫ�ֱ���
extern CClientMgr * g_pClientMgr; // ȫ�ֿͻ��˹���
extern DBInterface *g_pDBInterface;//���ݿ�ӿ�ȫ��ָ��
extern CGlobalSendQueue *g_pPendingSendQueue; // ȫ�ִ����͸��ͻ��˵���Ϣ����

CIOCPThread::CIOCPThread(HANDLE hIOCP, SOCKET skListener, HANDLE hAcceptEvent)
{
	m_hIOCP = hIOCP;
	m_skListener = skListener;
	m_hAcceptEvent = hAcceptEvent;
}

CIOCPThread::~CIOCPThread()
{

}

void CIOCPThread::Run()
{
	ULONG Key;
    SOCKET       sock;
    OVERLAPPED  *lpOverlapped = NULL;     // Pointer to overlapped structure for completed I/O
    DWORD        dwBytesTransfered = 0;       // Number of bytes transfered
	DWORD        dwFlags = 0;                 // Flags for completed I/O
    int          iReturnCode = 0; 
    int 		 iError = 0;
	
    while (!IsNeedStop())
    {
        iError = NO_ERROR;
        iReturnCode = GetQueuedCompletionStatus(
			m_hIOCP,
			&dwBytesTransfered,
			&Key,
			&lpOverlapped,
			INFINITE
			);
		// check if we are to die
		if ( Key == COMPKEY_DIE )
		{
			break; // sniffle
		}
		
        CClient *pClient = CONTAINING_RECORD(lpOverlapped, CClient, m_wsaOvLap);

        if (iReturnCode == FALSE)
        {
            // If the call fails, call WSAGetOverlappedResult to translate the
            //    error code into a Winsock error code.
            if (pClient->m_eOperation == OP_ACCEPT)
            {
                sock = m_skListener;
            }
            else
            {
                sock = pClient->m_skClient;
            }
			
            iReturnCode = WSAGetOverlappedResult(
				sock,
				&pClient->m_wsaOvLap,
				&dwBytesTransfered,
				FALSE,
				&dwFlags
				);
            if (iReturnCode == FALSE)
            {
                iError = WSAGetLastError();
				if ((iError == ERROR_OPERATION_ABORTED) || (iError == ERROR_IO_INCOMPLETE))
				{
					continue;
				}
            }
			if ((WSAECONNABORTED != GetLastError()) && (WSAECONNRESET != GetLastError()))
			{
				g_pSvrLog->AddRunLog(LL_DEBUG, "CompletionThread: GetQueuedCompletionStatus failed: %d [0x%x]",
					GetLastError(), lpOverlapped->Internal);
			}
        }
		
        // Handle the IO operation
        HandleIO(pClient, dwBytesTransfered, iError);
    }
	
    ExitThread(0);
}

void CIOCPThread::HandleIO(CClient*& pClient, DWORD dwTransfered, int iError)
{
    BOOL bCleanupSocket = FALSE;
	
    if (iError != 0)
    {
        g_pSvrLog->LogPrintf("OP = %d; Error = %d", pClient->m_eOperation, iError);
    }
	
    if (iError != NO_ERROR)
    {
        // An error occured on a TCP socket, free the associated per I/O buffer
        // and see if there are any more outstanding operations. If so we must
        // wait until they are complete as well.
        //
        if (pClient->m_eOperation != OP_ACCEPT)
        {
            if (pClient->m_eOperation == OP_READ)
            {
                if ((InterlockedDecrement(&pClient->m_lOutstandingRecv) == 0) &&
                    (pClient->m_lOutstandingSend == 0) )
                {
					g_pSvrLog->AddRunLog(LL_DEBUG, "Freeing socket obj in GetOverlappedResult");
                    FreeClient(pClient);
                }
            }
            else if (pClient->m_eOperation == OP_WRITE)
            {
                if ((InterlockedDecrement(&pClient->m_lOutstandingSend) == 0) &&
                    (pClient->m_lOutstandingRecv == 0) )
                {
					g_pSvrLog->AddRunLog(LL_DEBUG, "Freeing socket obj in GetOverlappedResult");
                    FreeClient(pClient );
                }
            }
        }
        else
        {
			if ((WSAECONNABORTED != GetLastError()) && (WSAECONNRESET != GetLastError()))
			{
				g_pSvrLog->LogPrintf("Accept failed");
			}
        }
		
        FreeClient(pClient);
	    return;
    }
	else
	{
		switch (pClient->m_eOperation)
		{
		case OP_ACCEPT:
			Accept(dwTransfered, pClient, iError, bCleanupSocket);
			break;
		case OP_READ:
			Read(dwTransfered, pClient, iError, bCleanupSocket);
			break;
		case OP_WRITE:
			Write(dwTransfered, pClient, iError, bCleanupSocket);
			break;
		default:
			break;
		}
	}

	if (pClient)
    {
        if (iError != NO_ERROR)
        {
			g_pSvrLog->LogPrintf("CIOCPThread::HandleIO: err = %d", iError);
            pClient->m_eStatus = stClosed;
        }
		
        //
        // Check to see if socket is closing
        //

        if (bCleanupSocket)
        {
            FreeClient(pClient);
        }
    }
}

void CIOCPThread::FreeClient(CClient*& pClient)
{
	assert(NULL != pClient);

	// ͳһ����,��ɾ��������������,�ڰ�ָ�뱾��ɾ��
	if (-1 != g_pClientMgr->DistroyClient(pClient))
	{
		// ��Ϊ�µ�Accept����ʹ��
		g_pClientMgr->InsertPendingAccept(pClient);
	}

}

void CIOCPThread::Accept(DWORD dwTransfered, CClient *pClient, int iError, BOOL& bClean)
{
	
	HANDLE hReturnCode = INVALID_HANDLE_VALUE;
	sockaddr_in *LocalSockaddr = NULL;
	sockaddr_in *RemoteSockaddr = NULL;
	const int addrlen = sizeof sockaddr_in + 16;
	int iLocalSockaddrLen = 0;
	int	iRemoteSockaddrLen = 0;
	
	pClient->MutexBufferLock();
	// Print the client's addresss
	GetAcceptExSockaddrs(
		pClient->m_szbuf,
		pClient->m_ulBuflen - ((sizeof(SOCKADDR_IN) + 16) * 2),
		addrlen,
		addrlen,
		(SOCKADDR **)&LocalSockaddr,
		&iLocalSockaddrLen,
		(SOCKADDR **)&RemoteSockaddr,
		&iLocalSockaddrLen
		);
	
	// Associate the new connection to our completion port
	hReturnCode = CreateIoCompletionPort(
		(HANDLE)pClient->m_skClient,
		m_hIOCP,
		0,
		0
		);
	pClient->MutexBufferUnLock();
	if (hReturnCode == NULL)
	{
		g_pSvrLog->AddRunLog(LL_DEBUG, "CompletionThread: CreateIoCompletionPort failed: %d", WSAGetLastError());
		iError = -1;
		return;
	}
	g_pClientMgr->RemovePendingAccept(pClient, RemoteSockaddr);	

	// δע��Ŀͻ��ˣ��ٳ���ע��һ�Σ����ʧ�ܾͶϿ�
	if (-1 == g_pClientMgr->RegClient(pClient))
	{
		g_pClientMgr->DeleteFromRawList(pClient);
		g_pClientMgr->InsertPendingAccept(pClient);
		iError = -1;
		return;
	}
	
	// �ͻ�������,��������״̬
	//char szSQL[1024] = {0};
	//sprintf(szSQL, "UPDATE School_Info SET status=1 WHERE id=%d", pClient->GetClientID());
    //g_pDBInterface->ExecSQL(szSQL);
	g_pDBInterface->SetLoginState(pClient->GetClientID(),1);
	char szSQL[1024] = {0};
	sprintf(szSQL, "UPDATE friend SET friendonline=1 WHERE friendimno=%d", pClient->GetClientID());
	g_pDBInterface->ExecSQL(szSQL);


	//���º����б��е�friendonline�ֶ�
    //����������Ϣ�����������ߵ������ͻ���
	g_pClientMgr->SendFriendLoginMsg(pClient->GetClientID());



	pClient->MutexBufferLock();
	pClient->m_lOutstandingRecv = dwTransfered;
	pClient->AddNewPacket(dwTransfered);
	iError = pClient->PostRecv();
	pClient->MutexBufferUnLock();
	
	if (iError != NO_ERROR)
	{
		// Check for socket closure
		pClient->m_eStatus = stClosed;
		bClean = TRUE;
	}
	
	InterlockedIncrement(&g_pClientMgr->m_lRepostCount);
	SetEvent(m_hAcceptEvent);
}

void CIOCPThread::Read(DWORD dwTransfered, CClient *pClient, int iError, BOOL& bClean)
{
	InterlockedDecrement(&pClient->m_lOutstandingRecv);
	
	//
	// Receive completed successfully
	//
	if (dwTransfered > 0)
	{
		if (-1 == pClient->GetClientID())
		{
			// δע��Ŀͻ��ˣ��ٳ���ע��һ�Σ����ʧ�ܾͶϿ�
			g_pSvrLog->AddRunLog(LL_INFO, "UnRecognise Client, discard the msg");
			iError = -1;
			return;
		}

		pClient->MutexBufferLock();
		pClient->m_lOutstandingRecv = dwTransfered;
    	pClient->AddNewPacket(dwTransfered);
		iError = pClient->PostRecv();
		pClient->MutexBufferUnLock();
	}
	else
	{
		// Graceful close - the receive returned 0 bytes read
		pClient->MutexBufferLock();
		pClient->m_eStatus = stClosed;
		pClient->MutexBufferUnLock();
		
		bClean = TRUE;
	}

	if ( (pClient->m_lOutstandingSend == 0) &&
		(pClient->m_lOutstandingRecv == 0) &&
		(pClient->m_eStatus != stClosed) )
	{
		bClean = TRUE;
    }

}

void CIOCPThread::Write(DWORD dwTransfered, CClient *pClient, int iError, BOOL& bClean)
{
	InterlockedDecrement(&pClient->m_lOutstandingSend);
	
	pClient->MutexBufferLock();
	if (pClient->m_eStatus != stClosed)
	{
		iError = pClient->PostRecv();
	}
	pClient->MutexBufferUnLock();
}


