//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// IOCPPostWriteThread.h: ר����������ɶ˿ڹ���Ŀͻ���д���ͷ�ļ�
//
// Author: Leo
// Date: 08/04/2008
//
////////////////////////////////////////////////////////////////////// 
#ifndef _IOCP_POST_WRITE_THREAD_H_
#define _IOCP_POST_WRITE_THREAD_H_
#include "../HeaderFiles/common.h"
#include "../BaseThread/commThread.H"

class CPostWriteThread : public CRunTimeThread  
{
public:
	CPostWriteThread(HANDLE hIOCP);
	virtual ~CPostWriteThread();
	virtual void Run();
	
protected:
	HANDLE m_hIOCP;	
};

#endif