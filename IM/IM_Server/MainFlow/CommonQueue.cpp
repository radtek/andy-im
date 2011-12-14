//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CommonQueue.cpp : �ͻ��˹��������ʵ���ļ�
//
// Author: Leo
// Date: 08/04/2008
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
//#include "../HeaderFiles/stdafx.h"
#include <assert.h>
#include "../IOCPServer/CClientMgr.h"
#include "CommonQueue.h"
#include "../LogFile/ServerLog.h"
#include "../HeaderFiles/comm_buff.h"
#include "../DBInterface/DBInterface.h"

extern CClientMgr * g_pClientMgr;  // ȫ�ֿͻ��˹���ģ��ָ��
extern CServerLog * g_pSvrLog;  // ��־�ļ�ȫ�ֱ���
//extern CGlobalRecvQueue *g_pShortMsgQueue; // ȫ�ִ����Ͷ��Ŷ���
extern DBInterface *g_pDBInterface;//���ݿ�ӿ�ȫ��ָ��

CGlobalSendQueue::CGlobalSendQueue()
{
	m_hHaveData = CreateEvent (NULL, TRUE, FALSE, "SendQueue");
	m_nWorkThread = 0;
	m_bAssistantStop = FALSE;
	InitializeCriticalSection(&m_csSendQueue);
	InitializeCriticalSection(&m_csPriorityQueue);
	m_dwIdleStart = GetTickCount();
}

CGlobalSendQueue::~CGlobalSendQueue()
{
	CloseHandle(m_hHaveData);

	m_bAssistantStop = TRUE;
	// ������е�����
	Clear();

	DeleteCriticalSection(&m_csSendQueue);
	DeleteCriticalSection(&m_csPriorityQueue);
}

int CGlobalSendQueue::AppendDataToBack(int iClientID, int iCommandID,BYTE byCmdCode, BYTE* szSndBuf, int iBufLen, BOOL bPriority)
{
    PendingSendData * pSndData = new PendingSendData;
	assert(NULL != pSndData);
	pSndData->iClientID = iClientID;
	pSndData->iCommandID = iCommandID;
	pSndData->byCmdCode = byCmdCode;
	pSndData->iLen = iBufLen;
	
	pSndData->szSendBuf = NULL;
	pSndData->szSendBuf = new char[iBufLen];
	if (NULL ==pSndData->szSendBuf)
	{
		return 0;
	}
	memcpy(pSndData->szSendBuf, szSndBuf, iBufLen);
	
	int nCount = 0;
	if (bPriority)
	{
		// ���ȶ���
		EnterCriticalSection(&m_csPriorityQueue);
		m_PriorityQueue.push(pSndData);
		nCount = m_PriorityQueue.size();
		LeaveCriticalSection(&m_csPriorityQueue);
	}
	else
	{
		// ��ͨ����
		EnterCriticalSection(&m_csSendQueue);
		m_SendQueue.push(pSndData);
		nCount = m_SendQueue.size();
		LeaveCriticalSection(&m_csSendQueue);		
	}

	// ��������ݣ�����Ϊ���ź�
	SetEvent(m_hHaveData);

	// �������ݴ���1000�����ǵ�ǰ���������ݸ߷壬��̬����ASSISTENT_THREAD_COUNT���߳��������������е�����
	if ((0 == m_nWorkThread)  // �����̶߳��˳���
		&& (nCount > 1000))   // ������������1000��
	{
		for (int i = 0; i < ASSISTENT_THREAD_COUNT; i++)
		{
			m_hThreadArray[i] = CreateThread(NULL, NULL, thread_run, (LPVOID)this, 0, NULL);
		}
		m_bAssistantStop = FALSE;
	}

	return nCount;
}

void CGlobalSendQueue::AddWorkThread()
{
	InterlockedIncrement((long*)&m_nWorkThread);
}

void CGlobalSendQueue::DelWorkThread()
{
	InterlockedDecrement((long*)&m_nWorkThread);
}
PendingSendData* CGlobalSendQueue::GetFrontData()
{
	PendingSendData* pSndData = NULL;
	BOOL bGotData = FALSE;
	// �Ӷ�����ȡ����ʱ�򣬸����ȼ����е���������
	EnterCriticalSection(&m_csPriorityQueue);
	if (m_PriorityQueue.size())
	{
		pSndData = m_PriorityQueue.front();
		assert(NULL != pSndData);
        m_PriorityQueue.pop();
		bGotData = TRUE;
	}
	LeaveCriticalSection(&m_csPriorityQueue);

	if (!bGotData)
	{
		// ���ȼ�����û�ж������ٿ�����ͨ����
		EnterCriticalSection(&m_csSendQueue);
		if (m_SendQueue.size())
		{
			pSndData = m_SendQueue.front();
			
			assert(NULL != pSndData);
			m_SendQueue.pop();		
		}
		LeaveCriticalSection(&m_csSendQueue);
	}
	return pSndData;
}

int CGlobalSendQueue::GetSize()
{
	return (m_SendQueue.size() + m_PriorityQueue.size());
}

void CGlobalSendQueue::Clear()
{
	PendingSendData* pData = NULL;
	// ���ȶ���
	EnterCriticalSection(&m_csPriorityQueue);
	while (m_PriorityQueue.size())
	{
		pData = m_PriorityQueue.front();
		m_PriorityQueue.pop();
		if (NULL != pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	LeaveCriticalSection(&m_csPriorityQueue);

	// ��ͨ����
	EnterCriticalSection(&m_csSendQueue);
	while (m_SendQueue.size())
	{
		pData = m_SendQueue.front();
		m_SendQueue.pop();
		if (NULL != pData)
		{
			delete pData;
			pData = NULL;
		}
	}
	LeaveCriticalSection(&m_csSendQueue);
}

BOOL CGlobalSendQueue::IsEmpty()
{
	return (m_SendQueue.empty() && m_PriorityQueue.empty());
}

void CGlobalSendQueue::Run()
{
    while (!IsNeedStop())
	{
		DWORD dwCurrent = GetTickCount();

		if (GetSize())
		{
			// �����ľ������ݷ���������Ƿ��㸨���̹߳���
			ThreadWork();
			m_dwIdleStart = dwCurrent;
		}
		else
		{
			// �����¼�Ϊ���ź�
			ResetEvent(m_hHaveData);

			// û5���Ӽ��һ��
			if (WAIT_TIMEOUT == WaitForSingleObject(m_hHaveData, 5000))
			{

				// ���˰��Сʱ�˾ͷ���ʱ��У��
				if (dwCurrent - m_dwIdleStart >= TIME_VERIFY_SPAN)
				{
					g_pClientMgr->SendTimeVerifyToAllClient();
					m_dwIdleStart = dwCurrent;
				}
			}
		}
	}

	if (m_nWorkThread > 0)
	{
		// ֪ͨ�����߳�ͣ��
		StopAssistantThread();
		// �ȴ������߳��˳�
		for(int i=0; i<ASSISTENT_THREAD_COUNT; i++)
		{
			WaitForSingleObject(m_hThreadArray[i], INFINITE);
			CloseHandle( m_hThreadArray[i]);
		}
	}
}

void CGlobalSendQueue::ThreadWork()
{
	PendingSendData* pData = NULL;
	pData = GetFrontData();
	
	if (NULL != pData)
	{
		////������ģ�ָ�벻��Ϊ��
		//ShortMessage * pMsg = NULL;
		//pMsg = g_pShortMsgQueue->GetAPedingAnswerMsg(pData->iCommandID);
		//if (NULL != pMsg)
		//{
		//	// ���ҵĽ���ǿգ�����Ҫ�ٴ������Ϣ
		//	// Ŀǰ�����Ķ���Ϣֻ�п�����Ϣ
		//	switch (pMsg->byMsgCode)
		//	{
		//	case CMD_CHECK_SHORT_MESSAGE:
		//		DealShortStatusRept(pMsg, pData);
		//		break;
		//	default:
		//		break;
		//	}
		//}
		
		// �м���Ķ���״̬���治��Ҫ�ظ�
		//if (((pData->iClientID) != 0) && (CMD_MESSAGE_STATUS_REPT == pData->byCmdCode))
		//{
		//	// �Ѿ��Ӷ����е��������������ˣ��ͷſռ�
		//	/*if (NULL != pMsg)
		//	{
		//		delete pMsg;
		//		pMsg = NULL;
		//	}*/
		//	
		//	if (NULL != pData)
		//	{
		//		if (NULL != pData->szSendBuf)
		//		{
		//			delete [] pData->szSendBuf;
		//		}
		//		delete pData;
		//		pData = NULL;
		//	}
		//}
		//else
		//{
			g_pClientMgr->SentToClient(pData->iClientID, pData->szSendBuf, pData->iLen);
			
			// �Ѿ��Ӷ����е��������������ˣ��ͷſռ�
			/*if (NULL != pMsg)
			{
				delete pMsg;
				pMsg = NULL;
			}*/
			
			if (NULL != pData)
			{
				if (NULL != pData->szSendBuf)
				{
					delete [] pData->szSendBuf;
				}
				delete pData;
				pData = NULL;
			}
		//}
	}
	Sleep(5);
}

void CGlobalSendQueue::StopAssistantThread()
{
	m_bAssistantStop = TRUE;
}

BOOL CGlobalSendQueue::IsAssistantThreadNeedStop()
{
	return m_bAssistantStop;
}
DWORD WINAPI CGlobalSendQueue::thread_run( LPVOID pdata)
{
	CGlobalSendQueue * pSendQueue = (CGlobalSendQueue *) pdata;

	// ��������
	pSendQueue->AddWorkThread();
	
	// �߳�ֹͣ����:1.�����е���������ʮ���ˣ�2.��֪ͨͣ����
	while ((pSendQueue->GetSize() > 10 )  // �����߳�ͣ��������
		&& (!pSendQueue->IsAssistantThreadNeedStop()))   // ��֪ͨͣ��
	{
		pSendQueue->ThreadWork();
	}

	// ��������
	pSendQueue->DelWorkThread();

	// ֪ͨ��ﶼͣ���������
	pSendQueue->StopAssistantThread();
	return 0;
}

void CGlobalSendQueue::Stop()
{
	NotifyStop();   	
    Wait(INFINITE);
}
//void CGlobalSendQueue::DealShortStatusRept(ShortMessage * pMsg, PendingSendData * pData)
//{
//	// �ȷ������صĽ������
//	CRecvBuff oRecvBuf((BYTE*)pData->szSendBuf, pData->iLen);
//
//	BYTE byTemp = 0;
//	int iTemp = 0;
//	std::string strUsrName = "";
//
//	int nSMDate = 0;
//	int nSeqID = 0;
//
//	oRecvBuf.GetByte(byTemp); // ȡ��֡ͷ
//	oRecvBuf.GetInt(iTemp);   // ��ˮ��
//	oRecvBuf.GetByte(byTemp); // ������
//	oRecvBuf.GetInt(iTemp);   // ����
//	oRecvBuf.GetInt(iTemp);   // ���ط��صĴ����룬�����������Ҫ��
//	oRecvBuf.GetInt(nSMDate); // ����Ϣ��ˮ���е����ڲ���
//	oRecvBuf.GetInt(nSeqID);  // ����Ϣ��ˮ���е�β������
//
//	char szSQL[nMaxBuffer] = {0};
//	char szCheckTime[50] = {0};
//	char szSendTime[50] = {0};
//
//	// ���سɹ��Ŀ��ڶ�����Ҫ���
//	if ((0 == iTemp) && (pMsg->byMsgCode == CMD_CHECK_SHORT_MESSAGE) )
//	{
//		// ������Ϊ0������ɹ��ˣ���Ҫ��⡣���ھ�Ҫ������������绰������
//		RFIDTel rfTel;
//        g_pDBInterface->getTelInfo(rfTel, std::string((char*)pMsg->byUserNumber), pMsg->iClientID, 0);
//
//		char chNetType = '0';
//		switch (pMsg->byNetType)
//		{
//		case NET_TYPE_MOBILE:
//			// �ƶ����ϴ��
//			chNetType = '1';
//			break;
//		case NET_TYPE_UNION:
//			chNetType = '2';
//			break;
//		case NET_TYPE_TELCOM:
//			// ������С���ò�����
//			chNetType = '3';
//			break;
//		default:
//			break;				
//		}
//
//		// ��õ�ǰϵͳʱ��
//		SYSTEMTIME systime;
//		::GetSystemTime(&systime);
//		::GetLocalTime(&systime);
//		sprintf(szSendTime, "%04d-%02d-%02d %02d:%02d:%02d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
//
//		// ����ˢ��ʱ��
//		long lCheckTime = (long)pMsg->iCheckTime;
//		tm gm = *localtime( &lCheckTime);
//        SYSTEMTIME st = { 1900+gm.tm_year, 1+gm.tm_mon, gm.tm_wday, gm.tm_mday, gm.tm_hour, gm.tm_min, gm.tm_sec};
//        sprintf(szCheckTime, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
//
//		// ����Ϣ��ˮ��
//		char szCmdSeq[50] = {0};
//		sprintf(szCmdSeq, "%d%d", nSMDate, nSeqID);
//
//		sprintf(szSQL, "INSERT INTO Unicom_Check (recverid, recvertel, recverphonetype, inout, sendtime, cardtime, schoolid, classid, rfidno, msgstatus, msgid)\
//			VALUES ('%s', '%s', '%c', %d, '%s', '%s', %d, %d, '%d', 0, '%s')",
//			rfTel.recvid.c_str(),
//			(char*)pMsg->byUserNumber,
//			chNetType,
//			pMsg->byCheckInfo,
//			szSendTime,
//			szCheckTime,
//			pMsg->iClientID,
//			rfTel.classid,
//			pMsg->iRfid,
//			szCmdSeq
//			);
//
//		if (g_pDBInterface->ExecSQL(szSQL))
//		{
//			// ��0��ʾ���ݿ�ִ���д��󣬼�¼��־�����ݿ�ӿ��Ѿ���¼�˱���ʧ�ܣ����ظ���¼��
//			//g_pSvrLog->AddRunLog(LL_ERROR, "CGlobalSendQueue::DealShortStatusRept: Exec SQL: %s", szSQL);
//		}
//		
//	}
//}
//////////////////////////////////////////////////////////////////////////
CGlobalRecvQueue::CGlobalRecvQueue()
{
	m_iSequenceNo = 0;
	m_hHaveData = CreateEvent(NULL, TRUE, FALSE, "RecvQueue");
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_csPendingMap);

	// ��ʼ���������е�Key
	for (int i = 0; i < MAP_SIZE;i++)
	{
		m_pAssistant[i] = i;
	}
}

CGlobalRecvQueue::~CGlobalRecvQueue()
{
	CloseHandle(m_hHaveData);

	// ��������
	Clear();

	// ����Map
	ClearMap();

	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_csPendingMap);
}

int CGlobalRecvQueue::AppendDataToBack(ShortMessage* pCodingMsg)
{
	if (NULL != pCodingMsg)
	{
		EnterCriticalSection(&m_cs);
		m_RecvQueue.push(pCodingMsg);
		LeaveCriticalSection(&m_cs);
	}

	// ��������ݣ�����Ϊ���ź�
	SetEvent(m_hHaveData);
	return m_RecvQueue.size();
}

ShortMessage* CGlobalRecvQueue::GetFrontData()
{
	ShortMessage * pShortMsg = NULL;
    EnterCriticalSection(&m_cs);
	if (m_RecvQueue.size())
	{
		pShortMsg = m_RecvQueue.front();

		assert(NULL != pShortMsg);
		m_RecvQueue.pop();
	}
	LeaveCriticalSection(&m_cs);
    return pShortMsg;
}

int CGlobalRecvQueue::GetSize()
{
	return m_RecvQueue.size();
}

void CGlobalRecvQueue::Clear()
{
    ShortMessage* pData = NULL;
    EnterCriticalSection(&m_cs);
    while (m_RecvQueue.size())
	{
	    pData = m_RecvQueue.front();
	    m_RecvQueue.pop();
	    if (NULL != pData)
		{
    		delete pData;
			pData = NULL;
		}
	}
    LeaveCriticalSection(&m_cs);
}

void CGlobalRecvQueue::ClearMap()
{
	EnterCriticalSection(&m_csPendingMap);
	PendingAnswerMsg::iterator it = m_PendingAnswer.begin();
    PendingAnswerMsg::iterator itEnd = m_PendingAnswer.end();
	while (it != itEnd)
	{
        delete (ShortMessage*)(*it).second;
        it = m_PendingAnswer.erase(it);
		it ++;
	}
	
    LeaveCriticalSection(&m_csPendingMap);		
}
BOOL CGlobalRecvQueue::IsEmpty()
{
    return m_RecvQueue.empty();
}

void CGlobalRecvQueue::Run()
{
 //   while (!IsNeedStop())
	//{
	//	if (m_RecvQueue.size())
	//	{
	//		// �ϲ������ݾͿ���Ͷ����
 //           ShortMessage *pMsg = GetFrontData();
	//		if (NULL == pMsg)
	//		{
	//			g_pSvrLog->AddRunLog(LL_ERROR, "Get Recv Item Failed.");
	//			continue;
	//		}

	//		// ���͵���������һ�ݿ������ɷ��͵��̸߳���ɾ��
	//		ShortMessage *pShortMsg = new ShortMessage;
	//		memcpy(pShortMsg, pMsg, sizeof(ShortMessage ));

	//		// �����ˮ�ţ�˳����뵽��Ӧ��Ķ��У�������д���ڶ��ű��ȡ��Ϣ
	//		if (pShortMsg->byMsgCode == CMD_CHECK_SHORT_MESSAGE)
	//		{
	//			// ���е����ݼ��뵽������������Ӧ����Ϣ�����Ժ���Ҫȡ��������ϸ����
	//			pShortMsg->iCommandID = InsertAPendingAnswerMsg(pMsg);
	//		}
	//		else
	//		{
	//			// ���ǿ��ڶ��ŵĻ������еĶ������ݾ�û�����ˣ��û����ڴ�
	//			delete pMsg;
	//			pMsg = NULL;
	//		}
	//		
	//		// ���ص���Ӧ�������ޣ����г���Ҳ���ޣ�������Ϣ100�����ٷ���
	//		Sleep(5);
	//		m_oSMS.AddQueue(pShortMsg);
	//	}
	//	else
	//	{
	//		ResetEvent(m_hHaveData);

	//		// Ϊ�˷�ֹ�����ݿ�ת��CPU���ģ����������Ƶȴ��ź�
	//		WaitForSingleObject(m_hHaveData, 5000);
	//	}
	//}
}

void CGlobalRecvQueue::Stop()
{
	NotifyStop();   	
    Wait(INFINITE);
}
int CGlobalRecvQueue::InsertAPendingAnswerMsg(ShortMessage* pPedingMsg)
{
	EnterCriticalSection(&m_csPendingMap);

	// ����MAP������Ƿ��п������к�
	if (!m_pAssistant.empty())
	{
		MapAssistant::iterator it = m_pAssistant.begin();
        m_iSequenceNo = (*it).second;
		// �Ѿ��õ����кŴӸ���MAP��ɾ��
		m_pAssistant.erase(m_iSequenceNo);
        // �ڴ�Ӧ������м���
        m_PendingAnswer[m_iSequenceNo] = pPedingMsg;
	}
	else
	{
		// �͵����ϸ����к���1���Ǹ�λ���Ǵ���ʱ����õģ��������
		g_pSvrLog->AddRunLog(LL_DEBUG, "Not enough sequence N.O. to use.");
		m_iSequenceNo++;

		// �ͷŵ�ռ�õĿռ�
		delete m_PendingAnswer[m_iSequenceNo];
		m_PendingAnswer.erase(m_iSequenceNo);
		
		// �����µ���Ϣ
        m_PendingAnswer[m_iSequenceNo] = pPedingMsg;
	}

	LeaveCriticalSection(&m_csPendingMap);
	return m_iSequenceNo;
}

ShortMessage* CGlobalRecvQueue::GetAPedingAnswerMsg(int iSeqNum)
{
	ShortMessage* pData = NULL;
	EnterCriticalSection(&m_csPendingMap);
	PendingAnswerMsg::iterator it = m_PendingAnswer.find(iSeqNum);

	if (m_PendingAnswer.end() != it)
	{
        // �ҵ��ˣ����������ݷ��أ�Ȼ�����ˮ�Ż���
		pData = (*it).second;
		m_PendingAnswer.erase(iSeqNum);

		// ��������кŷŵ�����map��
		m_pAssistant[iSeqNum] = iSeqNum;
	}

	LeaveCriticalSection(&m_csPendingMap);
	return pData;
}