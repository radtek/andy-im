//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// INIFile.cpp : �����ļ�ʵ��
//
// Author: ziweic
// Date: 08/04/2008
//
//////////////////////////////////////////////////////////////////////

#include "../HeaderFiles/stdafx.h"
#include "../INIFile/INIFile.h"
#include <memory>
CINIFile::CINIFile()
{

}

CINIFile::~CINIFile()
{

}
/*
* �������ܣ�Base64�������
* ������
* buf       [in]�����ܵ��ַ���
* text      [in]�������ܵ��ַ���
* size      [in]���ַ�������
* �� �� ֵ��buflen ���ܺ���ַ���
*/
int CINIFile::Base64Enc(char *buf, char*text,int size) 
{ 
	static char *base64_encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int buflen = 0; 
	
	while(size>0)
	{
		*buf++ = base64_encoding[ (text[0] >> 2 ) & 0x3f];
		if(size>2)
		{
			*buf++ = base64_encoding[((text[0] & 3) << 4) | (text[1] >> 4)];
			*buf++ = base64_encoding[((text[1] & 0xF) << 2) | (text[2] >> 6)];
			*buf++ = base64_encoding[text[2] & 0x3F];
		}
		else
		{
			switch(size)
			{
			case 1:
				*buf++ = base64_encoding[(text[0] & 3) << 4 ];
				*buf++ = '=';
				*buf++ = '=';
				break;
			case 2: 
				*buf++ = base64_encoding[((text[0] & 3) << 4) | (text[1] >> 4)]; 
				*buf++ = base64_encoding[((text[1] & 0x0F) << 2) | (text[2] >> 6)]; 
				*buf++ = '='; 
				break; 
			} 
		} 
		
		text +=3; 
		size -=3; 
		buflen +=4; 
	} 
	
	*buf = 0; 
	return buflen;  
} 

/*
* �������ܣ�base64�����ʵ��
* ������
* ch       [in]�����ܵ��ַ���
* �� �� ֵ�����ܺ���ַ�
*/
char CINIFile::GetBase64Value(char ch)
{
	if ((ch >= 'A') && (ch <= 'Z')) 
		return ch - 'A'; 
	if ((ch >= 'a') && (ch <= 'z')) 
		return ch - 'a' + 26; 
	if ((ch >= '0') && (ch <= '9')) 
		return ch - '0' + 52; 
	switch (ch) 
	{ 
	case '+': 
		return 62; 
	case '/': 
		return 63; 
	case '=': /* base64 padding */ 
		return 0; 
	default: 
		return 0; 
	} 
}

//����base64��������Ӧ����4�ı���(����mime��׼)
//�������4�������ش���
//ע�� ��������һ���ַ� ��ô���Ȳ�׼�� ���ܻ��1 
//����buf����
int CINIFile::Base64Dec(char *buf,char*text,int size) 
{
	if(size%4)
	{
		return -1;
	}
	char chunk[4];
	int parsenum=0;
	
	while(size>0)
	{
		chunk[0] = GetBase64Value(text[0]); 
		chunk[1] = GetBase64Value(text[1]); 
		chunk[2] = GetBase64Value(text[2]); 
		chunk[3] = GetBase64Value(text[3]); 
		
		*buf++ = (chunk[0] << 2) | (chunk[1] >> 4); 
		*buf++ = (chunk[1] << 4) | (chunk[2] >> 2); 
		*buf++ = (chunk[2] << 6) | (chunk[3]);
		
		text+=4;
		size-=4;
		parsenum+=3;
	}
	
	return parsenum;
} 

bool CINIFile::GetCurrDirectory(string & curr_dir)
{
  	// With a length of 0 and NULL buffer to get the required length.
  	int i = GetCurrentDirectory(0, NULL);
  	
  	if (i == 0)
  	return false; // Function call failed. so return false.
 	
 	auto_ptr<char> spbuf((char*)operator new((i+1)* sizeof(char)));
 	
 	// Get the directory with the correct length and sufficient buffer.
 	if (GetCurrentDirectory(i, spbuf.get()) == 0)
 		return false;
 	
  	curr_dir = spbuf.get();
  	
  	return true;
}

DWORD CINIFile::GetConfigString(LPCSTR lpAppName, 
								LPCSTR lpKeyName, 
								LPCSTR lpDefault, 
								LPSTR lpReturnString, 
								DWORD dwSize,
								LPCSTR lpFileName)

{
	return ::GetPrivateProfileString(lpAppName, lpKeyName, 
		lpDefault, lpReturnString, dwSize, lpFileName);

}

void CINIFile::GetDBIinInfo(string &dbservername,string &dbname, string &dbuser,string &dbpass, string &dblanguage)
{
	string filename = "";
	string szPath;
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\�����ļ�\\serversetting.ini";
	char iniBuf[80];
	memset(iniBuf,0,80);
	GetConfigString(	"���ݿ������IP", "DATABASE_SERVER", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    dbservername = iniBuf;
	GetConfigString(	"���ݿ���", "DATABASE_NAME",
		NULL, iniBuf,INI_BUFF_SIZE, filename.c_str());
	dbname = iniBuf;
	GetConfigString(	"���ݿ��½��", "USERNAME", NULL, 
		iniBuf, INI_BUFF_SIZE,filename.c_str());
	dbuser = iniBuf;
	GetConfigString(	"���ݿ��½����", "PASSWORD", NULL, 
		iniBuf,INI_BUFF_SIZE, filename.c_str());
	dbpass = iniBuf;
	char  decpass[40];
	memset(decpass,0,40);
	Base64Dec(decpass,iniBuf,strlen(iniBuf));//����BASE64���ܺ������
	dbpass = decpass;
	GetConfigString(	"���ݿ�����", "LANGUAGE",NULL,
		iniBuf,INI_BUFF_SIZE, filename.c_str());
	dblanguage = iniBuf;
}

void CINIFile::GetDBPoolInfo(int & nMaxConn, int & nMinConn)
{
	string filename = "";
	string szPath;
	string strTemp = "";
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\�����ļ�\\serversetting.ini";
	char iniBuf[10];
	
	// ���������
	memset(iniBuf, 0, 10);
	GetConfigString("���ݿ����ӳ���Ϣ", "MAXDBCONNECTION", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
	strTemp = iniBuf;

	nMaxConn = atoi(strTemp.c_str());

	// ��С������
	memset(iniBuf, 0, 10);
	GetConfigString("���ݿ����ӳ���Ϣ", "MINDBCONNECTION", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
	strTemp = iniBuf;

	nMinConn = atoi(strTemp.c_str());
}

void CINIFile::GetLocalPort(string &strPort)
{
	string filename = "";
	string szPath;
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\�����ļ�\\serversetting.ini";
	char iniBuf[10];
	memset(iniBuf,0,10);
	GetConfigString("�˿ں�", "LOCALPORT", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
	strPort= iniBuf;
}


void CINIFile::GetSgipInfo(SGIP_INIINFO &sgipinfo)

{
	string filename = "";
	string szPath = "";
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\�����ļ�\\smssetting.ini";
	char iniBuf[80];
    memset(iniBuf,0,80);
	GetConfigString(	"��ͨ���ص�ַ", "sgip_servip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
     sgipinfo.servip = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"��ͨ���ض˿�", "sgip_servport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.servport = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"��ͨ������ַ", "sgip_localip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.localip = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"��ͨ�����󶨶˿�", "sgip_localport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.localport = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"��ͨ�û���", "sgip_username", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.username = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"��ͨ�û�����", "sgip_pwd", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.pwd = iniBuf;
	
	memset(iniBuf,0,80);
	GetConfigString(	"��ͨ��ҵ����", "sgip_cpid", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.cpid = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"��ͨ�ڵ���", "sgip_nodeid", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.nodeid = iniBuf;
	
	memset(iniBuf,0,80);
	GetConfigString(	"��ͨ�������", "sgip_spid", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.spid = iniBuf;
	
	memset(iniBuf,0,80);
	GetConfigString(	"��ͨSOCKET��ʱʱ��", "sgip_sockettimeout", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.sockettimeout = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"��ͨSUBMIT��Ч��", "sgip_submittimeout", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.submittimeout = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"��ͨ����������", "sgip_smgconn", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.smgconn = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"Bind����", "sgip_logintype", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.logintype = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit����", "sgip_sp_number", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.spnumber = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"Submit����", "sgip_feetype", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.feetype = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit����", "sgip_feevalue", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.feevalue = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"Submit����", "sgip_givenvalue", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.givenvalue = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"Submit����", "sgip_agentflag", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.agentflag = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit����", "sgip_MTFlag", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.MTflag = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit����", "sgip_priority", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.priority = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit����", "sgip_reportflag", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.reportflag = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit����", "sgip_msgcoding", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.msgcoding = (BYTE)atoi(iniBuf);
	
	memset(iniBuf,0,80);
	GetConfigString(	"Submit����", "sgip_msgtype", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.msgtype = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"��������", "sgip_send_inter", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.nSendInterval = (int)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"�ط�����", "sgip_max_resend_times", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.nMaxResendTimes = (int)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"�ط�����", "sgip_resend_interval", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.nResendInterval = 1000*(int)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"����������", "sgip_book_cmd", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.bookcmd = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"�˶�������", "sgip_countermand", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.countermand = iniBuf;
}

void CINIFile::GetCmppinfo(CMPP_INIINFO &cmppinfo)
{
	string filename = "";
	string szPath;
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\�����ļ�\\smssetting.ini";
	char iniBuf[80];
	memset(iniBuf,0,80);
	GetConfigString(	"�ƶ����ص�ַ", "cmpp_servip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.servip = iniBuf;
	
	GetConfigString(	"�ƶ����ض˿�", "cmpp_servport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.servport = iniBuf;
	
	GetConfigString(	"�ƶ��������", "cmpp_spid", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.spid = iniBuf;
	
	GetConfigString(	"�ƶ���½����", "cmpp_loginpwd", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.loginpwd = iniBuf;
	
	GetConfigString(	"�ƶ�������ַ", "cmpp_localip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.localip = iniBuf;

	GetConfigString(	"�ƶ������󶨶˿�", "cmpp_localport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.localport = iniBuf;
}

void CINIFile::GetSmgpinfo(SMGP_INIINFO &smgpinfo)
{
	string filename = "";
	string szPath;
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\�����ļ�\\smssetting.ini";
	char iniBuf[80];\
	memset(iniBuf,0,80);
	
	GetConfigString(	"�������ص�ַ", "smgp_servip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.servip = iniBuf;
	
	GetConfigString(	"�������ض˿�", "smgp_servport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.servport = iniBuf;
	
	GetConfigString(	"���ŷ������", "smgp_spid", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.spid = iniBuf;
	
	GetConfigString(	"���ŵ�½����", "smgp_loginpwd", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.loginpwd = iniBuf;
	
	GetConfigString(	"���ű�����ַ", "smgp_localip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.localip = iniBuf;
	
	GetConfigString(	"���ű����󶨶˿�", "smgp_localport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.localport = iniBuf;
}

void CINIFile::GetDefaultOverlapped(string &strOverlapped)
{
	string filename = "";
	string szPath;
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\�����ļ�\\serversetting.ini";
	char iniBuf[10];
	memset(iniBuf,0,10);
	GetConfigString("Ԥ����ͻ�����Ŀ", "DEFAULTOVERLAPPED", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
	strOverlapped = iniBuf;
}

void CINIFile::GetChanleType(int & nChanleType)
{
	string filename = "";
	string szPath;
	string strTemp = "";
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\�����ļ�\\serversetting.ini";
	char iniBuf[10];
	memset(iniBuf,0,10);
	GetConfigString("��ʱ����ͨ����Ϣ", "TEMPORARYSMS", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
	strTemp = iniBuf;

	nChanleType = atoi(strTemp.c_str());
}

void CINIFile::GetNewClientVersion(string &FTPUser, string &FtpPassword, string &strPath)
{
	string filename = "";
	string szPath;
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\�����ļ�\\serversetting.ini";
	char iniBuf[MAX_PATH];

	// �����û���
	memset(iniBuf,0,MAX_PATH);
	GetConfigString("�ͻ���������Ϣ", "FTPUSER", 
		NULL,iniBuf,MAX_PATH, filename.c_str());
	FTPUser = iniBuf;

	// ��������
	memset(iniBuf,0,MAX_PATH);
	GetConfigString("�ͻ���������Ϣ", "FTPPASSWORD", 
		NULL,iniBuf,MAX_PATH, filename.c_str());
    FtpPassword = iniBuf;

	// �����ļ�·��
	memset(iniBuf,0,MAX_PATH);
	GetConfigString("�ͻ���������Ϣ", "FTPDIRECTORY", 
		NULL,iniBuf,MAX_PATH, filename.c_str());
	strPath = iniBuf;
}

void CINIFile::GetMaxDelay(int & nMaxDelay)
{
	string filename = "";
	string szPath;
	string strTemp = "";
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\�����ļ�\\serversetting.ini";
	char iniBuf[10];
	
	// ���������
	memset(iniBuf, 0, 10);
	GetConfigString("��������ӳ�����", "MAXMSGDELAY", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
	strTemp = iniBuf;
	
	nMaxDelay = atoi(strTemp.c_str());
}

void CINIFile::GetUploadFTPInfo(string &FTPUser, string &FtpPassword)
{
	string filename = "";
	string szPath;
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\�����ļ�\\serversetting.ini";
	char iniBuf[MAX_PATH];

	// �����û���
	memset(iniBuf,0,MAX_PATH);
	GetConfigString("�ϴ���־�ļ���Ϣ", "UPLOADFTPUSER", 
		NULL,iniBuf,MAX_PATH, filename.c_str());
	FTPUser = iniBuf;

	// ��������
	memset(iniBuf,0,MAX_PATH);
	GetConfigString("�ϴ���־�ļ���Ϣ", "UPLOADFTPPASSWORD", 
		NULL,iniBuf,MAX_PATH, filename.c_str());
    FtpPassword = iniBuf;
}