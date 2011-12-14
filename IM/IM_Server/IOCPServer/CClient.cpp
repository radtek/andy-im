//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CClient.cpp : 客户端类的实现文件
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

extern CServerLog * g_pSvrLog;  // 日志文件全局变量
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
	// 从受消息的缓冲区放到命令队列
	if ((m_iMessageBufLen + iLength) > BIG_BUFFER)
	{
		g_pSvrLog->AddRunLog(LL_ERROR, "Msg Buffer Error, iLength = %d, m_iMessageBufLen = %d", iLength, m_iMessageBufLen);
        // 丢弃本次的数据，重新开始
		return;
	}

	// 不处理心跳和握手信息
	if (iLength == 6)
	{
		if ((m_szbuf[0] == CMD_REAL_HEAD) && (m_szbuf[5] == CMD_REAL_TAIL))
		{
			// 丢弃
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
	// 接收处理新来的数据
	memcpy(m_szMessageBuffer+m_iMessageBufLen, m_szbuf, iLength);

	m_iMessageBufLen += iLength;

	unsigned int iHeadPos = 0; // 帧头的位置
	unsigned int iTailPos = 0;
    unsigned int i = 0; //循环子

	// 下面分析消息队列里面的数据，找到第一个帧头，把前面的东西都丢弃
	for (; i <= m_iMessageBufLen; i++)
	{
		if (CMD_REAL_HEAD == m_szMessageBuffer[i])
		{
			iHeadPos = i;
			// 保证判断部分，判断一下后面还够不够用来分析的长度
			// 假设不包含数据的话，一帧消息也要至少15个字节，所以看看够不够
			if ((iHeadPos+15) > m_iMessageBufLen)
			{
                // 大于表明是不完整的数据，保留下来
				memmove(m_szMessageBuffer, m_szMessageBuffer+i, m_iMessageBufLen-i);
				break;
			}
			else
			{
				//根据消息帧的定义，数据长度的位置为从帧头起10~13个字节		
				int iMsgLength = (m_szMessageBuffer[i+10] << 24) & 0xff000000;
				iMsgLength |= (m_szMessageBuffer[i+11] << 16) & 0x00ff0000;
				iMsgLength |= (m_szMessageBuffer[i+12] << 8) & 0x0000ff00;
				iMsgLength |= (m_szMessageBuffer[i+13]) & 0x000000ff;

				if ((iMsgLength < 0) || (iMsgLength > BIG_BUFFER - 15))
				{
					// 数据长度小于0就是错误了,丢弃这段数据
					continue;
				}
				
				// 这样，帧尾的位置如下，如果没错就是完整的一帧
				iTailPos = i + 13 + iMsgLength + 1;
				
				// 这次判断到帧尾是不是在这次的缓冲区里面
				if (iTailPos > m_iMessageBufLen)
				{
					// 大于表明是不完整的数据，保留下来
					memmove(m_szMessageBuffer, m_szMessageBuffer+i, m_iMessageBufLen-i);
					break;
				}
				else
				{
					int iMsgLen = iTailPos - iHeadPos + 1;
					if (CMD_REAL_TAIL == m_szMessageBuffer[iTailPos])
					{
						// 没错，拷贝出来去解析吧
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
	// 第一次发包本身是以帧头为开始的
	if ((CMD_REAL_HEAD == m_szbuf[0]) && (m_ulBuflen > 5))
	{
		// 帧头后紧跟的int就是ClientID
		iClientID = (m_szbuf[1] << 24) & 0xff000000;
		iClientID |= (m_szbuf[2] << 16) & 0x00ff0000;
		iClientID |= (m_szbuf[3] << 8) & 0x0000ff00;
		iClientID |= (m_szbuf[4]) & 0x000000ff;
	}

	return iClientID;
}

void CClient::MutexBufferLock()
{
	/*// 无限制的等待
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