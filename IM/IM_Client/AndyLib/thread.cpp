/**********************************************************************
//

//
//							AndySDK��� andylib.lib
//���ߣ�
//
//�汾��1.0
//

//
//ԭ�ļ�����thread.cpp
//
//˵�����߳���ʵ�ִ���
//
**********************************************************************/
#define MAKE_SELF_LIB

#include "andylib.h"

AndyThread::AndyThread()
{
	m_handle = NULL;
	m_info = NULL;
	m_bQuit = FALSE;
	m_event = NULL;
	m_dwId = 0;
	m_dwExitCode = 0xFFFFFFFF;
	m_nEvCnt = 0;
	m_nWaitSecond = 30;
}

AndyThread::~AndyThread()
{
	//�ȴ�30����߳��Զ��˳�������ǿ���˳���	
	Quit();
	if(m_event)
		CloseHandle(m_event);
}
	
BOOL AndyThread::Start(THREADPROC proc,LONG param)
{
	if(m_handle)
	{
		DWORD code = 0;
		GetExitCodeThread(m_handle,&code);
		if(code == STILL_ACTIVE)
			return TRUE;	//�����߳�����
	}
	if(proc)
		m_handle = NewThread((THREADPROC)proc,(LPVOID)param);
	return (BOOL)m_handle;
}

BOOL AndyThread::CanContinue(DWORD dwMilSec/* = INFINITE*/)
{
	if(m_event == NULL)
		m_event = CreateEvent(NULL,TRUE,TRUE,NULL);

	return (WAIT_OBJECT_0 == WaitForSingleObject(m_event,dwMilSec));
}

void AndyThread::Continue()
{
	m_crl.Enter();
	
	m_nEvCnt ++;
	
	if(m_event)
		SetEvent(m_event);
	else
		m_event = CreateEvent(NULL,TRUE,TRUE,NULL);

	m_crl.Leave();
}

void AndyThread::Wait(BOOL bForceWait/*=FALSE*/)
{
	m_crl.Enter();

	if(bForceWait)
		m_nEvCnt = 0;
	else
		m_nEvCnt --;
	
	if(m_nEvCnt <= 0)
	{
		m_nEvCnt = 0;
		
		if(m_event)
			ResetEvent(m_event);
		else
			m_event = CreateEvent(NULL,TRUE,FALSE,NULL);
	}

	m_crl.Leave();
}

void AndyThread::Pause()
{
	if(m_handle)
		SuspendThread(m_handle);
}

void AndyThread::Resume()
{
	if(m_handle)
		ResumeThread(m_handle);
}

BOOL AndyThread::SetPriority(int nPriority)
{
	if(m_handle)
		return SetThreadPriority(m_handle,nPriority);
	else
		return FALSE;
}

BOOL AndyThread::PostMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return PostThreadMessage(m_dwId,uMsg,wParam,lParam);
}

void AndyThread::Quit(int nWaitSecond/*=30*/)
{
	if(m_handle == NULL)
		return;

	m_nWaitSecond = nWaitSecond;
	WaitQuit();
}

void AndyThread::ThreadQuit(int nWaitSecond/*=30*/)
{
	if(m_bQuit || m_handle == NULL)	//m_bQuit = TRUE��ʾ���˳��߳�������
		return;

	//�����˳��̣߳�����������˳��̣߳�һ���˳���������Ҫ�˳����߳��е��ã��ᵼ��
	//��ʱ�������������ȴ��߳��˳���ʱ�䡣
	m_nWaitSecond = nWaitSecond;
	HANDLE handle = NewThread((THREADPROC)QuitProc,this);
	CloseHandle(handle);
}

DWORD AndyThread::NotifyThreadQuit()
{
	m_crl.Enter();
	try
	{
		if(m_handle)
			CloseHandle(m_handle);

		//���ò������Ա���һ������	
		m_handle = NULL;
		delete m_info;
		m_info = NULL;
		m_bQuit = FALSE;
		m_nEvCnt = 0;
		m_dwId = 0;
		m_dwExitCode = 0xFFFFFFFF;

		if(m_event)
			SetEvent(m_event);
	}
	catch(...)
	{
	}
	m_crl.Leave();
	return m_dwExitCode;
}

DWORD WINAPI AndyThread::ThreadProc(AndyThInfo* obj)
{	
	if(obj)
	{
		try
		{
			obj->m_thread->m_dwId = GetCurrentThreadId();
			obj->m_thread->m_dwExitCode = 0xFFFFFFFF;
			try
			{
				obj->m_thread->m_dwExitCode = (obj->m_obj->*(obj->m_lpthreadFunc))(obj->m_param);
			}
			catch(...)
			{
			}
			return obj->m_thread->NotifyThreadQuit();			
		}
		catch(...)
		{
		}
	}
	return 0;
}

DWORD WINAPI AndyThread::QuitProc(AndyThread* obj)
{
	if(obj)
		return obj->WaitQuit();
	return 0;
}

DWORD AndyThread::GetExitCode()
{
	if(m_handle == NULL)
		return m_dwExitCode;
	else
	{
		DWORD Code;
		if(GetExitCodeThread(m_handle,&Code))
			return Code;
		else
			return 0xFFFFFFFF;
	}	
}

DWORD AndyThread::WaitQuit()
{
	try
	{
		DWORD code = STILL_ACTIVE;
		int cnt = 0;
		int max = m_nWaitSecond * 1000;	//����
		//BOOL bAlive;

		do
		{
			if(code != STILL_ACTIVE)
				break;

			m_bQuit = TRUE;
			
			if(m_event)
				SetEvent(m_event);

			SwitchToThread();
			Sleep(200);			
			cnt += 200;
			if(cnt > max)
			{
				TerminateThread(m_handle,0xFFFFFFFF);
				break;
			}
			//m_crl.Enter();
			//bAlive = (m_handle && GetExitCodeThread(m_handle,&code));
			//m_crl.Leave();
		}while(m_handle && GetExitCodeThread(m_handle,&code));
		
		return NotifyThreadQuit();		
	}
	catch(...)
	{
	}
	return 0;
}
