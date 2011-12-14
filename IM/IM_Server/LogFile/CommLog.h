// ServerLog.h: interface for the CCommLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_COMMLOG_H__)
#define _COMMLOG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "../HeaderFiles/StdAfx.h"


//��־�����壺
//LL_ERROR��  ����
//LL_WARNING���쳣����һ���Ǵ���
//LL_INFO��   Ӧ�ó���������Ϣ�����ڴ������̸���
//LL_DEBUG��  �������ݣ������ڵ��Գ���ʱ�������ݸ���
//LL_DEBUGDETAIL: ����ϸ�����ݣ������������ϸ���ݵ�
#define MAX_LOG_SIZE   3000  //1048576 //1M ���ֽ�Ϊ��λ
enum LOGLEVEL
{
	LL_DEBUGDETAIL=0,
		LL_DEBUG,
		LL_INFO,		
		LL_WARNING,
		LL_ERROR,
		LL_MAXLEVEL
};

class CCommLog  
{
public:
	CCommLog();
	virtual ~CCommLog();
	
public:
	//������־��¼����
	void  SetLogLevel(LOGLEVEL ll)
	{
		m_RecLogLevel=ll;
	}
	//��ȡ��־��¼����
	LOGLEVEL GetLogLevel()
	{
		return m_RecLogLevel;
	}
	
	//���һ����ʱ�����־��¼���ɴ���ʽ������
	virtual void  AddTextLog(enum LOGLEVEL ll,const char* log)=0;
	
protected:
	//��־��¼����		
	LOGLEVEL m_RecLogLevel;	
};

#endif // !defined(_COMMLOG_H__)
