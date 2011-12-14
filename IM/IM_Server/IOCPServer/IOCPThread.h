//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// MyClass.h: 完成端口的工作线程类
//
// Author: Leo
// Date: 08/02/2008
//
////////////////////////////////////////////////////////////////////// 
#ifndef _IOCP_THREAD_H_
#define _IOCP_THREAD_H_
#include "CClientMgr.h"
#include "../HeaderFiles/common.h"
#include "../BaseThread/commThread.H"

#define COMPKEY_DIE ((DWORD) -1L)

class CIOCPThread : public CRunTimeThread
{
public:
	CIOCPThread(HANDLE hIOCP, SOCKET skListener, HANDLE hAcceptEvent);
	virtual ~CIOCPThread();

	virtual void Run();

	void HandleIO(CClient*& pClient, DWORD dwTransfered, int iError);

	void FreeClient(CClient*& pClient);

	void Accept(DWORD dwTransfered, CClient *pClient, int iError, BOOL& bClean);
	void Read(DWORD dwTransfered, CClient *pClient, int iError, BOOL& bClean);
	void Write(DWORD dwTransfered, CClient *pClient, int iError, BOOL& bClean);

protected:
private:
	HANDLE m_hIOCP;
	SOCKET m_skListener;
	HANDLE m_hAcceptEvent;
};
#endif