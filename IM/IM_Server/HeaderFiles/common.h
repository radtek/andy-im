//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// common.h : 公共宏定义文件
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
#define MAP_SIZE            8192*8                // Map中数据的个数
#define STRSQLMAX			8192*2				// SQL max len
#define nMaxBuffer			8192*4
#define BIG_BUFFER          8192*8              // 大缓冲区，存放
#define TABLECOLS			50					// max table columns
#define COLEN				50					// 最大列长度
#define INI_BUFF_SIZE		80  
#define DEFAULT_OVERLAPPED_COUNT 2000			// 默认overlapped结构的数目
#define RFID_BUFFER_SIZE    6                   // 目前的RFID卡号码只处理6位
#define TIME_VERIFY_SPAN    30*60*1000          // 时间校验间隔为30分钟
#define TELEPHONE_LENGTH    20                  // 当前协议中定电话号码长度为20字节
#define ASSISTENT_THREAD_COUNT 5               // 辅助线程的数目
#define PK_DATA_LEN         500                 // 短信数据包数据区长度


#define ERR_DBSYS			-1					//数据库系统错误

enum  DBI_ERROR
{	
		ERR_DBIFIRST=0,
		ERR_MAX_SQLSTR,							//SQL语句长度大于%d个字符
		ERR_SQL_CMD,							//SQL语句语法错误
		ERR_SQL_EXEC,							//SQL语句运行错误
		ERR_SQL_RESULTS,						//SQL语句结果错误
		ERR_BIND_DATABASE_FAIL,					//打开数据库失败
		ERR_RESULTS_NOCOLS,						//返回0列数据
		ERR_RESULTS_NOROWS,						//返回0行数据
		ERR_CANCEL_RESULTS,						//dbcancel failed
		ERR_NONE_FREE_DBPROCESS,				//没有空闲的DBPROCESS
		ERR_EXISTS_USER,						//数据库中已经存在此用户
		ERR_EXISTS_RECORD,						//数据库中已经有此记录
		ERR_PASSWORD,							//密码错误
		ERR_PARAMETER_NULL,						//输入参数为空
		ERR_DATA_INITIALIZE,					//初始化数据错误
		ERR_DATATIME_TYPE,						//日期型数据错误
		ERR_MAX_OBJID,							//对象数组长度超过了最大限定
		
		ERR_DBIMAX 
};

static char *ERR_MSG_C[]={
		"SQL语句长度大于规定个字符",
		"SQL语句语法错误",
		"SQL语句运行错误",
		"SQL语句结果错误",
		"打开数据库%s失败",
		"%s返回0列数据",
		"%s返回0行数据",
		"dbcancel() failed",
		"没有空闲的连接可提供使用",
		"数据库中已经存在此用户",
		"数据库中已经有此记录",
		"密码错误",
		"输入参数为空",
		"初始化数据错误",
		"日期型数据错误",
		"对象数组长度超过了最大限定"
		
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
//联通网关相关定义的结构体
//	联通消息ID定义
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
const int nSGIP_WINDOW_SIZE	= 32;   //滑动窗口大小（联通最多支持32个）
typedef struct sgip_submit
{
	char			sSpNumber[21];				//	sp的接入号码
	char			sChargeNumber[21];			//	付费号码
	unsigned char	cUserCount;					//	接收短消息的手机数量
	char			sUserNumber[21];			//	接受该短消息的手机号--sUserNumber[21*100],最多100个，暂时先接一个
	char			sCorpId[5];					//	企业代码，0-99999
	char			sServiceType[10];			//	业务代码，由sp定义
	unsigned __int8	cFeeType;					//	计费类型
	char			sFeeValue[6];				//	该条短消息的收费值,单位为分
	char			sGivenValue[6];				//	赠送用户的话费,0-99999
	unsigned __int8	cAgentFlag;					//	代收费标志,0:应收;1:实收
	unsigned __int8	cMorelatetoMTFlag;			//	引起MT消息的原因
	unsigned __int8	cPriority;					//	优先级0-9,从低到高
	char			sExpireTime[16];			//	短消息寿命的终止时间,"yymmddhhmmsstnnp","tnnp"取固定值"032+"
	char			sScheduleTime[16];			//	定时发送时间
	unsigned __int8	cReportFlag;				//	状态报告标志,0-出错返回状态报告;1-总返回状态报告;2-不要状态报告;3...
	unsigned __int8	cTpPid;						//	GSM协议类型。详细解释请参考GSM03.40中的9.2.3.9
	unsigned __int8	cUdhi;						//	GSM协议类型。详细解释请参考GSM03.40中的9.2.3.23,仅使用1位，右对齐
	unsigned __int8	cMessageCoding;				//	编码方式,0:Ascii;3:Write card;4:binary;8:ucs2;15:GBK
	unsigned __int8	cMessageType;				//	0:短消息信息
	unsigned int	nMsgLen;					//	短消息长度(不调用sgip_submit_sm_set_messagecontent,而手动赋值的话,需要调用函数htonl()转换为网络字节序)
	unsigned char	sMsgContent[361];			//	160;					
	char			sLinkId[8];					//	原来的保留字段，现在作为LinkId使用,只需要把对应的MO包中的字段拷过来即可(不用做转换).
} SGIP_SUBMIT;

//	登录包消息体
typedef struct sgip_bind 
{
	unsigned char	cLoginType;					//	1-sp连接smg; 2-smg连接sp
	char			sLoginName[16];				//	登录名
	char			sLoginPwd[16];				//	登录密码
	char			sReserved[8];				//	保留，扩展用
} SGIP_BIND;

//	SGIP1.2应答包(因为所有的应答包的格式是一样的，所以用统一共用这个包)
typedef struct sgip_resp 
{
	unsigned char	cResult;					//	0:成功
	char			sReserved[8];				//	保留，扩展用
} SGIP_RESP;

//	顺序号,也可以用整数数组表示.
typedef struct sgip_seqid 
{
	unsigned int	nSrcNodeNum;				//	源节点编号
	unsigned int	nDateTime;					//	日期时间,如:1120203225
	unsigned int	nSeqId;						//	流水号
} SGIP_SEQID;
//	消息头
typedef	struct sgip_head 
{
	unsigned int	nTotalLength;				//	消息长度
	unsigned int	nCommandId;					//	ID
	SGIP_SEQID		nSequenceId;				//	顺序号
} SGIP_HEAD;

//	sgip_packet接收时用到的消息包.
typedef struct sgip_packet 
{
	sgip_head pk_head;
	char	  pk_data[PK_DATA_LEN];
} SGIP_PACKET;

//	DELIVER消息体
typedef struct sgip_deliver 
{
	char			sUserNumber[21];			//	发送短消息的用户手机号
	char			sSpNumber[21];				//	sp的接入号码
	unsigned char	cTpPid;						//	GSM协议类型。详细解释请参考GSM03.40中的9.2.3.9
	unsigned char	cUdhi;						//	GSM协议类型。详细解释请参考GSM03.40中的9.2.3.23,仅使用1位，右对齐
	unsigned char	cMsgCoding;					//	短消息的编码格式。0：纯ASCII字符串.3：写卡操作.4：二进制编码.8：UCS2编码.15: GBK编码
	unsigned int	nMsgLen;					//	消息长度
	unsigned char	sMsgContent[160];			//	消息内容			
	char			sLinkId[8];					//	原来的保留字段，现在作为LinkId使用,由业务鉴权中心生成.
} SGIP_DELIVER;

//	REPORT消息体
typedef struct sgip_report 
{
	sgip_seqid		nSubmitSeqNum;				//	该命令所涉及的Submit或Deliver命令的序列号
	unsigned char	cReportType;				//	Report命令类型.0,1.
	char			sUserNumber[21];			//	接收短消息的手机号,手机号码前加"86"国别标志.
	unsigned char	cState;						//	该命令所涉及的短消息的当前执行状态.0:发送成功.1:等待发送.2:发送失败.
	unsigned char	cErrorCode;					//	当State==2时为错误码值,否则为0.
	char			sReserved[8];				//	保留,扩展用.
} SGIP_REPORT;

//	sgip_packet	发送时用到的消息包和其他信息.
typedef struct sgip_window 
{
	SGIP_PACKET pk;
	int n;
	time_t		time;							//	下次发送的时间
} SGIP_WINDOW;

typedef struct  sgip_iniinfo					//   配置文件定义的结构体，用于获取配置文件中的信息
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
typedef struct  CMPP_iniinfo					//   配置文件定义的结构体，用于获取配置文件中的信息
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

typedef struct  SMGP_iniinfo					//   配置文件定义的结构体，用于获取配置文件中的信息
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
//	消息包长度定义
const int	LEN_SGIP_HEAD    = sizeof(sgip_head);	//	消息头的长度
const int	LEN_SGIP_LOGIN	 = sizeof(sgip_bind);	//	登录消息包体的长度
const int	LEN_SGIP_RESP	 = sizeof(sgip_resp);	//	应答包体的长度
const int	LEN_SGIP_SUBMIT	 = sizeof(sgip_submit);	//	MT消息体的长度(发往手机用户的下行短信)
const int	LEN_SGIP_DELIVER = sizeof(sgip_deliver);//	MO消息体的长度(手机用户发出的上行短信)
const int	LEN_SGIP_REPORT	 = sizeof(sgip_report);	//	Report消息体的长度
//////////////////////////////////end///////////////////////////////////////////////
////////////////////////////////////移动网关结构体定义////////////////////////////////////
//消息定义（按照CMPP3.0协议定义）
const int nCMPP_VERSION          =0x30;				//	CMPP3.0
const int nCMPP_WINDOW_SIZE		 =16;				//	CMPP滑动窗口的大小（移动建议设为16，即收到应答前的消息不要超过16条）
const int nCMPP_PKG_SIZE		 =400;				//	CMPP数据包的最大长度

const int nCMPP_CONNECT          =0x00000001;		//	请求连接
const int nCMPP_CONNECT_RESP     =0x80000001;		//	请求连接应答
const int nCMPP_TERMINATE        =0x00000002;		//	终止连接
const int nCMPP_TERMINATE_RESP   =0x80000002;		//	终止连接应答
const int nCMPP_SUBMIT           =0x00000004;		//	提交短信
const int nCMPP_SUBMIT_RESP      =0x80000004;		//	提交短信应答
const int nCMPP_DELIVER          =0x00000005;		//	短信下发
const int nCMPP_DELIVER_RESP     =0x80000005;		//	下发短信应答
const int nCMPP_QUERY            =0x00000006;		//	短信状态查询
const int nCMPP_QUERY_RESP       =0x80000006;		//	短信状态查询应答
const int nCMPP_CANCEL           =0x00000007;		//	删除短信
const int nCMPP_CANCEL_RESP      =0x80000007;		//	删除短信应答
const int nCMPP_ACTIVE_TEST      =0x00000008;		//	激活测试
const int nCMPP_ACTIVE_TEST_RESP =0x80000008;		//	激活测试应答

const int eCMPP_INIT_OK			 =0;				//	接口初始化成功
const int eCMPP_INIT_CONNECT	 =-1;				//	不能连接服务器
const int eCMPP_INIT_INVAL_ADDR  =2;				//	非法源地址
const int eCMPP_INIT_AUTH		 =3;				//	认证错
const int eCMPP_INIT_VERSION	 =4;				//	版本太高
const int eCMPP_INIT_UNKNOWN	 =5;				//	其他错误
const int eCMPP_NEED_INIT		 =6;				//	接口尚未初始化

typedef struct cmpp_head
{
	 int	nTotalLength;							//	消息长度
	 int	nCommandId;								//	ID
	 int	nSequenceId;							//	流水号
}CMPP_HEAD;

typedef	struct cmpp_package
{
	CMPP_HEAD	pk_head;							//	数据报的包头
	char		pk_data[nCMPP_PKG_SIZE];			//	要发送的数据
	int			n;									//	重发次数，初始值3
	time_t		t;									//	下次发送的时间
} CMPP_PACKAGE;

typedef struct cmpp_connect
{
	unsigned char	cspid[6];						//	9开头的6位企业码
	unsigned char	cdigest[16];					//	MD5数字签名
	unsigned char	cver;							//	版本
	int				timestamp;
} CMPP_CONNECT;
typedef struct cmpp_resp
{
	__int32 status;					        		//	状态标识
	unsigned char cdigest[16];						//	服务器的MD5数字签名
	unsigned char cver;								//	服务器的版本
} CMPP_RESP;

/******************************************************************************
*	注意：这里的目的地手机号码只能填写一个，所以desttotal恒等于1
*       这里的短消息内容为固定长度，发送的时候需要转换成变长
******************************************************************************/
typedef struct cmpp_submit
{
	__int64 msgid;									//	信息标识
	unsigned char pkgtotal;							//	相同Msg_Id的信息总条数
	unsigned char pkgnumber;						//	相同Msg_Id的信息序号
	unsigned char delivery;							//	是否要求返回状态确认报告
	unsigned char msglevel;							//	信息级别
	unsigned char serviceid[10];					//	业务类型
	unsigned char feeusertype;						//	计费用户类型字段
	unsigned char feenumber[32];					//	被计费用户的号码
	unsigned char srcttype;							//  被计费用户的号码类型，0：真实号码；1：伪码。
	unsigned char tppid;							//	TP-Protocol-Identifier
	unsigned char tpudhi;							//	TP-User-Data-Header-Indicator
	unsigned char msgfmt;							//	信息格式，8：UCS2编码
	unsigned char msgsrc[6];						//	信息内容来源(SP_Id)
	unsigned char feetype[2];						//	资费类别
	unsigned char feecode[6];						//	资费代码（以分为单位）
	unsigned char validtime[17];					//	存活有效期，参见SMPP3.3
	unsigned char attime[17];						//	定时发送时间，参见SMPP3.3 
	unsigned char srcnumber[21];					//	源号码
	unsigned char desttotal;						//	接收信息的用户数量(===1)
	unsigned char destnumbers[32];					//	接收短信的MSISDN号码
	unsigned char desttype;							//  接收短信的用户的号码类型，0：真实号码；1：伪码。
	unsigned char msglen;							//	信息长度(<=140)
	unsigned char msgcontent[160];					//	信息内容
	unsigned char reserve[20];						//	保留
} CMPP_SUBMIT;

typedef struct cmpp_submit_resp
{
	__int64 msgid;									//	信息标识
	__int32 result;	        						//	结果
} CMPP_SUBMIT_RESP;

typedef struct cmpp_query
{
	unsigned char date[8];							//	日期YYYYMMDD(精确至日)
	unsigned char type;								//	查询类别0/1
	unsigned char serviceid[10];					//	查询码(Service_Id)
	unsigned char reserve[8];						//	保留
} CMPP_QUERY;

typedef struct cmpp_query_resp
{
	unsigned char date[8];							//	日期YYYYMMDD(精确至日)
	unsigned char type;								//	查询类别0/1
	unsigned char serviceid[10];					//	查询码(Service_Id)
	int	mtmsgtotal;									//	从SP接收信息总数
	int	mtusertotal;								//	从SP接收用户总数
	int	mtsuccess;									//	成功转发数量
	int	mtwait;										//	待转发数量
	int	mtfail;										//	转发失败数量
	int	mosuccess;									//	向SP成功送达数量
	int	mowait;										//	向SP待送达数量
	int	mofail;										//	向SP送达失败数量
} CMPP_QUERY_RESP;	

typedef struct cmpp_deliver
{
	__int64 msgid;									//	信息标识
	unsigned char destnumber[21];					//	目的号码
	unsigned char serviceid[10];					//	业务类型
	unsigned char tppid;							//	TP-Protocol-Identifier
	unsigned char tpudhi;							//	TP-User-Data-Header-Indicator
	unsigned char msgfmt;							//	信息格式，8：UCS2编码
	unsigned char srcnumber[32];					//	源终端MSISDN号码
    unsigned char srctype;							//  源终端类型
	unsigned char delivery;							//	是否为状态报告
	unsigned char msglen;							//	消息长度
	unsigned char msgcontent[200];					//	消息内容
    unsigned char LinkID[20] ;						//  LinkID
} CMPP_DELIVER;

typedef struct cmpp_deliver_resp
{
	__int64 msgid;									//	信息标识
	unsigned char result;							//	结果
} CMPP_DELIVER_RESP;	

typedef struct cmpp_cancel
{
	__int64 msgid;									//	信息标识
} CMPP_CANCEL;

typedef struct cmpp_cancel_resp
{
	unsigned char result;							//	成功标识(0/1)
} CMPP_CANCEL_RESP;

typedef struct cmpp_active_test_resp
{
	unsigned char reserve;							//	保留
} CMPP_ACTIVE_TEST_RESP;
////////////////////////////////////////移动网关结构体定义//////////////////////////////
//按照SMGP3.0协议定义（必须按照协议定义的长度来定义结构体，守则发送的包和接受的包会长度不一致）
typedef struct smgp_head
{
	int	nTotalLength;								//	消息长度
	int	nCommandId;									//	ID
	int	nSequenceId;								//	流水号
}SMGP_HEAD;

typedef struct smgp_connect
{
	unsigned char	cspid[6];						//	开头的位企业码
	unsigned char	cdigest[16];					//	MD5数字签名
	unsigned char   clogintype;                     // 登陆类型
	unsigned char	cver;							//	版本
	int				timestamp;                      // 时间戳
}SMGP_CONNECT;

typedef	struct smgp_package
{
	SMGP_HEAD	pk_head;							//	数据报的包头
	char		pk_data[nCMPP_PKG_SIZE];			//	要发送的数据
	int			n;									//	重发次数，初始值3
	time_t		t;									//	下次发送的时间
} SMGP_PACKAGE;

typedef struct smgp_resp
{
	int				 status;						//	状态标识
	unsigned char	 cdigest[16];					//	服务器的MD5数字签名
	unsigned char    cver;							//	服务器的版本
} SMGP_RESP;

typedef struct  smgp_submit
{
	
	unsigned char msgid;							//	信息标识
	unsigned char delivery;							//	是否要求返回状态确认报告
	unsigned char msglevel;							//	信息级别
	unsigned char serviceid[10];					//	业务代码
	unsigned char feetype[2];						//	收费类型
	unsigned char feecode[6];						//	资费代码
	unsigned char fixedfee[6];						//	包月费
	unsigned char msgfmt;							//	信息格式，8：UCS2编码
	unsigned char validtime[17];					//	存活有效期，参见SMGP3.3
	unsigned char attime[17];						//	定时发送时间，参见SMGP3.3 
	unsigned char srcnumber[21];					//	源号码
	unsigned char chargenumber[21];					//	计费用户号码
	unsigned char desttotal;						//	接收信息的用户数量
	unsigned char destnumbers[21];					//	接收短信的号码（21*desttotal）
	unsigned char msglen;							//	信息长度(<=140)
	unsigned char msgcontent[140];					//	信息内容
	unsigned char reserve[8];						//	保留
}SMGP_SUBMIT;

typedef struct smgp_submit_resp
{
	unsigned char  msgid[10];						//	信息标识
	int  result;	        						//	结果
} SMGP_SUBMIT_RESP;

typedef struct smgp_deliver
{
	__int64			msgid;							//	信息标识(流水号)
	unsigned char delivery;							//	是否为状态报告
	unsigned char msgfmt;							//	信息格式，8：UCS2编码
	unsigned char revetime[14];						//  接收时间
	unsigned char srcnumber[21];					//	发送号码
	unsigned char destnumber[21];					//	目的号码
	unsigned char msglen;							//	消息长度
	unsigned char msgcontent[200];					//	消息内容
    unsigned char reserve[8] ;						//  保留
	unsigned char tppid;							//	TP-Protocol-Identifier
	unsigned char tpudhi;							//	TP-User-Data-Header-Indicator
	unsigned char LinkID[20] ;						//  LinkID
} SMGP_DELIVER;


typedef struct tagPendingSendData
{
	int iClientID;  // 需要接受该消息的ClientID
	int iCommandID;	//流水号
	BYTE byCmdCode;  // 命令字
	int iLen;	  // 发送串的长度
	char* szSendBuf;  // 待发送的字符串
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
	int iCommandID;  // 客户端发来命令时候的流水号
    int iClientID;  // 发送该短消息的ClientID
    WORD wFeeType;  // 该短消息的计费类型字段，移动和电信专用，联通不填写
    BYTE byNetType;  // 运营商类型，1 联通、2 移动、3 电信小灵通
    BYTE byMsgCode;  // 标示该命令码，同Client发来的命令码，用来表示短信的作用
	BYTE byCheckInfo; // 考勤短信类型的标志，入库使用考勤短信需要
	int iCheckTime;  // 刷卡时间，入库考勤短信需要
	int iRfid; // RFID卡号
		
    // 下面的字段都是运营商的短信网关通信协议规定的字段，参见SGIP的Submit命令部分
    char  szSPNumber[21];
    char  szChargeNumber[32];  // 移动为32，联通和电信为21
    BYTE  byUserCount;
    char  byUserNumber[32];
    BYTE byUserNumberType;  // 移动的用户号码类型，0真实号码，1伪码。联通和电信没有。
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
    char  byReserve[8];  // 移动不使用，填空即可
    char  byLinkID[20];  // 移动专用，点播MT使用LinkID
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
	__int64 msgid;									//	信息标识
	unsigned char result;							//	结果
} SMGP_DELIVER_RESP;

const int nSMGP_VERSION          =0x30;				//	SMGP3.0
const int nSMGP_WINDOW_SIZE		 =16;				//	SMGP滑动窗口的大小
const int nSMGP_PKG_SIZE		 =400;				//	SMGP数据包的最大长度

const int nSMGP_CONNECT          =0x00000001;		//	请求连接
const int nSMGP_CONNECT_RESP     =0x80000001;		//	请求连接应答
const int nSMGP_SUBMIT           =0x00000002;		//	提交短信
const int nSMGP_SUBMIT_RESP      =0x80000002;		//	提交短信应答
const int nSMGP_DELIVER          =0x00000003;		//	短信下发
const int nSMGP_DELIVER_RESP     =0x80000003;		//	下发短信应答
const int nSMGP_ACTIVE_TEST      =0x00000004;		//	链路检查
const int nSMGP_ACTIVE_TEST_RESP =0x80000004;		//	链路检查应答
const int nSMGP_EXIT			 =0x00000006;		//	退出
const int nSMGP_EXIT_RESP        =0x80000006;		//	退出应答

const int eSMGP_INIT_OK			 =0;				//	接口初始化成功
const int eSMGP_INIT_CONNECT	 =-1;				//	不能连接服务器
const int eSMGP_INIT_INVAL_ADDR  =2;				//	非法源地址
const int eSMGP_INIT_AUTH		 =3;				//	认证错
const int eSMGP_INIT_VERSION	 =4;				//	版本太高
const int eSMGP_INIT_UNKNOWN	 =5;				//	其他错误
const int eSMGP_NEED_INIT		 =6;				//	接口尚未初始化
#pragma pack(pop)

#define		CMD_TYPE_REAL	1
#define		CMD_TYPE_COS	2
#define		CMD_TYPE_CHECK  3

#define		MAX_LEN		1024
#define		CMD_REAL_HEAD  0x02
#define		CMD_REAL_TAIL  0x03
#define		CMD_CHECK_HEAD 0x09
#define		CMD_DOT		   0x04

// 命令码
#define     CMD_LOGIN     1  // 登录
#define     CMD_GET_USER      2  // 获取用户信息
#define     CMD_GET_FRIEND     3  // 获取好友信息
#define     CMD_SEND_MSG        4  // 发送消息，服务器转发
#define     CMD_FRIEND_ONLINE        5  // 还有上线
#define     CMD_GET_GROUP             6//获取群信息
#define     CMD_TIME_CHECK              7  // 时间校验
#define     CMD_GET_GROUP_USER        8//获取群用户信息
#define     CMD_SEND_GROUP_MSG         9//发送群消息

//#define     CMD_UPDATE_COMPLETE         6  // 更新完毕

#define     CMD_DATA_UPDATE_NOTICE_RESP 8  // 更新通知返回消息
#define     CMD_UPDATE_STRATEGY         9  // 更新策略
#define     CMD_UPDATE_STUDENT_INFO     10 // 更新学生信息
#define     CMD_REQUEST_STUDENT_INFO    11 // 请求学生信息
#define     CMD_REQUEST_STRATEGY        12 // 请求更新策略
#define     CMD_UPDATE_CLIENT_VERSION   13 // 更新中间件版本
#define     CMD_REQUEST_UPLOAD_LOG      14 // 请求上传日志文件
#define     CMD_REQUEST_UPLOAD_LOG_RESP 15 // 请求上传日志文件应答
#define     CMD_UPLOAD_LOG_RESULT       16 // 上传日志文件操作完成

// 标示网络类型的宏
#define     NET_TYPE_UNION              1  // 联通
#define     NET_TYPE_MOBILE             2  // 移动
#define     NET_TYPE_TELCOM             3  // 电信小灵通


enum FestivalDay
{
	NewYearsDay = 1,
	SpringFestival,
	NationalDay,
	LaborDay,
	DragonBoatFestival,
	MidautumnFestival
};

#define MSG_TEMPLATE_BIRTHDAY "今天是您孩子的生日，联通家校网献上真诚的祝福，祝其学业进步、身体健康!"

#define MSG_TEMPLATE_NEW_YEAR_DAY "联通家校网献上新年的祝福，祝您元旦快乐!合家幸福!"
#define MSG_TEMPLATE_SPRING_FESTIVAL "联通家校网给您拜新年了，愿春节的欢声笑语和欢乐气氛永远萦绕在您身边，新年快乐!"
#define MSG_TEMPLATE_NATIONAL_DAY "国庆节到了，联通家校网献上祝福，让我们共祝愿国圆家圆,家和国兴!国庆节快乐!"
#define MSG_TEMPLATE_LABOR_DAY "劳动节到了，联通家校网祝您劳动节快乐，身体健康!用短暂的假日，放松一下疲惫的身心。"
#define MSG_TEMPLATE_DRAGON_BOAT_FESTIVAL "桃儿红，杏儿黄，五月初五是端阳；粽叶香，包五粮，剥个粽子裹上糖。联通家校网祝您幸福生活万年长!"
#define MSG_TEMPLATE_MIDAUTUMN_FESTIVAL "月圆人团圆，在这个想念的节日，联通家校网祝你一切圆满美好!"
#define MSG_TEMPLATE_WELCOME_MSG "您已经成功订阅烽火家校通短信业务，欢迎使用!"


typedef struct tag_user_info
{
	unsigned int id;
	string logo;
	string nick_name;
	string description;
	int  type;//在线状态
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