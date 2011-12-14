//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// INIFile.cpp : 配置文件实现
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
* 函数功能：Base64编码解码
* 参数：
* buf       [in]：加密的字符串
* text      [in]：待加密的字符串
* size      [in]：字符串长度
* 返 回 值：buflen 加密后的字符串
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
* 函数功能：base64解码的实现
* 参数：
* ch       [in]：解密的字符串
* 返 回 值：解密后的字符
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

//进行base64解码输入应该是4的倍数(根据mime标准)
//如果不是4倍数返回错误
//注意 如果是最后一个字符 那么长度不准备 可能会多1 
//返回buf长度
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
	else filename = szPath + "\\配置文件\\serversetting.ini";
	char iniBuf[80];
	memset(iniBuf,0,80);
	GetConfigString(	"数据库服务器IP", "DATABASE_SERVER", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    dbservername = iniBuf;
	GetConfigString(	"数据库名", "DATABASE_NAME",
		NULL, iniBuf,INI_BUFF_SIZE, filename.c_str());
	dbname = iniBuf;
	GetConfigString(	"数据库登陆名", "USERNAME", NULL, 
		iniBuf, INI_BUFF_SIZE,filename.c_str());
	dbuser = iniBuf;
	GetConfigString(	"数据库登陆密码", "PASSWORD", NULL, 
		iniBuf,INI_BUFF_SIZE, filename.c_str());
	dbpass = iniBuf;
	char  decpass[40];
	memset(decpass,0,40);
	Base64Dec(decpass,iniBuf,strlen(iniBuf));//解密BASE64加密后的密码
	dbpass = decpass;
	GetConfigString(	"数据库语言", "LANGUAGE",NULL,
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
	else filename = szPath + "\\配置文件\\serversetting.ini";
	char iniBuf[10];
	
	// 最大连接数
	memset(iniBuf, 0, 10);
	GetConfigString("数据库连接池信息", "MAXDBCONNECTION", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
	strTemp = iniBuf;

	nMaxConn = atoi(strTemp.c_str());

	// 最小连接数
	memset(iniBuf, 0, 10);
	GetConfigString("数据库连接池信息", "MINDBCONNECTION", 
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
	else filename = szPath + "\\配置文件\\serversetting.ini";
	char iniBuf[10];
	memset(iniBuf,0,10);
	GetConfigString("端口号", "LOCALPORT", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
	strPort= iniBuf;
}


void CINIFile::GetSgipInfo(SGIP_INIINFO &sgipinfo)

{
	string filename = "";
	string szPath = "";
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\配置文件\\smssetting.ini";
	char iniBuf[80];
    memset(iniBuf,0,80);
	GetConfigString(	"联通网关地址", "sgip_servip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
     sgipinfo.servip = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"联通网关端口", "sgip_servport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.servport = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"联通本机地址", "sgip_localip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.localip = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"联通本机绑定端口", "sgip_localport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.localport = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"联通用户名", "sgip_username", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.username = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"联通用户口令", "sgip_pwd", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.pwd = iniBuf;
	
	memset(iniBuf,0,80);
	GetConfigString(	"联通企业代码", "sgip_cpid", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.cpid = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"联通节点编号", "sgip_nodeid", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.nodeid = iniBuf;
	
	memset(iniBuf,0,80);
	GetConfigString(	"联通服务代码", "sgip_spid", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.spid = iniBuf;
	
	memset(iniBuf,0,80);
	GetConfigString(	"联通SOCKET超时时间", "sgip_sockettimeout", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.sockettimeout = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"联通SUBMIT有效期", "sgip_submittimeout", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.submittimeout = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"联通网关连接数", "sgip_smgconn", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.smgconn = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"Bind命令", "sgip_logintype", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.logintype = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit命令", "sgip_sp_number", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.spnumber = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"Submit命令", "sgip_feetype", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.feetype = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit命令", "sgip_feevalue", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.feevalue = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"Submit命令", "sgip_givenvalue", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.givenvalue = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"Submit命令", "sgip_agentflag", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.agentflag = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit命令", "sgip_MTFlag", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.MTflag = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit命令", "sgip_priority", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.priority = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit命令", "sgip_reportflag", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.reportflag = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"Submit命令", "sgip_msgcoding", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.msgcoding = (BYTE)atoi(iniBuf);
	
	memset(iniBuf,0,80);
	GetConfigString(	"Submit命令", "sgip_msgtype", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.msgtype = (BYTE)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"发送设置", "sgip_send_inter", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.nSendInterval = (int)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"重发设置", "sgip_max_resend_times", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.nMaxResendTimes = (int)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"重发设置", "sgip_resend_interval", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.nResendInterval = 1000*(int)atoi(iniBuf);

	memset(iniBuf,0,80);
	GetConfigString(	"订购命令字", "sgip_book_cmd", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.bookcmd = iniBuf;

	memset(iniBuf,0,80);
	GetConfigString(	"退订命令字", "sgip_countermand", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    sgipinfo.countermand = iniBuf;
}

void CINIFile::GetCmppinfo(CMPP_INIINFO &cmppinfo)
{
	string filename = "";
	string szPath;
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\配置文件\\smssetting.ini";
	char iniBuf[80];
	memset(iniBuf,0,80);
	GetConfigString(	"移动网关地址", "cmpp_servip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.servip = iniBuf;
	
	GetConfigString(	"移动网关端口", "cmpp_servport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.servport = iniBuf;
	
	GetConfigString(	"移动服务代码", "cmpp_spid", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.spid = iniBuf;
	
	GetConfigString(	"移动登陆密码", "cmpp_loginpwd", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.loginpwd = iniBuf;
	
	GetConfigString(	"移动本机地址", "cmpp_localip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.localip = iniBuf;

	GetConfigString(	"移动本机绑定端口", "cmpp_localport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    cmppinfo.localport = iniBuf;
}

void CINIFile::GetSmgpinfo(SMGP_INIINFO &smgpinfo)
{
	string filename = "";
	string szPath;
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\配置文件\\smssetting.ini";
	char iniBuf[80];\
	memset(iniBuf,0,80);
	
	GetConfigString(	"电信网关地址", "smgp_servip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.servip = iniBuf;
	
	GetConfigString(	"电信网关端口", "smgp_servport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.servport = iniBuf;
	
	GetConfigString(	"电信服务代码", "smgp_spid", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.spid = iniBuf;
	
	GetConfigString(	"电信登陆密码", "smgp_loginpwd", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.loginpwd = iniBuf;
	
	GetConfigString(	"电信本机地址", "smgp_localip", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.localip = iniBuf;
	
	GetConfigString(	"电信本机绑定端口", "smgp_localport", 
		NULL,iniBuf,INI_BUFF_SIZE, filename.c_str());
    smgpinfo.localport = iniBuf;
}

void CINIFile::GetDefaultOverlapped(string &strOverlapped)
{
	string filename = "";
	string szPath;
	if(!GetCurrDirectory(szPath)) 
		filename="";
	else filename = szPath + "\\配置文件\\serversetting.ini";
	char iniBuf[10];
	memset(iniBuf,0,10);
	GetConfigString("预分配客户端数目", "DEFAULTOVERLAPPED", 
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
	else filename = szPath + "\\配置文件\\serversetting.ini";
	char iniBuf[10];
	memset(iniBuf,0,10);
	GetConfigString("临时短信通道信息", "TEMPORARYSMS", 
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
	else filename = szPath + "\\配置文件\\serversetting.ini";
	char iniBuf[MAX_PATH];

	// 读出用户名
	memset(iniBuf,0,MAX_PATH);
	GetConfigString("客户端升级信息", "FTPUSER", 
		NULL,iniBuf,MAX_PATH, filename.c_str());
	FTPUser = iniBuf;

	// 读出密码
	memset(iniBuf,0,MAX_PATH);
	GetConfigString("客户端升级信息", "FTPPASSWORD", 
		NULL,iniBuf,MAX_PATH, filename.c_str());
    FtpPassword = iniBuf;

	// 读出文件路径
	memset(iniBuf,0,MAX_PATH);
	GetConfigString("客户端升级信息", "FTPDIRECTORY", 
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
	else filename = szPath + "\\配置文件\\serversetting.ini";
	char iniBuf[10];
	
	// 最大连接数
	memset(iniBuf, 0, 10);
	GetConfigString("短信最大延迟限制", "MAXMSGDELAY", 
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
	else filename = szPath + "\\配置文件\\serversetting.ini";
	char iniBuf[MAX_PATH];

	// 读出用户名
	memset(iniBuf,0,MAX_PATH);
	GetConfigString("上传日志文件信息", "UPLOADFTPUSER", 
		NULL,iniBuf,MAX_PATH, filename.c_str());
	FTPUser = iniBuf;

	// 读出密码
	memset(iniBuf,0,MAX_PATH);
	GetConfigString("上传日志文件信息", "UPLOADFTPPASSWORD", 
		NULL,iniBuf,MAX_PATH, filename.c_str());
    FtpPassword = iniBuf;
}