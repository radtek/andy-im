// IM_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../IOCPServer/IOCPServer.h"
#include "../DBInterface/DBInterface.h"
#include <iostream>
#include <iomanip>
#include <math.h>
#include "../INIFile/INIFile.h"
#include "../LogFile/ServerLog.h"
#include "../MainFlow/CommonQueue.h"
#include <windows.h>
#include <winsvc.h>
#include <string>
using namespace std;

CServerLog * g_pSvrLog;//��־�ļ�ȫ�ֱ���
CServerLog * g_pSmSendLog;//������¼���ű���
CINIFile *g_pIniFile;//�����ļ�ȫ�ֱ���
CClientMgr * g_pClientMgr;  // ȫ�ֿͻ��˹���ģ��ָ��
CGlobalSendQueue *g_pPendingSendQueue; // ȫ�ִ������ͻ��˵���Ϣ����
DBInterface *g_pDBInterface;//���ݿ�ӿ�ȫ��ָ��
enum OperationType
{
	OPERATION_START = 0,
	OPERATION_STOP
};

int Oper = OPERATION_START;
int _tmain(int argc, _TCHAR* argv[])
{
	// ���ý��������ļ�Ϊ�������л���Ŀ¼
	char chFilePath[MAX_PATH];
	GetModuleFileName(NULL, chFilePath, MAX_PATH);
	std::string strCurrentModule = chFilePath;

	std::string::size_type iPos = strCurrentModule.find_last_of('\\');
	strCurrentModule = strCurrentModule.substr(0, iPos +1);
	SetCurrentDirectory(strCurrentModule.c_str());

	//������־�ӿ�ȫ�ֱ���
	g_pSvrLog  = new CServerLog(); 
	g_pSmSendLog = new  CServerLog();
	string strSvrLog;
	string strSmsLog;
	strSvrLog = strCurrentModule + "RunLog.txt";

	//strCurrentModule += "RunLog.txt";
	//g_pSvrLog->CreateLogFile(strCurrentModule.c_str());
	g_pSvrLog->CreateLogFile(strSvrLog.c_str());
	//g_pSmSendLog->CreateLogFile(strSmsLog.c_str());

	unsigned short xx = htons(0x8000);
	//���������ļ�ȫ�ֱ���
	g_pIniFile = new CINIFile();

	// �ͻ��˹������
	g_pClientMgr = new CClientMgr;

	// �����ͻ�����Ϣ����
	g_pPendingSendQueue = new CGlobalSendQueue;
	g_pDBInterface = new DBInterface();


		CIOCPServer oIOCPServer;

	if (OPERATION_START == Oper)
	{

		g_pPendingSendQueue->Start();

		oIOCPServer.InitializeService();

		oIOCPServer.MainLoop();
	}
	else if (OPERATION_STOP == Oper)
	{

		g_pPendingSendQueue->Stop();

		oIOCPServer.DestroyService();

#ifdef _SERVICE
		g_pSvrLog->Stop();
#endif
		delete g_pClientMgr;
		delete g_pDBInterface;
		delete g_pPendingSendQueue;
		delete g_pIniFile;
		delete g_pSvrLog;
	}
	return 0;
}

