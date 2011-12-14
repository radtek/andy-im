// DBOperator.h: interface for the CDBOperator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBOPERATOR_H__85B85659_A8E0_4A53_946C_AE11FF179E9F__INCLUDED_)
#define AFX_DBOPERATOR_H__85B85659_A8E0_4A53_946C_AE11FF179E9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include"../DBInterface/DBDervice.h"
#import "c:\Program Files\Common Files\System\ADO\msado15.dll" rename_namespace("ADO") rename("EOF", "adoEOF")
using namespace ADO;
// struct Info_Tag
// {
// 	CString csTagID;
// 	CString csInUsr;
// 	CString csUserID;
// 	CString csRemark;
// };
// typedef std::vector<Info_Tag> TagVec;
// 
// struct Info_User
// {
// 	CString csUserID;
// 	CString csConferenceID;
// 	CString csUserName;
// 	CString csUserUnit;
// 	CString csUserCellPhone;
// 	CString csUserEmail;
// 	CString csUserSignTime;
// 	CString csPhotoPath;
// 	CString csUserTagID;
// 	CString csSignFlag;
// };
// typedef std::vector<Info_User> UserVec;
// 
// struct Info_Conference
// {
// 	CString csConfID;
// 	CString csConfName;
// 	CString csConfPlace;
// 	CString csConfUint;
// 	CString csConfSignTime;
// 	CString csIntendNum;
// };
// typedef std::vector<Info_Conference> ConfVec;

class CDBOperator  
{
public:
	virtual ~CDBOperator();
	static CDBOperator * Instance();

	//设置连接数据库的信息
	void SetDBInfo(string csDBIP, string csDBUser, string csDBPWD);

	//连接数据库
	BOOL OpenDB();

// 	//查询标签表信息
//     int SelectInfoTag(TagVec& vcTag);

	//查询用户信息,有针对性地查询，需要带ID。
	//nType 为1表示查询标签使用者，为2表示查询会议的参会人
//	int SelectInfoUser(UserVec& vcUser, CString csID, ViewContent vcType);

	//查询会议信息
//	int SelectInfoConference(ConfVec& vcConf);
    int	 getSchoolInfo( RFIDTelArray& rfidarray, int schoolid);

	//无返回的执行语句
	void NoReturnExcute(string csSQL);
private:
	
    void VarientToChar(_variant_t var, string& str);

protected:
	CDBOperator();

private:
    static CDBOperator * m_stInstance;
	string m_csDBServerIP;  //数据库IP
	string m_csDBUser;  //用户名
	string m_csDBPsWd;  //密码
	string m_csDBname;
	
    _ConnectionPtr	m_pConnection;
	_RecordsetPtr m_pRecordSet; //记录集指针


};

#endif // !defined(AFX_DBOPERATOR_H__85B85659_A8E0_4A53_946C_AE11FF179E9F__INCLUDED_)
