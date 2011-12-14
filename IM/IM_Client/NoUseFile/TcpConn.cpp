// TcpConn.cpp: implementation of the TcpConn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "IMServer.h"
#include "TcpConn.h"
//#include "../ImProDlg.h"
#include "main_frame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TcpConn::TcpConn(MainFrame *frame_wnd)
: frame_wnd_(frame_wnd)
{
	pThreadLisen = NULL;		//监听线程-->_ListenTcpThread
    pThreadSend  = NULL;

	//	初始化网络
	WSADATA wsaData;
	WSAStartup(MAKEWORD( 2, 2 ), &wsaData);

	//InitSocket();
}

TcpConn::~TcpConn()
{

}
int TcpConn::InitSocket()
{
	////clinet inti
	//if (INVALID_SOCKET != m_sockClient)
	//{
	//	closesocket(m_sockClient);
	//	m_sockClient = INVALID_SOCKET;
	//}

	//m_sockClient = socket(AF_INET, SOCK_STREAM, 0);

	//if (INVALID_SOCKET == m_sockClient)
	//{
	//	return -1;
	//}


	//int val = 1;
	//int valsize = sizeof(int);
	//int r = 0;

	////设置连接保活
	////r = setsockopt(m_sockClient, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof(val));

	////r = setsockopt(m_sockClient, IPPROTO_TCP, TCP_NODELAY, (char*)&val, sizeof(val));

	//if(INVALID_SOCKET == m_sockClient)
	//{
	//	//需要写日志
	//	return -1;
	//}
	//init socket server

	m_sockServer = socket(AF_INET, SOCK_STREAM, 0);
	if(INVALID_SOCKET == m_sockServer)
	{
		return false;
	}

	int iResult= -1;
	SOCKADDR_IN addrSockSrv;
	addrSockSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSockSrv.sin_family = AF_INET;
	addrSockSrv.sin_port = htons(m_tcpListernPara.port);

	iResult = bind( this->m_sockServer,(SOCKADDR*)&addrSockSrv,sizeof(SOCKADDR) ); 
	if ( SOCKET_ERROR==iResult )
	{
		return FALSE;
	}

	/*iResult = listen(m_sockServer, 1);
	if ( SOCKET_ERROR==iResult )
	{
		return FALSE;
	}*/

   
}
std::string TcpConn::GetError(DWORD error)	//返回错误信息
{
	std::string strError;
	switch(error)
	{
	case WSANOTINITIALISED:
		strError="初始化错误";
		break;
	case WSAENOTCONN:
		strError="对方没有启动";
		break;
	case WSAEWOULDBLOCK :
		strError="对方已经关闭";
		break;
	case WSAECONNREFUSED:
		strError="连接的尝试被拒绝";
		break;
	case WSAENOTSOCK:
		strError="在一个非套接字上尝试了一个操作";
		break;
	case WSAEADDRINUSE:
		strError="特定的地址已在使用中";
		break;
	case WSAECONNRESET:
		strError="与主机的连接被关闭";
		break;
	default:
		strError="一般错误";	
	}
	return strError;
	
}
//////////////////////////////////服务器线程开始///////////////////////////////////////////

//TCP监听线程
DWORD WINAPI TcpConn::_ListenTcpThread(LPVOID lparam)
{

	TcpConn &pthis=*(TcpConn *)lparam;
	if(pthis.StopServer==true)	return -1;
	
	//CSocket sockSrvr;
	//pDlg->m_Potr=PORT+pDlg->m_server;//保存当前使用端口，用于关闭
	//int createSucceed=sockSrvr.Create(pDlg->m_tcpListernPara.port);
	/*if(createSucceed==0)
	{
		AfxMessageBox("_ListenTcpThread Create错误!"+pDlg->GetError(GetLastError()));
		return -1;
	}*/
	
	int listenSucceed=listen(pthis.m_sockServer, 1);	//开始监听
	if(listenSucceed==SOCKET_ERROR)
	{
		//AfxMessageBox("_ListenTcpThread Listen错误!"+pthis->GetError(GetLastError()));
		return -1;
	}
	

	if (pthis.m_sock != INVALID_SOCKET)
	{
		closesocket(pthis.m_sock);
		pthis.m_sock = INVALID_SOCKET;
	}
	SOCKADDR_IN addrSockClient;
	int nLen = 0;
	nLen = sizeof(SOCKADDR);

	//	等待连接
	pthis.m_sock = accept( pthis.m_sockServer, (SOCKADDR*)&addrSockClient, &nLen);	
	//// 设置接受超时时间10秒
	//int val = 10000;
	//setsockopt(pthis->m_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&val,sizeof(val));
	char flag[FLAG]={0};		


	int result = -1;
	//	同步控制
	result = recv(pthis.m_sock, flag, FLAG, 0);
	if(result!=2) 
	{
		return -1;	
	}
	char myID[10] = {0};

	result = recv(pthis.m_sock, myID, USERID, 0);
	if (result!=10)
	{
		return -1;
	}
	std::string strfriendName;
	//strMyid.Format("%s",myID);
	sprintf((char*)strfriendName.c_str(),"%s",myID);
	pthis.m_type=flag[0];
	if(pthis.m_type=='D') 
		return 0;

	DWORD dwThreadID = 0;
	pthis.pThreadLisen= CreateThread(NULL,0,_ListenTcpThread, (LPVOID)&pthis,  0,&dwThreadID);
	//pthis->pThreadLisen=::AfxBeginThread(_ListenTcpThread,pthis);
	pthis.ReceiveMsg(pthis.m_sock,flag[0],pthis.m_tcpListernPara.strUserName,strfriendName);
	return 0;
	
}//发送线程
DWORD WINAPI TcpConn::_SendMsgThread(LPVOID lparam)
{
	TcpConn &pthis=*(TcpConn *)lparam;


	SOCKET sockClient;
	//clinet inti
	//if (INVALID_SOCKET != sockClient)
	//{
	//	closesocket(sockClient);
	//	sockClient = INVALID_SOCKET;
	//}

	sockClient = socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == sockClient)
	{
		return -1;
	}

	if(INVALID_SOCKET == sockClient)
	{
		//需要写日志
		return -1;
	}
	struct sockaddr_in addr;
    int err = -1;
	addr.sin_family		= AF_INET;
	addr.sin_port		= htons(pthis.m_tcpSendMsgPara.port);
	addr.sin_addr.s_addr= inet_addr(pthis.m_tcpSendMsgPara.strIp.c_str());

	err = connect(sockClient, (struct sockaddr *)&addr, sizeof( addr));

	if(SOCKET_ERROR == err)
	{
		int i = WSAGetLastError();
		//write log
		return 0;
	}
	//首先发送标记M为信息,2
	int end=0;
	//end=sockClient.Send("M",FLAG); 
	end = send(sockClient,"M",FLAG,0);
	if(end==SOCKET_ERROR)
	{
		//AfxMessageBox("_SendMsgThread Send错误!"+pDlg->GetError(GetLastError()));
		return -1;
	}
	else if(end!=2)
	{
		//AfxMessageBox("消息头错误");
		return -1;
	}
	//然后发送自己的名称，为10个byte
	end = send(sockClient,pthis.m_tcpSendMsgPara.strUserName.c_str(),USERID,0);
	if(end==SOCKET_ERROR)
	{
		//AfxMessageBox("_SendMsgThread Send错误!"+pDlg->GetError(GetLastError()));
		return -1;
	}
	end = send(sockClient,pthis.m_tcpSendMsgPara.strMsg.c_str(),strlen(pthis.m_tcpSendMsgPara.strMsg.c_str()),0);
	//end=sockClient.Send(pDlg->m_tcpSendMsgPara.strMsg,strlen(pDlg->m_tcpSendMsgPara.strMsg)); 
	if(end==SOCKET_ERROR)
	{
		//AfxMessageBox("_SendMsgThread Send错误!"+pDlg->GetError(GetLastError()));
		closesocket(sockClient);
		return -1;
	}
	/*sockClient.Close();*/
	//closesocket(pthis.m_sockClient);
	closesocket(sockClient);
	
	return 0;
}


///////////////////////////////////函数//////////////////////////////////////////

int TcpConn::ReceiveMsg(SOCKET &recSo,char type,std::string &strUserName,std::string &strfriendName)//接受函数
{

	//	if(StopServer==true)	return -1;
	if(type=='C')		//抓取
	{
	//	S7aveYouScreen(CopyScreenToBitmap(),client);//发送地址
	}
	
	else if(type=='F')			//文件
	{
	//	SaveYouFile(recSo,client);
	}
	
	else if(type=='M')	//信息
	{
		char buff[100]={0};
		std::string msg;
		int ret=0;
		for(;;)
		{
			memset(buff,0,100);
			ret = recv(recSo, buff, 100, 0);
			//ret=recSo.Receive(buff,100);
			if(ret==0 || ret == SOCKET_ERROR)
				break;
			msg+=buff;
		}
		MSGBODY  msgbody;
		sprintf((char*)msgbody.strUserName.c_str(),"%s",strUserName.c_str());
		sprintf((char*)msgbody.strfriendName.c_str(),"%s",strfriendName.c_str());
		sprintf((char*)msgbody.strMsg.c_str(),"%s",msg.c_str());
		//::SendMessage(m_wnd,MSG_HAVEDATA,(WPARAM)&msgbody,0);
		if (strlen(strfriendName.c_str()) !=0)
		{

		
			frame_wnd_->SendMessage(MSG_HAVEDATA,(WPARAM)&msgbody,0);
			//::SendMessage(m_wnd,MSG_HAVEDATA,(WPARAM)&msgbody,0);


			//frame_wnd_->SHowChatDlg(strfriendName,msg);
			//frame_wnd_->m_mChatDlg[strfriendName]->SendMsg(strfriendName.c_str(),msg.c_str());
		}
	
    //	::SendMessage(m_wnd,MSG_HAVEDATA,(WPARAM)msg.GetBuffer(msg.GetLength()),(LPARAM)userID.GetBuffer(userID.GetLength()));
 
	}
	//recSo.Close();	
	return 0;
}
void TcpConn::startListenThread(tcpPara para)
{
	SetTcpListrenPara(para);
	DWORD dwThreadID = 0;
	pThreadLisen= CreateThread(NULL,0,_ListenTcpThread, (LPVOID)this,  0,&dwThreadID);
	//pThreadLisen=::AfxBeginThread(_ListenTcpThread,this);	//开始TCP线程
}
void TcpConn::statrSendMsgThread(tcpPara para)
{
	SetTcpSendPara(para);
	DWORD dwThreadID = 0;
	pThreadSend= CreateThread(NULL,0,_SendMsgThread, (LPVOID)this,  0,&dwThreadID);
	//pThreadSend = ::AfxBeginThread(_SendMsgThread,this);
}

std::string TcpConn::GetMsg()
{
	return m_strmsg;
}

void TcpConn::SetWorkType(int type)
{
	m_WorkType = type;
}

////////////////////////////////////服务器线程结束//////////////////////////////////////


////////////////////////////////////客户端线程开始//////////////////////////////////////
void TcpConn::SetTcpListrenPara(tcpPara tcppara)
{
	m_tcpListernPara = tcppara;
}
void TcpConn::SetTcpSendPara(tcpPara tcppara)
{
	m_tcpSendMsgPara = tcppara;
}
int TcpConn::GetLocalHostName(std::string &sHostName)	//获得本地计算机名称
{
	char szHostName[256];
	int nRetCode;
	nRetCode=gethostname(szHostName,sizeof(szHostName));
	if(nRetCode!=0)
	{
		//产生错误
		sHostName=_T("没有取得");
		return GetLastError();
	}
	sHostName=szHostName;
	return 0;
}

int TcpConn::GetIpAddress(const std::string &sHostName, std::string &sIpAddress)//获得本地IP
{
	struct hostent FAR * lpHostEnt=gethostbyname(sHostName.c_str());
	if(lpHostEnt==NULL)
	{
		//产生错误
		sIpAddress=_T("");
		return GetLastError();
	}
	//获取IP
	LPSTR lpAddr=lpHostEnt->h_addr_list[0];
	if(lpAddr)
	{
		struct in_addr inAddr;
		memmove(&inAddr,lpAddr,4);
		//转换为标准格式
		sIpAddress=inet_ntoa(inAddr);
		if(strlen(sIpAddress.c_str())==0)
			sIpAddress=_T("没有取得");
	}
	return 0;
}
int TcpConn::GetNamebyAddress(const std::string IpAddress,std::string sYouName)//获得对方计算机名称
{
	unsigned long addr;
	addr=inet_addr(IpAddress.c_str());
	struct hostent FAR * lpHostEnt=gethostbyaddr((char *)&addr,4,AF_INET);
	if(lpHostEnt==NULL)
	{
		//产生错误
		sYouName=_T("");

		//AfxMessageBox("连接不上");//应该取得其错误
		return -1;
	}
	std::string name=lpHostEnt->h_name;
	sYouName=name;
	return 0;

}

// int TcpConn::SendMsg(tcpPara para)
// {
// 	CSocket sockClient;
// 	sockClient.Create();
// 	CString ip,strError;
// 	int conn=sockClient.Connect(para.strIp,para.port);
// 	if(conn==0)	///////////////////////////////////
// 	{
// 		AfxMessageBox("_SendMsgThread Connect错误!"+GetError(GetLastError()));
// 		sockClient.ShutDown(2);
// 		sockClient.Close();
// 		return 0;
// 		
// 	}
// 	//首先发送标记M为信息,2
// 	int end=0;
// 	end=sockClient.Send("M",FLAG); 
// 	if(end==SOCKET_ERROR)
// 	{
// 		AfxMessageBox("_SendMsgThread Send错误!"+GetError(GetLastError()));
// 		return -1;
// 	}
// 	else if(end!=2)
// 	{
// 		AfxMessageBox("消息头错误");
// 		return -1;
// 	}
//    //然后发送自己的名称，为10个byte
// 	end = sockClient.Send(para.strUserName,USERID);
// 	if(end==SOCKET_ERROR)
// 	{
// 		AfxMessageBox("_SendMsgThread Send错误!"+GetError(GetLastError()));
// 		return -1;
// 	}
// 
// 	end=sockClient.Send(para.strMsg,para.strMsg.GetLength()); 
// 	if(end==SOCKET_ERROR)
// 	{
// 		AfxMessageBox("_SendMsgThread Send错误!"+GetError(GetLastError()));
// 		return -1;
// 	}
// 	//sockClient.Close();
// 	
// 	return 0;
//}