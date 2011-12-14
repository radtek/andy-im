//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CClient.h: 客户端类，封装客户端的发送、接收缓冲区，还有相关操作
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
	
	// 发送函数，成功返回发送的字节数，否则返回0
	int Send(char* szSndBuf, int iLen);

	// 获得这个客户端的ID
	int GetClientID();
	
	// 从客户端的缓冲区分析出一个ID
	int GetRawClientID();

	// 异步接受函数
	int PostRecv();

	// 设置客户端的ID
	void SetClientID(int iClientID);

	// 添加消息包
	void AddNewPacket(int iLength);

	// 进入主流程
	static void MessageDispatch(BYTE* pBuffer, int iLen);

	// 控制缓冲区读写的互斥量加锁解锁
	void MutexBufferLock();
	void MutexBufferUnLock();

	// 清理所有的数据
	void ClearAllData();
	
	// 设置事件
	void SetAcceptEvent(HANDLE hAcceptEvent);

	OVERLAPPED m_wsaOvLap;  // OverLapped结构
    Operation m_eOperation;     // Type of operation issued
    LONG m_lOutstandingRecv; // Number of outstanding overlapped ops on 
	LONG m_lOutstandingSend;
	LONG m_lPendingSend;	
	bool m_bClosing; // Is the socket closing?
    SOCKET m_skClient;  // Used for AcceptEx client socket
	char m_szbuf[nMaxBuffer];           // Buffer for recv/send/AcceptEx
    HANDLE m_muBufferMutex;             // 多线程会访问这个缓冲区，特别是接受函数是交给系统进程完成对缓冲区的写入，所以必须用互斥量来同步线程
	sockaddr_in m_addrPeer;  // struct of peer sockaddr
	State m_eStatus;
    CRITICAL_SECTION   m_csSockCritSec;     // Protect access to this structure

	ULONG m_ulBuflen;        // Length of the buffer

    HANDLE m_hPostAccept;  // Accept Event

	char m_szMessageBuffer[BIG_BUFFER];  // 消息存放的缓冲区
	unsigned int m_iMessageBufLen;  // 消息缓冲区的长度
	
private:
	int m_iClentID;  // Client ID
	static CScheduler m_oScheduler;
};

#endif