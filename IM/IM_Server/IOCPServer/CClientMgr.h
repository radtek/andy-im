//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CClientMgr.h: 客户端管理，维护一个客户端链表，可以方便的查找添加和删除
//
// Author: Leo
// Date: 08/02/2008
//
////////////////////////////////////////////////////////////////////// 
#ifndef _CLIENT_MGR_H_
#define _CLIENT_MGR_H_
#include <list>
#include "CClient.h"

typedef std::list<CClient*> ClientPtrList;
class CClientMgr
{
public:
    CClientMgr();
    virtual ~CClientMgr();

	// 创建新客户端
	void CreateNewClient(CClient* pClient, sockaddr_in *pstAddr);

	// 确认客户端身份
    int RegClient(CClient* pClient);

	// 通过ID获取客户端指针
	CClient* GetClientByID(int iClientID);
	
	// 给对应的客户端发送数据
	void SentToClient(int iClientID, char* pSendData, int iLen);

	// 删除客户端
	int DistroyClient(int iClientID);
	int DistroyClient(CClient *pClient);

	// 创建待处理客户端
	void InsertPendingAccept(CClient* pClient);

	// 得到一个待处理
	CClient* GetPendingAccept();

	// 删除待处理客户端
	void RemovePendingAccept(CClient* pClient, sockaddr_in *RemoteSockaddr);
	
	// 简单轮询待处理客户端状态
	void PollPendingAccept(int &iReturnCode);
	
	// 销毁客户端链表里面所有客户端
	void DestroyAllClient();
	
	// 给每一个客户端发送时间校验包
	void SendTimeVerifyToAllClient();

	// 给每一个中间件更新版本
	void UpdateClientVersion(std::string & User, std::string Password);


	//发送给在线好友登录消息
   //imid 自己的im
	void SendFriendLoginMsg(int imid);

	int DeleteFromPendingAccept(CClient *pClient);
	int DeleteFromRawList(CClient *pClient);
	

	LONG m_lRepostCount;
	LONG m_lPendingAcceptCount;
	LONG m_lRegClientCount;
	LONG m_lRawClientCount;

protected:
private:
	ClientPtrList m_oRegClientPtrList;
    ClientPtrList m_oRawClientPtrList;
	ClientPtrList m_oPendingAcceptClientList;

    ULONG m_ulCurrentConnect;

	CRITICAL_SECTION m_csRegSection;
    CRITICAL_SECTION m_csPendingSection;
	CRITICAL_SECTION m_csRawSection;

};
#endif