/**********************************************************************
//

//
//							AndySDK��� ��ͬ�����ַ�������
//���ߣ�
//
//�汾��1.0
//

//
//
//ԭ�ļ�����lang_string.h
//
//˵������ͬ�����ַ������塣
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
////������ַ���
#define LANSTR_MENUBARNAME	_T("Menu bar")

//#elif defined()
//other languages string define here
//...
#else
//Ĭ������
//AndyComm�õ����ַ�����
#define LANSTR_COMM_OPENFAIL	_T("�򿪴���[%s]ʧ�� - ")
#define LANSTR_COMM_EVENTFAIL		_T("�������ڼ����¼�ʧ�� - ")
#define LANSTR_COMM_EVENTSETFAIL	_T("���ô����¼�ʧ�� - ")
#define LANSTR_COMM_GETTIMEOUTFAIL	_T("��ȡ���ڳ�ʱ����ʧ�� - ")
#define LANSTR_COMM_SETTIMEOUTFAIL	_T("���ô��ڳ�ʱʧ�� - ")
#define LANSTR_COMM_CLEARERRORFAIL	_T("ClearCommErrorʧ�� - ")
#define LANSTR_COMM_GETDCBFAIL	_T("��ȡDCB��Ϣʧ�� - ")
#define LANSTR_COMM_SETDCBFAIL	_T("����DCB��Ϣʧ�� - ")
#define LANSTR_COMM_WRITETIMEOUT	_T("д�����ݳ�ʱ - ")

#define LANSTR_WRITEFILEERROR	_T("д���ļ�ʧ�� - ")

#define LANSTR_FILEOPEN _T("��")
#define LANSTR_SELDIR _T("ѡ��·��")

//������ַ���
#define LANSTR_MENUBARNAME	_T("�˵���")

#endif

#endif
