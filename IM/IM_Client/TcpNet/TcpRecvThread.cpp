
//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CTcpRecvThread.cpp : 客户端管理类类的实现文件
//
// Author: Fan Feifei
// Date: 08/14/2008
//
//////////////////////////////////////////////////////////////////////

// TcpRecvThread.cpp: implementation of the CTcpRecvThread class.
//接收数据更新线程
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
	// 传进来的Socket是已经建立好的，所以线程启动时是连接状态
	m_bConnected = TRUE;
}


CTcpRecvThread::~CTcpRecvThread()
{
    m_pTcpCommu = NULL;
}

void CTcpRecvThread::Run()
{
	// 准备一个接收用的缓冲区
	char buff[RENEW_MAX_LEN];
	memset(buff, 0, RENEW_MAX_LEN);

	while ( !IsNeedStop() ) 
	{
		if (!m_bConnected) 
		{
			if (!m_pTcpCommu->InitSocket())			
			{
				// 初始化失败，等等再去尝试
				Sleep(1000);
				continue;
			}
			else
			{
				m_bConnected = TRUE;
			}
		}

		// 如果是优雅的关闭的Socket返回值为0，如果Socket出错误返回-1
		int nCount = m_pTcpCommu->Recv(buff, RENEW_MAX_LEN);
		if (SOCKET_ERROR == nCount)
		{
			// 需要重置连接
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
	// 先把Socket关闭，这样发送和接收函数就会返回
	m_pTcpCommu->CloseSocket();
	
	// 然后通知线程关闭
	NotifyStop();
	
	// 等待关闭，不能强制停止
	WaitForSingleObject(m_pThreadHandle, INFINITE);
	
	CloseHandle(m_pThreadHandle);
	m_pThreadHandle = NULL;
	//AfxOutputDebugString("CTcpRecvThread closed\n");
}


/**************************************************
对接收到的数据包进行处理函数
******************************************************/
void CTcpRecvThread::RenewHandShake(char* pData, int iCount)
{
 
	//接收到的数据放入接收缓存	
	CRecvBuff recvBuff((unsigned char*)pData, iCount,TRUE);
	S_SHAKE_DATA oShakeData;
	
	byte MsgHead = NULL;  //信息头
	byte m_bSerMsgType = NULL;  //短信平台的数据包类型
	
	BOOL m_bClientReady = TRUE;  //客户端准备好标志位
	
	BOOL m_bDataRenewed = FALSE;  //数据更新标志位
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
	case CMD_TIME_CHECK: //时间校验信息
        recvBuff.GetInt(iSerTime);
		ResetClientTime(iSerTime);  //重设系统时间
		break;
	case CMD_LOGIN:   //登录消息
		{	
			int flag=-1;

			recvBuff.GetInt(flag);

			m_pTcpCommu->frame_wnd_->m_p_login->SetLogin(flag);
		}
		break;
	case CMD_GET_USER: //登录成功后获取用户信息
		{

			// 取出用户名
			BYTE byUser[100];
			memset(byUser, 0, 100);
			BYTE byLen = 0;
			recvBuff.GetByte(byLen);
			recvBuff.GetBytes(byUser, byLen);

			// 取出签名
			BYTE bysign[100];
			memset(bysign, 0, 100);
			byLen = 0;
			recvBuff.GetByte(byLen);
			recvBuff.GetBytes(bysign, byLen);

			m_pTcpCommu->frame_wnd_->SetUser(LPCTSTR(byUser),LPCTSTR(bysign));
            m_pTcpCommu->frame_wnd_->ShowWindow(SW_SHOW);

			
		}
		break;
	case CMD_GET_FRIEND://得到好友信息
		{
           
			//取出IM号码
			int imno= -1;
			int type = -1;
			recvBuff.GetInt(imno);
			recvBuff.GetInt(type);//在线状态
			//取出用户名
			// 取出用户名
			BYTE byUser[100];
			memset(byUser, 0, 100);
			BYTE byLen = 0;
			recvBuff.GetByte(byLen);
			recvBuff.GetBytes(byUser, byLen);
			//取出签名

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
			//取出IM号码
			int imno;
			recvBuff.GetInt(imno);
			//取出消息
			BYTE byMsg[256];
			memset(byMsg, 0, 256);
			BYTE byLen = 0;
			recvBuff.GetByte(byLen);
			recvBuff.GetBytes(byMsg, byLen);
			//取出消息


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
	case CMD_FRIEND_ONLINE:		// 还有上线
		{
			//取出上线的IM号码
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
发送客户端->服务器应答消息,
**************************************/

void CTcpRecvThread::SendShakeMsg(S_SHAKE_DATA& oShakeData)
{
	CSendBuff oShakeBuff;
    
    oShakeBuff.AddByte(CMD_REAL_HEAD);   
	oShakeBuff.AddInt(g_ClientID);  //学校的ClientID
	oShakeBuff.AddInt(oShakeData.iCommandID); //流水号
	oShakeBuff.AddByte(oShakeData.bClientMsgType); //客户端返回信息类型
	oShakeBuff.AddInt(4); //命令长度
	
	oShakeBuff.AddInt(oShakeData.iMsgResult); //成功与否
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
		    g_pSvrLog->AddRunLog(LL_INFO, "[通信信息]：---本机向服务器已发送一条应答信息---" );
		}
		Sleep(100);
	}
	else
	{
	    Sleep(500);
	}
}
/******************************************************
校验客户端系统时间
*********************************************************/
void CTcpRecvThread::ResetClientTime(int iServerTime)
{
	//服务器时间强转为time_t格式,秒为单位
	//获取系统时间，转换成int格式
	SYSTEMTIME ct;
	//GetSystemTime(&ct);
	GetLocalTime( &ct);
	struct tm gm = { ct.wSecond, ct.wMinute, ct.wHour, ct.wDay, ct.wMonth-1, ct.wYear-1900, ct.wDayOfWeek, 0, 0 };
	time_t clienttime = mktime(&gm);
	int iClientTime = (int)clienttime;
    //当误差大于3分钟时，重设客户端时间
	if (abs(iServerTime-iClientTime)>180)
	{
		//服务器时间int->time_t->SYSTEMTIME
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
	// 从受消息的缓冲区放到命令队列
	if ((m_iBufferLength + iCount) > BIG_BUFFER)
	{
		g_pSvrLog->AddRunLog(LL_ERROR, "[通信信息]：Msg Buffer Error, iLength = %d, m_iBufferLength = %d", iCount, m_iBufferLength);
        // 丢弃本次的数据，重新开始
		return;
	}
	// 接收处理新来的数据
	memcpy(m_RecvBuffer+m_iBufferLength, pData, iCount);
	
	m_iBufferLength += iCount;
	
	unsigned int iHeadPos = 0; // 帧头的位置
	unsigned int iTailPos = 0;
    unsigned int i = 0; //循环子
	
	// 下面分析消息队列里面的数据，找到第一个帧头，把前面的东西都丢弃
	for (; i <= m_iBufferLength; i++)
	{
		if (CMD_REAL_HEAD == m_RecvBuffer[i])
		{
			iHeadPos = i;
			// 保证判断部分，判断一下后面还够不够用来分析的长度
			// 假设不包含数据的话，一帧消息也要至少15个字节，所以看看够不够
			if ((iHeadPos+11) > m_iBufferLength)
			{
                // 大于表明是不完整的数据，保留下来
				memmove(m_RecvBuffer, m_RecvBuffer+i, m_iBufferLength-i);
				break;
			}
			else
			{
				//根据消息帧的定义，数据长度的位置为从帧头起6~9字节		
				int iMsgLength = (m_RecvBuffer[i+6] << 24) & 0xff000000;
				iMsgLength |= (m_RecvBuffer[i+7] << 16) & 0x00ff0000;
				iMsgLength |= (m_RecvBuffer[i+8] << 8) & 0x0000ff00;
				iMsgLength |= (m_RecvBuffer[i+9]) & 0x000000ff;
				
				if ((iMsgLength < 0) || (iMsgLength > BIG_BUFFER - 15))
				{
					// 数据长度小于0就是错误了,丢弃这段数据
					continue;
				}
				
				// 这样，帧尾的位置如下，如果没错就是完整的一帧
				iTailPos = i + 9 + iMsgLength + 1;
				
				// 这次判断到帧尾是不是在这次的缓冲区里面
				if (iTailPos > m_iBufferLength)
				{
					// 大于表明是不完整的数据，保留下来
					memmove(m_RecvBuffer, m_RecvBuffer+i, m_iBufferLength-i);
					break;
				}
				else
				{
					int iMsgLen = iTailPos - iHeadPos + 1;
					if (CMD_REAL_TAIL == m_RecvBuffer[iTailPos])
					{
						// 没错，拷贝出来去解析吧
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
 //   //获取本地程序的完整路径
	////CRF_JXT_UNIONDlg *pMain = (CRF_JXT_UNIONDlg*)AfxGetApp()->m_pMainWnd;
	//string strLocalPath;
	//int     nPos = 0;
	////GetModuleFileName(NULL,strLocalPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
	//strLocalPath.ReleaseBuffer();   
	//nPos=strLocalPath.ReverseFind('\\');   
	//strLocalPath=strLocalPath.Left(nPos);
	//string strexePath;
	//strexePath = strLocalPath + _T("\\JXTUpdate.exe");
 //    //先停止中间件
	////pMain->StopService();
	////启动本地的EXE程序
	//HINSTANCE his;
	////string strparament = "-u jxt -p 1013";

	//
	//his =::ShellExecute(NULL, _T("open"), strexePath, (LPCTSTR)strInfo, NULL, SW_SHOW );
	//if ((int)his == ERROR_FILE_NOT_FOUND)
	//{
	//	//没找到exe文件，检查路径或者文件名称
	//	return FALSE;
	//}
	//return TRUE;
//}

//BOOL CTcpRecvThread::UploadLogFile(string &strUser, string &strPass, string &strLogData)
//{
//	//返回0-上传日志文件失败(两个文件有一个上传失败就算失败)
//	//返回1-两个文件都上传成功
//
//	BOOL flag; 
//	BOOL flagall;
//	BOOL flaga;
//
//	//获取本地运行路径
//	char chFilePath[MAX_PATH];                
//	GetModuleFileName(NULL, chFilePath, MAX_PATH);
//	string strRunPath = chFilePath;
//	string sAllLogFile ="";
//	string sLogFile = "";
//
//	int iIndex = strRunPath.ReverseFind('\\');
//	strRunPath = strRunPath.Left(iIndex);
//
//	//配置文件
//	string strIniPath = strRunPath + "\\Config.ini";
//	//从配置文件读取服务器地址
//	char iniBuf[80];
//	memset(iniBuf,0,80);
//	GetPrivateProfileString("IP", "PEERADDR", NULL,iniBuf,80, (LPCTSTR)strIniPath);
//	
//    string strIp = "";
//	strIp.Format("%s",iniBuf);
//
//	//初始化FTP服务器
//
//	CFtpClient ftpClient(strIp,strUser,strPass,21);
//
//	//连接服务器
//	if (ftpClient.Connect())
//	{
//		g_pSvrLog->AddRunLog(LL_INFO,"[通信信息]：开始往服务器上传日志");
//		//获取本地日志文件路径allstudent和student文档
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
//		//sAllLogFile和sLogFile为要上传的日志的本地路径
//		sAllLogFile += tempFileName1;
//		sLogFile += tempFileName2;
//
//
//
//		//获得FTP服务器的默认路径
//		string strdir;
//		BOOL err =  ftpClient.GetCurrentDirectory(strdir);
//		//获得远程文件的完整路径，此处默认要上传的日志文件放在FTP路径的根目录下
//		//sAllLogFile和sLogFile为往FTP服务器上上传文件的路径
//		string strFile = "";
//	    string strUpAllStudent,strUpStudent,strLstUpAllStudent,strLstUpStudent;
//		strUpAllStudent.Format("\\%d_AllStudent_%s.log",g_ClientID,strLogData);
//		strUpStudent.Format("\\%d_Student_%s.log",g_ClientID,strLogData);
//        //fclose()
//		//上传sAllLogFile
//		//把日志文件复制到新的文件中
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
//			strFile.Format("上传%s到服务器失败",tempFileName1);
//			g_pWriteAllFileThread->WriteStringToFile(strFile);
//
//			g_pSvrLog->AddRunLog(LL_INFO,"[通信信息]：往服务器上传AllLogFile日志失败");
//
//			flagall = 0;
//
//		}
//		else
//		{
//			strFile.Format("上传%s到服务器成功",tempFileName1);
//			g_pWriteAllFileThread->WriteStringToFile(strFile);
//			g_pSvrLog->AddRunLog(LL_INFO,"[通信信息]：往服务器上传AllLogFile日志成功");
//
//			flagall = 1;
//
//
//		}
//		//上传sLogFile
//		if (FALSE == ftpClient.UploadFile(strLocalStudentNewFile,strUpStudent))
//		{
//			strFile.Format("上传%s到服务器失败",tempFileName2);
//			g_pWriteAllFileThread->WriteStringToFile(strFile);
//
//			g_pSvrLog->AddRunLog(LL_INFO,"[通信信息]：往服务器上传LogFile日志失败");
//
//			flaga = 0;
//
//
//		}
//		else
//		{
//			strFile.Format("上传%s到服务器成功",tempFileName2);
//			g_pWriteAllFileThread->WriteStringToFile(strFile);
//
//			g_pSvrLog->AddRunLog(LL_INFO,"[通信信息]：往服务器上传LogFile日志成功");
//			
//			flaga = 1;
//
//
//		}
//
//
//		//关闭ftp连接
//		ftpClient.DisConnect();
//		//删除文件
//		DeleteFile(strLocalAllstudentNewFile);
//		DeleteFile(strLocalStudentNewFile);
//
//	}
//    flag = flagall&&flaga;
//	return flag;
//
//}

//进行base64解码输入应该是4的倍数(根据mime标准)
//如果不是4倍数返回错误
//注意 如果是最后一个字符 那么长度不准备 可能会多1 
//返回buf长度
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