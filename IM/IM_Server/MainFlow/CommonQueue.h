//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CommonQueue.h: overlapped的封装类
//
// Author: Leo
// Date: 08/04/2008
//
////////////////////////////////////////////////////////////////////// 
#ifndef _COMMON_QUEUE_H_
#define _COMMON_QUEUE_H_
#include <queue>
#include "../BaseThread/commThread.h"
#include "../HeaderFiles/common.h"

// 全局待发送队列
class CGlobalSendQueue : public CRunTimeThread
{
public:
    CGlobalSendQueue();
	virtual ~CGlobalSendQueue();

	virtual void Run();
	
	virtual void Stop();

	// 线程工作的内容
	void ThreadWork();

	// 获取队伍中的元素个数
	int GetSize();

	// 工作线程数统计
	void AddWorkThread();
	void DelWorkThread();
	void StopAssistantThread();
	BOOL IsAssistantThreadNeedStop();

	// 向队尾添加一个元素，返回队伍中元素的个数
	int AppendDataToBack(int iClientID,int iCommandID, BYTE byCmdCode, BYTE *szSndBuf, int iBufLen, BOOL bPriority = FALSE);

private:
	// 从队头取出一个元素，返回队首的元素的指针
	PendingSendData* GetFrontData();

	// 从队伍清空所有数据
	void Clear();

	// 判断队伍是否为空
	BOOL IsEmpty();

	// 处理短信息入库问题
	//void DealShortStatusRept(ShortMessage * pMsg, PendingSendData * pData);

	// 系列线程的函数入口
	static DWORD WINAPI	thread_run( LPVOID pdata);
	

	// 待发送数据队列
	PendingSendQueue m_SendQueue;

	// 高优先待发送队列
	PendingSendQueue m_PriorityQueue;

	// 访问控制临界区
	CRITICAL_SECTION m_csSendQueue;
	CRITICAL_SECTION m_csPriorityQueue;

	// 创建一个事件，用来通知线程开始工作
	HANDLE m_hHaveData;

	// 记录空闲时间的定时器
	DWORD m_dwIdleStart;

	// 工作线程数组
	BOOL m_bAssistantStop;
	int m_nWorkThread;
	HANDLE m_hThreadArray[ASSISTENT_THREAD_COUNT];
};

// 全局短消息接受队列, 作为对客户端发送数据的工作线程。
// 在目前的应用上没有性能要求，故可以有一些复杂操作
class CGlobalRecvQueue : public CRunTimeThread
{
public:
    CGlobalRecvQueue();
	virtual ~CGlobalRecvQueue();

	virtual void Run();

	virtual void Stop();

	// 向队尾添加一个元素，返回队伍中元素的个数
	int AppendDataToBack(ShortMessage* pCodingMsg);

	// 检查待应答的短信MAP
	ShortMessage* GetAPedingAnswerMsg(int iSeqNum);

private:
	
	// 从队头取出一个元素，返回队首的元素的指针
	ShortMessage* GetFrontData();
	
	// 获取队伍中的元素个数
	int GetSize();
	
	// 从队伍清空所有数据
	void Clear();

	// 从Map里面清理数据
	void ClearMap();
	
	// 判断是否为空
	BOOL IsEmpty();

	// 将一条发送出去的短信放到待回应map中
	int InsertAPendingAnswerMsg(ShortMessage* pPedingMsg);

    PendingMsgQueue m_RecvQueue;

	PendingAnswerMsg m_PendingAnswer; // map用来根据流水号找到数据
	MapAssistant m_pAssistant;

	unsigned int m_iSequenceNo;

	// 访问控制临界区
	CRITICAL_SECTION m_cs;
    CRITICAL_SECTION m_csPendingMap;
	
    // 访问短消息发送模块的对象
	//CSMS m_oSMS;
	
	HANDLE m_hHaveData;
};

#endif