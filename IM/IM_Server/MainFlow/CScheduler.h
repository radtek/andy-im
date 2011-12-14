//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CScheduler.h: �ͻ����࣬��װ�ͻ��˵ķ��͡����ջ�������������ز���
//
// Author: Leo
// Date: 08/07/2008
//
////////////////////////////////////////////////////////////////////// 
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include <map>
#include "../HeaderFiles/common.h"
#include <mmsystem.h>
 #include <varargs.h>

typedef struct tagMessage
{
    int iCmdID;
    int iCmdLen;
    int iClientID;
    BYTE byCmdCode;
    BYTE* pbyCmdContent;
}Message;

typedef std::map<unsigned int, PendingSendData*> SendingMap;

//void CALLBACK SpecialDayTimer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

class CScheduler
{
public:
    CScheduler()
	{
		m_iSequenceNO = 0;
		InitializeCriticalSection(&m_csSending);
		InitializeCriticalSection(&m_csTest);

		// ����һ���������Сʱ�Ķ�ʱ��
		//m_uiTimer = timeSetEvent(2*60*60*1000 , 0, SpecialDayTimer, (unsigned long)this, TIME_PERIODIC);

		// ��ʼ���ɰ汾��
		memset(m_nPreVer, 0, sizeof(WORD)*4);

		// ��ʼ������MAP
		for (int i=0; i < MAP_SIZE; i++)
		{
			m_oAssistant[i] = i;
		}
	};
	~CScheduler()
	{
		timeKillEvent(m_uiTimer);
		DeleteCriticalSection(&m_csSending);
		DeleteCriticalSection(&m_csTest);
	};

	void DecodeToMsgStruct(BYTE* pbBuffer, int iLen, Message & stMsg);
	
	void MessageDispatch(BYTE *pbBuffer, int iLen);

	void CheckNewVersion();

private:
    int IfFestivalDay(const SYSTEMTIME & systime);
	// ��YYYY-MM-DD HH:MM:SS.***��ʽ��ʱ��ת��Ϊtime_t
	void TimeFormat(time_t &stTimeT, std::string & strTime);

	// ��װ���������ķ���
	void AssembleVacation(BYTE * pbyByte, std::string & strTime1, std::string & strTime2);

	// ��������ʱ���ʽ�ĺ�����2008 ���� 8 12:12:12.000
	void DealDateTimeFormat(std::string & strDateTime);

	// �ж��Ƿ�ո����ַ�
	BOOL IsSpace(char c);

	// ȥ���ո�
	void TrimLeft(std::string & str);

	// ȥ�ҿո�
	void TrimRight(std::string & str);

	// �ý���ӵ��һ��ͳһ�ķ��͹���
	SendingMap m_oSendingMap;
	MapAssistant m_oAssistant;

	// һ������ֵ
	unsigned int m_iSequenceNO;

	// ����һ����Сʱ���һ�εĶ�ʱ����������賿3�㵽5�����Է������ն��źͽ��ն���
	UINT m_uiTimer;

	// ����һ���ͻ��˵İ汾����Ϣ������Ϊ��
	WORD m_nPreVer[4];


	// �������Ӧ��Ϣ���ٽ���
	CRITICAL_SECTION m_csSending;
	CRITICAL_SECTION m_csTest;


	//��Ϣ
    //��¼��Ϣ
	void UserLogin(const Message & stMsg);
    //��ȡ�û���Ϣ
	void GetUserinfo(const Message & stMsg);
	//��ȡ������Ϣ
	void GetFriendinfo(const Message & stMsg);
	//ת��������Ϣ
	void SendFriendMsg(const Message & stMsg);
	//��ȡȺ��Ϣ
	void GetGroupinfo(const Message & stMsg);
};

#endif