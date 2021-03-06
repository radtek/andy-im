// TcpCommunication.h: interface for the CTcpCommunication class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TCP_COMMUNICATION_
#define _TCP_COMMUNICATION_

#include "../StdAfx.h"
#include "TcpSendThread.h"	// Added by ClassView
#include "TcpRecvThread.h"
#include <WinSock.h>
//#include "WriteFileThread.h"
//#include "ClassList.h"
#include "BaseList.h"
class MainFrame;
class CTcpSendThread;
class CTcpRecvThread;
class CTcpCommunication
{
public:
	CTcpCommunication(MainFrame * frame_wnd);
	virtual ~CTcpCommunication();

public:
	BOOL				StopService();
	BYTE				GetType();
	BOOL				Initialize(string  strPeerIp,	unsigned short usPort, BYTE bType, int	nMaxRecordNum);
	// 封装了的Socket，提供一个操作的借口
    BOOL				InitSocket();
	void				CloseSocket();
	int					Send(TCHAR* szMsg, int nLen);
	int					Recv(TCHAR* szMsg, int nLen);
	int					SendClietReg();
	void                SendBuffer(int type,TCHAR *buff,int len);//发包
	void                SendMsg(byte type,unsigned int imid,TCHAR *buff,int bufflen);// byte 消息类型 ，发送消息
	void                SendGroupMsg(unsigned int groupid ,unsigned int imid,TCHAR *buff,int bufflen);
	void                SendGetGroupUserMsg(unsigned int groupid);//发送获取群用户信息
private:
	//CWriteFileThread	*m_pWriteFileThread;
	DWORD				m_dwServerPort;
	string			m_ServerIp;
	BYTE				m_bType;
	BOOL				m_bConnected;
	SOCKET				m_hSocket;
	//	发送线程
	HANDLE				m_hSend; 
	CTcpRecvThread		*m_pTcpRecvThread;
	UINT				mr;
	BOOL				m_theradexit;
	// 对Socket加一个临界区保护
	CRITICAL_SECTION	m_csSocket;
public:
	CTcpSendThread		*m_pTcpSendThread;
    CSynList			*m_psDatalist;

	MainFrame* frame_wnd_;
	// 发送标志 0为单条发1为组包发
	BOOL				m_sendType; 

};

#endif // !defined(AFX_TCPCOMMUNICATION_H__73338AFA_8FA3_4B2D_8B01_028AAFCC0F1C__INCLUDED_)
