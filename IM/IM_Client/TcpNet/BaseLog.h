// BaseLog.h: interface for the CBaseLog class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BASE_LOG_
#define _BASE_LOG_
#include "../StdAfx.h"

//��־�����壺
//LL_ERROR��  ����
//LL_WARNING���쳣����һ���Ǵ���
//LL_INFO��   Ӧ�ó���������Ϣ�����ڴ������̸���
//LL_DEBUG��  �������ݣ������ڵ��Գ���ʱ�������ݸ���
//LL_DEBUGDETAIL: ����ϸ�����ݣ������������ϸ���ݵ�
enum LOGLEVEL
{
	LL_DEBUGDETAIL=0,
		LL_DEBUG,
		LL_INFO,		
		LL_WARNING,
		LL_ERROR,
		LL_MAXLEVEL
};

class CBaseLog  
{
public:
	CBaseLog();
	virtual ~CBaseLog();

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

#endif // !defined(AFX_BASELOG_H__719CF753_905F_4D97_BEBE_2AEB09A097F9__INCLUDED_)
