//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// INIFile.h : 配置文件定义
//
// Author: ziweic
// Date: 08/04/2008
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__AF2B6BEE_69FA_4BF7_B3E8_CEA5DD861639__INCLUDED_)
#define AFX_INIFILE_H__AF2B6BEE_69FA_4BF7_B3E8_CEA5DD861639__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>
#include <string>
#include "../HeaderFiles/common.h"
using namespace std;
class CINIFile  
{
public:
	CINIFile();
	virtual ~CINIFile();
public:
	int			Base64Enc(char *buf, char*text,int size);
	char		GetBase64Value(char ch);
	int			Base64Dec(char *buf,char*text,int size);
	bool		GetCurrDirectory(string & curr_dir); //获得当前路径
	DWORD		GetConfigString(LPCSTR lpAppName, 
								LPCSTR lpKeyName, 
								LPCSTR lpDefault, 
								LPSTR lpReturnString, 
								DWORD dwSize,
								LPCSTR lpFileName);
   // 获得数据库配置信息
   void         GetDBIinInfo( string &dbservername,string &dbname,
							  string &dbuser,string &dbpass,
							  string &dblanguage);
   void         GetDBPoolInfo(int & nMaxConn, int & nMinConn);
   // 获得本地端口号
   void         GetLocalPort( string &strPort);
   void         GetDefaultOverlapped(string &strOverlapped);
   // 获得联通短息网关配置信息
   void         GetSgipInfo(SGIP_INIINFO &sgipinfo);
   void         GetCmppinfo(CMPP_INIINFO &cmppinfo);
   void         GetSmgpinfo(SMGP_INIINFO &smgpinfo);
   void         GetChanleType(int & nChanleType);
   // 获得新版本中间件的目录
   void         GetNewClientVersion(string &FTPUser, string &FtpPassword, string &strPath);
   // 获得做大延迟时间限制
   void         GetMaxDelay(int & nMaxDelay);
   // 获取上传日志的FTP信息
   void         GetUploadFTPInfo(string &FTPUser, string &FtpPassword);
};

#endif // !defined(AFX_INIFILE_H__AF2B6BEE_69FA_4BF7_B3E8_CEA5DD861639__INCLUDED_)
