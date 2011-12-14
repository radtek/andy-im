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

CServerLog * g_pSvrLog;//日志文件全局变量
CServerLog * g_pSmSendLog;//用来纪录短信报告
CINIFile *g_pIniFile;//配置文件全局变量
CClientMgr * g_pClientMgr;  // 全局客户端管理模块指针
CGlobalSendQueue *g_pPendingSendQueue; // 全局待发个客户端的信息队列
DBInterface *g_pDBInterface;//数据库接口全局指针
enum OperationType
{
	OPERATION_START = 0,
	OPERATION_STOP
};

int Oper = OPERATION_START;
int _tmain(int argc, _TCHAR* argv[])
{
	// 设置进程所在文件为进程运行环境目录
	char chFilePath[MAX_PATH];
	GetModuleFileName(NULL, chFilePath, MAX_PATH);
	std::string strCurrentModule = chFilePath;

	std::string::size_type iPos = strCurrentModule.find_last_of('\\');
	strCurrentModule = strCurrentModule.substr(0, iPos +1);
	SetCurrentDirectory(strCurrentModule.c_str());

	//生成日志接口全局变量
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
	//生成配置文件全局变量
	g_pIniFile = new CINIFile();

	// 客户端管理队列
	g_pClientMgr = new CClientMgr;

	// 创建客户端消息队列
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

