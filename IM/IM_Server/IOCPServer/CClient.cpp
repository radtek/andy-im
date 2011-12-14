//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CClient.cpp : �ͻ������ʵ���ļ�
//
// Author: Leo
// Date: 08/04/2008
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
#include "../HeaderFiles/stdafx.h"
#include "CClient.h"
#include "../LogFile/ServerLog.h"
#include <string>
#include <iostream>

extern CServerLog * g_pSvrLog;  // ��־�ļ�ȫ�ֱ���
CScheduler CClient::m_oScheduler;

CClient::CClient()
{
	m_iClentID = -1;
	m_skClient = INVALID_SOCKET;
	m_hPostAccept = INVALID_HANDLE_VALUE;
	m_eOperation = OP_ACCEPT;
	m_eStatus = stAccepting;
	m_bClosing = false;
	m_lOutstandingRecv = 0;
	m_lOutstandingSend = 0;
	m_lPendingSend = 0;

	memset(m_szbuf, 0, nMaxBuffer);
	m_muBufferMutex = CreateMutex(NULL, FALSE, "Buffer Mutex");

	memset(m_szMessageBuffer, 0, BIG_BUFFER);
	m_iMessageBufLen = 0;

	m_ulBuflen = nMaxBuffer;
	memset(&m_addrPeer, 0, sizeof(sockaddr_in));
	
	memset(&m_wsaOvLap, 0, sizeof(OVERLAPPED));
	
	InitializeCriticalSection(&m_csSockCritSec);
	//SetCriticalSectionSpinCount(&m_csSockCritSec, 4000);

}

CClient::~CClient()
{
    DeleteCriticalSection(&m_csSockCritSec);
	CloseHandle(m_muBufferMutex);
}

int CClient::Send(char* szSndBuf, int iLen)
{
	//m_eOperation = OP_WRITE;

    WSABUF wbuf;
    DWORD bytes = 0;
    int iRc = 0;
	int iErr = 0;
	
    wbuf.buf = szSndBuf;
    wbuf.len = iLen;
	
	iRc = send(m_skClient, szSndBuf, iLen, 0);
    /*iRc = WSASend(
		m_skClient,
		&wbuf,
		1,
		&bytes,
		0,
		&m_wsaOvLap,
		NULL
		);
	*/
    if (iRc == SOCKET_ERROR)
    {
        iRc = NO_ERROR;
        if ((iErr = WSAGetLastError()) != WSA_IO_PENDING)
        {
            if (iErr == WSAENOBUFS)
			{
                g_pSvrLog->AddRunLog(LL_DEBUG, "CClient::Send:WSANOBUFS");
				return 0;
			}
            iRc = SOCKET_ERROR;
        }
    }
	/*
    if (iRc == NO_ERROR)
    {
        // Increment the outstanding operation count
        InterlockedIncrement(&m_lOutstandingSend);
    }*/
    return iRc;
}

int CClient::GetClientID()
{
	return m_iClentID;
}


int CClient::PostRecv()
{
	WSABUF  wbuf;
    DWORD   dwBytes = 0;
	DWORD	dwFlags = 0;
	int iRc = 0;

	m_eOperation = OP_READ;
	//EnterCriticalSection(&m_csSockCritSec);
	memset(m_szbuf, 0, nMaxBuffer);
	wbuf.buf = m_szbuf;
	wbuf.len = m_ulBuflen;

	iRc = WSARecv(
		m_skClient,
		&wbuf,
		1,
		&dwBytes,
		&dwFlags,
		&m_wsaOvLap,
		NULL
		);
	//LeaveCriticalSection(&m_csSockCritSec);
	
	if (iRc == SOCKET_ERROR)
	{
		iRc = NO_ERROR;
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			g_pSvrLog->AddRunLog(LL_DEBUG, "CClient::PostRecv failed, err = %d", WSAGetLastError());
			iRc = SOCKET_ERROR;
		}
	}

	return iRc;
}

void CClient::SetClientID(int iClientID)
{
	MutexBufferLock();
	m_iClentID = iClientID;
	MutexBufferUnLock();
}

void CClient::AddNewPacket(int iLength)
{
	// ������Ϣ�Ļ������ŵ��������
	if ((m_iMessageBufLen + iLength) > BIG_BUFFER)
	{
		g_pSvrLog->AddRunLog(LL_ERROR, "Msg Buffer Error, iLength = %d, m_iMessageBufLen = %d", iLength, m_iMessageBufLen);
        // �������ε����ݣ����¿�ʼ
		return;
	}

	// ������������������Ϣ
	if (iLength == 6)
	{
		if ((m_szbuf[0] == CMD_REAL_HEAD) && (m_szbuf[5] == CMD_REAL_TAIL))
		{
			// ����
			return;
		}
	}
#ifdef _DEBUG
	g_pSvrLog->LogPrintf("client %d(%d.%d.%d.%d) send us %d \tbytes.\n",
		m_iClentID,
		m_addrPeer.sin_addr.S_un.S_un_b.s_b1,
		m_addrPeer.sin_addr.S_un.S_un_b.s_b2,
		m_addrPeer.sin_addr.S_un.S_un_b.s_b3,
		m_addrPeer.sin_addr.S_un.S_un_b.s_b4,
        iLength
		);
#endif
	// ���մ�������������
	memcpy(m_szMessageBuffer+m_iMessageBufLen, m_szbuf, iLength);

	m_iMessageBufLen += iLength;

	unsigned int iHeadPos = 0; // ֡ͷ��λ��
	unsigned int iTailPos = 0;
    unsigned int i = 0; //ѭ����

	// ���������Ϣ������������ݣ��ҵ���һ��֡ͷ����ǰ��Ķ���������
	for (; i <= m_iMessageBufLen; i++)
	{
		if (CMD_REAL_HEAD == m_szMessageBuffer[i])
		{
			iHeadPos = i;
			// ��֤�жϲ��֣��ж�һ�º��滹���������������ĳ���
			// ���費�������ݵĻ���һ֡��ϢҲҪ����15���ֽڣ����Կ���������
			if ((iHeadPos+15) > m_iMessageBufLen)
			{
                // ���ڱ����ǲ����������ݣ���������
				memmove(m_szMessageBuffer, m_szMessageBuffer+i, m_iMessageBufLen-i);
				break;
			}
			else
			{
				//������Ϣ֡�Ķ��壬���ݳ��ȵ�λ��Ϊ��֡ͷ��10~13���ֽ�		
				int iMsgLength = (m_szMessageBuffer[i+10] << 24) & 0xff000000;
				iMsgLength |= (m_szMessageBuffer[i+11] << 16) & 0x00ff0000;
				iMsgLength |= (m_szMessageBuffer[i+12] << 8) & 0x0000ff00;
				iMsgLength |= (m_szMessageBuffer[i+13]) & 0x000000ff;

				if ((iMsgLength < 0) || (iMsgLength > BIG_BUFFER - 15))
				{
					// ���ݳ���С��0���Ǵ�����,�����������
					continue;
				}
				
				// ������֡β��λ�����£����û�����������һ֡
				iTailPos = i + 13 + iMsgLength + 1;
				
				// ����жϵ�֡β�ǲ�������εĻ���������
				if (iTailPos > m_iMessageBufLen)
				{
					// ���ڱ����ǲ����������ݣ���������
					memmove(m_szMessageBuffer, m_szMessageBuffer+i, m_iMessageBufLen-i);
					break;
				}
				else
				{
					int iMsgLen = iTailPos - iHeadPos + 1;
					if (CMD_REAL_TAIL == m_szMessageBuffer[iTailPos])
					{
						// û����������ȥ������
						BYTE byBuff[nMaxBuffer] = {0};
						memcpy(byBuff, m_szMessageBuffer + i, iMsgLen);				
						MessageDispatch(byBuff, iMsgLen);
						i += iMsgLen - 1;
					}
				}
			}
		}
	}
	if (m_iMessageBufLen > i)
	{
		m_iMessageBufLen -= i;
	}
	else
	{
        m_iMessageBufLen = 0;
		//memset(m_szMessageBuffer, 0, BIG_BUFFER);
	}

	if (BIG_BUFFER < m_iMessageBufLen)
	{
		m_iMessageBufLen = 0;
	}
}

void CClient::MessageDispatch(BYTE* pBuffer, int iLen)
{
	m_oScheduler.MessageDispatch(pBuffer, iLen);
}

int CClient::GetRawClientID()
{
	int iClientID = -1;
	// ��һ�η�����������֡ͷΪ��ʼ��
	if ((CMD_REAL_HEAD == m_szbuf[0]) && (m_ulBuflen > 5))
	{
		// ֡ͷ�������int����ClientID
		iClientID = (m_szbuf[1] << 24) & 0xff000000;
		iClientID |= (m_szbuf[2] << 16) & 0x00ff0000;
		iClientID |= (m_szbuf[3] << 8) & 0x0000ff00;
		iClientID |= (m_szbuf[4]) & 0x000000ff;
	}

	return iClientID;
}

void CClient::MutexBufferLock()
{
	/*// �����Ƶĵȴ�
    WaitForSingleObject(m_muBufferMutex, INFINITE);*/
	EnterCriticalSection(&m_csSockCritSec);
}

void CClient::MutexBufferUnLock()
{
	/*ReleaseMutex(m_muBufferMutex);*/
	LeaveCriticalSection(&m_csSockCritSec);
}

void CClient::ClearAllData()
{
	MutexBufferLock();
	m_iClentID = -1;

	if (INVALID_SOCKET != m_skClient)
	{
		closesocket(m_skClient);
		m_skClient = INVALID_SOCKET;
	}
	
	m_eOperation = OP_ACCEPT;
	m_eStatus = stAccepting;
	m_bClosing = false;
	m_lOutstandingRecv = 0;
	m_lOutstandingSend = 0;
	m_lPendingSend = 0;
	
	memset(m_szbuf, 0, nMaxBuffer);
	memset(m_szMessageBuffer, 0, BIG_BUFFER);
	m_iMessageBufLen = 0;
	
	m_ulBuflen = nMaxBuffer;
	memset(&m_addrPeer, 0, sizeof(sockaddr_in));
	MutexBufferUnLock();
}

void CClient::SetAcceptEvent(HANDLE hAcceptEvent)
{
	MutexBufferLock();
	m_hPostAccept = hAcceptEvent;
	MutexBufferUnLock();
}