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
const int SOCK_TCP	= 0;	//TCP模式
const int SOCK_UDP  = 1;	//UPD模式

#define MSG_HAVEDATA   (WM_USER + 666)

typedef struct tcpPara 
{   
	//窗口句柄
	//CWnd		*pdlg;
	//端口
	int			port;
	//IP
	std::string		strIp;
	//消息内容
	std::string     strMsg;

	std::string    strFriend;
	//用户名
	std::string     strUserName;//自己
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
	HANDLE	pThreadLisen;		//监听线程-->_ListenTcpThread
	HANDLE  pThreadSend;//发送线程

public:
	TcpConn(MainFrame *frame_wnd);
	virtual ~TcpConn();
public:
    //线程函数
	int ReceiveMsg(SOCKET &socket,char type,std::string &strusername,std::string &strfriendname);

	int SendMsg(tcpPara para);
	int GetIpAddress(const std::string & sHostName,std::string & sIpAddress);
	int GetLocalHostName(std::string &sHostName);
	std::string GetError(DWORD error);
	int	m_nSockType;	//连接类型
	int m_WorkType;		//工作方式 server0,client1,both2
	int m_client,m_server;
//	CString m_fileName;
	bool FileWork,FileStop,StopServer;
	char m_type;//接受类型/F-文件，C-抓平，D-关闭,M-消息
	int m_Potr;//当前使用端口
	int GetNamebyAddress(const std::string IpAddress,std::string sYouName);//获得对方计算机名称
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
	//父窗体句柄,发消息用
	HWND     m_wnd;

	tcpPara   m_tcpListernPara;
	tcpPara   m_tcpSendMsgPara;

	//SOCKET			m_sockClient;									//	连接服务器的套接字(客户端的套接字)
	SOCKET			m_sockServer;	
	SOCKET			m_sock;											//	接受到的套接字
	int InitSocket();

   	MainFrame* frame_wnd_;

};

#endif // !defined(AFX_TCPCONN_H__C2E596EA_DFB1_4080_91DB_3848EA0C67C7__INCLUDED_)
