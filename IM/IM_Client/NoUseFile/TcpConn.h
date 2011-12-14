// TcpConn.h: interface for the TcpConn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPCONN_H__C2E596EA_DFB1_4080_91DB_3848EA0C67C7__INCLUDED_)
#define AFX_TCPCONN_H__C2E596EA_DFB1_4080_91DB_3848EA0C67C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "StdAfx.h"
#include <iostream>
#include <string.h>
#include <WinSock.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#define PORT		34567
#define FLAG		2
#define USERID      10
#define SIZEFILE	1024
const int SOCK_TCP	= 0;	//TCPģʽ
const int SOCK_UDP  = 1;	//UPDģʽ

#define MSG_HAVEDATA   (WM_USER + 666)

typedef struct tcpPara 
{   
	//���ھ��
	//CWnd		*pdlg;
	//�˿�
	int			port;
	//IP
	std::string		strIp;
	//��Ϣ����
	std::string     strMsg;

	std::string    strFriend;
	//�û���
	std::string     strUserName;//�Լ�
	tcpPara()
	{   
	   strMsg = "";
	   strFriend = "";
	   strUserName = "";
	   port = 8888;
	}	
}tcpPara;

struct MSGBODY
{
	std::string strUserName;
	std::string strfriendName;
	std::string strMsg;
};
class MainFrame;
class TcpConn  
{

public:
	HANDLE	pThreadLisen;		//�����߳�-->_ListenTcpThread
	HANDLE  pThreadSend;//�����߳�

public:
	TcpConn(MainFrame *frame_wnd);
	virtual ~TcpConn();
public:
    //�̺߳���
	int ReceiveMsg(SOCKET &socket,char type,std::string &strusername,std::string &strfriendname);

	int SendMsg(tcpPara para);
	int GetIpAddress(const std::string & sHostName,std::string & sIpAddress);
	int GetLocalHostName(std::string &sHostName);
	std::string GetError(DWORD error);
	int	m_nSockType;	//��������
	int m_WorkType;		//������ʽ server0,client1,both2
	int m_client,m_server;
//	CString m_fileName;
	bool FileWork,FileStop,StopServer;
	char m_type;//��������/F-�ļ���C-ץƽ��D-�ر�,M-��Ϣ
	int m_Potr;//��ǰʹ�ö˿�
	int GetNamebyAddress(const std::string IpAddress,std::string sYouName);//��öԷ����������
    //
	void startListenThread(tcpPara para);
	void statrSendMsgThread(tcpPara para);
	std::string  m_strmsg;

	std::string GetMsg();

	void SetWorkType(int type);

    void SetTcpListrenPara(tcpPara tcppara);
	void SetTcpSendPara(tcpPara tcppara);

	static DWORD WINAPI _ListenTcpThread(LPVOID lparam);
	static DWORD WINAPI _SendMsgThread(LPVOID lparam);
	//��������,����Ϣ��
	HWND     m_wnd;

	tcpPara   m_tcpListernPara;
	tcpPara   m_tcpSendMsgPara;

	//SOCKET			m_sockClient;									//	���ӷ��������׽���(�ͻ��˵��׽���)
	SOCKET			m_sockServer;	
	SOCKET			m_sock;											//	���ܵ����׽���
	int InitSocket();

   	MainFrame* frame_wnd_;

};

#endif // !defined(AFX_TCPCONN_H__C2E596EA_DFB1_4080_91DB_3848EA0C67C7__INCLUDED_)
