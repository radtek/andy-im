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
	// 传进来的Socket是已经建立好的，所以线程启动时是连接状态
	BOOL bConnected = TRUE;
	DWORD dwPreTime = GetTickCount();
	BOOL bFirstRun = TRUE;
	while (!IsNeedStop())
	{   	
		if (!bConnected) 
		{
			if (!m_pTcpComm->InitSocket() )
			{
				// 初始化失败，等等再去尝试
				Sleep(1000);
				continue;
			}
            bConnected = TRUE;		
		}

		// 大于10秒钟发送一次心跳包
 		DWORD dwendTime = GetTickCount();
 		if (dwendTime - dwPreTime > 1000*10 )
 		{
             if (SOCKET_ERROR == SendConnectPacket())
             {
 				// Socket出问题了，跳过本次循环，去重置Socket
 				bConnected = FALSE;
 				continue;
             }
 			else
 			{
 				// 记录新的时间
 				dwPreTime = GetTickCount();
 			}
 		}

		if (m_bType == 1)  // m_bType=1时为实时传输
		{
	
			while ((m_pSynsendList->GetCount() !=0) && (bConnected))
			{
				if (SOCKET_ERROR == SendBuildDatas())
				{
					// Socket出问题了，跳出当前循环，去初始化Socket
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
//构造短信息体结构函数
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
	// 然后通知线程关闭
	NotifyStop();
	// 先把Socket关闭，这样发送和接收函数就会返回
	m_pTcpComm->CloseSocket();
	// 等待关闭，不能强制停止
	WaitForSingleObject(m_pThreadHandle, INFINITE);

	CloseHandle(m_pThreadHandle);
	m_pThreadHandle = NULL;
	//AfxOutputDebugString("CTcpSendThread closed\n");
}
/**************************************************
构造发送数据包结构体

****************************************************/
int CTcpSendThread::SendBuildDatas( )
{
	//计算当前时间，如果时间间隔是100秒，那么就自动进行socket连接，否则不需要,为了解决server2分钟收不到数据关闭连接
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
			    g_pSvrLog->AddRunLog(LL_ERROR, "[通信信息]：数据发送::SendBuildDatas: Send Failed--- ");
			     
				Sleep(100);
				LeaveCriticalSection(&m_csendsection);
				return nRet;
			}
			else
			{
			  //  g_pSvrLog->AddRunLog(LL_DEBUG,  "[通信信息]：---本机向服务器已发送%d条记录.---",g_SendMsgNum);
// 				CString str;
// 				str.Format("---本机向服务器已发送%d条记录.---",g_SendMsgNum);
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
	// 发送空消息
	return m_pTcpComm->Send("", 0);
}

//int CTcpSendThread::SendRenewRequest(BYTE MsgType,int type)
//{
//    int nRet = 0;
//	CSendBuff oShakeBuff;
//    oShakeBuff.AddByte(CMD_REAL_HEAD);   
//	oShakeBuff.AddInt(g_ClientID);  //学校的ClientID
//	oShakeBuff.AddInt(0); //流水号
//	oShakeBuff.AddByte(MsgType); //客户端返回信息类型
//	oShakeBuff.AddInt(4); //命令长度
//	oShakeBuff.AddInt(0); //成功与否
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
//				g_pSvrLog->AddRunLog(LL_INFO, "[通信信息]：本机向服务器已发送一条请求电话号码更新信息." );
//				EnterCriticalSection(&m_cwritefilesection);
// 			    string str1;
//				str1 = "本机向服务器已发送一条请求电话号码更新信息.";
//				g_pWriteAllFileThread->WriteStringToFile(str1);
//				LeaveCriticalSection(&m_cwritefilesection);
//			}
//			else if (type == 1)
//			{
//				g_pSvrLog->AddRunLog(LL_INFO, "[通信信息]：本机向服务器已发送一条请求时间策略更新信息." );
//				EnterCriticalSection(&m_cwritefilesection);
//				string str2;
//				str2 = "本机向服务器已发送一条请求时间策略更新信息.";
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