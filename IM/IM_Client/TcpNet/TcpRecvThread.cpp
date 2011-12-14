
//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CTcpRecvThread.cpp : �ͻ��˹��������ʵ���ļ�
//
// Author: Fan Feifei
// Date: 08/14/2008
//
//////////////////////////////////////////////////////////////////////

// TcpRecvThread.cpp: implementation of the CTcpRecvThread class.
//�������ݸ����߳�
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
#include "../stdafx.h"
#include "TcpRecvThread.h"
#include "comm_buff.h"
#include "ServerLog.h"
//#include "TagsArray.h"
#include "MyHead.h"
#include <fstream>
#include <time.h>
#include "../MainUI/main_frame.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern  CServerLog*		g_pSvrLog;
extern int g_ClientID;
extern int g_ClientID;


CTcpRecvThread::CTcpRecvThread(CTcpCommunication * pTcpComm)
{
    m_pTcpCommu = pTcpComm;
		
	memset(m_RecvBuffer, 0, BIG_BUFFER);
	m_iBufferLength = 0;
	// ��������Socket���Ѿ������õģ������߳�����ʱ������״̬
	m_bConnected = TRUE;
}


CTcpRecvThread::~CTcpRecvThread()
{
    m_pTcpCommu = NULL;
}

void CTcpRecvThread::Run()
{
	// ׼��һ�������õĻ�����
	char buff[RENEW_MAX_LEN];
	memset(buff, 0, RENEW_MAX_LEN);

	while ( !IsNeedStop() ) 
	{
		if (!m_bConnected) 
		{
			if (!m_pTcpCommu->InitSocket())			
			{
				// ��ʼ��ʧ�ܣ��ȵ���ȥ����
				Sleep(1000);
				continue;
			}
			else
			{
				m_bConnected = TRUE;
			}
		}

		// ��������ŵĹرյ�Socket����ֵΪ0�����Socket�����󷵻�-1
		int nCount = m_pTcpCommu->Recv(buff, RENEW_MAX_LEN);
		if (SOCKET_ERROR == nCount)
		{
			// ��Ҫ��������
			m_bConnected = FALSE;
		}
		else if (0 == nCount)
		{
			continue;
		}
		else
		{
			SplitPacket(buff, nCount);
			//CString csInfo;
			//csInfo.Format("Recivide %d Bytes\n", nCount);
			//AfxOutputDebugString(csInfo);

		}
	    Sleep(5);
	}
}
void CTcpRecvThread::Stop()
{
	// �Ȱ�Socket�رգ��������ͺͽ��պ����ͻ᷵��
	m_pTcpCommu->CloseSocket();
	
	// Ȼ��֪ͨ�̹߳ر�
	NotifyStop();
	
	// �ȴ��رգ�����ǿ��ֹͣ
	WaitForSingleObject(m_pThreadHandle, INFINITE);
	
	CloseHandle(m_pThreadHandle);
	m_pThreadHandle = NULL;
	//AfxOutputDebugString("CTcpRecvThread closed\n");
}


/**************************************************
�Խ��յ������ݰ����д�����
******************************************************/
void CTcpRecvThread::RenewHandShake(char* pData, int iCount)
{
 
	//���յ������ݷ�����ջ���	
	CRecvBuff recvBuff((unsigned char*)pData, iCount,TRUE);
	S_SHAKE_DATA oShakeData;
	
	byte MsgHead = NULL;  //��Ϣͷ
	byte m_bSerMsgType = NULL;  //����ƽ̨�����ݰ�����
	
	BOOL m_bClientReady = TRUE;  //�ͻ���׼���ñ�־λ
	
	BOOL m_bDataRenewed = FALSE;  //���ݸ��±�־λ
	int iCommandLength = 0; 
	
	BYTE m_bFlag = 0;           
	WORD m_wMsgCount = 0;
	int iSerTime = 0;
	
	char pRenewData[BIG_BUFFER] = {0};
	
	recvBuff.GetByte(MsgHead);
    recvBuff.GetInt(oShakeData.iCommandID);  //get CommandID
	recvBuff.GetByte(m_bSerMsgType); //get 6th byte-commandCode
	recvBuff.GetInt(iCommandLength);
	
	switch(m_bSerMsgType) 
	{
	case CMD_TIME_CHECK: //ʱ��У����Ϣ
        recvBuff.GetInt(iSerTime);
		ResetClientTime(iSerTime);  //����ϵͳʱ��
		break;
	case CMD_LOGIN:   //��¼��Ϣ
		{	
			int flag=-1;

			recvBuff.GetInt(flag);

			m_pTcpCommu->frame_wnd_->m_p_login->SetLogin(flag);
		}
		break;
	case CMD_GET_USER: //��¼�ɹ����ȡ�û���Ϣ
		{

			// ȡ���û���
			BYTE byUser[100];
			memset(byUser, 0, 100);
			BYTE byLen = 0;
			recvBuff.GetByte(byLen);
			recvBuff.GetBytes(byUser, byLen);

			// ȡ��ǩ��
			BYTE bysign[100];
			memset(bysign, 0, 100);
			byLen = 0;
			recvBuff.GetByte(byLen);
			recvBuff.GetBytes(bysign, byLen);

			m_pTcpCommu->frame_wnd_->SetUser(LPCTSTR(byUser),LPCTSTR(bysign));
            m_pTcpCommu->frame_wnd_->ShowWindow(SW_SHOW);

			
		}
		break;
	case CMD_GET_FRIEND://�õ�������Ϣ
		{
           
			//ȡ��IM����
			int imno= -1;
			int type = -1;
			recvBuff.GetInt(imno);
			recvBuff.GetInt(type);//����״̬
			//ȡ���û���
			// ȡ���û���
			BYTE byUser[100];
			memset(byUser, 0, 100);
			BYTE byLen = 0;
			recvBuff.GetByte(byLen);
			recvBuff.GetBytes(byUser, byLen);
			//ȡ��ǩ��

			BYTE bysign[100];
			memset(bysign, 0, 100);
			byLen = 0;
			recvBuff.GetByte(byLen);
			recvBuff.GetBytes(bysign, byLen);

			FriendListItemInfo friendinfo;
			friendinfo.id = imno;
			friendinfo.type = type;
			_stprintf_s((char*)friendinfo.nick_name, 50 - 1, _T("%s"), (char*)byUser);
			_stprintf_s((char*)friendinfo.description, 100 - 1, _T("%s"), (char*)bysign);
			//m_pTcpCommu->frame_wnd_->AddUIList(ADD_LIST_FRIEND,friendinfo);
			 m_pTcpCommu->frame_wnd_->SendMessage(MSG_GETFRIEND,(WPARAM)&friendinfo,0);

		}
		break;
	case CMD_SEND_MSG:
		{
			//ȡ��IM����
			int imno;
			recvBuff.GetInt(imno);
			//ȡ����Ϣ
			BYTE byMsg[256];
			memset(byMsg, 0, 256);
			BYTE byLen = 0;
			recvBuff.GetByte(byLen);
			recvBuff.GetBytes(byMsg, byLen);
			//ȡ����Ϣ


			MSGBODY  msgbody;
			msgbody.imid = imno;
			sprintf(msgbody.msg,_T("%s"),(char*)byMsg);
			//::SendMessage(m_wnd,MSG_HAVEDATA,(WPARAM)&msgbody,0);
			if (strlen((char*)byMsg) !=0)
			{
			  //frame_wnd_->SendMessage(MSG_HAVEDATA,(WPARAM)&msgbody,0);
			  m_pTcpCommu->frame_wnd_->SendMessage(MSG_HAVEDATA,(WPARAM)&msgbody,0);
			}

		}
		break;
	case CMD_FRIEND_ONLINE:		// ��������
		{
			//ȡ�����ߵ�IM����
			int imno;
			recvBuff.GetInt(imno);
			m_pTcpCommu->frame_wnd_->UpdateFriend(imno);
		}
		break;
	default:
		
		break;
	}
}

/************************************
���Ϳͻ���->������Ӧ����Ϣ,
**************************************/

void CTcpRecvThread::SendShakeMsg(S_SHAKE_DATA& oShakeData)
{
	CSendBuff oShakeBuff;
    
    oShakeBuff.AddByte(CMD_REAL_HEAD);   
	oShakeBuff.AddInt(g_ClientID);  //ѧУ��ClientID
	oShakeBuff.AddInt(oShakeData.iCommandID); //��ˮ��
	oShakeBuff.AddByte(oShakeData.bClientMsgType); //�ͻ��˷�����Ϣ����
	oShakeBuff.AddInt(4); //�����
	
	oShakeBuff.AddInt(oShakeData.iMsgResult); //�ɹ����
	oShakeBuff.AddByte(CMD_REAL_TAIL);    //construct a shake hand data struct
    if (oShakeBuff.GetLength() > 0 && (m_bConnected))
	{
		int nRet = m_pTcpCommu->Send((char *)oShakeBuff.GetBuffer(), oShakeBuff.GetLength());
					
		if ( nRet == SOCKET_ERROR)
		{
			m_bConnected = FALSE;
			return;
		}
		else
		{
		    g_pSvrLog->AddRunLog(LL_INFO, "[ͨ����Ϣ]��---������������ѷ���һ��Ӧ����Ϣ---" );
		}
		Sleep(100);
	}
	else
	{
	    Sleep(500);
	}
}
/******************************************************
У��ͻ���ϵͳʱ��
*********************************************************/
void CTcpRecvThread::ResetClientTime(int iServerTime)
{
	//������ʱ��ǿתΪtime_t��ʽ,��Ϊ��λ
	//��ȡϵͳʱ�䣬ת����int��ʽ
	SYSTEMTIME ct;
	//GetSystemTime(&ct);
	GetLocalTime( &ct);
	struct tm gm = { ct.wSecond, ct.wMinute, ct.wHour, ct.wDay, ct.wMonth-1, ct.wYear-1900, ct.wDayOfWeek, 0, 0 };
	time_t clienttime = mktime(&gm);
	int iClientTime = (int)clienttime;
    //��������3����ʱ������ͻ���ʱ��
	if (abs(iServerTime-iClientTime)>180)
	{
		//������ʱ��int->time_t->SYSTEMTIME
         time_t ServerTime = (time_t)iServerTime;
         tm gm ;
		 gm = *localtime(&ServerTime);
		 SYSTEMTIME serst = { 1900+gm.tm_year, 1+gm.tm_mon, gm.tm_wday, gm.tm_mday, gm.tm_hour, gm.tm_min, 0, 0 };
		 //SetSystemTime(&serst);
		 SetLocalTime( &serst);
	      
		 string		strTime;
		 sprintf((char*)strTime.c_str(),_T("%04u.%02u.%02u %02u:%02u:%02u.%03u"),serst.wYear, 
			 serst.wMonth, 
			 serst.wDay, 
			 serst.wHour, 
			 serst.wMinute, 
			 serst.wSecond, 
			 serst.wMilliseconds);
	
	}
}

void CTcpRecvThread::SplitPacket(char* pData, int iCount)
{
	// ������Ϣ�Ļ������ŵ��������
	if ((m_iBufferLength + iCount) > BIG_BUFFER)
	{
		g_pSvrLog->AddRunLog(LL_ERROR, "[ͨ����Ϣ]��Msg Buffer Error, iLength = %d, m_iBufferLength = %d", iCount, m_iBufferLength);
        // �������ε����ݣ����¿�ʼ
		return;
	}
	// ���մ�������������
	memcpy(m_RecvBuffer+m_iBufferLength, pData, iCount);
	
	m_iBufferLength += iCount;
	
	unsigned int iHeadPos = 0; // ֡ͷ��λ��
	unsigned int iTailPos = 0;
    unsigned int i = 0; //ѭ����
	
	// ���������Ϣ������������ݣ��ҵ���һ��֡ͷ����ǰ��Ķ���������
	for (; i <= m_iBufferLength; i++)
	{
		if (CMD_REAL_HEAD == m_RecvBuffer[i])
		{
			iHeadPos = i;
			// ��֤�жϲ��֣��ж�һ�º��滹���������������ĳ���
			// ���費�������ݵĻ���һ֡��ϢҲҪ����15���ֽڣ����Կ���������
			if ((iHeadPos+11) > m_iBufferLength)
			{
                // ���ڱ����ǲ����������ݣ���������
				memmove(m_RecvBuffer, m_RecvBuffer+i, m_iBufferLength-i);
				break;
			}
			else
			{
				//������Ϣ֡�Ķ��壬���ݳ��ȵ�λ��Ϊ��֡ͷ��6~9�ֽ�		
				int iMsgLength = (m_RecvBuffer[i+6] << 24) & 0xff000000;
				iMsgLength |= (m_RecvBuffer[i+7] << 16) & 0x00ff0000;
				iMsgLength |= (m_RecvBuffer[i+8] << 8) & 0x0000ff00;
				iMsgLength |= (m_RecvBuffer[i+9]) & 0x000000ff;
				
				if ((iMsgLength < 0) || (iMsgLength > BIG_BUFFER - 15))
				{
					// ���ݳ���С��0���Ǵ�����,�����������
					continue;
				}
				
				// ������֡β��λ�����£����û�����������һ֡
				iTailPos = i + 9 + iMsgLength + 1;
				
				// ����жϵ�֡β�ǲ�������εĻ���������
				if (iTailPos > m_iBufferLength)
				{
					// ���ڱ����ǲ����������ݣ���������
					memmove(m_RecvBuffer, m_RecvBuffer+i, m_iBufferLength-i);
					break;
				}
				else
				{
					int iMsgLen = iTailPos - iHeadPos + 1;
					if (CMD_REAL_TAIL == m_RecvBuffer[iTailPos])
					{
						// û����������ȥ������
						BYTE byBuff[BIG_BUFFER] = {0};
						memcpy(byBuff, m_RecvBuffer + i, iMsgLen);				
						RenewHandShake((char*)byBuff, iMsgLen);
						i += iMsgLen - 1;
					}
				}
			}
		}
	}
	if (m_iBufferLength > i)
	{
		m_iBufferLength -= i;
	}
	else
	{
        m_iBufferLength = 0;
	}
	
	if (BIG_BUFFER < m_iBufferLength)
	{
		m_iBufferLength = 0;
	}

}

BOOL CTcpRecvThread::IsSpace(char c)
{
	switch (c)     
	{   
	default:   
		return   FALSE;   
	case   ' ':   
	case   '\r':   
	case   '\n':   
	case   '\t':   
	case   '\v':
	case   '0':
		return   TRUE;   
	}   
}

void CTcpRecvThread::TrimLeft(std::string & str)
{   
	std::string::iterator iBegin, strBegin, strEnd;   
	for(iBegin = strBegin = str.begin(), strEnd = str.end(); iBegin != strEnd; iBegin++)     
	{
		if (!IsSpace(*iBegin))
		{   
			break;   
		}   
	}   
    
	if (iBegin != strBegin)     
	{   
		unsigned int nSize = 0;   
		for(;iBegin!=strEnd; strBegin++, iBegin++, nSize++)     
		{   
			(*strBegin)	= (*iBegin);   
		}		
		str.resize(nSize);   
	}   
}   

//BOOL CTcpRecvThread::AutoUpdate(string &strInfo)
// {
 //   //��ȡ���س��������·��
	////CRF_JXT_UNIONDlg *pMain = (CRF_JXT_UNIONDlg*)AfxGetApp()->m_pMainWnd;
	//string strLocalPath;
	//int     nPos = 0;
	////GetModuleFileName(NULL,strLocalPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
	//strLocalPath.ReleaseBuffer();   
	//nPos=strLocalPath.ReverseFind('\\');   
	//strLocalPath=strLocalPath.Left(nPos);
	//string strexePath;
	//strexePath = strLocalPath + _T("\\JXTUpdate.exe");
 //    //��ֹͣ�м��
	////pMain->StopService();
	////�������ص�EXE����
	//HINSTANCE his;
	////string strparament = "-u jxt -p 1013";

	//
	//his =::ShellExecute(NULL, _T("open"), strexePath, (LPCTSTR)strInfo, NULL, SW_SHOW );
	//if ((int)his == ERROR_FILE_NOT_FOUND)
	//{
	//	//û�ҵ�exe�ļ������·�������ļ�����
	//	return FALSE;
	//}
	//return TRUE;
//}

//BOOL CTcpRecvThread::UploadLogFile(string &strUser, string &strPass, string &strLogData)
//{
//	//����0-�ϴ���־�ļ�ʧ��(�����ļ���һ���ϴ�ʧ�ܾ���ʧ��)
//	//����1-�����ļ����ϴ��ɹ�
//
//	BOOL flag; 
//	BOOL flagall;
//	BOOL flaga;
//
//	//��ȡ��������·��
//	char chFilePath[MAX_PATH];                
//	GetModuleFileName(NULL, chFilePath, MAX_PATH);
//	string strRunPath = chFilePath;
//	string sAllLogFile ="";
//	string sLogFile = "";
//
//	int iIndex = strRunPath.ReverseFind('\\');
//	strRunPath = strRunPath.Left(iIndex);
//
//	//�����ļ�
//	string strIniPath = strRunPath + "\\Config.ini";
//	//�������ļ���ȡ��������ַ
//	char iniBuf[80];
//	memset(iniBuf,0,80);
//	GetPrivateProfileString("IP", "PEERADDR", NULL,iniBuf,80, (LPCTSTR)strIniPath);
//	
//    string strIp = "";
//	strIp.Format("%s",iniBuf);
//
//	//��ʼ��FTP������
//
//	CFtpClient ftpClient(strIp,strUser,strPass,21);
//
//	//���ӷ�����
//	if (ftpClient.Connect())
//	{
//		g_pSvrLog->AddRunLog(LL_INFO,"[ͨ����Ϣ]����ʼ���������ϴ���־");
//		//��ȡ������־�ļ�·��allstudent��student�ĵ�
//		sLogFile = strRunPath+"\\log";
//		sAllLogFile = strRunPath + "\\AllStudentlog";
//
//		string tempFileName1 = "";
//		string tempFileName2 ="";
//
//       
//		tempFileName1.Format("\\%d_AllStudent_%s.log",g_ClientID,strLogData);
//		tempFileName2.Format("\\%d_Student_%s.log",g_ClientID,strLogData);
//
//
//		//sAllLogFile��sLogFileΪҪ�ϴ�����־�ı���·��
//		sAllLogFile += tempFileName1;
//		sLogFile += tempFileName2;
//
//
//
//		//���FTP��������Ĭ��·��
//		string strdir;
//		BOOL err =  ftpClient.GetCurrentDirectory(strdir);
//		//���Զ���ļ�������·�����˴�Ĭ��Ҫ�ϴ�����־�ļ�����FTP·���ĸ�Ŀ¼��
//		//sAllLogFile��sLogFileΪ��FTP���������ϴ��ļ���·��
//		string strFile = "";
//	    string strUpAllStudent,strUpStudent,strLstUpAllStudent,strLstUpStudent;
//		strUpAllStudent.Format("\\%d_AllStudent_%s.log",g_ClientID,strLogData);
//		strUpStudent.Format("\\%d_Student_%s.log",g_ClientID,strLogData);
//        //fclose()
//		//�ϴ�sAllLogFile
//		//����־�ļ����Ƶ��µ��ļ���
//		string strLocalAllstudentNewFile,strLocalStudentNewFile;
//        strLocalAllstudentNewFile.Format("%s\\AllStudentlog\\%d_AllStudent_%s_copy.log",strRunPath,g_ClientID,strLogData);
//		strLocalStudentNewFile.Format("%s\\log\\%d_Student_%s_copy.log",strRunPath,g_ClientID,strLogData);
//
//        CopyFile(sAllLogFile,strLocalAllstudentNewFile,FALSE);
//		CopyFile(sLogFile,strLocalStudentNewFile,FALSE);
//		int errcopy = GetLastError();
//		  
//		if (FALSE == ftpClient.UploadFile(strLocalAllstudentNewFile,strUpAllStudent))
//		{
//			int err1 = GetLastError();
//			strFile.Format("�ϴ�%s��������ʧ��",tempFileName1);
//			g_pWriteAllFileThread->WriteStringToFile(strFile);
//
//			g_pSvrLog->AddRunLog(LL_INFO,"[ͨ����Ϣ]�����������ϴ�AllLogFile��־ʧ��");
//
//			flagall = 0;
//
//		}
//		else
//		{
//			strFile.Format("�ϴ�%s���������ɹ�",tempFileName1);
//			g_pWriteAllFileThread->WriteStringToFile(strFile);
//			g_pSvrLog->AddRunLog(LL_INFO,"[ͨ����Ϣ]�����������ϴ�AllLogFile��־�ɹ�");
//
//			flagall = 1;
//
//
//		}
//		//�ϴ�sLogFile
//		if (FALSE == ftpClient.UploadFile(strLocalStudentNewFile,strUpStudent))
//		{
//			strFile.Format("�ϴ�%s��������ʧ��",tempFileName2);
//			g_pWriteAllFileThread->WriteStringToFile(strFile);
//
//			g_pSvrLog->AddRunLog(LL_INFO,"[ͨ����Ϣ]�����������ϴ�LogFile��־ʧ��");
//
//			flaga = 0;
//
//
//		}
//		else
//		{
//			strFile.Format("�ϴ�%s���������ɹ�",tempFileName2);
//			g_pWriteAllFileThread->WriteStringToFile(strFile);
//
//			g_pSvrLog->AddRunLog(LL_INFO,"[ͨ����Ϣ]�����������ϴ�LogFile��־�ɹ�");
//			
//			flaga = 1;
//
//
//		}
//
//
//		//�ر�ftp����
//		ftpClient.DisConnect();
//		//ɾ���ļ�
//		DeleteFile(strLocalAllstudentNewFile);
//		DeleteFile(strLocalStudentNewFile);
//
//	}
//    flag = flagall&&flaga;
//	return flag;
//
//}

//����base64��������Ӧ����4�ı���(����mime��׼)
//�������4�������ش���
//ע�� ��������һ���ַ� ��ô���Ȳ�׼�� ���ܻ��1 
//����buf����
//##ModelId=47CEBB9702A0
int CTcpRecvThread::Base64Dec(char *buf,char*text,int size) 
{
	if(size%4)
	{
		return -1;
	}
	char chunk[4];
	int parsenum=0;
	
	while(size>0)
	{
		chunk[0] = GetBase64Value(text[0]); 
		chunk[1] = GetBase64Value(text[1]); 
		chunk[2] = GetBase64Value(text[2]); 
		chunk[3] = GetBase64Value(text[3]); 
		
		*buf++ = (chunk[0] << 2) | (chunk[1] >> 4); 
		*buf++ = (chunk[1] << 4) | (chunk[2] >> 2); 
		*buf++ = (chunk[2] << 6) | (chunk[3]);
		
		text+=4;
		size-=4;
		parsenum+=3;
	}
	
	return parsenum;
} 

char CTcpRecvThread::GetBase64Value(char ch)
{
	if ((ch >= 'A') && (ch <= 'Z')) 
		return ch - 'A'; 
	if ((ch >= 'a') && (ch <= 'z')) 
		return ch - 'a' + 26; 
	if ((ch >= '0') && (ch <= '9')) 
		return ch - '0' + 52; 
	switch (ch) 
	{ 
	case '+': 
		return 62; 
	case '/': 
		return 63; 
	case '=': /* base64 padding */ 
		return 0; 
	default: 
		return 0; 
	} 
}