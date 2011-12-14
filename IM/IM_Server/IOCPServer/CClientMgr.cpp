//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CClientMgr.cpp : 客户端管理类类的实现文件
//
// Author: Leo
// Date: 08/04/2008
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
#include "../HeaderFiles/stdafx.h"
#include <time.h>
#include "CClientMgr.h"
#include <mswsock.h>
#include "../LogFile/ServerLog.h"
#include "../HeaderFiles/comm_buff.h"
#include "../HeaderFiles//common.h"
#include "../DBInterface/DBInterface.h"
#include "../MainFlow/CommonQueue.h"

extern CServerLog * g_pSvrLog;  // 日志文件全局变量
extern DBInterface *g_pDBInterface;//数据库接口全局指针
extern CGlobalSendQueue *g_pPendingSendQueue; // 全局待发送给客户端的消息队列


CClientMgr::CClientMgr()
{
	m_ulCurrentConnect = 0;
	m_lRepostCount = 0;
	m_lPendingAcceptCount = 0;
	m_lRegClientCount = 0;
	m_lRawClientCount = 0;
    InitializeCriticalSection(&m_csRegSection);
    InitializeCriticalSection(&m_csPendingSection);
	InitializeCriticalSection(&m_csRawSection);
}

CClientMgr::~CClientMgr()
{
	DeleteCriticalSection(&m_csRegSection);
	DeleteCriticalSection(&m_csPendingSection);
	DeleteCriticalSection(&m_csRawSection);
}

void CClientMgr::CreateNewClient(CClient* pClient, sockaddr_in *pstAddr)
{
	if ( (NULL == pClient) || (NULL == pstAddr))
	{
		g_pSvrLog->AddRunLog(LL_DEBUG, "CClientMgr::CreateNewClient, wrong pointer");
		return;
	}
	pClient->MutexBufferLock();
	pClient->m_addrPeer.sin_addr = pstAddr->sin_addr;
    pClient->m_addrPeer.sin_family = pstAddr->sin_family;
	pClient->m_addrPeer.sin_port = pstAddr->sin_port;
	strcpy(pClient->m_addrPeer.sin_zero, pstAddr->sin_zero);
	pClient->MutexBufferUnLock();

	EnterCriticalSection(&m_csRawSection);
	m_oRawClientPtrList.push_back(pClient);
	LeaveCriticalSection(&m_csRawSection);
	InterlockedIncrement(&m_lRawClientCount);
}

int CClientMgr::RegClient(CClient* pClient)
{
	if (NULL == pClient)
	{
		g_pSvrLog->AddRunLog(LL_DEBUG, "CClientMgr::RegClient, wrong pointer");
		return -1;
	}

	int iClientID = pClient->GetRawClientID();
	// 获取的ID不对，返回失败
	if (-1 == iClientID)
	{
		g_pSvrLog->AddRunLog(LL_DEBUG, "CClientMgr::RegClient, Get ClientID Error");
		return -1;
	}
	// 设置该ID
	pClient->SetClientID(iClientID);
	
	// 从未确认队伍删除
	EnterCriticalSection(&m_csRawSection);
	m_oRawClientPtrList.remove(pClient);
	LeaveCriticalSection(&m_csRawSection);
	InterlockedDecrement(&m_lRawClientCount);

	// 加入到已经确认的客户端中来
	EnterCriticalSection(&m_csRegSection);

	// 为了保持客户端ID唯一，必须判断一下,web的ID为0除外
	if (0 != iClientID)
	{
		ClientPtrList::iterator itReg = m_oRegClientPtrList.begin();
		ClientPtrList::iterator itRegEnd = m_oRegClientPtrList.end();
		
		for (; itReg != itRegEnd;)
		{
			if ((*itReg)->GetClientID() == pClient->GetClientID())
			{
				InsertPendingAccept(*itReg);
				itReg = m_oRegClientPtrList.erase(itReg);
				InterlockedDecrement(&m_lRegClientCount);
				break;
			}
			else
			{
				itReg++;
			}
		}
	}
	m_oRegClientPtrList.push_back(pClient);
	LeaveCriticalSection(&m_csRegSection);
	InterlockedIncrement(&m_lRegClientCount);
	return m_lRegClientCount;
}

CClient* CClientMgr::GetClientByID(int iClientID)
{
	EnterCriticalSection(&m_csRegSection);
	ClientPtrList::iterator it = m_oRegClientPtrList.begin();
    ClientPtrList::iterator itEnd = m_oRegClientPtrList.end();
    
	for (; it != itEnd; it++)
	{
		if ((*it)->GetClientID() == (int)iClientID)
		{
			LeaveCriticalSection(&m_csRegSection);
			return *it;
		}
	}

	LeaveCriticalSection(&m_csRegSection);
	return NULL;
}

int CClientMgr::DistroyClient(int iClientID)
{
	EnterCriticalSection(&m_csRegSection);
	ClientPtrList::iterator it = m_oRegClientPtrList.begin();
    ClientPtrList::iterator itEnd = m_oRegClientPtrList.end();

    for (; it != itEnd;)
	{
		if ((*it)->GetClientID() == iClientID)
		{
			it = m_oRegClientPtrList.erase(it);
			LeaveCriticalSection(&m_csRegSection);
			InterlockedDecrement(&m_lRegClientCount);
			
			// 标识客户离线
			//char szSQL[1024] = {0};
			//sprintf(szSQL, "UPDATE School_Info SET status=0 WHERE id=%d", iClientID);
			//g_pDBInterface->ExecSQL(szSQL);
			//更新好友列表
			g_pDBInterface->SetLoginState(iClientID,0);
			char szSQL[1024] = {0};
			sprintf(szSQL, "UPDATE friend SET friendonline=0 WHERE friendimno=%d", iClientID);
			g_pDBInterface->ExecSQL(szSQL);

			return m_lRegClientCount;
		}
		else
		{
			it++;
		}
	}
	
	LeaveCriticalSection(&m_csRegSection);

	return -1;
}

int CClientMgr::DistroyClient(CClient *pClient)
{
	if ( NULL != pClient)
	{
		int iReturnCode = DistroyClient(pClient->GetClientID());
		if (-1 == iReturnCode)
		{
			iReturnCode = DeleteFromPendingAccept(pClient);
			if (-1 == iReturnCode)
			{
				iReturnCode = DeleteFromRawList(pClient);
			}
		}

		return iReturnCode;
	}
	return -1;
}

void CClientMgr::InsertPendingAccept(CClient* pClient)
{
	if (NULL != pClient)
	{
		EnterCriticalSection(&m_csPendingSection);
		pClient->ClearAllData();
		m_oPendingAcceptClientList.push_back(pClient);
		LeaveCriticalSection(&m_csPendingSection);

		// Increment the outstanding overlapped count for this socket
		InterlockedIncrement(&m_lPendingAcceptCount);
	}
	
}

void CClientMgr::RemovePendingAccept(CClient* pClient, sockaddr_in *RemoteSockaddr)
{
    if (NULL != pClient)
	{
		EnterCriticalSection(&m_csPendingSection);
		m_oPendingAcceptClientList.remove(pClient);
		LeaveCriticalSection(&m_csPendingSection);
		InterlockedDecrement(&m_lPendingAcceptCount);

		CreateNewClient( pClient, RemoteSockaddr);
	}
}

void CClientMgr::PollPendingAccept(int& iReturnCode)
{
	CClient *pClient = NULL;

    EnterCriticalSection(&m_csPendingSection);

	ClientPtrList::iterator it = m_oPendingAcceptClientList.begin();
    ClientPtrList::iterator itEnd = m_oPendingAcceptClientList.end();

	for (; it != itEnd; it++)
	{
		pClient = *(it);

		int iOptval = 0;
		int iOptlen = 0;
		iOptlen = sizeof(iOptval);
		
		iReturnCode = getsockopt(
			pClient->m_skClient,
			SOL_SOCKET,
			SO_CONNECT_TIME,
			(char *)&iOptval,
			&iOptlen
			);
		
		if (iReturnCode == SOCKET_ERROR)
		{
			//g_pSvrLog->AddRunLog(LL_DEBUG, "getsockopt: SO_CONNECT_TIME failed: %d", WSAGetLastError());
			pClient->ClearAllData();
			continue;
		}
		else
		{
			// If the socket has been connected for more than 6 minutes,
			//    close it. If closed, the AcceptEx call will fail in the
			//    completion thread.
			if ((iOptval != 0xFFFFFFFF) && (iOptval > 360))
			{
				//g_pSvrLog->AddRunLog(LL_DEBUG, "closing stale handle");
				pClient->ClearAllData();
			}
		}
		
	}
	LeaveCriticalSection(&m_csPendingSection);

	EnterCriticalSection(&m_csRawSection);
    ClientPtrList::iterator itRaw = m_oRawClientPtrList.begin();
	ClientPtrList::iterator itRawEnd = m_oRawClientPtrList.end();
	for (; itRaw != itRawEnd; )
	{
		pClient = *(itRaw);
		
		int iOptval = 0;
		int iOptlen = 0;
		iOptlen = sizeof(iOptval);
		
		iReturnCode = getsockopt(
			pClient->m_skClient,
			SOL_SOCKET,
			SO_CONNECT_TIME,
			(char *)&iOptval,
			&iOptlen
			);
		
		if (iReturnCode == SOCKET_ERROR)
		{
			g_pSvrLog->AddRunLog(LL_DEBUG, "getsockopt: SO_CONNECT_TIME failed: %d", WSAGetLastError());
			itRaw = m_oRawClientPtrList.erase(itRaw);
			InterlockedDecrement(&m_lRawClientCount);
			InsertPendingAccept(pClient);

			continue;
		}
		else
		{
			// If the socket has been connected for more than 5 minutes,
			//    close it. If closed, the AcceptEx call will fail in the
			//    completion thread.
			if ((iOptval != 0xFFFFFFFF) && (iOptval > 300))
			{
				//g_pSvrLog->AddRunLog(LL_DEBUG, "closing stale handle");
				itRaw = m_oRawClientPtrList.erase(itRaw);
				InterlockedDecrement(&m_lRawClientCount);
                InsertPendingAccept(pClient);
			}
			else
			{
				itRaw++;
			}
		}
	}

	LeaveCriticalSection(&m_csRawSection);
}

void CClientMgr::DestroyAllClient()
{
	EnterCriticalSection(&m_csPendingSection);
	ClientPtrList::iterator itPending = m_oPendingAcceptClientList.begin();
    ClientPtrList::iterator itPendingEnd = m_oPendingAcceptClientList.end();
	for (; itPending != itPendingEnd;)
	{
		delete *itPending;
		// erase会把跌代器向后移动一位
		itPending = m_oPendingAcceptClientList.erase(itPending);
		InterlockedDecrement(&m_lPendingAcceptCount);
	}
	LeaveCriticalSection(&m_csPendingSection);

	EnterCriticalSection(&m_csRawSection);
	ClientPtrList::iterator itRaw = m_oRawClientPtrList.begin();
    ClientPtrList::iterator itRawEnd = m_oRawClientPtrList.end();
	for (;itRaw != itRawEnd; )
	{
		delete *itRaw;
		// erase会把跌代器向后移动一位
        itRaw = m_oRawClientPtrList.erase(itRaw);
		InterlockedDecrement(&m_lRawClientCount);
	}
	LeaveCriticalSection(&m_csRawSection);

	EnterCriticalSection(&m_csRegSection);
	ClientPtrList::iterator itReg = m_oRegClientPtrList.begin();
    ClientPtrList::iterator itRegEnd = m_oRegClientPtrList.end();
	for (; itReg != itRegEnd;)
	{
		delete *itReg;
		// erase会把跌代器向后移动一位
		itReg = m_oRegClientPtrList.erase(itReg);
		InterlockedDecrement(&m_lRegClientCount);
	}
	LeaveCriticalSection(&m_csRegSection);
}

int CClientMgr::DeleteFromPendingAccept(CClient *pClient)
{
	// 失败了就是不在注册的客户端队列里面，从accept里面找
	EnterCriticalSection(&m_csPendingSection);
	m_oPendingAcceptClientList.remove(pClient);
	LeaveCriticalSection(&m_csPendingSection);
	InterlockedDecrement(&m_lPendingAcceptCount);
	return m_lPendingAcceptCount;
}

int CClientMgr::DeleteFromRawList(CClient *pClient)
{
	EnterCriticalSection(&m_csRawSection);
	m_oRawClientPtrList.remove(pClient);
	LeaveCriticalSection(&m_csRawSection);
	InterlockedDecrement(&m_lRawClientCount);
	return m_lRawClientCount;
}

void CClientMgr::SentToClient(int iClientID, char* pSendData, int iLen)
{
	CClient* pClient = NULL;
	pClient = GetClientByID(iClientID);
	
	if (NULL == pClient)
	{
		g_pSvrLog->AddRunLog(LL_ERROR, "CClientMgr::SentToClient: the client is not exist");
		return;
	}
	
	pClient->MutexBufferLock();
    int iReturnCode = pClient->Send(pSendData, iLen);
	pClient->MutexBufferUnLock();
	if (0 == iReturnCode)
	{
		g_pSvrLog->AddRunLog(LL_ERROR, "CClientMgr::SentToClient: send data failed");
	}
}

void CClientMgr::SendTimeVerifyToAllClient()
{
	EnterCriticalSection(&m_csRegSection);
	ClientPtrList::iterator it = m_oRegClientPtrList.begin();
    ClientPtrList::iterator itEnd = m_oRegClientPtrList.end();

    // 不得已而为之在这里组装的时间校验包
    CSendBuff oSndBuff;
    oSndBuff.AddByte(CMD_REAL_HEAD); // 帧头
	oSndBuff.AddInt(0);              // 无返回的消息，流水号无意义
	oSndBuff.AddByte(CMD_TIME_CHECK); // 命令字
    oSndBuff.AddInt(4);              // 命令长度，4字节
	
	time_t lCurrentTime;
	::time(&lCurrentTime);
	oSndBuff.AddInt((int)lCurrentTime); // 当前时间
	oSndBuff.AddByte(CMD_REAL_TAIL); // 帧尾

	for (; it != itEnd; it++)
	{
		if (0 != (*it)->GetClientID())
		{
			(*it)->MutexBufferLock();
			(*it)->Send((char*)oSndBuff.GetBuffer(), oSndBuff.GetLength());
			(*it)->MutexBufferUnLock();
		}
	}
	LeaveCriticalSection(&m_csRegSection);	
}

void CClientMgr::UpdateClientVersion(std::string & User, std::string Password)
{
	EnterCriticalSection(&m_csRegSection);
	ClientPtrList::iterator it = m_oRegClientPtrList.begin();
    ClientPtrList::iterator itEnd = m_oRegClientPtrList.end();
	
    // 不得已而为之在这里组装的时间校验包
    CSendBuff oSndBuff;
	oSndBuff.AddByte(CMD_REAL_HEAD); // 帧头
	oSndBuff.AddInt(0); // 流水号，填写0，对这个命令没有意义
	oSndBuff.AddByte(CMD_UPDATE_CLIENT_VERSION); // 命令字

	// 计算长度
	int nTotalLen = 0;
	int nUserLen = 0;
	int nPassLen = 0;

	nUserLen = User.length();
	nPassLen = Password.length();
	nTotalLen = 1 + nUserLen + 1 + nPassLen; // 长度+用户名的长度+长度+密码的长度

	oSndBuff.AddInt(nTotalLen); // 命令长度
	oSndBuff.AddByte((BYTE)nUserLen);
	oSndBuff.AddBytes((BYTE*)User.c_str(), nUserLen);
	oSndBuff.AddByte((BYTE)nPassLen);
	oSndBuff.AddBytes((BYTE*)Password.c_str(), nPassLen);

	oSndBuff.AddByte(CMD_REAL_TAIL); // 帧尾
	
	for (; it != itEnd; it++)
	{
		// 不能给web发
		if (0 != (*it)->GetClientID())
		{
			(*it)->MutexBufferLock();
			(*it)->Send((char*)oSndBuff.GetBuffer(), oSndBuff.GetLength());
			(*it)->MutexBufferUnLock();
		}
	}
	LeaveCriticalSection(&m_csRegSection);	
}
void CClientMgr::SendFriendLoginMsg(int imid)
{

	vector<int> vecfriendim;

	g_pDBInterface->GetOnlineFriend(vecfriendim,imid);

	int size = vecfriendim.size();

	CSendBuff oSndBuff;
	oSndBuff.AddByte(CMD_REAL_HEAD); // 帧头
	oSndBuff.AddInt(0); // 流水号，填写0，对这个命令没有意义
	oSndBuff.AddByte(CMD_FRIEND_ONLINE); // 命令字
	oSndBuff.AddInt(4); // 命令长度
	oSndBuff.AddInt(imid);
	oSndBuff.AddByte(CMD_REAL_TAIL); // 帧尾
	if (size >0)
	{
		for (int i= 0; i <size ;i++)
		{
            int clientID = vecfriendim[i];

            g_pPendingSendQueue->AppendDataToBack(clientID, 0,CMD_FRIEND_ONLINE, oSndBuff.GetBuffer(), oSndBuff.GetLength());
		}

	}
    
}
CClient * CClientMgr::GetPendingAccept()
{
	CClient * pClient = NULL;
    EnterCriticalSection(&m_csPendingSection);
    if (m_lPendingAcceptCount)
    {
		pClient = m_oPendingAcceptClientList.front();
        m_oPendingAcceptClientList.pop_front();
		InterlockedDecrement(&m_lPendingAcceptCount);
    }
	else
	{
		pClient = new CClient;
	}

	LeaveCriticalSection(&m_csPendingSection);
	return pClient;
}