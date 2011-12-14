//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CScheduler.h: 客户端类，封装客户端的发送、接收缓冲区，还有相关操作
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

		// 启动一个间隔两个小时的定时器
		//m_uiTimer = timeSetEvent(2*60*60*1000 , 0, SpecialDayTimer, (unsigned long)this, TIME_PERIODIC);

		// 初始化旧版本号
		memset(m_nPreVer, 0, sizeof(WORD)*4);

		// 初始化辅助MAP
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
	// 将YYYY-MM-DD HH:MM:SS.***格式的时间转换为time_t
	void TimeFormat(time_t &stTimeT, std::string & strTime);

	// 组装寒暑假区间的方法
	void AssembleVacation(BYTE * pbyByte, std::string & strTime1, std::string & strTime2);

	// 分析处理时间格式的函数从2008 八月 8 12:12:12.000
	void DealDateTimeFormat(std::string & strDateTime);

	// 判断是否空格类字符
	BOOL IsSpace(char c);

	// 去做空格
	void TrimLeft(std::string & str);

	// 去右空格
	void TrimRight(std::string & str);

	// 该进程拥有一个统一的发送管理
	SendingMap m_oSendingMap;
	MapAssistant m_oAssistant;

	// 一个自增值
	unsigned int m_iSequenceNO;

	// 启动一个两小时检查一次的定时器，检查在凌晨3点到5点间可以发送生日短信和节日短信
	UINT m_uiTimer;

	// 保留一个客户端的版本号信息，允许为空
	WORD m_nPreVer[4];


	// 管理待响应消息的临界区
	CRITICAL_SECTION m_csSending;
	CRITICAL_SECTION m_csTest;


	//消息
    //登录消息
	void UserLogin(const Message & stMsg);
    //获取用户信息
	void GetUserinfo(const Message & stMsg);
	//获取好友信息
	void GetFriendinfo(const Message & stMsg);
	//转发好友消息
	void SendFriendMsg(const Message & stMsg);
	//获取群信息
	void GetGroupinfo(const Message & stMsg);
};

#endif