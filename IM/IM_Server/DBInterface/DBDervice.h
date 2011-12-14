//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001, Software R&D Section,FiberHome Telecom. Tech. 
// Co.,Ltd
// All rights reserved.
//
// DBDervice.h : 数据库结构体定义文件
//
// Author: ziweic
// Date: 08/04/2008
//
//////////////////////////////////////////////////////////////////////

#ifndef __DBDEVICE_H__
#define __DBDEVICE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

struct DBinfo
{
	string dbservername;  
	string dbname;
	string dbuser;
	string dbpass;
	string dblanguage;
	DBinfo()
	{
	 dbservername	="";  
	 dbname			="";
	 dbuser			="";
	 dbpass			="";
	 dblanguage		="";
	}
};


typedef struct tagCheckStratigy							//策略定义结构体
{
	int    issendnormalleave;
	string type;
	string moenterperiod1;
	string moenterperiod2;
	string moleaveperiod1;
	string moleaveperiod2;
	string amenterperiod1;
	string amenterperiod2;
	string amleaveperiod1;
	string amleaveperiod2;
	string pmenterperiod1;
	string pmenterperiod2;
	string pmleaveperiod1;
	string pmleaveperiod2;
	string eventerperiod1;
	string eventerperiod2;
	string evleaveperiod1;
	string evleaveperiod2;
	string issendlatemsg;
	string molateperiod1;
	string molateperiod2;
	string amlateperiod1;
	string amlateperiod2;
	string pmlateperiod1;
	string pmlateperiod2;
	string evlateperiod1;
	string evlateperiod2;
	string absentstratigy;
	string nomoperiod;
	string noamperiod;
	string nopmperiod;
	string noevperiod;
	tagCheckStratigy()
	{
		 issendnormalleave	= 0;
		 type               ="";
		 moenterperiod1		="";
		 moenterperiod2		="";
		 moleaveperiod1		="";
		 moleaveperiod2		="";
		 amenterperiod1		="";
		 amenterperiod2		="";
		 amleaveperiod1		="";
		 amleaveperiod2		="";
		 pmenterperiod1		="";
		 pmenterperiod2		="";
		 pmleaveperiod1		="";
		 pmleaveperiod2		="";
		 eventerperiod1		="";
		 eventerperiod2		="";
		 evleaveperiod1		="";
		 evleaveperiod2		="";
		 issendlatemsg		="";
		 molateperiod1		="";
		 molateperiod2		="";
		 amlateperiod1		="";
		 amlateperiod2		="";
		 pmlateperiod1		="";
		 pmlateperiod2		="";
		 evlateperiod1		="";
		 evlateperiod2		="";
		 absentstratigy		="";
		 nomoperiod			="";
		 noamperiod			="";
		 nopmperiod			="";
		 noevperiod			="";
	}
}CheckStratigy;

typedef vector<CheckStratigy*> DBCheckStratigyArray;


typedef struct tagBoardingschoolStratigy
{
	string type;
	int	   leavelimit;
	int   isnotify;
	string ontime;
	string offtime;
	tagBoardingschoolStratigy()
	{
		type        ="";
		leavelimit	=0;
		isnotify	=0;
		ontime		="";
		offtime	="";
	}
}BoardingschoolStratigy;

typedef  struct  tagBordingschoolinfo
{
	int			id;
	string		type;
	string		entermsg;
	int			leavelimit;
	int			isnotify;
	string      leavemsg;
	string		ontime;
	string		offtime;
	int			state;
	tagBordingschoolinfo()
	{
		id					= 0;
		type				="";
		entermsg			="";
		leavelimit			=0;
		isnotify			=0;
		leavemsg			="";
		ontime				="";
	    offtime				="";
		state				=0;
	}
}BordingSchoolInfo;
typedef struct tagWeekCheckDaySchool
{
	string    weekcheckday;
	string    summerbegintime;
	string    summerendtime;
	string    winterbegintime;
	string    winterendtime;

}WeekCheckDaySchool;

typedef struct tagRFIDTel
{
	int			classid;  
	int         schoolid;
	string      rfidno;
    string		mobile;
	string      recvid;
	string      recvmsgfrom;
	string      recvmsgto;
	string      username;
	tagRFIDTel()
	{
		classid = 0;     
		schoolid = 0;
        rfidno = "";
        mobile	= "";
		recvid = "";
		recvmsgfrom = "";
        recvmsgto = "";
		username = "";
	}
}RFIDTel;
typedef vector<RFIDTel*> RFIDTelArray;

typedef struct tagSmsInfo
{
	    string Account;	
		string Sendtime;  //发送时间
		string Msisdn;    //等待发送的手机号
		string Content;   //varchar	发送内容
		int Remark;		   //发送完成标记 0未完成 1发送完成
		string Longnum;		//长号码
		string Servicetype; //业务编号
		int Sendnum;
		int Stype;	//审核标记
		int  Sendovertype;		//发送完成标记0未完成 1发送完成
		string Destinetime;	//预计发送
		int Threadtype;		
		string Opercode;	//录入员
		string Note;		//注释
		int  Grouptype;		//是否群组发送
		int  Delmark;		//是否删除标记
		//string Adddate;		//录入时间
		int  Priority;		
		int Sendingtype;		//是否正在发送标记 0没有 1 正在发送
		int Errortype;		//错误标记
		string Errorvalue;		//发送错误的手机号

}SMSInfo;
//////////////////////////test use/////////////////////////////
struct DBMaster
{
	string  name;
	int     number;
	int     type;
	int     low;
	int     high;
	int     status;
};
typedef vector<DBMaster*>  DBMastertArray;
////////////////////test use ///////////////////////////////////////////

#endif