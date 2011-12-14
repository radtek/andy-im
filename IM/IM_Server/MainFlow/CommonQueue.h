//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CommonQueue.h: overlapped�ķ�װ��
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

// ȫ�ִ����Ͷ���
class CGlobalSendQueue : public CRunTimeThread
{
public:
    CGlobalSendQueue();
	virtual ~CGlobalSendQueue();

	virtual void Run();
	
	virtual void Stop();

	// �̹߳���������
	void ThreadWork();

	// ��ȡ�����е�Ԫ�ظ���
	int GetSize();

	// �����߳���ͳ��
	void AddWorkThread();
	void DelWorkThread();
	void StopAssistantThread();
	BOOL IsAssistantThreadNeedStop();

	// ���β���һ��Ԫ�أ����ض�����Ԫ�صĸ���
	int AppendDataToBack(int iClientID,int iCommandID, BYTE byCmdCode, BYTE *szSndBuf, int iBufLen, BOOL bPriority = FALSE);

private:
	// �Ӷ�ͷȡ��һ��Ԫ�أ����ض��׵�Ԫ�ص�ָ��
	PendingSendData* GetFrontData();

	// �Ӷ��������������
	void Clear();

	// �ж϶����Ƿ�Ϊ��
	BOOL IsEmpty();

	// �������Ϣ�������
	//void DealShortStatusRept(ShortMessage * pMsg, PendingSendData * pData);

	// ϵ���̵߳ĺ������
	static DWORD WINAPI	thread_run( LPVOID pdata);
	

	// ���������ݶ���
	PendingSendQueue m_SendQueue;

	// �����ȴ����Ͷ���
	PendingSendQueue m_PriorityQueue;

	// ���ʿ����ٽ���
	CRITICAL_SECTION m_csSendQueue;
	CRITICAL_SECTION m_csPriorityQueue;

	// ����һ���¼�������֪ͨ�߳̿�ʼ����
	HANDLE m_hHaveData;

	// ��¼����ʱ��Ķ�ʱ��
	DWORD m_dwIdleStart;

	// �����߳�����
	BOOL m_bAssistantStop;
	int m_nWorkThread;
	HANDLE m_hThreadArray[ASSISTENT_THREAD_COUNT];
};

// ȫ�ֶ���Ϣ���ܶ���, ��Ϊ�Կͻ��˷������ݵĹ����̡߳�
// ��Ŀǰ��Ӧ����û������Ҫ�󣬹ʿ�����һЩ���Ӳ���
class CGlobalRecvQueue : public CRunTimeThread
{
public:
    CGlobalRecvQueue();
	virtual ~CGlobalRecvQueue();

	virtual void Run();

	virtual void Stop();

	// ���β���һ��Ԫ�أ����ض�����Ԫ�صĸ���
	int AppendDataToBack(ShortMessage* pCodingMsg);

	// ����Ӧ��Ķ���MAP
	ShortMessage* GetAPedingAnswerMsg(int iSeqNum);

private:
	
	// �Ӷ�ͷȡ��һ��Ԫ�أ����ض��׵�Ԫ�ص�ָ��
	ShortMessage* GetFrontData();
	
	// ��ȡ�����е�Ԫ�ظ���
	int GetSize();
	
	// �Ӷ��������������
	void Clear();

	// ��Map������������
	void ClearMap();
	
	// �ж��Ƿ�Ϊ��
	BOOL IsEmpty();

	// ��һ�����ͳ�ȥ�Ķ��ŷŵ�����Ӧmap��
	int InsertAPendingAnswerMsg(ShortMessage* pPedingMsg);

    PendingMsgQueue m_RecvQueue;

	PendingAnswerMsg m_PendingAnswer; // map����������ˮ���ҵ�����
	MapAssistant m_pAssistant;

	unsigned int m_iSequenceNo;

	// ���ʿ����ٽ���
	CRITICAL_SECTION m_cs;
    CRITICAL_SECTION m_csPendingMap;
	
    // ���ʶ���Ϣ����ģ��Ķ���
	//CSMS m_oSMS;
	
	HANDLE m_hHaveData;
};

#endif