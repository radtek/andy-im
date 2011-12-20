#if !defined(_MYHEAD_H__)
#define _MYHEAD_H__
#include "../StdAfx.h"
//#include <afxtempl.h>
#include <map>
#include <vector>


//typedef	CArray<S_COMINFO, S_COMINFO>	ARCOMINFO;


//typedef CArray<S_TAG_RECORD, S_TAG_RECORD>	TAGRECORDS;

typedef struct _S_SEND_DATA
{
	BYTE	*bData;
	int		nLen;
}S_SEND_DATA;

//����Э��ṹ
typedef struct _S_SHAKE_DATA
{   
    int iClientID;
	byte bClientMsgType;
	int iCommandID;
	int iMsgResult;
    _S_SHAKE_DATA()
	{
		iClientID = -1;
        bClientMsgType = -1;
        iCommandID = -1;
        iMsgResult = -1;
	}
}S_SHAKE_DATA;









//typedef	CArray<S_PHONE_STRATEGY, S_PHONE_STRATEGY>	oPhoneStrategyArrays;

#define		CMD_TYPE_REAL	1
#define		CMD_TYPE_COS	2
#define		CMD_TYPE_CHECK  3

#define		CMDHEAD					0x02
#define		FRAME_HEAD				0x02		//֡ͷ��ʾ��  
#define		FRAME_END				0x03		//֡β��ʾ�� 



#define		CMD_REAL_HEAD  0x02
#define		CMD_REAL_TAIL  0x03
#define		CMD_DOT		   0x04
#define     CMD_LEN			14

#define     RENEW_MAX_LEN   4096
#define		CMD_REAL_HEAD  0x02
#define		CMD_REAL_TAIL  0x03
#define		CMD_CHECK_HEAD 0x09
#define		CMD_DOT		   0x04

#define     COMMAND_LENGTH 2104
#define     BIG_BUFFER     8192*8*6
#define     MIN_BUFFER     1024

// ��ʾ�������͵ĺ�
#define     NET_TYPE_UNION              1  // ��ͨ
#define     NET_TYPE_MOBILE             2  // �ƶ�
#define     NET_TYPE_TELCOM             3  // ����С��ͨ






#define     CMD_LOGIN     1  // ��¼
#define     CMD_GET_USER      2  // ��ȡ�û���Ϣ
#define     CMD_GET_FRIEND    3 //��ȡ������Ϣ
#define     CMD_SEND_MSG      4//������Ϣ
#define     CMD_FRIEND_ONLINE        5  // ��������
#define     CMD_GET_GROUP             6//��ȡȺ��Ϣ
#define     CMD_TIME_CHECK              7  // ʱ��У��
#define     CMD_GET_GROUP_USER        8//��ȡȺ�û���Ϣ





#define MSG_HAVEDATA WM_USER+1000
#define MSG_GETFRIEND WM_USER+1001

typedef struct tag_MSGBODY 
{
	unsigned int imid;
    TCHAR msg[256];
	tag_MSGBODY()
	{
		imid = -1;

		memset(msg,0,256);
	}

}MSGBODY;


#endif