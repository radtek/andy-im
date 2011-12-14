// TcpRecvThread.h: interface for the CTcpRecvThread class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TCP_RECV_THREAD_
#define _TCP_RECV_THREAD_

#include "commThread.H"
//#include "BaseList.h"
#include "comm_buff.h"
#include "MyHead.h"
//#include "WriteFileThread.h"
#include "TcpCommunication.h"
class CTcpCommunication;
class CTcpRecvThread : public CRunTimeThread  
{
public:
	CTcpRecvThread(CTcpCommunication * pTcpComm);
	virtual ~CTcpRecvThread();
public:
	void				Stop();
	void				Run();
private:
	void				TrimLeft(std::string & str);
	BOOL				IsSpace(char c);
	void				ResetClientTime(int iServerTime);
	//void				WriteTimeIniFile(S_TIME_STRATEGY& oTimeDataStrategy);
	//void				ReceivePhoneRenewMsg(CRecvBuff& recvDataBuff, BOOL &m_bDataRenewed,WORD wMsgCount);
	//void				ReceiveTimeRenewMsg(CRecvBuff& recvDataBuff, BOOL &m_bDataRenewed,WORD wMsgCount);
	//void				RenewRFIDPhoneStrategy(S_PHONE_STRATEGY &oPhoneDataStrategy,  std::ofstream & oFile);
	//void				RenewTimeStrategy(S_TIME_STRATEGY &oTimeDataStrategy, const std::ofstream & oFile);
	void				SendShakeMsg(S_SHAKE_DATA& pShakeData);
	void				SplitPacket(char* pData, int iCount);
	void				RenewHandShake(char* pData, int iCount);
	//自动升级
	//BOOL                AutoUpdate(string &strInfo);
	//上传日志文件给服务器
	//BOOL                UploadLogFile(string &strUser, string &strPass,string &strLogData);
	int                 Base64Dec(char *buf,char*text,int size);
    char                GetBase64Value(char ch);

 
private:
	BOOL				m_bConnected;
	int					m_iBufferLength;
	char				m_RecvBuffer[BIG_BUFFER];
	CTcpCommunication	*m_pTcpCommu;
	string				m_strIniFile;
};	

#endif // !defined(AFX_TCPRECVTHREAD_H__CC764CA4_04B6_46DD_BD36_C191A438EE29__INCLUDED_)
