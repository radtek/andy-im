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
	// ��װ�˵�Socket���ṩһ�������Ľ��
    BOOL				InitSocket();
	void				CloseSocket();
	int					Send(char* szMsg, int nLen);
	int					Recv(char* szMsg, int nLen);
	int					SendClietReg();
	void                SendBuffer(int type,char *buff,int len);//����
	void                SendMsg(unsigned int imid,char *buff,int len);//������Ϣ
private:
	//CWriteFileThread	*m_pWriteFileThread;
	DWORD				m_dwServerPort;
	string			m_ServerIp;
	BYTE				m_bType;
	BOOL				m_bConnected;
	SOCKET				m_hSocket;
	//	�����߳�
	HANDLE				m_hSend; 
	CTcpRecvThread		*m_pTcpRecvThread;
	UINT				mr;
	BOOL				m_theradexit;
	// ��Socket��һ���ٽ�������
	CRITICAL_SECTION	m_csSocket;
public:
	CTcpSendThread		*m_pTcpSendThread;
    CSynList			*m_psDatalist;

	MainFrame* frame_wnd_;
	// ���ͱ�־ 0Ϊ������1Ϊ�����
	BOOL				m_sendType; 

};

#endif // !defined(AFX_TCPCOMMUNICATION_H__73338AFA_8FA3_4B2D_8B01_028AAFCC0F1C__INCLUDED_)
