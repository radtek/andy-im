//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// INIFile.h : �����ļ�����
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
	bool		GetCurrDirectory(string & curr_dir); //��õ�ǰ·��
	DWORD		GetConfigString(LPCSTR lpAppName, 
								LPCSTR lpKeyName, 
								LPCSTR lpDefault, 
								LPSTR lpReturnString, 
								DWORD dwSize,
								LPCSTR lpFileName);
   // ������ݿ�������Ϣ
   void         GetDBIinInfo( string &dbservername,string &dbname,
							  string &dbuser,string &dbpass,
							  string &dblanguage);
   void         GetDBPoolInfo(int & nMaxConn, int & nMinConn);
   // ��ñ��ض˿ں�
   void         GetLocalPort( string &strPort);
   void         GetDefaultOverlapped(string &strOverlapped);
   // �����ͨ��Ϣ����������Ϣ
   void         GetSgipInfo(SGIP_INIINFO &sgipinfo);
   void         GetCmppinfo(CMPP_INIINFO &cmppinfo);
   void         GetSmgpinfo(SMGP_INIINFO &smgpinfo);
   void         GetChanleType(int & nChanleType);
   // ����°汾�м����Ŀ¼
   void         GetNewClientVersion(string &FTPUser, string &FtpPassword, string &strPath);
   // ��������ӳ�ʱ������
   void         GetMaxDelay(int & nMaxDelay);
   // ��ȡ�ϴ���־��FTP��Ϣ
   void         GetUploadFTPInfo(string &FTPUser, string &FtpPassword);
};

#endif // !defined(AFX_INIFILE_H__AF2B6BEE_69FA_4BF7_B3E8_CEA5DD861639__INCLUDED_)
