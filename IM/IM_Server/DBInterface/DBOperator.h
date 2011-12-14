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

	//�����������ݿ����Ϣ
	void SetDBInfo(string csDBIP, string csDBUser, string csDBPWD);

	//�������ݿ�
	BOOL OpenDB();

// 	//��ѯ��ǩ����Ϣ
//     int SelectInfoTag(TagVec& vcTag);

	//��ѯ�û���Ϣ,������Եز�ѯ����Ҫ��ID��
	//nType Ϊ1��ʾ��ѯ��ǩʹ���ߣ�Ϊ2��ʾ��ѯ����Ĳλ���
//	int SelectInfoUser(UserVec& vcUser, CString csID, ViewContent vcType);

	//��ѯ������Ϣ
//	int SelectInfoConference(ConfVec& vcConf);
    int	 getSchoolInfo( RFIDTelArray& rfidarray, int schoolid);

	//�޷��ص�ִ�����
	void NoReturnExcute(string csSQL);
private:
	
    void VarientToChar(_variant_t var, string& str);

protected:
	CDBOperator();

private:
    static CDBOperator * m_stInstance;
	string m_csDBServerIP;  //���ݿ�IP
	string m_csDBUser;  //�û���
	string m_csDBPsWd;  //����
	string m_csDBname;
	
    _ConnectionPtr	m_pConnection;
	_RecordsetPtr m_pRecordSet; //��¼��ָ��


};

#endif // !defined(AFX_DBOPERATOR_H__85B85659_A8E0_4A53_946C_AE11FF179E9F__INCLUDED_)
