// TcpSendThread.cpp: implementation of the CTcpSendThread class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "TcpSendThread.h"
#include "comm_buff.h"
#include "ServerLog.h"
#include <mmsystem.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern  CServerLog*		g_pSvrLog;
extern int g_ClientID;



CTcpSendThread::CTcpSendThread(CTcpCommunication *pTcpCommu, BYTE bType, int nMaxRecordNum)
{
	m_pSynsendList = new CSynList;
	InitializeCriticalSection(&m_otagRecordSection);
	InitializeCriticalSection(&m_csendsection);
	InitializeCriticalSection(&m_cwritefilesection);
	
	m_bType = bType;
	
	m_bCount = 0;
	m_nMaxRecordSize = nMaxRecordNum;
	m_pTcpComm = pTcpCommu;
}

CTcpSendThread::~CTcpSendThread()
{
	
	m_pTcpComm = NULL;
	DeleteCriticalSection(&m_otagRecordSection);
	DeleteCriticalSection(&m_csendsection);
	DeleteCriticalSection(&m_cwritefilesection);
}

void CTcpSendThread::Run()
{
	// ��������Socket���Ѿ������õģ������߳�����ʱ������״̬
	BOOL bConnected = TRUE;
	DWORD dwPreTime = GetTickCount();
	BOOL bFirstRun = TRUE;
	while (!IsNeedStop())
	{   	
		if (!bConnected) 
		{
			if (!m_pTcpComm->InitSocket() )
			{
				// ��ʼ��ʧ�ܣ��ȵ���ȥ����
				Sleep(1000);
				continue;
			}
            bConnected = TRUE;		
		}

		// ����10���ӷ���һ��������
 		DWORD dwendTime = GetTickCount();
 		if (dwendTime - dwPreTime > 1000*10 )
 		{
             if (SOCKET_ERROR == SendConnectPacket())
             {
 				// Socket�������ˣ���������ѭ����ȥ����Socket
 				bConnected = FALSE;
 				continue;
             }
 			else
 			{
 				// ��¼�µ�ʱ��
 				dwPreTime = GetTickCount();
 			}
 		}

		if (m_bType == 1)  // m_bType=1ʱΪʵʱ����
		{
	
			while ((m_pSynsendList->GetCount() !=0) && (bConnected))
			{
				if (SOCKET_ERROR == SendBuildDatas())
				{
					// Socket�������ˣ�������ǰѭ����ȥ��ʼ��Socket
					bConnected = FALSE;
					break;
				}
			}

			ResetEvent(m_pSynsendList->m_hSynListEvent);
			WaitForSingleObject(m_pSynsendList->m_hSynListEvent, 1000);
		}
		else
		{
			Sleep(5);
		}
		bFirstRun = FALSE;
	}
	Sleep(5);	
}



///*********************************************************
//�������Ϣ��ṹ����
//******************************************************/
//void CTcpSendThread::BuildRealTimeData(S_TAG_RECORD &oTagRecord, CSendBuff &oSndBuff)
//{
//    char szUserNum = {0};
//	sprintf((char*)szUserNum, "%020s", oTagRecord.szTelephone);
//	oSndBuff.AddBytes((BYTE*)szUserNum, 20);
//	oSndBuff.AddInt(oTagRecord.lVisitTime);
//	oSndBuff.AddByte(oTagRecord.byCheckInfo);
//}


void CTcpSendThread::Stop()
{
	// Ȼ��֪ͨ�̹߳ر�
	NotifyStop();
	// �Ȱ�Socket�رգ��������ͺͽ��պ����ͻ᷵��
	m_pTcpComm->CloseSocket();
	// �ȴ��رգ�����ǿ��ֹͣ
	WaitForSingleObject(m_pThreadHandle, INFINITE);

	CloseHandle(m_pThreadHandle);
	m_pThreadHandle = NULL;
	//AfxOutputDebugString("CTcpSendThread closed\n");
}
/**************************************************
���췢�����ݰ��ṹ��

****************************************************/
int CTcpSendThread::SendBuildDatas( )
{
	//���㵱ǰʱ�䣬���ʱ������100�룬��ô���Զ�����socket���ӣ�������Ҫ,Ϊ�˽��server2�����ղ������ݹر�����
	int nRet;
	EnterCriticalSection(&m_csendsection);
	int nsize = m_pSynsendList->GetCount();
	if( nsize > 0 )
	{
		IMMsg  *temp = NULL;
		temp = (IMMsg *) m_pSynsendList->GetHead();
		
		if(NULL != temp)
		{
			OutputDebugStr(LPCTSTR(temp->szSendBuf));
			nRet = m_pTcpComm->Send(temp->szSendBuf+4, temp->iLen);			   
			{
// 				CString m_ReceiveData;
// 				for (int i = 0; i< temp->GetLength()-4;i++)
// 				{
// 					CString str;
// 				    str.Format("%02X ",temp->GetBuffer()[i+4]);
// 					m_ReceiveData+=str;
// 				}
// 		        AfxOutputDebugString(LPCTSTR(m_ReceiveData));
// 				AfxOutputDebugString("\n");
// 				g_pWriteAllFileThread->WriteStringToFile(m_ReceiveData);

		
				
			}
			int error = WSAGetLastError();
			if ( nRet == SOCKET_ERROR)
			{
				delete temp;
			    g_pSvrLog->AddRunLog(LL_ERROR, "[ͨ����Ϣ]�����ݷ���::SendBuildDatas: Send Failed--- ");
			     
				Sleep(100);
				LeaveCriticalSection(&m_csendsection);
				return nRet;
			}
			else
			{
			  //  g_pSvrLog->AddRunLog(LL_DEBUG,  "[ͨ����Ϣ]��---������������ѷ���%d����¼.---",g_SendMsgNum);
// 				CString str;
// 				str.Format("---������������ѷ���%d����¼.---",g_SendMsgNum);
//  			g_pWriteAllFileThread->WriteStringToFile(str);
				delete temp;
				//m_pSynsendList->RemoveHead();				   
			}			
		}
		Sleep(20);
	}
	LeaveCriticalSection(&m_csendsection);
	return nRet;
}
int CTcpSendThread::SendConnectPacket()
{
	// ���Ϳ���Ϣ
	return m_pTcpComm->Send("", 0);
}

//int CTcpSendThread::SendRenewRequest(BYTE MsgType,int type)
//{
//    int nRet = 0;
//	CSendBuff oShakeBuff;
//    oShakeBuff.AddByte(CMD_REAL_HEAD);   
//	oShakeBuff.AddInt(g_ClientID);  //ѧУ��ClientID
//	oShakeBuff.AddInt(0); //��ˮ��
//	oShakeBuff.AddByte(MsgType); //�ͻ��˷�����Ϣ����
//	oShakeBuff.AddInt(4); //�����
//	oShakeBuff.AddInt(0); //�ɹ����
//	oShakeBuff.AddByte(CMD_REAL_TAIL);    //construct a shake hand data struct
//	if (oShakeBuff.GetLength() > 0)
//	{
//		nRet = m_pTcpComm->Send((char *)oShakeBuff.GetBuffer(), oShakeBuff.GetLength());
//		if ( nRet == SOCKET_ERROR)
//		{
//			return nRet;
//		}		
//		else 
//		{
//			if (type == 0)
//			{
//				g_pSvrLog->AddRunLog(LL_INFO, "[ͨ����Ϣ]��������������ѷ���һ������绰���������Ϣ." );
//				EnterCriticalSection(&m_cwritefilesection);
// 			    string str1;
//				str1 = "������������ѷ���һ������绰���������Ϣ.";
//				g_pWriteAllFileThread->WriteStringToFile(str1);
//				LeaveCriticalSection(&m_cwritefilesection);
//			}
//			else if (type == 1)
//			{
//				g_pSvrLog->AddRunLog(LL_INFO, "[ͨ����Ϣ]��������������ѷ���һ������ʱ����Ը�����Ϣ." );
//				EnterCriticalSection(&m_cwritefilesection);
//				string str2;
//				str2 = "������������ѷ���һ������ʱ����Ը�����Ϣ.";
//				g_pWriteAllFileThread->WriteStringToFile(str2);
//				EnterCriticalSection(&m_cwritefilesection);				
//			}
//			return nRet;
//		}
//	}
//	else
//	{
//		return nRet;
//	}	   
//}