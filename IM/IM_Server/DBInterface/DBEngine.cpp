// DBEngine.cpp: implementation of the DBEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "DBEngine.h"

#include "../LogFile/CommLog.h"
#include "../LogFile/ServerLog.h"

extern CServerLog * g_pSvrLog;//日志文件全局变量

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DBEngine::DBEngine(LPCTSTR dbservername, LPCTSTR dbname, LPCTSTR dbuser, LPCTSTR dbpass, LPCTSTR dblanguage, BOOL &bStatus)
{
	/*m_strSQL = "";
	memset(m_szError, 0, nMaxBuffer);
	memset(m_cSQL, 0, STRSQLMAX);
	if (0 != OpenDatabase(dbservername, dbname, dbuser, dbpass, dblanguage))
	{
        bStatus = FALSE;
	}	
	else
	{
		bStatus = TRUE;
	}*/

	m_bDBOpen = false;
	if (0 == OpenDatabase(dbservername,dbname,dbuser,dbpass))
	{
		bStatus =true;
	}
	else
	{
		bStatus = false;
	}
}

DBEngine::~DBEngine()
{
    CloseDatabase();
}
int DBEngine::ExecSQL(char *szSQL)
{

	if (!m_bDBOpen)
	{
		return -1;
	}
	// 创建Command对象
	_CommandPtr cmd;
	HRESULT hr = cmd.CreateInstance(__uuidof(Command));
	if (FAILED(hr))
	{
		return -1;
	}

	//char szSQL[SQL_BUFFER_LEN] = {0};
	//sprintf(szSQL, "select count(*) as count from userinfo where name =\'%s\' and password =\'%s\'",
	//	name,password);
	cmd->ActiveConnection = _connection_ptr;
	cmd->CommandText = _bstr_t(szSQL);
	cmd->CommandType = adCmdText;
	try 
	{
		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdUnknown);

		if (FAILED(hr)) 
		{
			return -1;
		}
	}
	catch (_com_error &err)
	{
		//TRACE(_T("数据库操作失败! 错误信息:%s, 文件:%s, 行:%d.\n"), err.ErrorMessage(), __FILE__, __LINE__);

		return -1;
	}
	return 0;
}
int DBEngine::OpenDatabase(LPCTSTR dbservername, LPCTSTR dbname, LPCTSTR dbuser, LPCTSTR dbpass)
{
	_strDstAddress = dbservername;
	_strDBName       = dbname;
	_strUsername     = dbuser;
	_strPassword     = dbpass;

	HRESULT comhr = ::CoInitialize(NULL);
	if (FAILED(comhr))

	{
		return -1;
	}
	HRESULT hr = _connection_ptr.CreateInstance(__uuidof(Connection));

	if (FAILED(hr))
	{
		return -1;
	}

	char szSQL[SQL_BUFFER_LEN] = {0};
	memset(szSQL, 0, SQL_BUFFER_LEN);
	//"DSN=ADOmysql;Server= localhost;Database=testmysql","root","123456",adModeUnknown
	sprintf(szSQL,"DSN=ADOmysql;Server=%s;Database=%s",dbservername, dbname);
	//sprintf(szSQL, "Driver=SQL Server;Server=%s;DATABASE=%s", strDstAddress, strDBName);

	try
	{
		// 连接到服务器上数据库
		_connection_ptr->Open(szSQL, dbuser, dbpass,adModeUnknown) ;
		if (FAILED(hr))
			return -1;
	}
	catch (_com_error &err)
	{
		//		TRACE(_T("数据库操作失败! 错误信息:%s, 文件:%s, 行:%d.\n"), err.ErrorMessage(), __FILE__, __LINE__);

		return -1;
	}

	m_bDBOpen = TRUE;

	return 0;
}

int DBEngine::CloseDatabase()
{
	if (m_bDBOpen)
	{
		HRESULT hr =_connection_ptr->Close();

		if (FAILED(hr))
		{
			return -1;
		}
		::CoUninitialize();
		m_bDBOpen = FALSE;
	}

	return 0;
}
int DBEngine::UserLogin(string imnum,string password)
{
	if (!m_bDBOpen)
	{
		return -1;
	}
	// 创建Command对象
	_CommandPtr cmd;
	HRESULT hr = cmd.CreateInstance(__uuidof(Command));
	if (FAILED(hr))
	{
		return -1;
	}

	char szSQL[SQL_BUFFER_LEN] = {0};
	sprintf(szSQL, "select count(*) as count from userinfo where ID =\'%s\' and password =\'%s\'",
		imnum.c_str(),password.c_str());
	cmd->ActiveConnection = _connection_ptr;
	cmd->CommandText = _bstr_t(szSQL);
	cmd->CommandType = adCmdText;
	try 
	{
		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdUnknown);

		if (FAILED(hr)) 
		{
			return -1;
		}
		_variant_t count;
		while (!rs->ADOEOF)
		{     
			count = rs->GetCollect("count");
			rs->MoveNext() ;
		}
		if(count.intVal == 0)
			return -1;
	}
	catch (_com_error &err)
	{
		//TRACE(_T("数据库操作失败! 错误信息:%s, 文件:%s, 行:%d.\n"), err.ErrorMessage(), __FILE__, __LINE__);

		return -1;
	}
	return 0;
}
int DBEngine::SetLoginState(int imid,int state)
{
	if (!m_bDBOpen)
	{
		return -1;
	}
	// 创建Command对象
	_CommandPtr cmd;
	HRESULT hr = cmd.CreateInstance(__uuidof(Command));
	if (FAILED(hr))
	{
		return -1;
	}

	char szSQL[SQL_BUFFER_LEN] = {0};
	//set online=1 from userinfo where id=123456
	sprintf(szSQL, "update userinfo set online=%d where id=%d",
		state,imid);
	cmd->ActiveConnection = _connection_ptr;
	cmd->CommandText = _bstr_t(szSQL);
	cmd->CommandType = adCmdText;
	try 
	{
		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdUnknown);

		if (FAILED(hr)) 
		{
			return -1;
		}
	}
	catch (_com_error &err)
	{
		//TRACE(_T("数据库操作失败! 错误信息:%s, 文件:%s, 行:%d.\n"), err.ErrorMessage(), __FILE__, __LINE__);

		return -1;
	}
	return 0;
}
int DBEngine::GetUserInfo(USER_INFO &user)
{
	if (!m_bDBOpen)
	{
		return -1;
	}
	// 创建Command对象
	_CommandPtr cmd;
	HRESULT hr = cmd.CreateInstance(__uuidof(Command));
	if (FAILED(hr))
	{
		return -1;
	}
	char szSQL[SQL_BUFFER_LEN] = {0};
	sprintf(szSQL, "select name,sign from userinfo where id=%d",
		user.id);
	cmd->ActiveConnection = _connection_ptr;
	cmd->CommandText = _bstr_t(szSQL);
	cmd->CommandType = adCmdText;
	try 
	{
		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdUnknown);

		if (FAILED(hr)) 
		{
			return -1;
		}
		while (!rs->ADOEOF)
		{     
			_variant_t		name;
			_variant_t		sign;


			name = rs->GetCollect("name");
			sign = rs->GetCollect("sign");

			VarientToString(name, user.nick_name);
		    VarientToString(sign, user.description);

			rs->MoveNext() ;
		}
	}
	catch (_com_error &err)
	{
		//		TRACE(_T("数据库操作失败! 错误信息:%s, 文件:%s, 行:%d.\n"), err.ErrorMessage(), __FILE__, __LINE__);

		return -1;
	}
	return 0;
}

int DBEngine::GetFriendInfo(vector<USER_INFO> &verfriend,int imid)
{

	if (!m_bDBOpen)
	{
		return -1;
	}
	// 创建Command对象
	_CommandPtr cmd;
	HRESULT hr = cmd.CreateInstance(__uuidof(Command));
	if (FAILED(hr))
	{
		return -1;
	}
	char szSQL[SQL_BUFFER_LEN] = {0};
	sprintf(szSQL, "select friendname,friendsign,friendimno from friend where imnum=%d",
		imid);
	cmd->ActiveConnection = _connection_ptr;
	cmd->CommandText = _bstr_t(szSQL);
	cmd->CommandType = adCmdText;
	try 
	{
		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdUnknown);

		if (FAILED(hr)) 
		{
			return -1;
		}
		while (!rs->ADOEOF)
		{     
			_variant_t		_v_friendname;
			_variant_t		_v_friendsign;
			_variant_t      _v_friendimno;

			_v_friendname = rs->GetCollect("friendname");
			_v_friendsign = rs->GetCollect("friendsign");
			_v_friendimno = rs->GetCollect("friendimno");

			USER_INFO user;
			user.id = (int)(long)_v_friendimno;
			VarientToString(_v_friendsign, user.description);
			VarientToString(_v_friendname, user.nick_name);

			verfriend.push_back(user);

			rs->MoveNext() ;
		}
	}
	catch (_com_error &err)
	{
		//		TRACE(_T("数据库操作失败! 错误信息:%s, 文件:%s, 行:%d.\n"), err.ErrorMessage(), __FILE__, __LINE__);

		return -1;
	}
	return 0;
}
int DBEngine::GetLoginState(int imno,LOGININFO &loginfo)
{
	if (!m_bDBOpen)
	{
		return -1;
	}
	// 创建Command对象
	_CommandPtr cmd;
	HRESULT hr = cmd.CreateInstance(__uuidof(Command));
	if (FAILED(hr))
	{
		return -1;
	}
	char szSQL[SQL_BUFFER_LEN] = {0};
	sprintf(szSQL, "select nowip,online from userinfo where id =%d",
		imno);
	cmd->ActiveConnection = _connection_ptr;
	cmd->CommandText = _bstr_t(szSQL);
	cmd->CommandType = adCmdText;
	try 
	{
		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdUnknown);

		if (FAILED(hr)) 
		{
			return -1;
		}
		while (!rs->ADOEOF)
		{     
			_variant_t		_V_nowip;
			_variant_t		_V_online;


			_V_nowip = rs->GetCollect("nowip");
			_V_online = rs->GetCollect("online");

			VarientToString(_V_nowip, loginfo.ip);
			loginfo.type = (int)(long)_V_online;

			rs->MoveNext() ;
		}
		if(loginfo.type ==-1)
			return -1;
	}
	catch (_com_error &err)
	{
		//		TRACE(_T("数据库操作失败! 错误信息:%s, 文件:%s, 行:%d.\n"), err.ErrorMessage(), __FILE__, __LINE__);

		return -1;
	}
	return 0;
}

int DBEngine::GetOnlineFriend(vector<int> &vecfriend,int imid)
{
	if (!m_bDBOpen)
	{
		return -1;
	}
	// 创建Command对象
	_CommandPtr cmd;
	HRESULT hr = cmd.CreateInstance(__uuidof(Command));
	if (FAILED(hr))
	{
		return -1;
	}
	char szSQL[SQL_BUFFER_LEN] = {0};
	sprintf(szSQL, "select friendimno from friend where imnum=%d and friendonline=1",
		imid);
	cmd->ActiveConnection = _connection_ptr;
	cmd->CommandText = _bstr_t(szSQL);
	cmd->CommandType = adCmdText;
	try 
	{
		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdUnknown);

		if (FAILED(hr)) 
		{
			return -1;
		}
		while (!rs->ADOEOF)
		{     
			_variant_t		_v_friendname;
			_variant_t		_v_friendsign;
			_variant_t      _v_friendimno;

			_v_friendimno = rs->GetCollect("friendimno");

			int  id;
			id = (int)(long)_v_friendimno;
			

			vecfriend.push_back(id);

			rs->MoveNext() ;
		}
	}
	catch (_com_error &err)
	{
		//		TRACE(_T("数据库操作失败! 错误信息:%s, 文件:%s, 行:%d.\n"), err.ErrorMessage(), __FILE__, __LINE__);

		return -1;
	}
	return 0;
}
//ip为带出去的参数
int DBEngine::GetIpByName(string name, string &ip,int &port)
{
	if (!m_bDBOpen)
	{
		return -1;
	}
	// 创建Command对象
	_CommandPtr cmd;
	HRESULT hr = cmd.CreateInstance(__uuidof(Command));
	if (FAILED(hr))
	{
		return -1;
	}
	char szSQL[SQL_BUFFER_LEN] = {0};
	sprintf(szSQL, "select nowip,port from userinfo where name =\'%s\'",
		name.c_str());
	cmd->ActiveConnection = _connection_ptr;
	cmd->CommandText = _bstr_t(szSQL);
	cmd->CommandType = adCmdText;
	try 
	{
		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdUnknown);

		if (FAILED(hr)) 
		{
			return -1;
		}
		while (!rs->ADOEOF)
		{     
			_variant_t		tip;
			_variant_t		tport;


			tip = rs->GetCollect("nowip");
			tport = rs->GetCollect("port");

			VarientToString(tip, ip);
			port = (int)(long)tport;

			rs->MoveNext() ;
		}
		if(ip =="")
			return -1;
	}
	catch (_com_error &err)
	{
		//		TRACE(_T("数据库操作失败! 错误信息:%s, 文件:%s, 行:%d.\n"), err.ErrorMessage(), __FILE__, __LINE__);

		return -1;
	}
	return 0;
}

int DBEngine::UpdateIpByName(string  name, string ip,int port)
{
	if (!m_bDBOpen)
	{
		return -1;
	}
	// 创建Command对象
	_CommandPtr cmd;
	HRESULT hr = cmd.CreateInstance(__uuidof(Command));
	if (FAILED(hr))
	{
		return -1;
	}

	char szSQL[SQL_BUFFER_LEN] = {0};
	sprintf(szSQL, "update userinfo SET nowip=\'%s\',port=%d where name=\'%s\'",
		ip.c_str(),port,name.c_str());
	cmd->ActiveConnection = _connection_ptr;
	cmd->CommandText = _bstr_t(szSQL);
	cmd->CommandType = adCmdText;
	try 
	{
		_RecordsetPtr rs = cmd->Execute(NULL, NULL, adCmdUnknown);

		if (FAILED(hr)) 
		{
			return -1;
		}
	}
	catch (_com_error &err)
	{
		//		TRACE(_T("数据库操作失败! 错误信息:%s, 文件:%s, 行:%d.\n"), err.ErrorMessage(), __FILE__, __LINE__);
		return -1;
	}
	return 0;
}
void DBEngine::VarientToString(_variant_t var, string& str)
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