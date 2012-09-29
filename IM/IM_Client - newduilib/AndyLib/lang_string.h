/**********************************************************************
//

//
//							AndySDK类库 不同语言字符串定义
//作者：
//
//版本：1.0
//

//
//
//原文件名：lang_string.h
//
//说明：不同语言字符串定义。
//
**********************************************************************/
#ifndef _INC_LANGUAE_STANDY_HEADER
#define _INC_LANGUAE_STANDY_HEADER

#if defined(ANDYSDK_VER_ENGLISH)
//English strings
//for AndyComm:
#define LANSTR_COMM_OPENFAIL	_T("Open communications device[%s] fail - ")
#define LANSTR_COMM_EVENTFAIL		_T("CreateEvent for communications device fail - ")
#define LANSTR_COMM_EVENTSETFAIL	_T("SetCommMask fail - ")
#define LANSTR_COMM_GETTIMEOUTFAIL	_T("GetCommTimeouts fail - ")
#define LANSTR_COMM_SETTIMEOUTFAIL	_T("SetCommTimeouts fail - ")
#define LANSTR_COMM_CLEARERRORFAIL	_T("ClearCommError fail - ")
#define LANSTR_COMM_GETDCBFAIL	_T("Unable to obtain DCB information - ")
#define LANSTR_COMM_SETDCBFAIL	_T("Unable to set DCB information - ")
#define LANSTR_COMM_WRITETIMEOUT	_T("Write data timeout - ")

#define LANSTR_WRITEFILEERROR	_T("WriteFile error - ")

#define LANSTR_FILEOPEN _T("Open")
#define LANSTR_SELDIR _T("Choose a folder")
////界面库字符串
#define LANSTR_MENUBARNAME	_T("Menu bar")

//#elif defined()
//other languages string define here
//...
#else
//默认中文
//AndyComm用到的字符串：
#define LANSTR_COMM_OPENFAIL	_T("打开串口[%s]失败 - ")
#define LANSTR_COMM_EVENTFAIL		_T("创建串口监视事件失败 - ")
#define LANSTR_COMM_EVENTSETFAIL	_T("设置串口事件失败 - ")
#define LANSTR_COMM_GETTIMEOUTFAIL	_T("获取串口超时设置失败 - ")
#define LANSTR_COMM_SETTIMEOUTFAIL	_T("设置串口超时失败 - ")
#define LANSTR_COMM_CLEARERRORFAIL	_T("ClearCommError失败 - ")
#define LANSTR_COMM_GETDCBFAIL	_T("获取DCB信息失败 - ")
#define LANSTR_COMM_SETDCBFAIL	_T("设置DCB信息失败 - ")
#define LANSTR_COMM_WRITETIMEOUT	_T("写入数据超时 - ")

#define LANSTR_WRITEFILEERROR	_T("写入文件失败 - ")

#define LANSTR_FILEOPEN _T("打开")
#define LANSTR_SELDIR _T("选择路径")

//界面库字符串
#define LANSTR_MENUBARNAME	_T("菜单栏")

#endif

#endif
