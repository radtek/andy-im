// DBOperator.cpp: implementation of the CDBOperator class.
//
//////////////////////////////////////////////////////////////////////

#include "../HeaderFiles/StdAfx.h"

#include "DBOperator.h"


CDBOperator * CDBOperator::m_stInstance = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDBOperator::CDBOperator()
{
    m_csDBServerIP = "10.10.10.81";
	m_csDBUser = "sa";
	m_csDBPsWd = "123";
	m_csDBname ="jxt";
	OpenDB();
}

CDBOperator::~CDBOperator()
{
	if (adStateClosed != m_pConnection->GetState())
	{
		m_pConnection->Close();
		m_pConnection->Release();
	}
	m_pConnection = NULL;

    ::CoUninitialize();

    if (NULL != m_stInstance)
    {
		delete m_stInstance;
		m_stInstance = NULL;
    }
}

CDBOperator * CDBOperator::Instance()
{
	if (NULL == m_stInstance) 
	{
        m_stInstance = new CDBOperator;
	}

	return m_stInstance;
}
void CDBOperator::SetDBInfo(string csDBIP, string csDBUser, string csDBPWD)
{
	m_csDBServerIP = csDBIP;
	m_csDBUser = csDBUser;
	m_csDBPsWd = csDBPWD;
}

BOOL CDBOperator::OpenDB()
{

	HRESULT comhr = ::CoInitialize(NULL);
	if (FAILED(comhr))
		
	{
		return -1;
	}
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	
	if (FAILED(hr))
	{
		return -1;
	}
	try
	{
		string  strSRC;
		char cSRC[100]; 
		memset( cSRC,0,100);
		sprintf(cSRC,"Driver=SQL Server;Server=%s;DATABASE=%s",m_csDBServerIP.c_str(),m_csDBname.c_str());
		strSRC = cSRC;
		m_pConnection->Open(_bstr_t(strSRC.c_str()), _bstr_t(m_csDBUser.c_str()), _bstr_t(m_csDBPsWd.c_str()), adModeUnknown);
	}
	catch (_com_error e)
	{
		//AfxMessageBox("数据库连接失败，请确认数据库配置是否正确");
		return FALSE;
	}
	
	return TRUE;
}

int CDBOperator::getSchoolInfo(RFIDTelArray& rfidarray, int schoolid)
{


	//_variant_t RecordsAffected;
	
	char cSQL[100];
	sprintf(cSQL,"SELECT rfidno,classid,phone,recvid FROM v_rf_phone WHERE schoolid = %d", schoolid);

	_CommandPtr cmd;
	HRESULT hr = cmd.CreateInstance(__uuidof(Command));
	if (FAILED(hr)) 
		return -1;
	cmd->ActiveConnection = m_pConnection;
	cmd->CommandText =_bstr_t(cSQL) ;
	cmd->CommandType = adCmdText;
    cmd->CommandTimeout = 1;
	_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdUnknown);

	//m_pRecordSet = m_pConnection->Execute(_bstr_t(cSQL), &RecordsAffected, adCmdText);
	while (!rs->adoEOF)
	{
		_variant_t rfidno;
		_variant_t classid;
		_variant_t phone;
		_variant_t recvid;
		rfidno = rs->GetCollect("rfidno");
		classid = rs->GetCollect("classid");
		phone = rs->GetCollect("phone");
		recvid = rs->GetCollect("recvid");
        string tempclassid;
		tagRFIDTel * pRfidTel = new tagRFIDTel;
 		VarientToChar(rfidno, pRfidTel->rfidno);
		VarientToChar(classid,tempclassid );
	  	pRfidTel->classid = atoi(tempclassid.c_str());
		VarientToChar(phone, pRfidTel->mobile);
		VarientToChar(recvid, pRfidTel->recvid);
		rfidarray.push_back(pRfidTel);
	}

	return -1;
}

// int CDBOperator::SelectInfoTag(TagVec& vcTag)
// {
// 	_variant_t RecordsAffected;
//     m_pRecordSet = m_pConnection->Execute(_bstr_t("SELECT * FROM Info_Tag"), &RecordsAffected, adCmdText);
// 
// 	while (!m_pRecordSet->adoEOF)
// 	{
// 		_variant_t varTag;
// 		_variant_t varInUse;
// 		_variant_t varUserID;
// 		_variant_t varRemark;
// 
// 		varTag = m_pRecordSet->GetCollect("tag_id");
// 		varInUse = m_pRecordSet->GetCollect("tag_been_used");
// 		varUserID = m_pRecordSet->GetCollect("tag_user_id");
// 		varRemark = m_pRecordSet->GetCollect("tag_remark");
// 
// 		Info_Tag oTag;
// 
// 		VarientToChar(varTag, oTag.csTagID);
// 		VarientToChar(varInUse, oTag.csInUsr);
// 		VarientToChar(varUserID, oTag.csUserID);
// 		VarientToChar(varRemark, oTag.csRemark);
// 
// 		vcTag.push_back(oTag);
// 		m_pRecordSet->MoveNext();		
// 	}
// 
// 	return vcTag.size();
// }

// int CDBOperator::SelectInfoUser(UserVec& vcUser, CString csID, ViewContent vcType)
// {
// 	_variant_t RecordsAffected;
// 
// 	CString csSelect;
// 	csSelect.Format("SELECT * FROM Info_User WHERE ");
// 
// 	switch(vcType)
// 	{
// 	case ViewTag:
// 		{
// 			csSelect += "user_tag_id = '";
// 			break;
// 		}
// 	case ViewConference:
// 		{
// 			csSelect += "user_conference_id = '";
// 			break;
// 		}
// 	case ViewEmpty:
// 		{
// 			csSelect += "user_name = '";
// 			break;
// 		}
// 	default:
// 		break;
// 	}
// 	csSelect = csSelect + csID + "'";
// 
// 	TRACE0(csSelect);
//     m_pRecordSet = m_pConnection->Execute(_bstr_t(csSelect), &RecordsAffected, adCmdText);
// 
// 	while (!m_pRecordSet->adoEOF)
// 	{
// 		_variant_t varUserID;
// 		_variant_t varConfID;
// 		_variant_t varUserName;
// 		_variant_t varUserUnit;
// 		_variant_t varUserCellPhone;
// 		_variant_t varUserEmail;
// 		_variant_t varUserSignTime;
// 		_variant_t varPhotoPath;
//         _variant_t varUserTagID;
// 		_variant_t varSignFlag;
// 
// 		varUserID = m_pRecordSet->GetCollect("user_id");
// 		varConfID = m_pRecordSet->GetCollect("user_conference_id");
// 		varUserName = m_pRecordSet->GetCollect("user_name");
// 		varUserUnit = m_pRecordSet->GetCollect("user_unit");
// 		varUserCellPhone = m_pRecordSet->GetCollect("user_cellphone_number");
// 		varUserEmail = m_pRecordSet->GetCollect("user_email_address");
// 		varUserSignTime = m_pRecordSet->GetCollect("user_sign_time");
// 		varPhotoPath = m_pRecordSet->GetCollect("user_photo_path");
// 		varUserTagID = m_pRecordSet->GetCollect("user_tag_id");
// 		varSignFlag = m_pRecordSet->GetCollect("user_status_flag");
// 
//         Info_User oUser;
// 		
// 		VarientToChar(varUserID, oUser.csUserID);
// 		VarientToChar(varConfID, oUser.csConferenceID);
// 		VarientToChar(varUserName, oUser.csUserName);
// 		VarientToChar(varUserUnit, oUser.csUserUnit);
//         VarientToChar(varUserCellPhone, oUser.csUserCellPhone);
// 		VarientToChar(varUserEmail, oUser.csUserEmail);
// 		VarientToChar(varUserSignTime, oUser.csUserSignTime);
// 		VarientToChar(varPhotoPath, oUser.csPhotoPath);
// 		VarientToChar(varUserTagID, oUser.csUserTagID);
// 		VarientToChar(varSignFlag, oUser.csSignFlag);
// 		
// 		//签到状态为数字，显示需要转换为对应的意义
// 		int nFlag = atoi(oUser.csSignFlag);
// 		switch(nFlag)
// 		{
// 		case 0:
//             oUser.csSignFlag = SINGIN_ABSENT;
// 			break;
// 		case 1:
// 			oUser.csSignFlag = SIGNIN_INTIME;
// 			break;
// 		case 2:
// 			oUser.csSignFlag = SIGNIN_LATE;
// 			break;
// 		case 3:
// 			oUser.csSignFlag = SINGIN_LEAVE;
// 			break;
// 		default:
// 			break;
// 		}
// 
// 		vcUser.push_back(oUser);
// 		m_pRecordSet->MoveNext();		
// 	}
// 
// 	return vcUser.size();
// }

// int CDBOperator::SelectInfoConference(ConfVec& vcConf)
// {
// 	_variant_t RecordsAffected;
//     m_pRecordSet = m_pConnection->Execute(_bstr_t("SELECT * FROM Info_Conference"), &RecordsAffected, adCmdText);
// 
// 	while (!m_pRecordSet->adoEOF)
// 	{
// 		_variant_t varConfID;
// 		_variant_t varConfName;
// 		_variant_t varConfPlace;
// 		_variant_t varConfUnit;
// 		_variant_t varConfSignTime;
// 		_variant_t varIntendNum;
// 
// 		varConfID = m_pRecordSet->GetCollect("conference_id");
// 		varConfName = m_pRecordSet->GetCollect("conference_name");
// 		varConfPlace = m_pRecordSet->GetCollect("conference_place");
// 		varConfUnit = m_pRecordSet->GetCollect("conference_unit");
// 		varConfSignTime = m_pRecordSet->GetCollect("conference_time");
// 		varIntendNum = m_pRecordSet->GetCollect("conference_intend_num");
// 
// 		Info_Conference oConf;
// 
//         VarientToChar(varConfID, oConf.csConfID);
// 		VarientToChar(varConfName, oConf.csConfName);
// 		VarientToChar(varConfPlace, oConf.csConfPlace);
// 		VarientToChar(varConfUnit, oConf.csConfUint);
// 		VarientToChar(varConfSignTime, oConf.csConfSignTime);
// 		VarientToChar(varIntendNum, oConf.csIntendNum);
// 
// 		vcConf.push_back(oConf);
// 		m_pRecordSet->MoveNext();		
// 	}
// 
// 	return vcConf.size();
// 
// }

void CDBOperator::VarientToChar(_variant_t var, string& str )
{
	if (VT_NULL == var.vt)
	{
		str = "";
	}
	else
	{
		str = (char*)_bstr_t(var);
	}
}

void CDBOperator::NoReturnExcute(string csSQL)
{
	_variant_t RecordsAffected;

    m_pConnection->Execute(_bstr_t(csSQL.c_str()), &RecordsAffected, adCmdText);
}
