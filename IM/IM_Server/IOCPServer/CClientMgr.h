//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// CClientMgr.h: �ͻ��˹���ά��һ���ͻ����������Է���Ĳ�����Ӻ�ɾ��
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

	// �����¿ͻ���
	void CreateNewClient(CClient* pClient, sockaddr_in *pstAddr);

	// ȷ�Ͽͻ������
    int RegClient(CClient* pClient);

	// ͨ��ID��ȡ�ͻ���ָ��
	CClient* GetClientByID(int iClientID);
	
	// ����Ӧ�Ŀͻ��˷�������
	void SentToClient(int iClientID, char* pSendData, int iLen);

	// ɾ���ͻ���
	int DistroyClient(int iClientID);
	int DistroyClient(CClient *pClient);

	// ����������ͻ���
	void InsertPendingAccept(CClient* pClient);

	// �õ�һ��������
	CClient* GetPendingAccept();

	// ɾ��������ͻ���
	void RemovePendingAccept(CClient* pClient, sockaddr_in *RemoteSockaddr);
	
	// ����ѯ������ͻ���״̬
	void PollPendingAccept(int &iReturnCode);
	
	// ���ٿͻ��������������пͻ���
	void DestroyAllClient();
	
	// ��ÿһ���ͻ��˷���ʱ��У���
	void SendTimeVerifyToAllClient();

	// ��ÿһ���м�����°汾
	void UpdateClientVersion(std::string & User, std::string Password);


	//���͸����ߺ��ѵ�¼��Ϣ
   //imid �Լ���im
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