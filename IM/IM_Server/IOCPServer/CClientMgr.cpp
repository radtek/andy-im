//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CClientMgr.cpp : �ͻ��˹��������ʵ���ļ�
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

extern CServerLog * g_pSvrLog;  // ��־�ļ�ȫ�ֱ���
extern DBInterface *g_pDBInterface;//���ݿ�ӿ�ȫ��ָ��
extern CGlobalSendQueue *g_pPendingSendQueue; // ȫ�ִ����͸��ͻ��˵���Ϣ����


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
	// ��ȡ��ID���ԣ�����ʧ��
	if (-1 == iClientID)
	{
		g_pSvrLog->AddRunLog(LL_DEBUG, "CClientMgr::RegClient, Get ClientID Error");
		return -1;
	}
	// ���ø�ID
	pClient->SetClientID(iClientID);
	
	// ��δȷ�϶���ɾ��
	EnterCriticalSection(&m_csRawSection);
	m_oRawClientPtrList.remove(pClient);
	LeaveCriticalSection(&m_csRawSection);
	InterlockedDecrement(&m_lRawClientCount);

	// ���뵽�Ѿ�ȷ�ϵĿͻ�������
	EnterCriticalSection(&m_csRegSection);

	// Ϊ�˱��ֿͻ���IDΨһ�������ж�һ��,web��IDΪ0����
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
			
			// ��ʶ�ͻ�����
			//char szSQL[1024] = {0};
			//sprintf(szSQL, "UPDATE School_Info SET status=0 WHERE id=%d", iClientID);
			//g_pDBInterface->ExecSQL(szSQL);
			//���º����б�
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
		// erase��ѵ���������ƶ�һλ
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
		// erase��ѵ���������ƶ�һλ
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
		// erase��ѵ���������ƶ�һλ
		itReg = m_oRegClientPtrList.erase(itReg);
		InterlockedDecrement(&m_lRegClientCount);
	}
	LeaveCriticalSection(&m_csRegSection);
}

int CClientMgr::DeleteFromPendingAccept(CClient *pClient)
{
	// ʧ���˾��ǲ���ע��Ŀͻ��˶������棬��accept������
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

    // �����Ѷ�Ϊ֮��������װ��ʱ��У���
    CSendBuff oSndBuff;
    oSndBuff.AddByte(CMD_REAL_HEAD); // ֡ͷ
	oSndBuff.AddInt(0);              // �޷��ص���Ϣ����ˮ��������
	oSndBuff.AddByte(CMD_TIME_CHECK); // ������
    oSndBuff.AddInt(4);              // ����ȣ�4�ֽ�
	
	time_t lCurrentTime;
	::time(&lCurrentTime);
	oSndBuff.AddInt((int)lCurrentTime); // ��ǰʱ��
	oSndBuff.AddByte(CMD_REAL_TAIL); // ֡β

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
	
    // �����Ѷ�Ϊ֮��������װ��ʱ��У���
    CSendBuff oSndBuff;
	oSndBuff.AddByte(CMD_REAL_HEAD); // ֡ͷ
	oSndBuff.AddInt(0); // ��ˮ�ţ���д0�����������û������
	oSndBuff.AddByte(CMD_UPDATE_CLIENT_VERSION); // ������

	// ���㳤��
	int nTotalLen = 0;
	int nUserLen = 0;
	int nPassLen = 0;

	nUserLen = User.length();
	nPassLen = Password.length();
	nTotalLen = 1 + nUserLen + 1 + nPassLen; // ����+�û����ĳ���+����+����ĳ���

	oSndBuff.AddInt(nTotalLen); // �����
	oSndBuff.AddByte((BYTE)nUserLen);
	oSndBuff.AddBytes((BYTE*)User.c_str(), nUserLen);
	oSndBuff.AddByte((BYTE)nPassLen);
	oSndBuff.AddBytes((BYTE*)Password.c_str(), nPassLen);

	oSndBuff.AddByte(CMD_REAL_TAIL); // ֡β
	
	for (; it != itEnd; it++)
	{
		// ���ܸ�web��
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
	oSndBuff.AddByte(CMD_REAL_HEAD); // ֡ͷ
	oSndBuff.AddInt(0); // ��ˮ�ţ���д0�����������û������
	oSndBuff.AddByte(CMD_FRIEND_ONLINE); // ������
	oSndBuff.AddInt(4); // �����
	oSndBuff.AddInt(imid);
	oSndBuff.AddByte(CMD_REAL_TAIL); // ֡β
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