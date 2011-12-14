//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CClient.h: �ͻ����࣬��װ�ͻ��˵ķ��͡����ջ�������������ز���
//
// Author: Leo
// Date: 08/02/2008
//
////////////////////////////////////////////////////////////////////// 
#ifndef _CLIENT_H_
#define _CLIENT_H_
#include <WINSOCK2.H>
#include "../HeaderFiles/common.h"
#include "../MainFlow/CScheduler.h"


enum Operation
{
    OP_ACCEPT = 1,
	OP_READ,
	OP_WRITE
};

enum State
{ 
	stIdle,
	stClosed, 
	stAccepting, 
	stReading, 
	stWriting 
};
class CClient
{
public:
	CClient();
	~CClient();
	
	// ���ͺ������ɹ����ط��͵��ֽ��������򷵻�0
	int Send(char* szSndBuf, int iLen);

	// �������ͻ��˵�ID
	int GetClientID();
	
	// �ӿͻ��˵Ļ�����������һ��ID
	int GetRawClientID();

	// �첽���ܺ���
	int PostRecv();

	// ���ÿͻ��˵�ID
	void SetClientID(int iClientID);

	// �����Ϣ��
	void AddNewPacket(int iLength);

	// ����������
	static void MessageDispatch(BYTE* pBuffer, int iLen);

	// ���ƻ�������д�Ļ�������������
	void MutexBufferLock();
	void MutexBufferUnLock();

	// �������е�����
	void ClearAllData();
	
	// �����¼�
	void SetAcceptEvent(HANDLE hAcceptEvent);

	OVERLAPPED m_wsaOvLap;  // OverLapped�ṹ
    Operation m_eOperation;     // Type of operation issued
    LONG m_lOutstandingRecv; // Number of outstanding overlapped ops on 
	LONG m_lOutstandingSend;
	LONG m_lPendingSend;	
	bool m_bClosing; // Is the socket closing?
    SOCKET m_skClient;  // Used for AcceptEx client socket
	char m_szbuf[nMaxBuffer];           // Buffer for recv/send/AcceptEx
    HANDLE m_muBufferMutex;             // ���̻߳����������������ر��ǽ��ܺ����ǽ���ϵͳ������ɶԻ�������д�룬���Ա����û�������ͬ���߳�
	sockaddr_in m_addrPeer;  // struct of peer sockaddr
	State m_eStatus;
    CRITICAL_SECTION   m_csSockCritSec;     // Protect access to this structure

	ULONG m_ulBuflen;        // Length of the buffer

    HANDLE m_hPostAccept;  // Accept Event

	char m_szMessageBuffer[BIG_BUFFER];  // ��Ϣ��ŵĻ�����
	unsigned int m_iMessageBufLen;  // ��Ϣ�������ĳ���
	
private:
	int m_iClentID;  // Client ID
	static CScheduler m_oScheduler;
};

#endif