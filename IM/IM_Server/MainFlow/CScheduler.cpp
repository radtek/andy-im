//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CClient.cpp : �ͻ������ʵ���ļ�
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


//extern CGlobalRecvQueue *g_pShortMsgQueue; // ȫ�ִ����Ͷ��Ŷ���
extern CServerLog * g_pSvrLog;  // ��־�ļ�ȫ�ֱ���
extern CGlobalSendQueue *g_pPendingSendQueue; // ȫ�ִ����͸��ͻ��˵���Ϣ����
extern DBInterface *g_pDBInterface;//���ݿ�ӿ�ȫ��ָ��
extern CINIFile *g_pIniFile;    //�����ļ�ȫ�ֱ���
extern CClientMgr * g_pClientMgr;  // ȫ�ֿͻ��˹���ģ��ָ��


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
    // ��⴫��ָ������ĺϷ���
	if ((NULL != pbBuffer) && (0 != iLen))
	{
		// ����ͷ��β�����Ƿ�����������
		if ((pbBuffer[0] == CMD_REAL_HEAD) && (pbBuffer[iLen - 1] == CMD_REAL_TAIL))
		{
		    // ���ա�����ƽ̨����Э�顷�����ȡ���ֽ���д��stMsg�ṹ��
			CRecvBuff oRecvBuf(pbBuffer, iLen);
			BYTE byTemp = 0;

			// ȡ����ͷ
			oRecvBuf.GetByte(byTemp);

			// �ͻ���ID
			oRecvBuf.GetInt(stMsg.iClientID);

			// ȡ����ˮ��
			oRecvBuf.GetInt(stMsg.iCmdID);

			// ȡ��������
			oRecvBuf.GetByte(stMsg.byCmdCode);

			// ȡ�������峤��
			oRecvBuf.GetInt(stMsg.iCmdLen);

			// ȡ��������
			oRecvBuf.GetBytes(stMsg.pbyCmdContent, stMsg.iCmdLen);
		}
	}
}


void CScheduler::MessageDispatch(BYTE* pbBuffer, int iLen)
{
    Message stMsg;
	stMsg.pbyCmdContent = new BYTE[iLen];

	// ���ֽ���ȡ������ṹ
    DecodeToMsgStruct(pbBuffer, iLen, stMsg);
	
	// ��ͬ����������ò�ͬ�Ĵ�����
    switch((int)stMsg.byCmdCode)
	{
	case CMD_LOGIN: //��¼��Ϣ
		UserLogin(stMsg);
		break;

	case CMD_GET_USER: // ��ȡ�û���Ϣ
        GetUserinfo(stMsg);
		break;

	case CMD_GET_FRIEND: //��ȡ������Ϣ
		GetFriendinfo(stMsg);
		break;

	case CMD_SEND_MSG: // ת����Ϣ
		SendFriendMsg(stMsg);
		break;

    case CMD_GET_GROUP: // ��ȡȺ��Ϣ
       // UpdateStrategyNotice(stMsg);
		break;

	case CMD_UPDATE_STUDENT_INFO: // ����ѧ������
       // UpdateStudentInfoNotice(stMsg);
		break;

	case CMD_REQUEST_STUDENT_INFO: // ����ѧ����Ϣ������
		//RequestStudentInfo(stMsg);
		break;

	case CMD_REQUEST_STRATEGY: // �����ڲ��Ե�����
		//RequestStrategy(stMsg);
		break;

	case CMD_REQUEST_UPLOAD_LOG: // ����ͻ����ϴ���־�ļ�������
		//RequestUploadLog(stMsg);
		break;

	case CMD_REQUEST_UPLOAD_LOG_RESP: // �м���յ��ϴ������Ӧ��
		//StartToUploadLogResp(stMsg);
		break;

	case CMD_UPLOAD_LOG_RESULT: // �м���ϴ���־�ļ����
		//UploadLogComplete(stMsg);
		break;
	default:
		break;
	}

	delete [] stMsg.pbyCmdContent;
}

int CScheduler::IfFestivalDay(const SYSTEMTIME & systime)
{
	// �ȱȽ��ǲ���ʲô��������,1.1\5.1\10.1
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

	// �����������������գ������ǲ���ũ����
	WORD iLunarYear, iLunarMonth, iLunarDay;
	CCalendar::GetLunarDate(systime.wYear, systime.wMonth, systime.wDay, 
		                    iLunarYear, iLunarMonth, iLunarDay);
	if ((1 == iLunarDay) && (1 == iLunarMonth))
	{
		// ���³�һ�Ǵ���
		return SpringFestival;
	}
	else if ((5 == iLunarDay) && (5 == iLunarMonth))
	{
		// ���³����Ƕ����
		return DragonBoatFestival;
	}
	else if ((15 == iLunarDay) && (8 == iLunarMonth))
	{
		// ����ʮ���������
		return MidautumnFestival;
	}

	// ʲô���ն����Ƿ���0
	return 0;
}
//
void CScheduler::CheckNewVersion()
{
	// ��Щ������Ϣ�������������Ŷ��룬���һ�ʱ���ı�ģ�������Ϊ��ʱ����
	std::string strPath = "";
	std::string strUser = "";
	std::string strPassword = "";
	g_pIniFile->GetNewClientVersion(strUser, strPassword, strPath);
	
	if ((!strPath.empty()) && (!strUser.empty()) && (!strPassword.empty()))
	{
		std::string  strVersion = "";
		char  szSrcfn[MAX_PATH] = {0}; // Ҫ��ȡ��Ϣ���ļ�
		strcpy(szSrcfn, strPath.c_str());		
		
		// ���ļ���Ϣ
		BOOL bReadyToUp = FALSE;
		DWORD   dwVerHnd = 0;   
		DWORD   dwVerInfoSize = ::GetFileVersionInfoSize(szSrcfn,&dwVerHnd);   
		if(dwVerInfoSize)   
		{   
			// ����ȡ���ļ��Ͱ汾��
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
			
			// ��֮ǰ�İ汾���бȽϣ�������ھõİ汾�͸���
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
	// �����ֽڴ�Ϊ�ֽڴ�����ṹ
    int usrNo = stMsg.iClientID; //clientIDΪIM����
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
	oSendBuf.AddByte(CMD_REAL_HEAD);  // ֡ͷ
	oSendBuf.AddInt(stMsg.iCmdID);      // ��ˮ��
	oSendBuf.AddByte(CMD_LOGIN);  // ������
	oSendBuf.AddInt(4);               // ��Ϣ��
	oSendBuf.AddInt(loginflag);
	oSendBuf.AddByte(CMD_REAL_TAIL);  // ֡β
	g_pPendingSendQueue->AppendDataToBack(stMsg.iClientID, stMsg.iCmdID,stMsg.byCmdCode, oSendBuf.GetBuffer(), oSendBuf.GetLength());


}
void CScheduler::GetUserinfo(const Message & stMsg)
{
    int usrNo = stMsg.iClientID; //clientIDΪIM����

	USER_INFO user;
	user.id = usrNo;
	if (0==g_pDBInterface->GetUserInfo(user))
	{
		char sendbuff[200]={0};
		int len_name=strlen(user.nick_name.c_str());
		int len_sing = strlen(user.description.c_str());

		int totallen = 2+len_name+len_sing;

		CSendBuff oSendBuf;
		oSendBuf.AddByte(CMD_REAL_HEAD);  // ֡ͷ
		oSendBuf.AddInt(stMsg.iCmdID);      // ��ˮ��
		oSendBuf.AddByte(CMD_GET_USER);  // ������
		oSendBuf.AddInt(totallen);               // ��Ϣ��
		oSendBuf.AddByte((BYTE)len_name); //�û����Ƴ���
		oSendBuf.AddBytes((BYTE*)user.nick_name.c_str(),len_name);//�û���
		oSendBuf.AddByte((BYTE)len_sing);//ǩ���̶�
		oSendBuf.AddBytes((BYTE*)user.description.c_str(),len_sing);
		oSendBuf.AddByte(CMD_REAL_TAIL);  // ֡β
		g_pPendingSendQueue->AppendDataToBack(stMsg.iClientID, stMsg.iCmdID,stMsg.byCmdCode, oSendBuf.GetBuffer(), oSendBuf.GetLength());


	}

}

void CScheduler::GetFriendinfo(const Message & stMsg)
{

	int usrNo = stMsg.iClientID; //clientIDΪIM����
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
				//�������״̬
				g_pDBInterface->GetLoginState(friendid,loginfo);
				char sendbuff[200]={0};
				int len_name = strlen(verfriend[i].nick_name.c_str());
				int len_sing = strlen(verfriend[i].description.c_str());
				int len_total = 4+1+1+len_name+len_sing+4;
				//���ͷ�ʽimid =4 name =strlen sign strlen
				CSendBuff oSendBuf;
				oSendBuf.AddByte(CMD_REAL_HEAD);  // ֡ͷ
				oSendBuf.AddInt(stMsg.iCmdID);      // ��ˮ��
				oSendBuf.AddByte(CMD_GET_FRIEND);  // ������
				oSendBuf.AddInt(len_total);               // ��Ϣ��
				oSendBuf.AddInt(verfriend[i].id);//imno
				oSendBuf.AddInt(loginfo.type);
				oSendBuf.AddByte((BYTE)len_name); //�û����Ƴ���
				oSendBuf.AddBytes((BYTE*)verfriend[i].nick_name.c_str(),len_name);//�û���
				oSendBuf.AddByte((BYTE)len_sing);//ǩ��
				oSendBuf.AddBytes((BYTE*)verfriend[i].description.c_str(),len_sing);
				oSendBuf.AddByte(CMD_REAL_TAIL);  // ֡β
				g_pPendingSendQueue->AppendDataToBack(stMsg.iClientID, stMsg.iCmdID,stMsg.byCmdCode, oSendBuf.GetBuffer(), oSendBuf.GetLength());
				Sleep(20);
			}

		}

	}

}

void CScheduler::GetGroupinfo(const Message & stMsg)
{

	vector<GROUP_INFO> vecgroup;
	int usrNo = stMsg.iClientID; //clientIDΪIM����
	if (0==g_pDBInterface->GetFriendInfo(vecgroup,usrNo))
	{

	}

}
void CScheduler::SendFriendMsg(const Message & stMsg)
{

	int usrNo = stMsg.iClientID; //clientIDΪIM����
	CRecvBuff oContentBuf(stMsg.pbyCmdContent, stMsg.iCmdLen,TRUE);
	//ȡ��IM����
	int imno;
	oContentBuf.GetInt(imno);
	//ȡ����Ϣ
	BYTE msgbuff[250];
	memset(msgbuff, 0, 250);
	BYTE byLen = 0;
	oContentBuf.GetByte(byLen);
	oContentBuf.GetBytes(msgbuff, byLen);
    int bufflen = int(byLen);
	int totallen= bufflen+4+1;
	CSendBuff oSendBuf;
	oSendBuf.AddByte(CMD_REAL_HEAD);  // ֡ͷ
	oSendBuf.AddInt(stMsg.iCmdID);      // ��ˮ��
	oSendBuf.AddByte(CMD_SEND_MSG);  // ������
	oSendBuf.AddInt(totallen);               // ��Ϣ��
	oSendBuf.AddInt(usrNo);//����clientid
	oSendBuf.AddByte((BYTE)byLen); //�û����Ƴ���
	oSendBuf.AddBytes((BYTE*)msgbuff,byLen);//�û���
	oSendBuf.AddByte(CMD_REAL_TAIL);  // ֡β
	g_pPendingSendQueue->AppendDataToBack(imno, stMsg.iCmdID,stMsg.byCmdCode, oSendBuf.GetBuffer(), oSendBuf.GetLength());
}