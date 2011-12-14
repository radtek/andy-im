// TcpSendThread.h: interface for the CTcpSendThread class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TCP_SEND_THREAD_
#define _TCP_SEND_THREAD_

#include "commThread.H"
#include "comm_buff.h"
#include "MyHead.h"	
#include <vector>
#include<queue>
#include "BaseList.h"
//#include "WriteFileThread.h"
#include "TcpCommunication.h"
class CTcpCommunication;
class CTcpSendThread : public CRunTimeThread  
{
public:
	CTcpSendThread(CTcpCommunication *pTcpCommu, BYTE bType, int nMaxRecordNum);
	virtual ~CTcpSendThread();

public:
	void				Stop();
	void				Run();
	int					SendBuildDatas( );
private:
	//int					SendRenewRequest(BYTE MsgType, int type);
	//void				BuildRealTimeData(S_TAG_RECORD &oTagRecord, CSendBuff &oSndBuff);
	//发送检查SOCKET的空包
	int					SendConnectPacket();
private:	
	UINT				mr;
	CRITICAL_SECTION	m_otagRecordSection;
	CTcpCommunication	* m_pTcpComm;
	int					m_nMaxRecordSize;
//	TAGRECORDS			m_oTagRecords;
	BYTE				m_bCount;
	BYTE				m_bType;
public:
	//  发送临界区
	CRITICAL_SECTION    m_csendsection;                                   
    CRITICAL_SECTION    m_cwritefilesection;
	CSynList			*m_pSynsendList;

};

#endif // !defined(AFX_TCPSENDTHREAD_H__CC1DE887_BCB3_4700_B9AF_4FF7CDDBEAEC__INCLUDED_)
