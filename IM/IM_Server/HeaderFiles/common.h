//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// common.h : �����궨���ļ�
//
// Author: ziweic
// Date: 08/04/2008
//
//////////////////////////////////////////////////////////////////////
#pragma   warning (disable: 4786)
#ifndef __COMMON_H__
#define __COMMON_H__
#include <wtypes.h>
#include <TIME.h>
#include <string>
#include <queue>
#include <map>
using namespace std;
#define MAP_SIZE            8192*8                // Map�����ݵĸ���
#define STRSQLMAX			8192*2				// SQL max len
#define nMaxBuffer			8192*4
#define BIG_BUFFER          8192*8              // �󻺳��������
#define TABLECOLS			50					// max table columns
#define COLEN				50					// ����г���
#define INI_BUFF_SIZE		80  
#define DEFAULT_OVERLAPPED_COUNT 2000			// Ĭ��overlapped�ṹ����Ŀ
#define RFID_BUFFER_SIZE    6                   // Ŀǰ��RFID������ֻ����6λ
#define TIME_VERIFY_SPAN    30*60*1000          // ʱ��У����Ϊ30����
#define TELEPHONE_LENGTH    20                  // ��ǰЭ���ж��绰���볤��Ϊ20�ֽ�
#define ASSISTENT_THREAD_COUNT 5               // �����̵߳���Ŀ
#define PK_DATA_LEN         500                 // �������ݰ�����������


#define ERR_DBSYS			-1					//���ݿ�ϵͳ����

enum  DBI_ERROR
{	
		ERR_DBIFIRST=0,
		ERR_MAX_SQLSTR,							//SQL��䳤�ȴ���%d���ַ�
		ERR_SQL_CMD,							//SQL����﷨����
		ERR_SQL_EXEC,							//SQL������д���
		ERR_SQL_RESULTS,						//SQL���������
		ERR_BIND_DATABASE_FAIL,					//�����ݿ�ʧ��
		ERR_RESULTS_NOCOLS,						//����0������
		ERR_RESULTS_NOROWS,						//����0������
		ERR_CANCEL_RESULTS,						//dbcancel failed
		ERR_NONE_FREE_DBPROCESS,				//û�п��е�DBPROCESS
		ERR_EXISTS_USER,						//���ݿ����Ѿ����ڴ��û�
		ERR_EXISTS_RECORD,						//���ݿ����Ѿ��д˼�¼
		ERR_PASSWORD,							//�������
		ERR_PARAMETER_NULL,						//�������Ϊ��
		ERR_DATA_INITIALIZE,					//��ʼ�����ݴ���
		ERR_DATATIME_TYPE,						//���������ݴ���
		ERR_MAX_OBJID,							//�������鳤�ȳ���������޶�
		
		ERR_DBIMAX 
};

static char *ERR_MSG_C[]={
		"SQL��䳤�ȴ��ڹ涨���ַ�",
		"SQL����﷨����",
		"SQL������д���",
		"SQL���������",
		"�����ݿ�%sʧ��",
		"%s����0������",
		"%s����0������",
		"dbcancel() failed",
		"û�п��е����ӿ��ṩʹ��",
		"���ݿ����Ѿ����ڴ��û�",
		"���ݿ����Ѿ��д˼�¼",
		"�������",
		"�������Ϊ��",
		"��ʼ�����ݴ���",
		"���������ݴ���",
		"�������鳤�ȳ���������޶�"
		
};

static char *ERR_MSG_E[]={
		"SQL length exceeds limit",
		"SQL grammar error",
		"SQL run error",
		"SQL result error",
		"Open database failed",
		"%s 0 column data back",
		"%s 0 row data back",
		"dbcancel() failed",
		"No connection can be used",
		"This user exists in database",
		"This record exists in database",
		"Password error",
		"Parameter is empty",
		"Initialize data error",
		"Datatime type error",
		"OBJID length exceed Max limit"
};

typedef unsigned long ULONG_PTR, *PULONG_PTR;

#pragma pack(push)
#pragma pack(1)
//��ͨ������ض���Ľṹ��
//	��ͨ��ϢID����
const int nSGIP_BIND		= 0x1;		
const int nSGIP_BIND_RESP	= 0x80000001;		
const int nSGIP_UNBIND		= 0x2;		
const int nSGIP_UNBIND_RESP	= 0x80000002;		
const int nSGIP_SUBMIT		= 0x3;		
const int nSGIP_SUBMIT_RESP	= 0x80000003;		
const int nSGIP_DELIVER		= 0x4;		
const int nSGIP_DELIVER_RESP= 0x80000004;
const int nSGIP_REPORT		= 0x5;
const int nSGIP_REPORT_RESP	= 0x80000005;
const int nSGIP_TRACE		= 0x1000;
const int nSGIP_TRACE_RESP	= 0x80001000;
const int nSGIP_WINDOW_SIZE	= 32;   //�������ڴ�С����ͨ���֧��32����
typedef struct sgip_submit
{
	char			sSpNumber[21];				//	sp�Ľ������
	char			sChargeNumber[21];			//	���Ѻ���
	unsigned char	cUserCount;					//	���ն���Ϣ���ֻ�����
	char			sUserNumber[21];			//	���ܸö���Ϣ���ֻ���--sUserNumber[21*100],���100������ʱ�Ƚ�һ��
	char			sCorpId[5];					//	��ҵ���룬0-99999
	char			sServiceType[10];			//	ҵ����룬��sp����
	unsigned __int8	cFeeType;					//	�Ʒ�����
	char			sFeeValue[6];				//	��������Ϣ���շ�ֵ,��λΪ��
	char			sGivenValue[6];				//	�����û��Ļ���,0-99999
	unsigned __int8	cAgentFlag;					//	���շѱ�־,0:Ӧ��;1:ʵ��
	unsigned __int8	cMorelatetoMTFlag;			//	����MT��Ϣ��ԭ��
	unsigned __int8	cPriority;					//	���ȼ�0-9,�ӵ͵���
	char			sExpireTime[16];			//	����Ϣ��������ֹʱ��,"yymmddhhmmsstnnp","tnnp"ȡ�̶�ֵ"032+"
	char			sScheduleTime[16];			//	��ʱ����ʱ��
	unsigned __int8	cReportFlag;				//	״̬�����־,0-������״̬����;1-�ܷ���״̬����;2-��Ҫ״̬����;3...
	unsigned __int8	cTpPid;						//	GSMЭ�����͡���ϸ������ο�GSM03.40�е�9.2.3.9
	unsigned __int8	cUdhi;						//	GSMЭ�����͡���ϸ������ο�GSM03.40�е�9.2.3.23,��ʹ��1λ���Ҷ���
	unsigned __int8	cMessageCoding;				//	���뷽ʽ,0:Ascii;3:Write card;4:binary;8:ucs2;15:GBK
	unsigned __int8	cMessageType;				//	0:����Ϣ��Ϣ
	unsigned int	nMsgLen;					//	����Ϣ����(������sgip_submit_sm_set_messagecontent,���ֶ���ֵ�Ļ�,��Ҫ���ú���htonl()ת��Ϊ�����ֽ���)
	unsigned char	sMsgContent[361];			//	160;					
	char			sLinkId[8];					//	ԭ���ı����ֶΣ�������ΪLinkIdʹ��,ֻ��Ҫ�Ѷ�Ӧ��MO���е��ֶο���������(������ת��).
} SGIP_SUBMIT;

//	��¼����Ϣ��
typedef struct sgip_bind 
{
	unsigned char	cLoginType;					//	1-sp����smg; 2-smg����sp
	char			sLoginName[16];				//	��¼��
	char			sLoginPwd[16];				//	��¼����
	char			sReserved[8];				//	��������չ��
} SGIP_BIND;

//	SGIP1.2Ӧ���(��Ϊ���е�Ӧ����ĸ�ʽ��һ���ģ�������ͳһ���������)
typedef struct sgip_resp 
{
	unsigned char	cResult;					//	0:�ɹ�
	char			sReserved[8];				//	��������չ��
} SGIP_RESP;

//	˳���,Ҳ���������������ʾ.
typedef struct sgip_seqid 
{
	unsigned int	nSrcNodeNum;				//	Դ�ڵ���
	unsigned int	nDateTime;					//	����ʱ��,��:1120203225
	unsigned int	nSeqId;						//	��ˮ��
} SGIP_SEQID;
//	��Ϣͷ
typedef	struct sgip_head 
{
	unsigned int	nTotalLength;				//	��Ϣ����
	unsigned int	nCommandId;					//	ID
	SGIP_SEQID		nSequenceId;				//	˳���
} SGIP_HEAD;

//	sgip_packet����ʱ�õ�����Ϣ��.
typedef struct sgip_packet 
{
	sgip_head pk_head;
	char	  pk_data[PK_DATA_LEN];
} SGIP_PACKET;

//	DELIVER��Ϣ��
typedef struct sgip_deliver 
{
	char			sUserNumber[21];			//	���Ͷ���Ϣ���û��ֻ���
	char			sSpNumber[21];				//	sp�Ľ������
	unsigned char	cTpPid;						//	GSMЭ�����͡���ϸ������ο�GSM03.40�е�9.2.3.9
	unsigned char	cUdhi;						//	GSMЭ�����͡���ϸ������ο�GSM03.40�е�9.2.3.23,��ʹ��1λ���Ҷ���
	unsigned char	cMsgCoding;					//	����Ϣ�ı����ʽ��0����ASCII�ַ���.3��д������.4�������Ʊ���.8��UCS2����.15: GBK����
	unsigned int	nMsgLen;					//	��Ϣ����
	unsigned char	sMsgContent[160];			//	��Ϣ����			
	char			sLinkId[8];					//	ԭ���ı����ֶΣ�������ΪLinkIdʹ��,��ҵ���Ȩ��������.
} SGIP_DELIVER;

//	REPORT��Ϣ��
typedef struct sgip_report 
{
	sgip_seqid		nSubmitSeqNum;				//	���������漰��Submit��Deliver��������к�
	unsigned char	cReportType;				//	Report��������.0,1.
	char			sUserNumber[21];			//	���ն���Ϣ���ֻ���,�ֻ�����ǰ��"86"�����־.
	unsigned char	cState;						//	���������漰�Ķ���Ϣ�ĵ�ǰִ��״̬.0:���ͳɹ�.1:�ȴ�����.2:����ʧ��.
	unsigned char	cErrorCode;					//	��State==2ʱΪ������ֵ,����Ϊ0.
	char			sReserved[8];				//	����,��չ��.
} SGIP_REPORT;

//	sgip_packet	����ʱ�õ�����Ϣ����������Ϣ.
typedef struct sgip_window 
{
	SGIP_PACKET pk;
	int n;
	time_t		time;							//	�´η��͵�ʱ��
} SGIP_WINDOW;

typedef struct  sgip_iniinfo					//   �����ļ�����Ľṹ�壬���ڻ�ȡ�����ļ��е���Ϣ
{
	int nMaxResendTimes;
	int nResendInterval;
	int nSendInterval;
	string servip;
	string servport; 
	string localip;
	string localport; 
	string username;
	string pwd;
	string cpid; 
	string nodeid; 
	string spid;
	string sockettimeout; 
	string submittimeout; 
	string smgconn;
	BYTE logintype;
	string spnumber;
	BYTE feetype;
	string feevalue;
	string givenvalue;
	string bookcmd;
	string countermand;
	BYTE agentflag;
	BYTE MTflag;
	BYTE priority;
	BYTE reportflag;
	BYTE msgcoding;
	BYTE msgtype;
	
	sgip_iniinfo()
	{
		nMaxResendTimes = 0;
        nResendInterval = 0;
		nSendInterval = 0;
		servip = "";
		servport = ""; 
		localip = "";
		localport = ""; 
		username = "";
		pwd = "";
		cpid = ""; 
		nodeid = ""; 
		spid = "";
		sockettimeout = ""; 
		submittimeout = ""; 
		smgconn = "";
		logintype = 0;
		spnumber = "";
		feetype = 0;
		feevalue = "";
		givenvalue = "";
		bookcmd = "";
		countermand = "";
		agentflag = 0;
		MTflag = 0;
		priority = 0;
		reportflag = 0;
		msgcoding = 0;
		msgtype = 0;
	}
}SGIP_INIINFO;
typedef struct  CMPP_iniinfo					//   �����ļ�����Ľṹ�壬���ڻ�ȡ�����ļ��е���Ϣ
{
	string servip;
	string servport; 
	string spid;
	string loginpwd;
	string localip;
	string localport;
	string spnumber;
	BYTE feetype;
	string feevalue;
	string givenvalue;
	BYTE agentflag;
	BYTE MTflag;
	BYTE priority;
	BYTE reportflag;
	BYTE msgcoding;
	BYTE msgtype;

	CMPP_iniinfo()
	{
		servip		= "";
		servport	= ""; 
		spid		= "";
		loginpwd	= "";
		localip	= "";
		localport  = "";
		spnumber = "";
		feetype = 0;
		feevalue = "";
		givenvalue = "";
		agentflag = 0;
		MTflag = 0;
		priority = 0;
		reportflag = 0;
		msgcoding = 0;
		msgtype = 0;
	}
}CMPP_INIINFO;

typedef struct  SMGP_iniinfo					//   �����ļ�����Ľṹ�壬���ڻ�ȡ�����ļ��е���Ϣ
{
	string servip;
	string servport; 
	string spid;
	string loginpwd;
	string localip;
	string localport;
	BYTE logintype;
	string spnumber;
	BYTE feetype;
	string feevalue;
	string givenvalue;
	BYTE agentflag;
	BYTE MTflag;
	BYTE priority;
	BYTE reportflag;
	BYTE msgcoding;
	BYTE msgtype;

	SMGP_iniinfo()
	{
		servip		= "";
		servport	= ""; 
		spid		= "";
		loginpwd	= "";
		localip		= "";
		localport	= "";
		logintype = 0;
		spnumber = "";
		feetype = 0;
		feevalue = "";
		givenvalue = "";
		agentflag = 0;
		MTflag = 0;
		priority = 0;
		reportflag = 0;
		msgcoding = 0;
		msgtype = 0;
	}
}SMGP_INIINFO;
//	��Ϣ�����ȶ���
const int	LEN_SGIP_HEAD    = sizeof(sgip_head);	//	��Ϣͷ�ĳ���
const int	LEN_SGIP_LOGIN	 = sizeof(sgip_bind);	//	��¼��Ϣ����ĳ���
const int	LEN_SGIP_RESP	 = sizeof(sgip_resp);	//	Ӧ�����ĳ���
const int	LEN_SGIP_SUBMIT	 = sizeof(sgip_submit);	//	MT��Ϣ��ĳ���(�����ֻ��û������ж���)
const int	LEN_SGIP_DELIVER = sizeof(sgip_deliver);//	MO��Ϣ��ĳ���(�ֻ��û����������ж���)
const int	LEN_SGIP_REPORT	 = sizeof(sgip_report);	//	Report��Ϣ��ĳ���
//////////////////////////////////end///////////////////////////////////////////////
////////////////////////////////////�ƶ����ؽṹ�嶨��////////////////////////////////////
//��Ϣ���壨����CMPP3.0Э�鶨�壩
const int nCMPP_VERSION          =0x30;				//	CMPP3.0
const int nCMPP_WINDOW_SIZE		 =16;				//	CMPP�������ڵĴ�С���ƶ�������Ϊ16�����յ�Ӧ��ǰ����Ϣ��Ҫ����16����
const int nCMPP_PKG_SIZE		 =400;				//	CMPP���ݰ�����󳤶�

const int nCMPP_CONNECT          =0x00000001;		//	��������
const int nCMPP_CONNECT_RESP     =0x80000001;		//	��������Ӧ��
const int nCMPP_TERMINATE        =0x00000002;		//	��ֹ����
const int nCMPP_TERMINATE_RESP   =0x80000002;		//	��ֹ����Ӧ��
const int nCMPP_SUBMIT           =0x00000004;		//	�ύ����
const int nCMPP_SUBMIT_RESP      =0x80000004;		//	�ύ����Ӧ��
const int nCMPP_DELIVER          =0x00000005;		//	�����·�
const int nCMPP_DELIVER_RESP     =0x80000005;		//	�·�����Ӧ��
const int nCMPP_QUERY            =0x00000006;		//	����״̬��ѯ
const int nCMPP_QUERY_RESP       =0x80000006;		//	����״̬��ѯӦ��
const int nCMPP_CANCEL           =0x00000007;		//	ɾ������
const int nCMPP_CANCEL_RESP      =0x80000007;		//	ɾ������Ӧ��
const int nCMPP_ACTIVE_TEST      =0x00000008;		//	�������
const int nCMPP_ACTIVE_TEST_RESP =0x80000008;		//	�������Ӧ��

const int eCMPP_INIT_OK			 =0;				//	�ӿڳ�ʼ���ɹ�
const int eCMPP_INIT_CONNECT	 =-1;				//	�������ӷ�����
const int eCMPP_INIT_INVAL_ADDR  =2;				//	�Ƿ�Դ��ַ
const int eCMPP_INIT_AUTH		 =3;				//	��֤��
const int eCMPP_INIT_VERSION	 =4;				//	�汾̫��
const int eCMPP_INIT_UNKNOWN	 =5;				//	��������
const int eCMPP_NEED_INIT		 =6;				//	�ӿ���δ��ʼ��

typedef struct cmpp_head
{
	 int	nTotalLength;							//	��Ϣ����
	 int	nCommandId;								//	ID
	 int	nSequenceId;							//	��ˮ��
}CMPP_HEAD;

typedef	struct cmpp_package
{
	CMPP_HEAD	pk_head;							//	���ݱ��İ�ͷ
	char		pk_data[nCMPP_PKG_SIZE];			//	Ҫ���͵�����
	int			n;									//	�ط���������ʼֵ3
	time_t		t;									//	�´η��͵�ʱ��
} CMPP_PACKAGE;

typedef struct cmpp_connect
{
	unsigned char	cspid[6];						//	9��ͷ��6λ��ҵ��
	unsigned char	cdigest[16];					//	MD5����ǩ��
	unsigned char	cver;							//	�汾
	int				timestamp;
} CMPP_CONNECT;
typedef struct cmpp_resp
{
	__int32 status;					        		//	״̬��ʶ
	unsigned char cdigest[16];						//	��������MD5����ǩ��
	unsigned char cver;								//	�������İ汾
} CMPP_RESP;

/******************************************************************************
*	ע�⣺�����Ŀ�ĵ��ֻ�����ֻ����дһ��������desttotal�����1
*       ����Ķ���Ϣ����Ϊ�̶����ȣ����͵�ʱ����Ҫת���ɱ䳤
******************************************************************************/
typedef struct cmpp_submit
{
	__int64 msgid;									//	��Ϣ��ʶ
	unsigned char pkgtotal;							//	��ͬMsg_Id����Ϣ������
	unsigned char pkgnumber;						//	��ͬMsg_Id����Ϣ���
	unsigned char delivery;							//	�Ƿ�Ҫ�󷵻�״̬ȷ�ϱ���
	unsigned char msglevel;							//	��Ϣ����
	unsigned char serviceid[10];					//	ҵ������
	unsigned char feeusertype;						//	�Ʒ��û������ֶ�
	unsigned char feenumber[32];					//	���Ʒ��û��ĺ���
	unsigned char srcttype;							//  ���Ʒ��û��ĺ������ͣ�0����ʵ���룻1��α�롣
	unsigned char tppid;							//	TP-Protocol-Identifier
	unsigned char tpudhi;							//	TP-User-Data-Header-Indicator
	unsigned char msgfmt;							//	��Ϣ��ʽ��8��UCS2����
	unsigned char msgsrc[6];						//	��Ϣ������Դ(SP_Id)
	unsigned char feetype[2];						//	�ʷ����
	unsigned char feecode[6];						//	�ʷѴ��루�Է�Ϊ��λ��
	unsigned char validtime[17];					//	�����Ч�ڣ��μ�SMPP3.3
	unsigned char attime[17];						//	��ʱ����ʱ�䣬�μ�SMPP3.3 
	unsigned char srcnumber[21];					//	Դ����
	unsigned char desttotal;						//	������Ϣ���û�����(===1)
	unsigned char destnumbers[32];					//	���ն��ŵ�MSISDN����
	unsigned char desttype;							//  ���ն��ŵ��û��ĺ������ͣ�0����ʵ���룻1��α�롣
	unsigned char msglen;							//	��Ϣ����(<=140)
	unsigned char msgcontent[160];					//	��Ϣ����
	unsigned char reserve[20];						//	����
} CMPP_SUBMIT;

typedef struct cmpp_submit_resp
{
	__int64 msgid;									//	��Ϣ��ʶ
	__int32 result;	        						//	���
} CMPP_SUBMIT_RESP;

typedef struct cmpp_query
{
	unsigned char date[8];							//	����YYYYMMDD(��ȷ����)
	unsigned char type;								//	��ѯ���0/1
	unsigned char serviceid[10];					//	��ѯ��(Service_Id)
	unsigned char reserve[8];						//	����
} CMPP_QUERY;

typedef struct cmpp_query_resp
{
	unsigned char date[8];							//	����YYYYMMDD(��ȷ����)
	unsigned char type;								//	��ѯ���0/1
	unsigned char serviceid[10];					//	��ѯ��(Service_Id)
	int	mtmsgtotal;									//	��SP������Ϣ����
	int	mtusertotal;								//	��SP�����û�����
	int	mtsuccess;									//	�ɹ�ת������
	int	mtwait;										//	��ת������
	int	mtfail;										//	ת��ʧ������
	int	mosuccess;									//	��SP�ɹ��ʹ�����
	int	mowait;										//	��SP���ʹ�����
	int	mofail;										//	��SP�ʹ�ʧ������
} CMPP_QUERY_RESP;	

typedef struct cmpp_deliver
{
	__int64 msgid;									//	��Ϣ��ʶ
	unsigned char destnumber[21];					//	Ŀ�ĺ���
	unsigned char serviceid[10];					//	ҵ������
	unsigned char tppid;							//	TP-Protocol-Identifier
	unsigned char tpudhi;							//	TP-User-Data-Header-Indicator
	unsigned char msgfmt;							//	��Ϣ��ʽ��8��UCS2����
	unsigned char srcnumber[32];					//	Դ�ն�MSISDN����
    unsigned char srctype;							//  Դ�ն�����
	unsigned char delivery;							//	�Ƿ�Ϊ״̬����
	unsigned char msglen;							//	��Ϣ����
	unsigned char msgcontent[200];					//	��Ϣ����
    unsigned char LinkID[20] ;						//  LinkID
} CMPP_DELIVER;

typedef struct cmpp_deliver_resp
{
	__int64 msgid;									//	��Ϣ��ʶ
	unsigned char result;							//	���
} CMPP_DELIVER_RESP;	

typedef struct cmpp_cancel
{
	__int64 msgid;									//	��Ϣ��ʶ
} CMPP_CANCEL;

typedef struct cmpp_cancel_resp
{
	unsigned char result;							//	�ɹ���ʶ(0/1)
} CMPP_CANCEL_RESP;

typedef struct cmpp_active_test_resp
{
	unsigned char reserve;							//	����
} CMPP_ACTIVE_TEST_RESP;
////////////////////////////////////////�ƶ����ؽṹ�嶨��//////////////////////////////
//����SMGP3.0Э�鶨�壨���밴��Э�鶨��ĳ���������ṹ�壬�����͵İ��ͽ��ܵİ��᳤�Ȳ�һ�£�
typedef struct smgp_head
{
	int	nTotalLength;								//	��Ϣ����
	int	nCommandId;									//	ID
	int	nSequenceId;								//	��ˮ��
}SMGP_HEAD;

typedef struct smgp_connect
{
	unsigned char	cspid[6];						//	��ͷ��λ��ҵ��
	unsigned char	cdigest[16];					//	MD5����ǩ��
	unsigned char   clogintype;                     // ��½����
	unsigned char	cver;							//	�汾
	int				timestamp;                      // ʱ���
}SMGP_CONNECT;

typedef	struct smgp_package
{
	SMGP_HEAD	pk_head;							//	���ݱ��İ�ͷ
	char		pk_data[nCMPP_PKG_SIZE];			//	Ҫ���͵�����
	int			n;									//	�ط���������ʼֵ3
	time_t		t;									//	�´η��͵�ʱ��
} SMGP_PACKAGE;

typedef struct smgp_resp
{
	int				 status;						//	״̬��ʶ
	unsigned char	 cdigest[16];					//	��������MD5����ǩ��
	unsigned char    cver;							//	�������İ汾
} SMGP_RESP;

typedef struct  smgp_submit
{
	
	unsigned char msgid;							//	��Ϣ��ʶ
	unsigned char delivery;							//	�Ƿ�Ҫ�󷵻�״̬ȷ�ϱ���
	unsigned char msglevel;							//	��Ϣ����
	unsigned char serviceid[10];					//	ҵ�����
	unsigned char feetype[2];						//	�շ�����
	unsigned char feecode[6];						//	�ʷѴ���
	unsigned char fixedfee[6];						//	���·�
	unsigned char msgfmt;							//	��Ϣ��ʽ��8��UCS2����
	unsigned char validtime[17];					//	�����Ч�ڣ��μ�SMGP3.3
	unsigned char attime[17];						//	��ʱ����ʱ�䣬�μ�SMGP3.3 
	unsigned char srcnumber[21];					//	Դ����
	unsigned char chargenumber[21];					//	�Ʒ��û�����
	unsigned char desttotal;						//	������Ϣ���û�����
	unsigned char destnumbers[21];					//	���ն��ŵĺ��루21*desttotal��
	unsigned char msglen;							//	��Ϣ����(<=140)
	unsigned char msgcontent[140];					//	��Ϣ����
	unsigned char reserve[8];						//	����
}SMGP_SUBMIT;

typedef struct smgp_submit_resp
{
	unsigned char  msgid[10];						//	��Ϣ��ʶ
	int  result;	        						//	���
} SMGP_SUBMIT_RESP;

typedef struct smgp_deliver
{
	__int64			msgid;							//	��Ϣ��ʶ(��ˮ��)
	unsigned char delivery;							//	�Ƿ�Ϊ״̬����
	unsigned char msgfmt;							//	��Ϣ��ʽ��8��UCS2����
	unsigned char revetime[14];						//  ����ʱ��
	unsigned char srcnumber[21];					//	���ͺ���
	unsigned char destnumber[21];					//	Ŀ�ĺ���
	unsigned char msglen;							//	��Ϣ����
	unsigned char msgcontent[200];					//	��Ϣ����
    unsigned char reserve[8] ;						//  ����
	unsigned char tppid;							//	TP-Protocol-Identifier
	unsigned char tpudhi;							//	TP-User-Data-Header-Indicator
	unsigned char LinkID[20] ;						//  LinkID
} SMGP_DELIVER;


typedef struct tagPendingSendData
{
	int iClientID;  // ��Ҫ���ܸ���Ϣ��ClientID
	int iCommandID;	//��ˮ��
	BYTE byCmdCode;  // ������
	int iLen;	  // ���ʹ��ĳ���
	char* szSendBuf;  // �����͵��ַ���
	tagPendingSendData()
	{
		iClientID = -1;
        iCommandID = -1;
		byCmdCode = -1;
		iLen = -1;
        szSendBuf = NULL;
	}
}PendingSendData;

typedef std::queue<PendingSendData*> PendingSendQueue;

typedef struct tagShortMessage
{
	int iCommandID;  // �ͻ��˷�������ʱ�����ˮ��
    int iClientID;  // ���͸ö���Ϣ��ClientID
    WORD wFeeType;  // �ö���Ϣ�ļƷ������ֶΣ��ƶ��͵���ר�ã���ͨ����д
    BYTE byNetType;  // ��Ӫ�����ͣ�1 ��ͨ��2 �ƶ���3 ����С��ͨ
    BYTE byMsgCode;  // ��ʾ�������룬ͬClient�����������룬������ʾ���ŵ�����
	BYTE byCheckInfo; // ���ڶ������͵ı�־�����ʹ�ÿ��ڶ�����Ҫ
	int iCheckTime;  // ˢ��ʱ�䣬��⿼�ڶ�����Ҫ
	int iRfid; // RFID����
		
    // ������ֶζ�����Ӫ�̵Ķ�������ͨ��Э��涨���ֶΣ��μ�SGIP��Submit�����
    char  szSPNumber[21];
    char  szChargeNumber[32];  // �ƶ�Ϊ32����ͨ�͵���Ϊ21
    BYTE  byUserCount;
    char  byUserNumber[32];
    BYTE byUserNumberType;  // �ƶ����û��������ͣ�0��ʵ���룬1α�롣��ͨ�͵���û�С�
    char  szCorpId[5];
    char  szServiceType[10];
    BYTE  byFeeUserType;
    char  szFeeValue[6];
    char  szGivenValue[6];
    BYTE  byAgentFlag;
    BYTE  byMorelatetoMTFlag;
    BYTE  byPriority;
    char  szExpireTime[16];
    char  szScheduleTime[16];
    BYTE  byReportFlag;
    BYTE  byTP_pid;
    BYTE  byTP_udhi;
    BYTE  byMessageCoding;
    BYTE  byMessageType;
    int	  iMessageLength;
    char  szMessageContent[361];
    char  byReserve[8];  // �ƶ���ʹ�ã���ռ���
    char  byLinkID[20];  // �ƶ�ר�ã��㲥MTʹ��LinkID
    tagShortMessage()
	{
        iCommandID = -1;
        iClientID = -1;
		wFeeType = -1;
		byNetType = -1;
		byMsgCode = -1;
		byCheckInfo = -1;
		iCheckTime = -1;
		iRfid = -1;
		memset(szSPNumber, 0, 21);
		memset(szChargeNumber, 0, 32);
		byUserCount = -1;
		memset(byUserNumber, 0, 32);
		byUserNumberType = -1;
		memset(szCorpId, 0, 5);
		memset(szServiceType, 0, 10);
		byFeeUserType = -1;
		memset(szFeeValue, 0, 6);
		memset(szGivenValue, 0, 6);
		byAgentFlag = -1;
		byMorelatetoMTFlag = -1;
		byPriority = -1;
        memset(szExpireTime, 0, 16);
		memset(szScheduleTime, 0, 16);
		byReportFlag = -1;
		byTP_pid = -1;
		byTP_udhi = -1;
		byMessageCoding = -1;
		byMessageType = -1;
		iMessageLength = -1;
		memset(szMessageContent, 0, 361);
		memset(byReserve, 0, 8);
		memset(byLinkID, 0, 20);
	}
}ShortMessage;

typedef std::queue<ShortMessage*> PendingMsgQueue; 
typedef std::map<int, ShortMessage*> PendingAnswerMsg;
typedef std::map<int, int> MapAssistant;

typedef struct smgp_deliver_resp
{
	__int64 msgid;									//	��Ϣ��ʶ
	unsigned char result;							//	���
} SMGP_DELIVER_RESP;

const int nSMGP_VERSION          =0x30;				//	SMGP3.0
const int nSMGP_WINDOW_SIZE		 =16;				//	SMGP�������ڵĴ�С
const int nSMGP_PKG_SIZE		 =400;				//	SMGP���ݰ�����󳤶�

const int nSMGP_CONNECT          =0x00000001;		//	��������
const int nSMGP_CONNECT_RESP     =0x80000001;		//	��������Ӧ��
const int nSMGP_SUBMIT           =0x00000002;		//	�ύ����
const int nSMGP_SUBMIT_RESP      =0x80000002;		//	�ύ����Ӧ��
const int nSMGP_DELIVER          =0x00000003;		//	�����·�
const int nSMGP_DELIVER_RESP     =0x80000003;		//	�·�����Ӧ��
const int nSMGP_ACTIVE_TEST      =0x00000004;		//	��·���
const int nSMGP_ACTIVE_TEST_RESP =0x80000004;		//	��·���Ӧ��
const int nSMGP_EXIT			 =0x00000006;		//	�˳�
const int nSMGP_EXIT_RESP        =0x80000006;		//	�˳�Ӧ��

const int eSMGP_INIT_OK			 =0;				//	�ӿڳ�ʼ���ɹ�
const int eSMGP_INIT_CONNECT	 =-1;				//	�������ӷ�����
const int eSMGP_INIT_INVAL_ADDR  =2;				//	�Ƿ�Դ��ַ
const int eSMGP_INIT_AUTH		 =3;				//	��֤��
const int eSMGP_INIT_VERSION	 =4;				//	�汾̫��
const int eSMGP_INIT_UNKNOWN	 =5;				//	��������
const int eSMGP_NEED_INIT		 =6;				//	�ӿ���δ��ʼ��
#pragma pack(pop)

#define		CMD_TYPE_REAL	1
#define		CMD_TYPE_COS	2
#define		CMD_TYPE_CHECK  3

#define		MAX_LEN		1024
#define		CMD_REAL_HEAD  0x02
#define		CMD_REAL_TAIL  0x03
#define		CMD_CHECK_HEAD 0x09
#define		CMD_DOT		   0x04

// ������
#define     CMD_LOGIN     1  // ��¼
#define     CMD_GET_USER      2  // ��ȡ�û���Ϣ
#define     CMD_GET_FRIEND     3  // ��ȡ������Ϣ
#define     CMD_SEND_MSG        4  // ������Ϣ��������ת��
#define     CMD_FRIEND_ONLINE        5  // ��������
#define     CMD_GET_GROUP             6//��ȡȺ��Ϣ
#define     CMD_TIME_CHECK              7  // ʱ��У��
#define     CMD_GET_GROUP_USER        8//��ȡȺ�û���Ϣ
#define     CMD_SEND_GROUP_MSG         9//����Ⱥ��Ϣ

//#define     CMD_UPDATE_COMPLETE         6  // �������

#define     CMD_DATA_UPDATE_NOTICE_RESP 8  // ����֪ͨ������Ϣ
#define     CMD_UPDATE_STRATEGY         9  // ���²���
#define     CMD_UPDATE_STUDENT_INFO     10 // ����ѧ����Ϣ
#define     CMD_REQUEST_STUDENT_INFO    11 // ����ѧ����Ϣ
#define     CMD_REQUEST_STRATEGY        12 // ������²���
#define     CMD_UPDATE_CLIENT_VERSION   13 // �����м���汾
#define     CMD_REQUEST_UPLOAD_LOG      14 // �����ϴ���־�ļ�
#define     CMD_REQUEST_UPLOAD_LOG_RESP 15 // �����ϴ���־�ļ�Ӧ��
#define     CMD_UPLOAD_LOG_RESULT       16 // �ϴ���־�ļ��������

// ��ʾ�������͵ĺ�
#define     NET_TYPE_UNION              1  // ��ͨ
#define     NET_TYPE_MOBILE             2  // �ƶ�
#define     NET_TYPE_TELCOM             3  // ����С��ͨ


enum FestivalDay
{
	NewYearsDay = 1,
	SpringFestival,
	NationalDay,
	LaborDay,
	DragonBoatFestival,
	MidautumnFestival
};

#define MSG_TEMPLATE_BIRTHDAY "�����������ӵ����գ���ͨ��У��������ϵ�ף����ף��ѧҵ���������彡��!"

#define MSG_TEMPLATE_NEW_YEAR_DAY "��ͨ��У�����������ף����ף��Ԫ������!�ϼ��Ҹ�!"
#define MSG_TEMPLATE_SPRING_FESTIVAL "��ͨ��У�������������ˣ�Ը���ڵĻ���Ц��ͻ���������Զ����������ߣ��������!"
#define MSG_TEMPLATE_NATIONAL_DAY "����ڵ��ˣ���ͨ��У������ף���������ǹ�ףԸ��Բ��Բ,�Һ͹���!����ڿ���!"
#define MSG_TEMPLATE_LABOR_DAY "�Ͷ��ڵ��ˣ���ͨ��У��ף���Ͷ��ڿ��֣����彡��!�ö��ݵļ��գ�����һ��ƣ�������ġ�"
#define MSG_TEMPLATE_DRAGON_BOAT_FESTIVAL "�Ҷ��죬�Ӷ��ƣ����³����Ƕ�������Ҷ�㣬���������������ӹ����ǡ���ͨ��У��ף���Ҹ��������곤!"
#define MSG_TEMPLATE_MIDAUTUMN_FESTIVAL "��Բ����Բ�����������Ľ��գ���ͨ��У��ף��һ��Բ������!"
#define MSG_TEMPLATE_WELCOME_MSG "���Ѿ��ɹ����ķ���Уͨ����ҵ�񣬻�ӭʹ��!"


typedef struct tag_user_info
{
	unsigned int id;
	string logo;
	string nick_name;
	string description;
	int  type;//����״̬
	tag_user_info()
	{
		id = -1;
		logo = "";
		nick_name="";
		description="";
		type = -1;
	}
}USER_INFO;
typedef struct tag_group_info
{
	unsigned int id;
    string logo;
	string groupname;
	string description;
	tag_group_info()
	{
		id=-1;
		logo="";
		description="";
		groupname="";
	}
	

}GROUP_INFO;
typedef struct tag_loginInfo
{
	unsigned int id;
	int type;
	string ip;
	tag_loginInfo()
	{
		id = -1;
		type = -1;
		ip="";
	}
}LOGININFO;
#endif