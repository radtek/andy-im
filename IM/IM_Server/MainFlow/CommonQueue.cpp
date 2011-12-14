//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CommonQueue.cpp : 客户端管理类类的实现文件
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

extern CClientMgr * g_pClientMgr;  // 全局客户端管理模块指针
extern CServerLog * g_pSvrLog;  // 日志文件全局变量
//extern CGlobalRecvQueue *g_pShortMsgQueue; // 全局待发送短信队列
extern DBInterface *g_pDBInterface;//数据库接口全局指针

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
	// 清理队列的数据
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
		// 优先队列
		EnterCriticalSection(&m_csPriorityQueue);
		m_PriorityQueue.push(pSndData);
		nCount = m_PriorityQueue.size();
		LeaveCriticalSection(&m_csPriorityQueue);
	}
	else
	{
		// 普通队列
		EnterCriticalSection(&m_csSendQueue);
		m_SendQueue.push(pSndData);
		nCount = m_SendQueue.size();
		LeaveCriticalSection(&m_csSendQueue);		
	}

	// 添加了数据，设置为有信号
	SetEvent(m_hHaveData);

	// 发现数据大于1000个就是当前遇到了数据高峰，动态创建ASSISTENT_THREAD_COUNT个线程来辅助处理所有的数据
	if ((0 == m_nWorkThread)  // 辅助线程都退出了
		&& (nCount > 1000))   // 并且总数大于1000了
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
	// 从队列中取数据时候，高优先级队列的数据优先
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
		// 优先级队列没有东西了再看看普通队列
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
	// 优先队列
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

	// 普通队列
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
			// 工作的具体内容分离出来就是方便辅助线程工作
			ThreadWork();
			m_dwIdleStart = dwCurrent;
		}
		else
		{
			// 设置事件为无信号
			ResetEvent(m_hHaveData);

			// 没5秒钟检查一次
			if (WAIT_TIMEOUT == WaitForSingleObject(m_hHaveData, 5000))
			{

				// 闲了半个小时了就发送时间校验
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
		// 通知辅助线程停工
		StopAssistantThread();
		// 等待辅助线程退出
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
		////最基本的，指针不能为空
		//ShortMessage * pMsg = NULL;
		//pMsg = g_pShortMsgQueue->GetAPedingAnswerMsg(pData->iCommandID);
		//if (NULL != pMsg)
		//{
		//	// 查找的结果非空，是需要再处理的消息
		//	// 目前这样的短消息只有考勤消息
		//	switch (pMsg->byMsgCode)
		//	{
		//	case CMD_CHECK_SHORT_MESSAGE:
		//		DealShortStatusRept(pMsg, pData);
		//		break;
		//	default:
		//		break;
		//	}
		//}
		
		// 中间件的短信状态报告不需要回复
		//if (((pData->iClientID) != 0) && (CMD_MESSAGE_STATUS_REPT == pData->byCmdCode))
		//{
		//	// 已经从队列中弹出，现在用完了，释放空间
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
			
			// 已经从队列中弹出，现在用完了，释放空间
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

	// 数量增加
	pSendQueue->AddWorkThread();
	
	// 线程停止条件:1.队列中的数据少于十个了；2.被通知停工了
	while ((pSendQueue->GetSize() > 10 )  // 辅助线程停工的下限
		&& (!pSendQueue->IsAssistantThreadNeedStop()))   // 被通知停工
	{
		pSendQueue->ThreadWork();
	}

	// 数量减少
	pSendQueue->DelWorkThread();

	// 通知大伙都停下来别干了
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
//	// 先分析返回的结果内容
//	CRecvBuff oRecvBuf((BYTE*)pData->szSendBuf, pData->iLen);
//
//	BYTE byTemp = 0;
//	int iTemp = 0;
//	std::string strUsrName = "";
//
//	int nSMDate = 0;
//	int nSeqID = 0;
//
//	oRecvBuf.GetByte(byTemp); // 取出帧头
//	oRecvBuf.GetInt(iTemp);   // 流水号
//	oRecvBuf.GetByte(byTemp); // 命令码
//	oRecvBuf.GetInt(iTemp);   // 长度
//	oRecvBuf.GetInt(iTemp);   // 网关返回的错误码，这才是我们想要的
//	oRecvBuf.GetInt(nSMDate); // 短信息流水号中的日期部分
//	oRecvBuf.GetInt(nSeqID);  // 短信息流水号中的尾数部分
//
//	char szSQL[nMaxBuffer] = {0};
//	char szCheckTime[50] = {0};
//	char szSendTime[50] = {0};
//
//	// 返回成功的考勤短信需要入库
//	if ((0 == iTemp) && (pMsg->byMsgCode == CMD_CHECK_SHORT_MESSAGE) )
//	{
//		// 返回码为0，代表成功了，需要入库。现在就要分析短信里面电话号码了
//		RFIDTel rfTel;
//        g_pDBInterface->getTelInfo(rfTel, std::string((char*)pMsg->byUserNumber), pMsg->iClientID, 0);
//
//		char chNetType = '0';
//		switch (pMsg->byNetType)
//		{
//		case NET_TYPE_MOBILE:
//			// 移动是老大哥
//			chNetType = '1';
//			break;
//		case NET_TYPE_UNION:
//			chNetType = '2';
//			break;
//		case NET_TYPE_TELCOM:
//			// 电信最小不得不承认
//			chNetType = '3';
//			break;
//		default:
//			break;				
//		}
//
//		// 获得当前系统时间
//		SYSTEMTIME systime;
//		::GetSystemTime(&systime);
//		::GetLocalTime(&systime);
//		sprintf(szSendTime, "%04d-%02d-%02d %02d:%02d:%02d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
//
//		// 解析刷卡时间
//		long lCheckTime = (long)pMsg->iCheckTime;
//		tm gm = *localtime( &lCheckTime);
//        SYSTEMTIME st = { 1900+gm.tm_year, 1+gm.tm_mon, gm.tm_wday, gm.tm_mday, gm.tm_hour, gm.tm_min, gm.tm_sec};
//        sprintf(szCheckTime, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
//
//		// 短信息流水号
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
//			// 非0表示数据库执行有错误，记录日志，数据库接口已经记录了本次失败，不重复记录了
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

	// 初始化里面所有的Key
	for (int i = 0; i < MAP_SIZE;i++)
	{
		m_pAssistant[i] = i;
	}
}

CGlobalRecvQueue::~CGlobalRecvQueue()
{
	CloseHandle(m_hHaveData);

	// 清理数据
	Clear();

	// 清理Map
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

	// 添加了数据，设置为有信号
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
	//		// 合并了数据就可以投递了
 //           ShortMessage *pMsg = GetFrontData();
	//		if (NULL == pMsg)
	//		{
	//			g_pSvrLog->AddRunLog(LL_ERROR, "Get Recv Item Failed.");
	//			continue;
	//		}

	//		// 发送的数据另作一份拷贝，由发送的线程负责删除
	//		ShortMessage *pShortMsg = new ShortMessage;
	//		memcpy(pShortMsg, pMsg, sizeof(ShortMessage ));

	//		// 填充流水号，顺便加入到待应答的队列，用来填写考勤短信表获取信息
	//		if (pShortMsg->byMsgCode == CMD_CHECK_SHORT_MESSAGE)
	//		{
	//			// 堆中的数据加入到队列中来，在应答消息到了以后还需要取出来看详细数据
	//			pShortMsg->iCommandID = InsertAPendingAnswerMsg(pMsg);
	//		}
	//		else
	//		{
	//			// 不是考勤短信的话，堆中的短信数据就没有用了，得回收内存
	//			delete pMsg;
	//			pMsg = NULL;
	//		}
	//		
	//		// 网关的响应能力有限，队列长度也有限，所以休息100毫秒再发送
	//		Sleep(5);
	//		m_oSMS.AddQueue(pShortMsg);
	//	}
	//	else
	//	{
	//		ResetEvent(m_hHaveData);

	//		// 为了防止无数据空转的CPU消耗，这里无限制等待信号
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

	// 辅助MAP来检查是否有空闲序列号
	if (!m_pAssistant.empty())
	{
		MapAssistant::iterator it = m_pAssistant.begin();
        m_iSequenceNo = (*it).second;
		// 已经用的序列号从辅助MAP中删除
		m_pAssistant.erase(m_iSequenceNo);
        // 在待应答队列中加入
        m_PendingAnswer[m_iSequenceNo] = pPedingMsg;
	}
	else
	{
		// 就当作上个序列号增1的那个位置是存在时间最久的，处理掉他
		g_pSvrLog->AddRunLog(LL_DEBUG, "Not enough sequence N.O. to use.");
		m_iSequenceNo++;

		// 释放掉占用的空间
		delete m_PendingAnswer[m_iSequenceNo];
		m_PendingAnswer.erase(m_iSequenceNo);
		
		// 放入新的消息
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
        // 找到了，用它的数据返回，然后把流水号回收
		pData = (*it).second;
		m_PendingAnswer.erase(iSeqNum);

		// 将这个序列号放到辅助map中
		m_pAssistant[iSeqNum] = iSeqNum;
	}

	LeaveCriticalSection(&m_csPendingMap);
	return pData;
}