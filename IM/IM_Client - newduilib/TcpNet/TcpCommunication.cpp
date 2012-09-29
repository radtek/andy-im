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

// TCP初始化
BOOL CTcpCommunication::Initialize(string strPeerIp, unsigned short usPort, BYTE bType, int nMaxRecordNum)
{
	m_ServerIp = strPeerIp;
	m_dwServerPort = usPort;
	
	// 初始化Socket
	if ( FALSE == InitSocket())
	{
		return FALSE;
	}
	m_bType = bType;
	
	if ( bType == CMD_TYPE_REAL ) //1 实时发送线程
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

	//设置定时器，每30秒发送一次短信
	// 在屏幕上面打印一些信息
	g_pSvrLog->AddTextLog(LL_DEBUG, "[启动日志]：数据通信线程启动完成。\r\n");
	return TRUE;
}

BYTE CTcpCommunication::GetType()
{
	return m_bType;
}

BOOL CTcpCommunication::StopService()
{
	// 清理定时器
	timeKillEvent(mr);
	m_theradexit = TRUE;
	if (m_hSend != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_hSend, INFINITE);
		CloseHandle(m_hSend);
		m_hSend = INVALID_HANDLE_VALUE;
	}
	// 停止发送线程
	if (m_pTcpSendThread )
	{
		m_pTcpSendThread->Stop();
		delete m_pTcpSendThread;
		m_pTcpSendThread = NULL;
	}
	// 停止接收线程
	if(m_pTcpRecvThread)
	{
         m_pTcpRecvThread->Stop();
		 delete m_pTcpRecvThread;
		 m_pTcpRecvThread = NULL;
	}
	// 先把Socket关闭
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
	//添加一个时间戳，用于判断是否过时效
	oSndBuff->AddInt(iPacketTime);
	oSndBuff->AddByte(CMD_REAL_HEAD);
	oSndBuff->AddInt(g_ClientID);
	oSndBuff->AddInt(0);//流水号
	oSndBuff->AddByte(CMD_GET_GROUP_USER);//
	oSndBuff->AddInt(4); //消息长度
	oSndBuff->AddInt(groupid);
	oSndBuff->AddByte(CMD_REAL_TAIL);

	IMMsg *pmsg = new IMMsg;
	pmsg->iLen = oSndBuff->GetLength();
	pmsg->szSendBuf = (TCHAR*)oSndBuff->GetBuffer();
	m_pTcpSendThread->m_pSynsendList->AddTail(pmsg);
}
//type 意义
//1   登录消息。发送用户名和密码
//2   获取用户信息
//3    获取好友信息
void CTcpCommunication::SendBuffer(int type ,TCHAR *buff,int len)
{
	
	SYSTEMTIME ct;
	GetLocalTime( &ct);
	struct tm gm = { ct.wSecond, ct.wMinute, ct.wHour, ct.wDay, ct.wMonth-1, ct.wYear-1900, ct.wDayOfWeek, 0, 0 };
	time_t packettime = mktime(&gm);
	int iPacketTime = (int)packettime;
	CSendBuff *oSndBuff = NULL;
	oSndBuff = new CSendBuff;
	//添加一个时间戳，用于判断是否过时效
	oSndBuff->AddInt(iPacketTime);
	oSndBuff->AddByte(CMD_REAL_HEAD);
	oSndBuff->AddInt(g_ClientID);
	oSndBuff->AddInt(0);//流水号
	oSndBuff->AddByte(type);//
	oSndBuff->AddInt(len); //消息程度
	oSndBuff->AddBytes((BYTE*)buff,len);
	oSndBuff->AddByte(CMD_REAL_TAIL);

	IMMsg *pmsg = new IMMsg;
	pmsg->iLen = oSndBuff->GetLength();
	pmsg->szSendBuf = (TCHAR*)oSndBuff->GetBuffer();
	m_pTcpSendThread->m_pSynsendList->AddTail(pmsg);

}
//imid 对方 im号码
//buff 消息内容
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

	//添加一个时间戳，用于判断是否过时效
	oSndBuff->AddInt(iPacketTime);
	oSndBuff->AddByte(CMD_REAL_HEAD);
	oSndBuff->AddInt(g_ClientID);//自己的IM号码
	oSndBuff->AddInt(0);//流水号
	oSndBuff->AddByte(CMD_SEND_MSG);//
	oSndBuff->AddInt(totallen); //消息长度
	oSndBuff->AddByte(type);
	oSndBuff->AddInt(imid);//对方imid
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

	//添加一个时间戳，用于判断是否过时效
	oSndBuff->AddInt(iPacketTime);
	oSndBuff->AddByte(CMD_REAL_HEAD);
	oSndBuff->AddInt(g_ClientID);//自己的IM号码
	oSndBuff->AddInt(0);//流水号
	oSndBuff->AddByte(CMD_SEND_GROUP_MSG);//
	oSndBuff->AddInt(totallen); //消息长度
	oSndBuff->AddInt(groupid);
	oSndBuff->AddInt(imid);//对方imid
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
	// 先清理Socket
	CloseSocket();

	// 准备创建
	struct sockaddr_in serverAddress;
	int err=0;
	EnterCriticalSection(&m_csSocket);
	// 创建Socket
	m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	

	err = GetLastError();
	if( m_hSocket == INVALID_SOCKET)
	{
		g_pSvrLog->AddTextLog(LL_DEBUG, "[通信信息]：初始化SOCKET失败：INVALID_SOCKET.\r\n");
		LeaveCriticalSection(&m_csSocket);
		return FALSE;
	}
	
	// 长连接
	int val = 1; 	
	int r = setsockopt(m_hSocket,SOL_SOCKET,SO_KEEPALIVE,(char*)&val,sizeof(val));
	
	// 取消发送优化算法
	r = setsockopt(m_hSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&val, sizeof(val));
	
	memset(&serverAddress, 0, sizeof(serverAddress));     
	serverAddress.sin_family      = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(m_ServerIp.c_str());
	serverAddress.sin_port        = htons(m_dwServerPort);
	// 准备一个Select检查是否可以连接
	/*
	fd_set fdSocket;
	FD_ZERO(&fdSocket);
	FD_SET(m_hSocket,&fdSocket);
	timeval timeout;
	timeout.tv_sec=3; //超时时间设为3秒
	fd_set fdwrite=fdSocket;
	err = select(1, NULL, &fdwrite,	NULL, &timeout);
	*/
	if( SOCKET_ERROR == connect(m_hSocket,(sockaddr*)&serverAddress,sizeof(serverAddress)) )
	{
		int err = GetLastError();
		g_pSvrLog->AddTextLog(LL_DEBUG, "[通信信息]：初始化SOCKET失败: CONNECT ERROR.\r\n");
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;
		LeaveCriticalSection(&m_csSocket);
		return FALSE;	
	}
	
	//CString	str;
	//str.Format("[通信信息]：初始化SOCKET成功. 端口:%d\r\n", g_port);/////////
	//g_pSvrLog->AddTextLog(LL_DEBUG, (LPCSTR)str);
	LeaveCriticalSection(&m_csSocket);

	// 连接成功即发送一条注册消息
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
	//	轮询间隔1秒
	timeout.tv_sec = 1;
	timeout.tv_usec= 0;
	FD_ZERO( &fdset);
	FD_SET(m_hSocket, &fdset);
	err = select(1, NULL, &fdset, NULL, &timeout);
	//	出错
	if( err < 1)
	{
		g_pSvrLog->AddTextLog(LL_DEBUG, "[通信信息]：数据发送失败: send timeout.\r\n");
		return err;
	}
	// 操作Socket时候对临界区进行保护
	EnterCriticalSection(&m_csSocket);
	err = send(m_hSocket, szMsg, nLen, 0);
	LeaveCriticalSection(&m_csSocket);
	return err;
	
}

int CTcpCommunication::Recv(TCHAR* szMsg, int nLen)
{
	//select超时检查
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
		g_pSvrLog->AddRunLog(LL_DEBUG, "[通信信息]：数据接受失败: recv fail: %d.", gle);
		closesocket(m_hSocket);
		return nRecvCount;
	}
	else if (FD_ISSET(m_hSocket,&rfd))
	{
		EnterCriticalSection(&m_csSocket);
		nRecvCount = recv(m_hSocket, szMsg, nLen, 0);  // data is received in buff[MAX_LEN]
		LeaveCriticalSection(&m_csSocket);
		
		// 反常处理一下，这里返回0代表关闭，统一按照-1来返回，避免和select 返回0代表没有数据可读的冲突
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
	oShakeBuff.AddInt(g_ClientID);  //学校的ClientID
	oShakeBuff.AddByte(CMD_REAL_TAIL);    //construct a shake hand data struct
	int nRet = Send((TCHAR *)oShakeBuff.GetBuffer(),  oShakeBuff.GetLength());
	if (nRet == SOCKET_ERROR)
	{
        return nRet;
	}
	
	g_pSvrLog->AddRunLog(LL_INFO, "[通信信息]：本机向服务器已发送一条注册信息." );
	return nRet;			
}
