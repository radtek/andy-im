//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// IOCPPostWriteThread.cpp : ר����������ɶ˿ڹ���Ŀͻ���д���ʵ���ļ�
//
// Author: Leo
// Date: 08/04/2008
//
//////////////////////////////////////////////////////////////////////
#include "../HeaderFiles/stdafx.h"
#include "IOCPPostWriteThread.h"


CPostWriteThread::CPostWriteThread(HANDLE hIOCP)
{
	m_hIOCP= hIOCP ; 
}

CPostWriteThread::~CPostWriteThread()
{
	
}

void  CPostWriteThread::Run()
{/*
	while( !IsNeedStop() )
	{
		CSendData* pSendData = NULL;
		if( !pQueueDataToPolling->GetFromFront( (void*&)pSendData,50) )
		{
			if( !pQueueDataToGUI->GetFromFront( (void*&)pSendData,50) )
				continue;
		}
		
		if( NULL==pSendData )
			continue;
		
		//
		m_poClientMgr->LockClientMgr( FALSE /*ReadLock* /);
		
		CClient* pClient = m_poClientMgr->GetClientByID(pSendData->uClientID);
		if( NULL!=pClient )
		{			
			MyOverLapped* pOV = (MyOverLapped*)pClient->GetIOFlag();
			//
			m_poClientMgr->UnLockClientMgr( );
			
			if( NULL!=pOV )
			{
				pOV->pSendByteBuff = pSendData->pSendBuff;				
				//pOV->state = stWriting;
				//PostQueuedCompletionStatus( m_hIOCP, 0, 0, pOV );
			}
		}
		else
		{
			//
			m_poClientMgr->UnLockClientMgr( );
		}
		
		delete pSendData;		
	}*/
}
