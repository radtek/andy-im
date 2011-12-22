// TcpCommunication.cpp: implementation of the CTcpCommunication class.
//
//////////////////////////////////////////////////////////////////////
#include "TcpCommunication.h"
#include "ServerLog.h"
#include "TcpRecvThread.h"
#include <mmsystem.h>
#include <time.h>
#include "../MainUI/main_frame.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern  CServerLog*		g_pSvrLog;
extern int g_ClientID;
CTcpCommunication::CTcpCommunication(MainFrame * frame_wnd)
:frame_wnd_(frame_wnd)
{
	m_pTcpSendThread = NULL;

	m_hSocket = NULL;
	m_bConnected = FALSE;

	m_pTcpRecvThread = NULL;
	m_psDatalist = new CSynList();
	m_theradexit=FALSE;
    m_hSend = INVALID_HANDLE_VALUE;
	InitializeCriticalSection(&m_csSocket);
}

CTcpCommunication::~CTcpCommunication()
{
   	if ( m_psDatalist )
	{
		while ( m_psDatalist->GetCount() > 0 ) 
		{
			S_SEND_DATA		*oSendData = NULL;
			oSendData = (S_SEND_DATA *)m_psDatalist->GetHead();
			
			if (NULL != oSendData)
			{
				delete oSendData;
				oSendData = NULL;
			}
		}
		delete m_psDatalist;
		m_psDatalist = NULL;
	}
	timeKillEvent(mr);
	DeleteCriticalSection(&m_csSocket);
}

// TCP��ʼ��
BOOL CTcpCommunication::Initialize(string strPeerIp, unsigned short usPort, BYTE bType, int nMaxRecordNum)
{
	m_ServerIp = strPeerIp;
	m_dwServerPort = usPort;
	
	// ��ʼ��Socket
	if ( FALSE == InitSocket())
	{
		return FALSE;
	}
	m_bType = bType;
	
	if ( bType == CMD_TYPE_REAL ) //1 ʵʱ�����߳�
	{
		if (!m_pTcpSendThread)
		{
			m_pTcpSendThread = new CTcpSendThread(this, bType, nMaxRecordNum);			
			m_pTcpSendThread->Start();
		}
		Sleep(5);

		if (!m_pTcpRecvThread)
		{
			m_pTcpRecvThread = new CTcpRecvThread(this);
			m_pTcpRecvThread->Start();
		}

	}

	//���ö�ʱ����ÿ30�뷢��һ�ζ���
	// ����Ļ�����ӡһЩ��Ϣ
	g_pSvrLog->AddTextLog(LL_DEBUG, "[������־]������ͨ���߳�������ɡ�\r\n");
	return TRUE;
}

BYTE CTcpCommunication::GetType()
{
	return m_bType;
}

BOOL CTcpCommunication::StopService()
{
	// ����ʱ��
	timeKillEvent(mr);
	m_theradexit = TRUE;
	if (m_hSend != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_hSend, INFINITE);
		CloseHandle(m_hSend);
		m_hSend = INVALID_HANDLE_VALUE;
	}
	// ֹͣ�����߳�
	if (m_pTcpSendThread )
	{
		m_pTcpSendThread->Stop();
		delete m_pTcpSendThread;
		m_pTcpSendThread = NULL;
	}
	// ֹͣ�����߳�
	if(m_pTcpRecvThread)
	{
         m_pTcpRecvThread->Stop();
		 delete m_pTcpRecvThread;
		 m_pTcpRecvThread = NULL;
	}
	// �Ȱ�Socket�ر�
	CloseSocket();
	return TRUE;
}
void CTcpCommunication::SendGetGroupUserMsg(unsigned int groupid)
{
	SYSTEMTIME ct;
	GetLocalTime( &ct);
	struct tm gm = { ct.wSecond, ct.wMinute, ct.wHour, ct.wDay, ct.wMonth-1, ct.wYear-1900, ct.wDayOfWeek, 0, 0 };
	time_t packettime = mktime(&gm);
	int iPacketTime = (int)packettime;
	CSendBuff *oSndBuff = NULL;
	oSndBuff = new CSendBuff;
	//���һ��ʱ����������ж��Ƿ��ʱЧ
	oSndBuff->AddInt(iPacketTime);
	oSndBuff->AddByte(CMD_REAL_HEAD);
	oSndBuff->AddInt(g_ClientID);
	oSndBuff->AddInt(0);//��ˮ��
	oSndBuff->AddByte(CMD_GET_GROUP_USER);//
	oSndBuff->AddInt(4); //��Ϣ����
	oSndBuff->AddInt(groupid);
	oSndBuff->AddByte(CMD_REAL_TAIL);

	IMMsg *pmsg = new IMMsg;
	pmsg->iLen = oSndBuff->GetLength();
	pmsg->szSendBuf = (TCHAR*)oSndBuff->GetBuffer();
	m_pTcpSendThread->m_pSynsendList->AddTail(pmsg);
}
//type ����
//1   ��¼��Ϣ�������û���������
//2   ��ȡ�û���Ϣ
//3    ��ȡ������Ϣ
void CTcpCommunication::SendBuffer(int type ,TCHAR *buff,int len)
{
	
	SYSTEMTIME ct;
	GetLocalTime( &ct);
	struct tm gm = { ct.wSecond, ct.wMinute, ct.wHour, ct.wDay, ct.wMonth-1, ct.wYear-1900, ct.wDayOfWeek, 0, 0 };
	time_t packettime = mktime(&gm);
	int iPacketTime = (int)packettime;
	CSendBuff *oSndBuff = NULL;
	oSndBuff = new CSendBuff;
	//���һ��ʱ����������ж��Ƿ��ʱЧ
	oSndBuff->AddInt(iPacketTime);
	oSndBuff->AddByte(CMD_REAL_HEAD);
	oSndBuff->AddInt(g_ClientID);
	oSndBuff->AddInt(0);//��ˮ��
	oSndBuff->AddByte(type);//
	oSndBuff->AddInt(len); //��Ϣ�̶�
	oSndBuff->AddBytes((BYTE*)buff,len);
	oSndBuff->AddByte(CMD_REAL_TAIL);

	IMMsg *pmsg = new IMMsg;
	pmsg->iLen = oSndBuff->GetLength();
	pmsg->szSendBuf = (TCHAR*)oSndBuff->GetBuffer();
	m_pTcpSendThread->m_pSynsendList->AddTail(pmsg);

}
//imid �Է� im����
//buff ��Ϣ����
void CTcpCommunication::SendMsg(byte type,unsigned int imid,TCHAR *buff,int bufflen)
{
	SYSTEMTIME ct;
	GetLocalTime( &ct);
	struct tm gm = { ct.wSecond, ct.wMinute, ct.wHour, ct.wDay, ct.wMonth-1, ct.wYear-1900, ct.wDayOfWeek, 0, 0 };
	time_t packettime = mktime(&gm);
	int iPacketTime = (int)packettime;
	CSendBuff *oSndBuff = NULL;
	oSndBuff = new CSendBuff;

   // int bufflen = strlen(buff);

	int totallen = bufflen +4+1+1;

	//���һ��ʱ����������ж��Ƿ��ʱЧ
	oSndBuff->AddInt(iPacketTime);
	oSndBuff->AddByte(CMD_REAL_HEAD);
	oSndBuff->AddInt(g_ClientID);//�Լ���IM����
	oSndBuff->AddInt(0);//��ˮ��
	oSndBuff->AddByte(CMD_SEND_MSG);//
	oSndBuff->AddInt(totallen); //��Ϣ����
	oSndBuff->AddByte(type);
	oSndBuff->AddInt(imid);//�Է�imid
	oSndBuff->AddByte((BYTE)bufflen);
	oSndBuff->AddBytes((BYTE*)buff,bufflen);
	oSndBuff->AddByte(CMD_REAL_TAIL);
	IMMsg *pmsg = new IMMsg;
	pmsg->iLen = oSndBuff->GetLength();
	pmsg->szSendBuf = (TCHAR*)oSndBuff->GetBuffer();
	m_pTcpSendThread->m_pSynsendList->AddTail(pmsg);
}
void CTcpCommunication::SendGroupMsg(unsigned int groupid ,unsigned int imid,TCHAR *buff,int bufflen)
{
	SYSTEMTIME ct;
	GetLocalTime( &ct);
	struct tm gm = { ct.wSecond, ct.wMinute, ct.wHour, ct.wDay, ct.wMonth-1, ct.wYear-1900, ct.wDayOfWeek, 0, 0 };
	time_t packettime = mktime(&gm);
	int iPacketTime = (int)packettime;
	CSendBuff *oSndBuff = NULL;
	oSndBuff = new CSendBuff;

	// int bufflen = strlen(buff);

	int totallen = bufflen +4+1+4;

	//���һ��ʱ����������ж��Ƿ��ʱЧ
	oSndBuff->AddInt(iPacketTime);
	oSndBuff->AddByte(CMD_REAL_HEAD);
	oSndBuff->AddInt(g_ClientID);//�Լ���IM����
	oSndBuff->AddInt(0);//��ˮ��
	oSndBuff->AddByte(CMD_SEND_GROUP_MSG);//
	oSndBuff->AddInt(totallen); //��Ϣ����
	oSndBuff->AddInt(groupid);
	oSndBuff->AddInt(imid);//�Է�imid
	oSndBuff->AddByte((BYTE)bufflen);
	oSndBuff->AddBytes((BYTE*)buff,bufflen);
	oSndBuff->AddByte(CMD_REAL_TAIL);
	IMMsg *pmsg = new IMMsg;
	pmsg->iLen = oSndBuff->GetLength();
	pmsg->szSendBuf = (TCHAR*)oSndBuff->GetBuffer();
	m_pTcpSendThread->m_pSynsendList->AddTail(pmsg);
}
BOOL CTcpCommunication::InitSocket()
{
	// ������Socket
	CloseSocket();

	// ׼������
	struct sockaddr_in serverAddress;
	int err=0;
	EnterCriticalSection(&m_csSocket);
	// ����Socket
	m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	

	err = GetLastError();
	if( m_hSocket == INVALID_SOCKET)
	{
		g_pSvrLog->AddTextLog(LL_DEBUG, "[ͨ����Ϣ]����ʼ��SOCKETʧ�ܣ�INVALID_SOCKET.\r\n");
		LeaveCriticalSection(&m_csSocket);
		return FALSE;
	}
	
	// ������
	int val = 1; 	
	int r = setsockopt(m_hSocket,SOL_SOCKET,SO_KEEPALIVE,(char*)&val,sizeof(val));
	
	// ȡ�������Ż��㷨
	r = setsockopt(m_hSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&val, sizeof(val));
	
	memset(&serverAddress, 0, sizeof(serverAddress));     
	serverAddress.sin_family      = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(m_ServerIp.c_str());
	serverAddress.sin_port        = htons(m_dwServerPort);
	// ׼��һ��Select����Ƿ��������
	/*
	fd_set fdSocket;
	FD_ZERO(&fdSocket);
	FD_SET(m_hSocket,&fdSocket);
	timeval timeout;
	timeout.tv_sec=3; //��ʱʱ����Ϊ3��
	fd_set fdwrite=fdSocket;
	err = select(1, NULL, &fdwrite,	NULL, &timeout);
	*/
	if( SOCKET_ERROR == connect(m_hSocket,(sockaddr*)&serverAddress,sizeof(serverAddress)) )
	{
		int err = GetLastError();
		g_pSvrLog->AddTextLog(LL_DEBUG, "[ͨ����Ϣ]����ʼ��SOCKETʧ��: CONNECT ERROR.\r\n");
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;
		LeaveCriticalSection(&m_csSocket);
		return FALSE;	
	}
	
	//CString	str;
	//str.Format("[ͨ����Ϣ]����ʼ��SOCKET�ɹ�. �˿�:%d\r\n", g_port);/////////
	//g_pSvrLog->AddTextLog(LL_DEBUG, (LPCSTR)str);
	LeaveCriticalSection(&m_csSocket);

	// ���ӳɹ�������һ��ע����Ϣ
	Sleep(2000);
	SendClietReg();
	return TRUE;
}

void CTcpCommunication::CloseSocket()
{
	EnterCriticalSection(&m_csSocket);
	if (INVALID_SOCKET != m_hSocket)
	{
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
	LeaveCriticalSection(&m_csSocket);
}

int CTcpCommunication::Send(TCHAR* szMsg, int nLen)
{
	int err;
	TIMEVAL	timeout;
	FD_SET  fdset;
	FD_ZERO( &fdset);
	FD_SET( m_hSocket, &fdset);
	//	��ѯ���1��
	timeout.tv_sec = 1;
	timeout.tv_usec= 0;
	FD_ZERO( &fdset);
	FD_SET(m_hSocket, &fdset);
	err = select(1, NULL, &fdset, NULL, &timeout);
	//	����
	if( err < 1)
	{
		g_pSvrLog->AddTextLog(LL_DEBUG, "[ͨ����Ϣ]�����ݷ���ʧ��: send timeout.\r\n");
		return err;
	}
	// ����Socketʱ����ٽ������б���
	EnterCriticalSection(&m_csSocket);
	err = send(m_hSocket, szMsg, nLen, 0);
	LeaveCriticalSection(&m_csSocket);
	return err;
	
}

int CTcpCommunication::Recv(TCHAR* szMsg, int nLen)
{
	//select��ʱ���
	int	nRecvCount = -1;
	
	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	fd_set rfd;
	
	FD_ZERO(&rfd);
	FD_SET(m_hSocket,&rfd);
	nRecvCount= select(1, &rfd, NULL, NULL, &timeout);
	if (0 == nRecvCount)
	{
		return nRecvCount;
	}
	else if (SOCKET_ERROR == nRecvCount)
	{
    	int gle = GetLastError();
		g_pSvrLog->AddRunLog(LL_DEBUG, "[ͨ����Ϣ]�����ݽ���ʧ��: recv fail: %d.", gle);
		closesocket(m_hSocket);
		return nRecvCount;
	}
	else if (FD_ISSET(m_hSocket,&rfd))
	{
		EnterCriticalSection(&m_csSocket);
		nRecvCount = recv(m_hSocket, szMsg, nLen, 0);  // data is received in buff[MAX_LEN]
		LeaveCriticalSection(&m_csSocket);
		
		// ��������һ�£����ﷵ��0����رգ�ͳһ����-1�����أ������select ����0����û�����ݿɶ��ĳ�ͻ
		if (nRecvCount == 0)
		{
			nRecvCount = SOCKET_ERROR;
		}
	}
	return nRecvCount;
}

int CTcpCommunication::SendClietReg()
{
	CSendBuff oShakeBuff;
    oShakeBuff.AddByte(CMD_REAL_HEAD);   
	oShakeBuff.AddInt(g_ClientID);  //ѧУ��ClientID
	oShakeBuff.AddByte(CMD_REAL_TAIL);    //construct a shake hand data struct
	int nRet = Send((TCHAR *)oShakeBuff.GetBuffer(),  oShakeBuff.GetLength());
	if (nRet == SOCKET_ERROR)
	{
        return nRet;
	}
	
	g_pSvrLog->AddRunLog(LL_INFO, "[ͨ����Ϣ]��������������ѷ���һ��ע����Ϣ." );
	return nRet;			
}
