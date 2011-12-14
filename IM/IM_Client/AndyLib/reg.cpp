/**********************************************************************
//
//

//
//							AndySDK类库 andylib.lib
//作者：
//
//版本：1.0
//
/
//
//原文件名：reg.cpp
//
//说明：注册表操作类实现代码
//
**********************************************************************/
#define MAKE_SELF_LIB

//#ifndef __BORLANDC__
//#include "andylib.h"
//#else
#include "andylib.h"
//#endif

andyReg::andyReg()
{
	m_dwKeyCnt=0;
   m_dwKeyMax=0;
   m_lprv=NULL;
   m_hKey=NULL;
}

andyReg::~andyReg()
{
	Destroy();
}

void andyReg::Destroy()
{
	m_dwKeyCnt=0;
   m_dwKeyMax=0;

  	m_lprv = (LPREGVALINFO)Del(m_lprv);

   Close();
}

BOOL andyReg::error(HKEY hKey,LPCTSTR szSubKey)
{
  	TCHAR err[20];

   if(hKey == HKEY_LOCAL_MACHINE)
     	wsprintf(err,_T("HKEY_LOCAL_MACHINE"));
   else if(hKey == HKEY_CLASSES_ROOT)
     	wsprintf(err,_T("HKEY_CLASSES_ROOT"));
   else if(hKey == HKEY_CURRENT_USER)
     	wsprintf(err,_T("HKEY_CURRENT_USER"));
   else if(hKey == HKEY_USERS)
     	wsprintf(err,_T("HKEY_USERS"));
   else
    	wsprintf(err,_T(""));

	Errmsg(_T("错误：不能创建注册表项：%s\\%s"),err,szSubKey);
	return FALSE;
}

BOOL andyReg::Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,LPCTSTR szVal)
{
   HKEY hKey1;
   LONG lRetCode;
   DWORD dwDisposition;

   Close();

   if(hKey==HKEY_LOCAL_MACHINE || hKey==HKEY_CLASSES_ROOT || hKey==HKEY_CURRENT_USER || hKey==HKEY_USERS)
   {

   	if(!szSubKey)

      	return FALSE;


   	lRetCode=RegCreateKeyEx(hKey,szSubKey,0, NULL, REG_OPTION_NON_VOLATILE,

                        KEY_ALL_ACCESS,NULL,&hKey1,&dwDisposition);
   	if (lRetCode != ERROR_SUCCESS)
   		return error(hKey,szSubKey);

      lRetCode=RegSetValueEx(hKey1,szKey,NULL,REG_SZ,(LPBYTE)szVal,_tcslen(szVal));
      RegCloseKey(hKey1);
   }
   else
      lRetCode=RegSetValueEx(hKey,szKey,NULL,REG_SZ,(LPBYTE)szVal,_tcslen(szVal));

   return (lRetCode == ERROR_SUCCESS);
}

BOOL andyReg::Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,DWORD dwVal)
{
   HKEY hKey1;
   LONG lRetCode;
   DWORD dwDisposition;

   Close();

   if(hKey==HKEY_LOCAL_MACHINE || hKey==HKEY_CLASSES_ROOT || hKey==HKEY_CURRENT_USER || hKey==HKEY_USERS)
   {

   	if(!szSubKey)

      	return FALSE;

      lRetCode=RegCreateKeyEx(hKey,szSubKey,0, NULL, REG_OPTION_NON_VOLATILE,
                        KEY_ALL_ACCESS,NULL,&hKey1,&dwDisposition);
   	if (lRetCode != ERROR_SUCCESS)
   		return error(hKey,szSubKey);

      lRetCode=RegSetValueEx(hKey1,szKey,NULL,REG_DWORD,(LPBYTE)&dwVal,sizeof(DWORD));
      RegCloseKey(hKey1);
   }
   else
	   lRetCode=RegSetValueEx(hKey,szKey,NULL,REG_DWORD,(LPBYTE)&dwVal,sizeof(DWORD));

   return (lRetCode == ERROR_SUCCESS);
}

BOOL andyReg::Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,LPVOID lpData,DWORD dwType,DWORD cbData)
{
   HKEY hKey1;
   LONG lRetCode;
   DWORD dwDisposition;

   Close();

   if(hKey==HKEY_LOCAL_MACHINE || hKey==HKEY_CLASSES_ROOT || hKey==HKEY_CURRENT_USER || hKey==HKEY_USERS)
   {

   	if(!szSubKey)

      	return FALSE;

      lRetCode = RegCreateKeyEx(hKey,szSubKey,0, NULL, REG_OPTION_NON_VOLATILE,
                        KEY_ALL_ACCESS,NULL,&hKey1,&dwDisposition);
   	if (lRetCode != ERROR_SUCCESS)
   		return error(hKey,szSubKey);

      lRetCode = RegSetValueEx(hKey1,szKey,NULL,dwType,(LPBYTE)lpData,cbData);
      RegCloseKey(hKey1);
   }
   else
	   lRetCode=RegSetValueEx(hKey,szKey,NULL,dwType,(LPBYTE)lpData,cbData);

   return (lRetCode == ERROR_SUCCESS);
}

BOOL andyReg::Check(DWORD dwNum)
{
	if(!m_lprv)
   {
   	m_lprv = (LPREGVALINFO)New(dwNum*sizeof(REGVALINFO));
   	if(!m_lprv)
   		return FALSE;
      else
      {
      	m_dwKeyMax = dwNum;
         m_dwKeyCnt = 0;
      }
   }
   else
   	if(m_dwKeyCnt >= m_dwKeyMax)
      	return FALSE;
   return TRUE;
}

BOOL andyReg::Add(LPCTSTR szKey,LPVOID lpData,DWORD dwType,DWORD cbData,DWORD dwNum)
{
	if(!Check(dwNum))
   	return FALSE;

   m_lprv[m_dwKeyCnt].szValName = szKey;
	m_lprv[m_dwKeyCnt].dwType = dwType;
   m_lprv[m_dwKeyCnt].lpData = lpData;
   m_lprv[m_dwKeyCnt].cbData = cbData;

   m_dwKeyCnt++;

   return TRUE;
}

BOOL andyReg::Add(LPCTSTR szKey,DWORD dwData,DWORD dwNum)
{
	if(!Check(dwNum))
   	return FALSE;

   m_lprv[m_dwKeyCnt].szValName = szKey;
	m_lprv[m_dwKeyCnt].dwType = REG_DWORD;
   m_lprv[m_dwKeyCnt].dwData = dwData;
   m_lprv[m_dwKeyCnt].lpData = (LPBYTE)&m_lprv[m_dwKeyCnt].dwData;
   m_lprv[m_dwKeyCnt].cbData = sizeof(DWORD);

   m_dwKeyCnt++;

   return TRUE;
}

BOOL andyReg::Add(LPCTSTR szKey,LPCTSTR szData,DWORD dwNum)
{
	if(!Check(dwNum))
   	return FALSE;

   m_lprv[m_dwKeyCnt].szValName = szKey;
	m_lprv[m_dwKeyCnt].dwType = REG_SZ;
   m_lprv[m_dwKeyCnt].lpData = (LPBYTE)szData;
   m_lprv[m_dwKeyCnt].cbData = _tcslen(szData);

   m_dwKeyCnt++;

   return TRUE;
}

BOOL andyReg::Set(HKEY hKey,LPCTSTR szSubKey)
{
   HKEY hKey1;
   LONG   lRetCode;
   BOOL bUseKeyName;
   DWORD  dwDisposition;
   DWORD i;

   Close();

   if(hKey==HKEY_LOCAL_MACHINE || hKey==HKEY_CLASSES_ROOT || hKey==HKEY_CURRENT_USER || hKey==HKEY_USERS)
   {
   	if(!szSubKey)
      	return FALSE;

      bUseKeyName = TRUE;

   	lRetCode=RegCreateKeyEx(hKey,szSubKey,0, NULL, REG_OPTION_NON_VOLATILE,
                        KEY_ALL_ACCESS,NULL,&hKey1,&dwDisposition);
   	if (lRetCode != ERROR_SUCCESS)
   		return error(hKey,szSubKey);
   }
   else
   {
   	bUseKeyName = FALSE;
   	if(hKey)
	     	hKey1 = hKey;
      else
      	return FALSE;
   }

   for(i=0;i<m_dwKeyCnt;i++)
   {
   	lRetCode = RegSetValueEx(hKey1,m_lprv[i].szValName,NULL,m_lprv[i].dwType,(LPBYTE)m_lprv[i].lpData,m_lprv[i].cbData);

   	if (lRetCode != ERROR_SUCCESS)
     	{
      	//Errmsg("错误：写注册表失败");
         if(bUseKeyName)
         	RegCloseKey(hKey1);
         Destroy();
      	return FALSE;
      }
   }

   if(bUseKeyName)
     	RegCloseKey(hKey1);

   Destroy();
   return TRUE;
}

BOOL andyReg::Get(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,
						LPVOID lpData,DWORD dwDataSize,LPDWORD lpdwType,
						LPDWORD lpDataSize)
{
   if(!szSubKey || szSubKey[0]=='\0')
   	return FALSE;

   Close();

   if(RegOpenKeyEx(hKey,szSubKey,0,KEY_ALL_ACCESS,&m_hKey) != ERROR_SUCCESS)
   {
   	m_hKey = NULL;
   	return FALSE;
   }
	if(lpDataSize)
		*lpDataSize = dwDataSize;
	else
		lpDataSize = &dwDataSize;
   return(RegQueryValueEx(m_hKey,szKey,NULL,lpdwType,(LPBYTE)lpData,lpDataSize) == ERROR_SUCCESS);
}

DWORD andyReg::GetInt(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,DWORD dwDef)
{
   if(!szSubKey || szSubKey[0]=='\0')
   	return FALSE;

   DWORD dwDataSize,dwVal;

   Close();

   if(RegOpenKeyEx(hKey,szSubKey,0,KEY_ALL_ACCESS,&m_hKey) != ERROR_SUCCESS)
   {
   	m_hKey = NULL;
		return dwDef;
   }
   dwDataSize = sizeof(DWORD);

   if(RegQueryValueEx(m_hKey,szKey,NULL,NULL,(LPBYTE)&dwVal,&dwDataSize) == ERROR_SUCCESS)
   	dwDef = dwVal;

  	return dwDef;
}

BOOL andyReg::Get(LPCTSTR szKey,LPVOID lpData,DWORD dwDataSize,
						LPDWORD lpdwType/*=NULL*/,LPDWORD lpDataSize/*=NULL*/)
{
   if(!m_hKey)
   	return FALSE;

	if(lpDataSize)
		*lpDataSize = dwDataSize;
	else
		lpDataSize = &dwDataSize;

   return (RegQueryValueEx(m_hKey,szKey,NULL,lpdwType,(LPBYTE)lpData,lpDataSize) == ERROR_SUCCESS);
}

DWORD andyReg::GetInt(LPCTSTR szKey,DWORD dwDef)
{
   if(!m_hKey)
   	return dwDef;

   DWORD dwType,dwDataSize,dwVal;

   dwDataSize = sizeof(DWORD);

   if(RegQueryValueEx(m_hKey,szKey,NULL,&dwType,(LPBYTE)&dwVal,&dwDataSize)  == ERROR_SUCCESS)
   	dwDef = dwVal;

   return dwDef;
}

BOOL andyReg::Delete(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey)
{
	LONG lResult;
	HKEY hKey1;
	
	Close();
	
	lResult = RegOpenKeyEx(hKey,szSubKey,0,KEY_ALL_ACCESS,&hKey1);
	if(lResult != ERROR_SUCCESS)
		return FALSE;
	
	lResult = RegDeleteValue(hKey1,szKey);
	RegCloseKey(hKey1);
	
	return (lResult == ERROR_SUCCESS);
}

BOOL andyReg::Delete(HKEY hKey,LPCTSTR szSubKey)
{
   Close();
	if(OSType()==NT)
   	return DelNT(hKey,szSubKey);
   else
   	return (RegDeleteKey(hKey,szSubKey) == ERROR_SUCCESS);
}

BOOL andyReg::DelNT(HKEY hKey,LPCTSTR szSubKey)
{
	LONG lResult;
	HKEY hKey1;
	DWORD dwIndex=0;
	TCHAR szKeyName[300];
	TCHAR szKey[500];
	DWORD dwKeySize=300;
	
	lResult = RegOpenKeyEx(hKey,szSubKey,0,KEY_ALL_ACCESS,&hKey1);
	if(lResult != ERROR_SUCCESS)
		return FALSE;
	
	while(lResult == ERROR_SUCCESS)
	{
		lResult = RegEnumKeyEx(hKey1,dwIndex,szKeyName,&dwKeySize,NULL,NULL,NULL,NULL);
		if(lResult == ERROR_NO_MORE_ITEMS)
		{
			RegCloseKey(hKey1);
			return (RegDeleteKey(hKey,szSubKey)==ERROR_SUCCESS);
		}
		else if(lResult != ERROR_SUCCESS)
		{
			RegCloseKey(hKey1);
			return FALSE;
		}
		else
		{
			wsprintf(szKey,_T("%s\\%s\0"),szSubKey,szKeyName);
			if(!DelNT(hKey,szKey))
			{
				RegCloseKey(hKey1);
				return FALSE;
			}
		}
		dwIndex++;
	}
	return TRUE;
}

void andyReg::Close()
{
	if(m_hKey!=NULL)
		RegCloseKey(m_hKey);
	m_hKey = NULL;
}

///////////////////////////////////////////////////////////////////////
//
//配置文件操作类
//
///////////////////////////////////////////////////////////////////////

andyINI::andyINI()
{
	ZeroMemory(m_filename,MAX_PATH);
   bInited = FALSE;
}

andyINI::andyINI(LPCTSTR szFile)
{
	SetFile(szFile);
}

andyINI::~andyINI()
{
	ZeroMemory(m_filename,MAX_PATH);
   bInited = FALSE;
}

void andyINI::SetFile(LPCTSTR format,...)
{
	va_list ap;

   if(m_filename[0] != '\0')
		ZeroMemory(m_filename,MAX_PATH);

	if(format)
   {
		va_start(ap, format);
		_vsntprintf(m_filename,MAX_PATH-2,format,ap);
		//vsprintf(m_filename, format, ap);
		va_end(ap);
	}

   if(m_filename[0] != '\0')
	   bInited = TRUE;
   else
   	bInited = FALSE;
}

int andyINI::GetInt(LPCTSTR section,LPCTSTR key,int nDefault/* = 0*/)
{
	if(bInited)
   	return GetPrivateProfileInt(section,key,nDefault,m_filename);
   else
   	return nDefault;
}

DWORD andyINI::GetString(LPCTSTR section,LPCTSTR key,LPTSTR szBuf,int nBufSize,LPCTSTR szDefault/*=NULL*/)
{
   if(szDefault == NULL)
   	szDefault = _T("");
      
	if(bInited)
   	return GetPrivateProfileString(section,key,szDefault,szBuf,nBufSize,m_filename);
   else
   	return 0;
}

BOOL andyINI::Write(LPCTSTR section,LPCTSTR key,LPCTSTR szBuf)
{
	if(bInited)
   	return WritePrivateProfileString(section,key,szBuf,m_filename);
   else
   	return FALSE;
}

BOOL andyINI::Write(LPCTSTR section,LPCTSTR key,int nVal)
{
   TCHAR s[20];

   wsprintf(s,_T("%d\0\0"),nVal);

	if(bInited)
   	return WritePrivateProfileString(section,key,s,m_filename);
   else
   	return FALSE;
}

BOOL andyINI::DeleteKey(LPCTSTR section,LPCTSTR szKey)
{
	if(bInited)
		return WritePrivateProfileString(section,szKey,NULL,m_filename);
   else
		return FALSE;
}

BOOL andyINI::DeleteSection(LPCTSTR section)
{
	if(bInited)
		return WritePrivateProfileString(section,NULL,NULL,m_filename);
   else
		return FALSE;
}

DWORD andyINI::GetSections(LPTSTR lpBuffer,int nSize)
{
	if(bInited)
		return GetPrivateProfileSectionNames(lpBuffer,nSize,m_filename);
   else
		return 0;
}

DWORD andyINI::GetKeys(LPCTSTR szSec,LPTSTR lpBuffer,int nSize)
{
	if(bInited)
		return GetPrivateProfileSection(szSec,lpBuffer,nSize,m_filename);
   else
		return 0;
}
