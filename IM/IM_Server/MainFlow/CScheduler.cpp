//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CClient.cpp : 客户端类的实现文件
//
// Author: Leo
// Date: 08/04/2008
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
#include "../HeaderFiles/stdafx.h"
#include "../IOCPServer/CClientMgr.h"
#include "CScheduler.h"
#include "Calendar.h"
#include <stdio.h>
#include <time.h>
#include <iostream>
#include "../HeaderFiles/comm_buff.h"
#include "../LogFile/ServerLog.h"
#include "../DBInterface/DBInterface.h"
#include "../MainFlow/CommonQueue.h"
#include "../INIFile/INIFile.h"


//extern CGlobalRecvQueue *g_pShortMsgQueue; // 全局待发送短信队列
extern CServerLog * g_pSvrLog;  // 日志文件全局变量
extern CGlobalSendQueue *g_pPendingSendQueue; // 全局待发送给客户端的消息队列
extern DBInterface *g_pDBInterface;//数据库接口全局指针
extern CINIFile *g_pIniFile;    //配置文件全局变量
extern CClientMgr * g_pClientMgr;  // 全局客户端管理模块指针


typedef std::vector<int> RFIDVec;
typedef std::vector<RFIDVec *> CLASSPtrVec;

typedef struct tagClassStrategy
{
	int iClassID;
	BOOL bBoardingFlag;
	void * pStrategyContent;
	tagClassStrategy()
	{ 
		iClassID = -1;
		bBoardingFlag = FALSE;
		pStrategyContent = NULL;
	}
}ClassStrategy;
typedef std::vector<ClassStrategy*> vecStrategy;

void CScheduler::DecodeToMsgStruct(BYTE* pbBuffer, int iLen, Message & stMsg)
{
    // 检测传入指针参数的合法性
	if ((NULL != pbBuffer) && (0 != iLen))
	{
		// 检测包头包尾，看是否完整的数据
		if ((pbBuffer[0] == CMD_REAL_HEAD) && (pbBuffer[iLen - 1] == CMD_REAL_TAIL))
		{
		    // 按照《短信平台数据协议》，逐个取出字节填写到stMsg结构中
			CRecvBuff oRecvBuf(pbBuffer, iLen);
			BYTE byTemp = 0;

			// 取出包头
			oRecvBuf.GetByte(byTemp);

			// 客户端ID
			oRecvBuf.GetInt(stMsg.iClientID);

			// 取出流水号
			oRecvBuf.GetInt(stMsg.iCmdID);

			// 取出命令字
			oRecvBuf.GetByte(stMsg.byCmdCode);

			// 取出命令体长度
			oRecvBuf.GetInt(stMsg.iCmdLen);

			// 取出命令体
			oRecvBuf.GetBytes(stMsg.pbyCmdContent, stMsg.iCmdLen);
		}
	}
}


void CScheduler::MessageDispatch(BYTE* pbBuffer, int iLen)
{
    Message stMsg;
	stMsg.pbyCmdContent = new BYTE[iLen];

	// 从字节流取出命令结构
    DecodeToMsgStruct(pbBuffer, iLen, stMsg);
	
	// 不同的命令码才用不同的处理函数
    switch((int)stMsg.byCmdCode)
	{
	case CMD_LOGIN: //登录消息
		UserLogin(stMsg);
		break;

	case CMD_GET_USER: // 获取用户信息
        GetUserinfo(stMsg);
		break;

	case CMD_GET_FRIEND: //获取好友信息
		GetFriendinfo(stMsg);
		break;

	case CMD_SEND_MSG: // 转发消息
		SendFriendMsg(stMsg);
		break;

    case CMD_GET_GROUP: // 获取群信息
       // UpdateStrategyNotice(stMsg);
		break;

	case CMD_UPDATE_STUDENT_INFO: // 更新学生数据
       // UpdateStudentInfoNotice(stMsg);
		break;

	case CMD_REQUEST_STUDENT_INFO: // 请求学生信息的命令
		//RequestStudentInfo(stMsg);
		break;

	case CMD_REQUEST_STRATEGY: // 请求考勤策略的命令
		//RequestStrategy(stMsg);
		break;

	case CMD_REQUEST_UPLOAD_LOG: // 请求客户端上传日志文件的命令
		//RequestUploadLog(stMsg);
		break;

	case CMD_REQUEST_UPLOAD_LOG_RESP: // 中间件收到上传命令的应答
		//StartToUploadLogResp(stMsg);
		break;

	case CMD_UPLOAD_LOG_RESULT: // 中间件上传日志文件完成
		//UploadLogComplete(stMsg);
		break;
	default:
		break;
	}

	delete [] stMsg.pbyCmdContent;
}

int CScheduler::IfFestivalDay(const SYSTEMTIME & systime)
{
	// 先比较是不是什么公历节日,1.1\5.1\10.1
	if (1 == systime.wDay)
	{
		switch (systime.wMonth)
		{
		case 1: // 1.1
			return NewYearsDay;
		case 5: // 5.1
			return LaborDay;
		case 10: //10.1
			return NationalDay;
		default:
			break;
		}
	}

	// 不是这三个公历节日，看看是不是农历的
	WORD iLunarYear, iLunarMonth, iLunarDay;
	CCalendar::GetLunarDate(systime.wYear, systime.wMonth, systime.wDay, 
		                    iLunarYear, iLunarMonth, iLunarDay);
	if ((1 == iLunarDay) && (1 == iLunarMonth))
	{
		// 正月初一是春节
		return SpringFestival;
	}
	else if ((5 == iLunarDay) && (5 == iLunarMonth))
	{
		// 五月初五是端午节
		return DragonBoatFestival;
	}
	else if ((15 == iLunarDay) && (8 == iLunarMonth))
	{
		// 八月十五是中秋节
		return MidautumnFestival;
	}

	// 什么节日都不是返回0
	return 0;
}
//
void CScheduler::CheckNewVersion()
{
	// 这些配置信息是在启动任务后才读入，并且会时常改变的，所以作为临时变量
	std::string strPath = "";
	std::string strUser = "";
	std::string strPassword = "";
	g_pIniFile->GetNewClientVersion(strUser, strPassword, strPath);
	
	if ((!strPath.empty()) && (!strUser.empty()) && (!strPassword.empty()))
	{
		std::string  strVersion = "";
		char  szSrcfn[MAX_PATH] = {0}; // 要获取信息的文件
		strcpy(szSrcfn, strPath.c_str());		
		
		// 读文件信息
		BOOL bReadyToUp = FALSE;
		DWORD   dwVerHnd = 0;   
		DWORD   dwVerInfoSize = ::GetFileVersionInfoSize(szSrcfn,&dwVerHnd);   
		if(dwVerInfoSize)   
		{   
			// 可以取到文件和版本号
			HANDLE     hMem = INVALID_HANDLE_VALUE;
			LPVOID     lpvMem = NULL;
			unsigned   int uInfoSize = 0;
			VS_FIXEDFILEINFO * pFileInfo;
			
			hMem = ::GlobalAlloc(GMEM_MOVEABLE,   dwVerInfoSize);   
			lpvMem = ::GlobalLock(hMem);   
			::GetFileVersionInfo(szSrcfn,   dwVerHnd,   dwVerInfoSize,   lpvMem);   
			::VerQueryValue(lpvMem,   (LPTSTR)("\\"),   (void**)&pFileInfo,   &uInfoSize);   
			
			WORD   nVer[4];   
			nVer[0]   =   HIWORD(pFileInfo->dwProductVersionMS);     
			nVer[1]   =   LOWORD(pFileInfo->dwProductVersionMS);   
			nVer[2]   =   HIWORD(pFileInfo->dwProductVersionLS);   
			nVer[3]   =   LOWORD(pFileInfo->dwProductVersionLS);     
			
			::GlobalUnlock(hMem);   
			::GlobalFree(hMem);
			
			// 和之前的版本进行比较，如果大于久的版本就更新
			if ((nVer[3] > m_nPreVer[3])
				||(nVer[2] > m_nPreVer[2])
				||(nVer[1] > m_nPreVer[1])
				||(nVer[0] > m_nPreVer[0]))
			{
				bReadyToUp = TRUE;
				g_pSvrLog->AddRunLog(LL_INFO, "New version %d.%d.%02d.%02d is ready to update!",
					nVer[0], nVer[1], nVer[2], nVer[3]);
			}
		}
		
		if (bReadyToUp)
		{
			g_pClientMgr->UpdateClientVersion(strUser, strPassword);
		}	

	}
}
BOOL CScheduler::IsSpace(char c)
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
//
void CScheduler::TrimLeft(std::string & str)
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
//
void CScheduler::TrimRight(std::string & str)   
{   
	std::string::reverse_iterator i, strBegin;   
	unsigned int nSize = str.length();   
    
	for(i = str.rbegin(), strBegin = str.rend(); i != strBegin; i++)     
	{
		if (IsSpace(char(*i)))     
		{   
			nSize--;   
		}   
		else   
		{   
			break;   
		}   
	}    
	str.resize(nSize);   
}
void CScheduler::UserLogin(const Message & stMsg)
{
	// 构建字节串为字节串处理结构
    int usrNo = stMsg.iClientID; //clientID为IM号码
	CRecvBuff oContentBuf(stMsg.pbyCmdContent, stMsg.iCmdLen);
	char recvBuff[200]={0};

	memcpy(recvBuff,(char*)stMsg.pbyCmdContent,stMsg.iCmdLen);
	//sprintf_s(recvBuff,stMsg.iCmdLen+1,"%s",(char*)stMsg.pbyCmdContent);
	char user[100], pwd[100];
	sscanf(recvBuff, "USER=%s PSW=%s", user, pwd);

	int loginflag= 0;

    if (0==g_pDBInterface->UserLogin(user,pwd))
    {
       loginflag = 1;


    }
	CSendBuff oSendBuf;
	oSendBuf.AddByte(CMD_REAL_HEAD);  // 帧头
	oSendBuf.AddInt(stMsg.iCmdID);      // 流水号
	oSendBuf.AddByte(CMD_LOGIN);  // 命令字
	oSendBuf.AddInt(4);               // 消息长
	oSendBuf.AddInt(loginflag);
	oSendBuf.AddByte(CMD_REAL_TAIL);  // 帧尾
	g_pPendingSendQueue->AppendDataToBack(stMsg.iClientID, stMsg.iCmdID,stMsg.byCmdCode, oSendBuf.GetBuffer(), oSendBuf.GetLength());


}
void CScheduler::GetUserinfo(const Message & stMsg)
{
    int usrNo = stMsg.iClientID; //clientID为IM号码

	USER_INFO user;
	user.id = usrNo;
	if (0==g_pDBInterface->GetUserInfo(user))
	{
		char sendbuff[200]={0};
		int len_name=strlen(user.nick_name.c_str());
		int len_sing = strlen(user.description.c_str());

		int totallen = 2+len_name+len_sing;

		CSendBuff oSendBuf;
		oSendBuf.AddByte(CMD_REAL_HEAD);  // 帧头
		oSendBuf.AddInt(stMsg.iCmdID);      // 流水号
		oSendBuf.AddByte(CMD_GET_USER);  // 命令字
		oSendBuf.AddInt(totallen);               // 消息长
		oSendBuf.AddByte((BYTE)len_name); //用户名称长度
		oSendBuf.AddBytes((BYTE*)user.nick_name.c_str(),len_name);//用户名
		oSendBuf.AddByte((BYTE)len_sing);//签名程度
		oSendBuf.AddBytes((BYTE*)user.description.c_str(),len_sing);
		oSendBuf.AddByte(CMD_REAL_TAIL);  // 帧尾
		g_pPendingSendQueue->AppendDataToBack(stMsg.iClientID, stMsg.iCmdID,stMsg.byCmdCode, oSendBuf.GetBuffer(), oSendBuf.GetLength());


	}

}

void CScheduler::GetFriendinfo(const Message & stMsg)
{

	int usrNo = stMsg.iClientID; //clientID为IM号码
	vector<USER_INFO> verfriend;
	if (0==g_pDBInterface->GetFriendInfo(verfriend,usrNo))
	{
		int size = verfriend.size();
		if (size >0)
		{ 
			for(int i = 0; i < size; i++)
			{

				int friendid = verfriend[i].id;
				LOGININFO loginfo;
				//获得在线状态
				g_pDBInterface->GetLoginState(friendid,loginfo);
				char sendbuff[200]={0};
				int len_name = strlen(verfriend[i].nick_name.c_str());
				int len_sing = strlen(verfriend[i].description.c_str());
				int len_total = 4+1+1+len_name+len_sing+4;
				//发送方式imid =4 name =strlen sign strlen
				CSendBuff oSendBuf;
				oSendBuf.AddByte(CMD_REAL_HEAD);  // 帧头
				oSendBuf.AddInt(stMsg.iCmdID);      // 流水号
				oSendBuf.AddByte(CMD_GET_FRIEND);  // 命令字
				oSendBuf.AddInt(len_total);               // 消息长
				oSendBuf.AddInt(verfriend[i].id);//imno
				oSendBuf.AddInt(loginfo.type);
				oSendBuf.AddByte((BYTE)len_name); //用户名称长度
				oSendBuf.AddBytes((BYTE*)verfriend[i].nick_name.c_str(),len_name);//用户名
				oSendBuf.AddByte((BYTE)len_sing);//签名
				oSendBuf.AddBytes((BYTE*)verfriend[i].description.c_str(),len_sing);
				oSendBuf.AddByte(CMD_REAL_TAIL);  // 帧尾
				g_pPendingSendQueue->AppendDataToBack(stMsg.iClientID, stMsg.iCmdID,stMsg.byCmdCode, oSendBuf.GetBuffer(), oSendBuf.GetLength());
				Sleep(20);
			}

		}

	}

}

void CScheduler::GetGroupinfo(const Message & stMsg)
{

	vector<GROUP_INFO> vecgroup;
	int usrNo = stMsg.iClientID; //clientID为IM号码
	if (0==g_pDBInterface->GetFriendInfo(vecgroup,usrNo))
	{

	}

}
void CScheduler::SendFriendMsg(const Message & stMsg)
{

	int usrNo = stMsg.iClientID; //clientID为IM号码
	CRecvBuff oContentBuf(stMsg.pbyCmdContent, stMsg.iCmdLen,TRUE);
	//取出IM号码
	int imno;
	oContentBuf.GetInt(imno);
	//取出消息
	BYTE msgbuff[250];
	memset(msgbuff, 0, 250);
	BYTE byLen = 0;
	oContentBuf.GetByte(byLen);
	oContentBuf.GetBytes(msgbuff, byLen);
    int bufflen = int(byLen);
	int totallen= bufflen+4+1;
	CSendBuff oSendBuf;
	oSendBuf.AddByte(CMD_REAL_HEAD);  // 帧头
	oSendBuf.AddInt(stMsg.iCmdID);      // 流水号
	oSendBuf.AddByte(CMD_SEND_MSG);  // 命令字
	oSendBuf.AddInt(totallen);               // 消息长
	oSendBuf.AddInt(usrNo);//带上clientid
	oSendBuf.AddByte((BYTE)byLen); //用户名称长度
	oSendBuf.AddBytes((BYTE*)msgbuff,byLen);//用户名
	oSendBuf.AddByte(CMD_REAL_TAIL);  // 帧尾
	g_pPendingSendQueue->AppendDataToBack(imno, stMsg.iCmdID,stMsg.byCmdCode, oSendBuf.GetBuffer(), oSendBuf.GetLength());
}