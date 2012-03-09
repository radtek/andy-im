/**********************************************************************
//

//
//							AndySDK类库 andylib.lib
//作者：
//
//版本：1.0
//

//
//原文件名：global.cpp
//
//说明：全局函数实现，方便编程
//
**********************************************************************/

#define MAKE_SELF_LIB

#include "andylib.h"
#include <process.h>

HINSTANCE	m_hInst = NULL;
HWND		m_hWnd = NULL;

BOOL		GetPathStr(LPCTSTR path,LPTSTR temp,int* mark,int len);
DWORDLONG	GetPathSize1(LPCTSTR szPath,int& nFileCnt,int& nPathCnt);
BOOL		TrayMessage(HWND hDlg, DWORD dwMessage, UINT uID, HICON hIcon, LPCTSTR lpszTip);


//初始化类库
void InitAndyLib(HWND hWnd,HINSTANCE hInst/*=NULL*/)
{
	m_hWnd  = hWnd;
	m_hInst = hInst?hInst:GetModuleHandle(NULL);
}

HINSTANCE GetInstance()
{
	return m_hInst;
}

HWND GetMainWnd()
{
	return m_hWnd;
}

//内存分配，分配失败系统自动防护。(new则防护不住)
//#ifdef _DEBUG
//LPVOID _New(DWORD dwSize, char *file, int line)
//#else
LPVOID New(DWORD dwSize)
//#endif
{
	if(dwSize == 0)
		return NULL;
//	return GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,dwSize);


	try
	{
		LPTSTR res = new TCHAR[dwSize];
		if(res)
			memset(res,0,dwSize);
		return res;
	}
	catch(...)
	{
		return NULL;
	}
//*/
}

// 释放由 _New() 申请的内存
//#ifdef _DEBUG
//LPVOID _Del(LPVOID lpv, char *file, int line)
//#else
LPVOID Del(LPVOID lpv)
//#endif
{
	if (lpv == NULL)
	{
		return NULL;
	}
	LPSTR s = (LPSTR)lpv;
	delete []s;
	//GlobalFree(lpv);
	
	return NULL;
}


//检测可用内存
DWORD GetFreeMem(void)
{
	MEMORYSTATUS mem;
	mem.dwLength=sizeof(MEMORYSTATUS);

	GlobalMemoryStatus(&mem);

	return mem.dwAvailPageFile;
}



//////////////////////////////////////////////////////////////////////////////////////

BOOL TrayMessage(HWND hDlg,DWORD dwMessage,UINT uID,HICON hIcon,LPCTSTR lpszTip)
{
	NOTIFYICONDATA tnd;

	tnd.cbSize	= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= hDlg;
	tnd.uID		= uID;

	tnd.uFlags	= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage	= MYWM_ICON;
	tnd.hIcon	= hIcon;
   if(lpszTip)
   {	
		//tnd.szTip大小为64，如果lpszTip大于该值则截断
   	memset(tnd.szTip,0,sizeof(tnd.szTip));
		lstrcpyn(tnd.szTip,lpszTip,sizeof(tnd.szTip));		
   }
   else
   	*tnd.szTip = '\0';

	return Shell_NotifyIcon(dwMessage, &tnd);
}

BOOL AddInTaskBar(HWND hWnd,HICON hIcon,LPCTSTR s,UINT uId/* = 0*/)
{
	return TrayMessage(hWnd,NIM_ADD,uId,hIcon,s);
}

BOOL ModiInTaskBar(HWND hWnd,HICON hIcon,LPCTSTR s,UINT uId/* = 0*/)
{
	return TrayMessage(hWnd,NIM_MODIFY,uId,hIcon,s);
}

BOOL DelInTaskBar(HWND hWnd,UINT uId/*=0*/)
{
	return TrayMessage(hWnd,NIM_DELETE,uId,NULL,NULL);
}

void Errmsg(LPCTSTR format,...)
{
   TCHAR buf[MAX_PATH];
	memset(buf,0,MAX_PATH);

	va_list ap;

   if(format)
	{
		va_start(ap, format);
		_vsntprintf(buf,MAX_PATH-2,format,ap);
		//vsprintf(buf, format, ap);
		va_end(ap);
	}
	MessageBox(GetFocus(),buf,_T("错误"),MB_OK | MB_ICONWARNING);
}

BOOL Confirm(LPCTSTR format,...)
{
   TCHAR buf[MAX_PATH];
	memset(buf,0,MAX_PATH);
	va_list ap;

   if(format)
	{
		va_start(ap, format);
		_vsntprintf(buf,MAX_PATH-2,format,ap);
		//vsprintf(buf, format, ap);
		va_end(ap);
	}

	if(MessageBox(GetFocus(),buf,_T("确认"),MB_YESNO | MB_ICONQUESTION)==IDYES)
   	return TRUE;
   else
   	return FALSE;
}

//限定在MAX_PATH字节数以内
void MsgBox(LPCTSTR szTitle,LPCTSTR format, ...)
{
   TCHAR buf[MAX_PATH];
	memset(buf,0,MAX_PATH);

	va_list ap;
	
   if(format)
	{
		va_start(ap, format);
		_vsntprintf(buf,MAX_PATH-2,format,ap);
		//vsprintf(buf, format, ap);
		va_end(ap);
	}	
	MessageBox(GetFocus(),buf,szTitle,MB_OK);
}

//带字数控制，由dwMsgLength指定
int MsgBoxEx(LPCTSTR szTitle,UINT uType,DWORD dwMsgLength,LPCTSTR format, ...)
{
   LPTSTR buf = (LPTSTR)New(sizeof(TCHAR)*(dwMsgLength+2));
   if(!buf)
		return 0;
	
	va_list ap;
	
   if(format)
	{
		va_start(ap, format);
		_vsntprintf(buf,dwMsgLength,format,ap);
		//vsprintf(buf, format, ap);
		va_end(ap);
	}
	
   int res = MessageBox(GetFocus(),buf,szTitle,uType);
   Del(buf);

	return res;
}

void ShowHelpInfo(LPCTSTR szText,LPCTSTR szCaption,LPCTSTR lpszIcon)
{
	MSGBOXPARAMS msgbox ;

   msgbox.cbSize = sizeof(msgbox);
   msgbox.hwndOwner = m_hWnd;
   msgbox.hInstance = m_hInst;
   msgbox.lpszText = szText;
   msgbox.lpszCaption = szCaption;
   msgbox.dwStyle = MB_OK | MB_USERICON ;
   msgbox.lpszIcon = lpszIcon;
   msgbox.dwContextHelpId = 1;
   msgbox.lpfnMsgBoxCallback = NULL ;
   msgbox.dwLanguageId = MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL);

   MessageBoxIndirect(&msgbox);
}

BOOL MkDir(LPCTSTR szPath)
{
	TCHAR temp[MAX_PATH];
	int nPathMark = 0;
   int len = _tcslen(szPath);
   DWORD flag;

   ZeroMemory(temp,MAX_PATH);

   if(CreateDirectory(szPath,NULL))
   	return TRUE;
   else
	   while(GetPathStr(szPath,temp,&nPathMark,len))
   	{
      	CreateDirectory(temp,NULL);
	      temp[nPathMark-1] = '\\';
	   }

   flag = GetFileAttributes(szPath);
   if(flag == 0xFFFFFFFF || (flag & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
   	return FALSE;
   else
   	return TRUE;
}

BOOL GetPathStr(LPCTSTR path,LPTSTR temp,int* mark,int len)
{
   if(path[*mark] == '\0')
     	return FALSE;

   for(;*mark<len;(*mark)++)
   {
      temp[*mark] = path[*mark];
   	if(path[*mark] == '\\')
      {
         temp[*mark] = '\0';
         (*mark) ++;
      	return TRUE;
      }
   }
   temp[*mark] = '\0';
   return TRUE;
}

HINSTANCE andyExec(LPCTSTR lpFile,LPCTSTR lpDir/*=NULL*/,LPCTSTR lpParam/*=NULL*/,LPCTSTR lpOp/*=NULL*/,INT nShowCmd/*=SW_SHOW*/)
{
   if(lpOp == NULL)
   	lpOp = _T("open");

	return ShellExecute(NULL,lpOp,lpFile,lpParam,lpDir,nShowCmd);

}

//执行文件（调用CreateProcess）
int RunProcess(LPCTSTR lpFile,LPCTSTR lpDir,LPCTSTR lpParam,DWORD dwWait,int nShowCmd,
							DWORD dwCreateFlag,LPPROCESS_INFORMATION lpProcessInformation)
{
	STARTUPINFO si;
	memset(&si,0,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = nShowCmd;
	
	PROCESS_INFORMATION pi;

	if(CreateProcess(lpFile,(LPTSTR)lpParam,NULL,NULL,FALSE,dwCreateFlag,NULL,lpDir,&si,&pi))
	{
		if(lpProcessInformation)
			memcpy(lpProcessInformation,&pi,sizeof(PROCESS_INFORMATION));
		else
		{
			if(pi.hThread)
				CloseHandle(pi.hThread);
			if(pi.hProcess)
				CloseHandle(pi.hProcess);
		}

		if(dwWait != 0)
			return (WaitForInputIdle(pi.hProcess,dwWait) == 0);
		else
			return TRUE;
	}
	return 0;
}

BOOL SelectDir(LPTSTR lpDir,LPCTSTR lpszTitle/*="选择路径"*/,
					UINT uFlag/*=BIF_RETURNONLYFSDIRS*/,BFFCALLBACK lpfn/*=NULL*/,
					LPARAM lParam/*=0*/,int iImage/*=0*/)
{
   if(lpDir == NULL)
   	return FALSE;

   BROWSEINFO bi;
   ITEMIDLIST* pidl;

   bi.hwndOwner = NULL;
   bi.pidlRoot = NULL;
   bi.pszDisplayName = lpDir;
   bi.lpszTitle = lpszTitle;
   bi.ulFlags = uFlag;//BIF_RETURNONLYFSDIRS;
   bi.lpfn = lpfn;
   bi.lParam = lParam;
   bi.iImage = iImage;

   pidl = SHBrowseForFolder(&bi);
    /*Display "SelectFolder" dialogbox,Getthefoldername and convertit in to a
    ITEMLIST data structure.*/

   if(pidl == NULL)
   {
   	lpDir[0] = 0;
      return FALSE;
   }
   if(!SHGetPathFromIDList(pidl,lpDir))
   {
      lpDir[0] = 0;
      return FALSE;
   }
   return TRUE;
}

DWORD GetFilePointer(HANDLE hFile)
{
	return SetFilePointer(hFile,0,NULL,FILE_CURRENT);
}

DWORDLONG GetPathSize(LPCTSTR lpszPathName,int* lpnFileCnt/*=NULL*/,int* lpnPathCnt/*=NULL*/)
{
   DWORD flag = GetFileAttributes(lpszPathName);
   DWORDLONG cnt;
   int nFileCnt = 0,nPathCnt = 0;

   if(flag == 0xFFFFFFFF)
   {
   	if(lpnFileCnt)
      	*lpnFileCnt = 0;
      if(lpnPathCnt)
      	*lpnPathCnt = 0;
      return 0;
   }
   else if((flag & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
   {
   	cnt = GetPathSize1(lpszPathName,nFileCnt,nPathCnt);
      if(lpnFileCnt)
        	*lpnFileCnt = nFileCnt;
      if(lpnPathCnt)
      	*lpnPathCnt = nPathCnt;
      return cnt;
   }
   else
   {
      WIN32_FIND_DATA wInfo;
   	HANDLE hFile = FindFirstFile(lpszPathName,&wInfo);
      if(INVALID_HANDLE_VALUE != hFile)
      {
      	if(lpnFileCnt)
         	*lpnFileCnt = 1;
         if(lpnPathCnt)
	      	*lpnPathCnt = 0;
         FindClose(hFile);
         return ((DWORDLONG)wInfo.nFileSizeHigh << 32) + wInfo.nFileSizeLow;
      }
      else
      {
      	if(lpnFileCnt)
   			*lpnFileCnt = 0;
         if(lpnPathCnt)
	      	*lpnPathCnt = 0;
		   return 0;
      }
   }
}

DWORDLONG GetPathSize1(LPCTSTR szPath,int& nFileCnt,int& nPathCnt)
{
	WIN32_FIND_DATA wInfo;
   HANDLE hFile;
   TCHAR temp[MAX_PATH],next[MAX_PATH];
   DWORDLONG dwFileSizeTotal = 0;

  	wsprintf(temp,_T("%s\\*.*\0\0"),szPath);

  	hFile = FindFirstFile(temp,&wInfo);

   if(INVALID_HANDLE_VALUE != hFile)
   {
	   while(FindNextFile(hFile,&wInfo))
      {
      	if(_tcscmp(wInfo.cFileName,_T("..")) == 0)
         	continue;

         if((wInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
         {
            nPathCnt ++;
         	wsprintf(next,_T("%s\\%s\0\0"),szPath,wInfo.cFileName);
            dwFileSizeTotal += GetPathSize1(next,nFileCnt,nPathCnt);
         }
         else
         {
         	nFileCnt ++;
            dwFileSizeTotal += ((DWORDLONG)wInfo.nFileSizeHigh << 32) + wInfo.nFileSizeLow;
         }
      }
      FindClose(hFile);
      return dwFileSizeTotal;
   }
   else
   	return 0;
}

/*
BOOL CheckCDROM(LPTSTR szCdrom)
{
	char cDriveLetter[]="DEFGHIJKLMNOPQRSTUVWXYZ";
  	char szRootName[3];
   int i;

  	for(i=0;i<23;i++)
  	{
      szRootName[0]=cDriveLetter[i];
      szRootName[1]=':';
      szRootName[2]='\0';

    	if(GetDriveType(szRootName)==DRIVE_CDROM)
      {
      	wsprintf(szCdrom,(LPCTSTR)szRootName);
         return TRUE;
      }
	}
   return FALSE;
}
*/

int GetErrMessage(DWORD dwErrcode,LPTSTR lpstr,int nLen)
{
	if(lpstr == NULL)
		return 0;

	return FormatMessage( 
						FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						dwErrcode,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						lpstr,
						nLen,
						NULL 
						);
}

int WinWidth(HWND hWnd)
{
	RECT rt;
   GetWindowRect(hWnd,&rt);

   return (rt.right-rt.left);
}

int WinHeight(HWND hWnd)
{
	RECT rt;
   GetWindowRect(hWnd,&rt);

   return (rt.bottom-rt.top);
}

int ClientWidth(HWND hWnd)
{
	RECT rt;
   GetClientRect(hWnd,&rt);

   return rt.right;
}

int ClientHeight(HWND hWnd)
{
	RECT rt;
   GetClientRect(hWnd,&rt);

   return rt.bottom;
}

void EasyMove(HWND hWnd)
{
	SendMessage(hWnd,WM_NCLBUTTONDOWN,(WPARAM)HTCAPTION,MAKELPARAM(5,5));
}

BOOL SetAutoRun(LPCTSTR szKeyName,LPCTSTR szParam/*=NULL*/)
{
	HKEY  hKey1;

  	DWORD dwDisposition;
  	LONG  lRetCode;

   TCHAR  cmd[400];


   lRetCode = RegCreateKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),0, NULL, REG_OPTION_NON_VOLATILE,
                           KEY_WRITE,NULL,&hKey1,&dwDisposition);
   if (lRetCode != ERROR_SUCCESS)
      return FALSE;

   ZeroMemory(cmd,400);
   GetModuleFileName(m_hInst,cmd,400);

   if(szParam)
   {
   	lstrcat(cmd,_T(" "));
      lstrcat(cmd,szParam);
   }

   lRetCode = RegSetValueEx( hKey1,
                             szKeyName,
                             0,
                             REG_SZ,
                             (LPBYTE)cmd,
                             (DWORD)(_tcslen(cmd)));

	if(lRetCode != ERROR_SUCCESS)
   	return FALSE;

   return TRUE;
}

BOOL DelAutoRun(LPCTSTR szKeyName)
{
   TCHAR  s[256]=_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\");
   HKEY  hKey1;
  	DWORD dwDisposition;
  	LONG  lRetCode;

   lRetCode = RegCreateKeyEx(HKEY_LOCAL_MACHINE,s,0, NULL, REG_OPTION_NON_VOLATILE,
                             KEY_WRITE,NULL,&hKey1,&dwDisposition);

   if(lRetCode != ERROR_SUCCESS)
      return FALSE;

   lstrcat(s,szKeyName);
	if(RegDeleteValue(hKey1,s) == ERROR_SUCCESS)
   	return TRUE;
   else
   {
   	//Errmsg("错误：删除注册表\"HKEY_LOCAL_MACHINE\\%s\"出错.",s);
   	return FALSE;
   }
}

HANDLE RegOnlyWindow(LPCTSTR szMutex)
{
	HANDLE hImHere = CreateMutex(NULL,TRUE,szMutex);

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
	   CloseHandle(hImHere);
	   return NULL;
	}
   else
	   //第一次运行，不关闭句柄，阻止第二副本执行。退出时系统自动释放
   	return hImHere;
}

void PaintNull(HWND hWnd)
{
   PAINTSTRUCT ps;

   BeginPaint(hWnd,&ps);
   EndPaint(hWnd,&ps);
}

HANDLE NewThread(LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParam)
{
	//gdwThreadId ++;
	UINT nThreadID;
	return (HANDLE)_beginthreadex(NULL,0,(unsigned int (__stdcall *)( void * ))lpStartAddress,lpParam,0,&nThreadID);
	//return CreateThread(NULL,0,lpStartAddress,lpParam,0,&nThreadID);
}

int OSType(void)
{
	OSVERSIONINFOEX OsVersionInfo;
	BOOL bOK;
	
   memset(&OsVersionInfo,0,sizeof(OSVERSIONINFOEX));
   OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	
	bOK = GetVersionEx((LPOSVERSIONINFO)&OsVersionInfo);
   if(!bOK)
   {
      //失败，不支持OSVERSIONINFOEX，使用OSVERSIONINFO
      OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      if(!GetVersionEx((LPOSVERSIONINFO)&OsVersionInfo))
         return FALSE;
   }
	
	switch (OsVersionInfo.dwPlatformId)
	{
	case	VER_PLATFORM_WIN32_WINDOWS:
		if(OsVersionInfo.dwMajorVersion > 4 || 
			(OsVersionInfo.dwMajorVersion == 4 && OsVersionInfo.dwMinorVersion > 0))
			return OST_WIN98;
		else 
			return OST_WIN95;		
	case	VER_PLATFORM_WIN32_NT:
		if(OsVersionInfo.dwMajorVersion <= 4)
			return OST_WINNT;
		else if(OsVersionInfo.dwMajorVersion == 5)
		{
			if(bOK)
			{
				if(OsVersionInfo.wProductType == VER_NT_WORKSTATION)
					return OST_WIN2KPRO;
				else if(OsVersionInfo.wProductType == VER_NT_SERVER)
					return OST_WIN2KSERVER;
			}
			return OST_WIN2K;
		}
		break;
	case	VER_PLATFORM_WIN32s:
		return OST_WIN31;
	}
	return OST_UNKNOWN;
}

BOOL GetBmpSize(HBITMAP hbm,SIZE *size)
{
	BITMAP bm;

   if(GetObject(hbm,sizeof(BITMAP),&bm)==0)
   	return FALSE;

   size->cx=bm.bmWidth;
   size->cy=bm.bmHeight;

   return TRUE;
}
BOOL ChangeGray(BYTE *pSrcDib, int width, int height)  
{     
	BOOL   bSuccess=false;   
	for   (register   int   y=0;y<height;y++)   
	{   
		for(register   int   x=0;x<width;x++)   
		{    
			int temp = (BYTE)(0.30*pSrcDib[y*width*3 + x*3] + 0.59*pSrcDib[y*width*3 + x*3 + 1] 
			+ 0.11*pSrcDib[y*width*3 + x*3 + 2]);

			pSrcDib[y*width*3 + x*3] = pSrcDib[y*width*3 + x*3 + 1] = pSrcDib[y*width*3 + x*3 + 2] = temp;
		}   
	}   
	return   bSuccess;     

}
BOOL SaveBmp(HDC   hDC,HBITMAP hBitmap,LPCTSTR szFilename)
{
	andyFile rf = szFilename;
	BITMAP   Bm;   
	BITMAPINFO   BitInfo;   
	ZeroMemory(&BitInfo,   sizeof(BITMAPINFO));   
	BitInfo.bmiHeader.biSize   =   sizeof(BITMAPINFOHEADER);   
	BitInfo.bmiHeader.biBitCount   =   0;   

	if(!::GetDIBits(hDC,   hBitmap,   0,   0,   NULL,   &BitInfo,   DIB_RGB_COLORS))   
		return   FALSE;   

	Bm.bmHeight   =   BitInfo.bmiHeader.biHeight;   
	Bm.bmWidth     =   BitInfo.bmiHeader.biWidth;   

	BITMAPFILEHEADER         BmHdr;   

	BmHdr.bfType                 =   0x4d42;       //   'BM'   WINDOWS_BITMAP_SIGNATURE   
	BmHdr.bfSize                 =   (((3   *   Bm.bmWidth   +   3)   &   ~3)   *   Bm.bmHeight)     
		+   sizeof(BITMAPFILEHEADER)     
		+   sizeof(BITMAPINFOHEADER);   
	BmHdr.bfReserved1         =   BmHdr.bfReserved2   =   0;   
	BmHdr.bfOffBits             =   (DWORD)   sizeof(BITMAPFILEHEADER)     
		+   sizeof(BITMAPINFOHEADER);   

	BitInfo.bmiHeader.biCompression   =   0;   
	//   Writing   Bitmap   File   Header   ////   
	if(!rf.Create(RF_NEW))
	    return FALSE;

	if(!rf.Write(&BmHdr,sizeof(BITMAPFILEHEADER),TRUE))
	{
		rf.Close();
		return FALSE;
	}
	if(!rf.Write(&BitInfo.bmiHeader,sizeof(BITMAPINFOHEADER),TRUE))	
	{
		rf.Close();
		return FALSE;
	}
	BYTE   *pData   =   new   BYTE[BitInfo.bmiHeader.biSizeImage   +   5];   
	if(!::GetDIBits(hDC,   hBitmap,   0,   Bm.bmHeight,     
		pData,   &BitInfo,   DIB_RGB_COLORS))   
		return   FALSE;   
	   if(pData   !=   NULL)     
		if(!rf.Write(pData,BitInfo.bmiHeader.biSizeImage,TRUE))
		{
			Del(pData);
			rf.Close();
			return FALSE;
		}
		Del(pData);
		rf.Close();
		return TRUE;

}

BOOL SaveGrayBmp(HDC   hDC,HBITMAP hBitmap,LPCTSTR szFilename)
{
	andyFile rf = szFilename;
	BITMAP   Bm;   
	BITMAPINFO   BitInfo;   
	ZeroMemory(&BitInfo,   sizeof(BITMAPINFO));   
	BitInfo.bmiHeader.biSize   =   sizeof(BITMAPINFOHEADER);   
	BitInfo.bmiHeader.biBitCount   =   0;   

	if(!::GetDIBits(hDC,   hBitmap,   0,   0,   NULL,   &BitInfo,   DIB_RGB_COLORS))   
		return   FALSE;   

	Bm.bmHeight   =   BitInfo.bmiHeader.biHeight;   
	Bm.bmWidth     =   BitInfo.bmiHeader.biWidth;   

	BITMAPFILEHEADER         BmHdr;   

	BmHdr.bfType                 =   0x4d42;       //   'BM'   WINDOWS_BITMAP_SIGNATURE   
	BmHdr.bfSize                 =   (((3   *   Bm.bmWidth   +   3)   &   ~3)   *   Bm.bmHeight)     
		+   sizeof(BITMAPFILEHEADER)     
		+   sizeof(BITMAPINFOHEADER);   
	BmHdr.bfReserved1         =   BmHdr.bfReserved2   =   0;   
	BmHdr.bfOffBits             =   (DWORD)   sizeof(BITMAPFILEHEADER)     
		+   sizeof(BITMAPINFOHEADER);   

	BitInfo.bmiHeader.biCompression   =   0;   
	//   Writing   Bitmap   File   Header   ////   
	if(!rf.Create(RF_NEW))
		return FALSE;

	if(!rf.Write(&BmHdr,sizeof(BITMAPFILEHEADER),TRUE))
	{
		rf.Close();
		return FALSE;
	}
	if(!rf.Write(&BitInfo.bmiHeader,sizeof(BITMAPINFOHEADER),TRUE))	
	{
		rf.Close();
		return FALSE;
	}
	BYTE   *pData   =   new   BYTE[BitInfo.bmiHeader.biSizeImage   +   5];   
	if(!::GetDIBits(hDC,   hBitmap,   0,   Bm.bmHeight,     
		pData,   &BitInfo,   DIB_RGB_COLORS))   
		return   FALSE;   
	if(pData   !=   NULL)    
	{
		ChangeGray(pData,Bm.bmWidth,Bm.bmHeight);
		if(!rf.Write(pData,BitInfo.bmiHeader.biSizeImage,TRUE))
		{
			Del(pData);
			rf.Close();
			return FALSE;
		}
	}
	Del(pData);
	rf.Close();
	return TRUE;

}
/**************************************************************
//
//程序间传递数据
//
//参数：
//		hWnd:接收数据的目标窗口
//		szNAme:共享数据名称，该名称唯一标识该数据，目标窗口通过该名称取得数据
//		lpData:要传递的数据指针
//		dwSize:数据大小
//
****************************************************************/
BOOL SendShareData(HWND hWnd,LPCTSTR szName,LPVOID lpData,DWORD dwSize)
{
	HANDLE hfile;
	LPBYTE lpvMem,lpTemp=(LPBYTE)lpData;
	BOOL bOK;

	hfile = CreateFileMapping(
                (HANDLE)0xFFFFFFFF, 	// use paging file
                NULL,                	// no security attributes
                PAGE_READWRITE,      	// read/write access
                0,                   	// size: high 32-bits
                dwSize+sizeof(DWORD),			      	// size: low 32-bits
                szName);

	if(hfile == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hfile);
		return FALSE;
	}

	lpvMem = (LPBYTE)MapViewOfFile(hfile,FILE_MAP_WRITE,0,0,0);

	if(lpvMem == NULL)
	{
		CloseHandle(hfile);
		return FALSE;
	}

	for(DWORD i=0;i<dwSize;i++)
	{
		*lpvMem = *lpTemp;
		lpvMem ++;
		lpTemp ++;
	}

	bOK = SendMessage(hWnd,MYWM_SHAREDATA,dwSize,0);
	
	UnmapViewOfFile(lpvMem);
   CloseHandle(hfile);
	
	return bOK;
}

/**************************************************************
//
//接收另一程序发送的数据
//
//参数：
//		szNAme:共享数据名称，程序通过该名称取得数据
//		lpData:要接收数据的数据指针地址
//
****************************************************************/
BOOL GetShareData(LPCTSTR szName,LPVOID* lpData,DWORD dwSize)
{
	HANDLE hfile;

	hfile = OpenFileMapping(FILE_MAP_READ,FALSE,szName);
	if(hfile)
	{
		LPVOID lpvMem = (LPBYTE)MapViewOfFile(hfile,FILE_MAP_READ,0,0,0);

		if(lpvMem == NULL)
			return FALSE;
	
		*lpData = New(dwSize);
		if(*lpData == NULL)
		{
			UnmapViewOfFile(lpvMem);
			CloseHandle(hfile);
			return FALSE;
		}
		
		LPBYTE lpTemp1 = (LPBYTE)*lpData,lpTemp2 = (LPBYTE)lpvMem;

		for(DWORD i=0;i<dwSize;i++)
		{
			*lpTemp1 = *lpTemp2;
			lpTemp1 ++;
			lpTemp2 ++;
		}
		UnmapViewOfFile(lpvMem);
		CloseHandle(hfile);

		return TRUE;
	}
	return FALSE;
}

/*****************************************************************
//
//	允许/禁止应用程序权限
//		参数:
//		lpName:权限名（winnt.h）如SE_DEBUG_NAME可关闭“拒绝访问”的进程
//										  SE_SHUTDOWN_NAME可关机
//		bEnable:允许，禁止标记
//
*****************************************************************/
BOOL EnableNTRight(LPCTSTR lpName,BOOL bEnable/*=TRUE*/)
{
	BOOL bOK= FALSE;
	HANDLE hToken= NULL;

	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))
	{
		TOKEN_PRIVILEGES tkp;
		
		tkp.PrivilegeCount=1;
		LookupPrivilegeValue(NULL,lpName,&tkp.Privileges[0].Luid);
		tkp.Privileges[0].Attributes= bEnable?SE_PRIVILEGE_ENABLED:0;		
		AdjustTokenPrivileges(hToken,FALSE,&tkp,sizeof(tkp),NULL,NULL);
		
		bOK = (GetLastError()==ERROR_SUCCESS);
		
		CloseHandle(hToken);
	}
	return bOK;
}	
	
	
/*
void MoveImage(HWND hWnd,HBITMAP hBkg,HBITMAP hBmp,HBITMAP hTemp,int iWidth,int iHeight,POINTS Oldpt,LONG lParam)
{
	HDC hDC,hMemDC,hBkgDC,hTempDC;
   POINTS ppt,pt,npt;

   ppt=MAKEPOINTS(lParam);

   hDC=GetDC(hWnd);
   hMemDC=CreateCompatibleDC(hDC);
   hBkgDC=CreateCompatibleDC(hDC);
   hTempDC=CreateCompatibleDC(hDC);

   SelectObject(hMemDC,hBmp);
   SelectObject(hBkgDC,hBkg);
   SelectObject(hTempDC,hTemp);

   if((ppt.x-Oldpt.x)>iWidth || (ppt.y-Oldpt.y)>iHeight)
   {
   	//BitBlt(hTempDC,0,0,2*iWidth,2*iHeight,hBkgDC,Oldpt.x,Oldpt.y,SRCCOPY);
      BitBlt(hDC,Oldpt.x,Oldpt.y,iWidth,iHeight,hBkgDC,Oldpt.x,Oldpt.y,SRCCOPY);
      BitBlt(hDC,ppt.x,ppt.y,iWidth,iHeight,hMemDC,0,0,SRCCOPY);
   }
   else if((Oldpt.x-ppt.x)>iWidth || (Oldpt.y-ppt.y)>iHeight)
   {
   	//BitBlt(hTempDC,0,0,2*iWidth,2*iHeight,hBkgDC,Oldpt.x,Oldpt.y,SRCCOPY);
      BitBlt(hDC,Oldpt.x-iWidth,Oldpt.y-iHeight,2*iWidth,2*iHeight,hBkgDC,Oldpt.x-iWidth,Oldpt.y-iHeight,SRCCOPY);
      BitBlt(hDC,ppt.x,ppt.y,iWidth,iHeight,hMemDC,0,0,SRCCOPY);
   }
   else
   {
      if(ppt.x>=Oldpt.x)
      {
      	pt.x=Oldpt.x;
         npt.x=(short)(ppt.x-Oldpt.x);
      }
      else
      {
      	pt.x=ppt.x;
         npt.x=0;
      }
      if(ppt.y>=Oldpt.y)
      {
      	pt.y=Oldpt.y;
         npt.y=(short)(ppt.y-pt.y);
      }
      else
      {
      	pt.y=ppt.y;
         npt.y=0;
      }

      BitBlt(hTempDC,0,0,2*iWidth,2*iHeight,hBkgDC,pt.x,pt.y,SRCCOPY);
      BitBlt(hTempDC,npt.x,npt.y,iWidth,iHeight,hMemDC,0,0,SRCCOPY);
      BitBlt(hDC,pt.x,pt.y,2*iWidth,2*iHeight,hTempDC,0,0,SRCCOPY);
   }

   DeleteDC(hMemDC);
   DeleteDC(hBkgDC);
   DeleteDC(hTempDC);
   ReleaseDC(hWnd,hDC);
}

//#pragma argsused
BOOL CreateMaskBitmap(HBITMAP hbmSource,HBITMAP hbmDest,COLORREF crKey)
{
	HDC hDC,hMemDC,hMaskDC;
   BITMAP bm;
   COLORREF crTemp;
   int i,j;

   if(GetObject(hbmSource,sizeof(BITMAP),&bm)==0)
   	return FALSE;

   hDC=GetDC(NULL);
   hMemDC=CreateCompatibleDC(hDC);
   hMaskDC=CreateCompatibleDC(hDC);

   SelectObject(hMemDC,hbmSource);
   SelectObject(hMaskDC,hbmDest);

   BitBlt(hMaskDC,0,0,bm.bmWidth,bm.bmHeight,hDC,0,0,WHITENESS);

   for(i=0;i<bm.bmWidth;i++)
   {
   	for(j=0;j<bm.bmHeight;j++)
      {
      	crTemp=GetPixel(hMemDC,i,j);
      	if(crTemp!=crKey)
         	SetPixelV(hMaskDC,i,j,0x00000000);
         else
            SetPixelV(hMemDC,i,j,0x00000000);
      }
   }

   DeleteDC(hMemDC);
   DeleteDC(hMaskDC);
	ReleaseDC(NULL,hDC);
   return TRUE;
}

void MoveTransImage(HWND hWnd,HBITMAP hBkg,HBITMAP hBmp,HBITMAP hMaskBmp,HBITMAP hTemp,int iWidth,int iHeight,POINTS Oldpt,LONG lParam)
{
	HDC hDC,hMemDC,hBkgDC,hTempDC,hMaskDC;
   POINTS ppt,pt,npt;

   ppt=MAKEPOINTS(lParam);

   hDC=GetDC(hWnd);
   hMemDC=CreateCompatibleDC(hDC);
   hBkgDC=CreateCompatibleDC(hDC);
   hTempDC=CreateCompatibleDC(hDC);
   hMaskDC=CreateCompatibleDC(hDC);

   SelectObject(hMemDC,hBmp);
   SelectObject(hBkgDC,hBkg);
   SelectObject(hTempDC,hTemp);
   SelectObject(hMaskDC,hMaskBmp);

   if((ppt.x-Oldpt.x)>iWidth || (ppt.y-Oldpt.y)>iHeight)
   {
   	//BitBlt(hTempDC,0,0,2*iWidth,2*iHeight,hBkgDC,Oldpt.x,Oldpt.y,SRCCOPY);
      BitBlt(hDC,Oldpt.x,Oldpt.y,iWidth,iHeight,hBkgDC,Oldpt.x,Oldpt.y,SRCCOPY);

      BitBlt(hDC,ppt.x,ppt.y,iWidth,iHeight,hMaskDC,0,0,SRCAND);
      BitBlt(hDC,ppt.x,ppt.y,iWidth,iHeight,hMemDC,0,0,SRCINVERT);
   }
   else if((Oldpt.x-ppt.x)>iWidth || (Oldpt.y-ppt.y)>iHeight)
   {
   	//BitBlt(hTempDC,0,0,2*iWidth,2*iHeight,hBkgDC,Oldpt.x,Oldpt.y,SRCCOPY);
      BitBlt(hDC,Oldpt.x-iWidth,Oldpt.y-iHeight,2*iWidth,2*iHeight,hBkgDC,Oldpt.x-iWidth,Oldpt.y-iHeight,SRCCOPY);
      BitBlt(hDC,ppt.x,ppt.y,iWidth,iHeight,hMaskDC,0,0,SRCAND);
      BitBlt(hDC,ppt.x,ppt.y,iWidth,iHeight,hMemDC,0,0,SRCINVERT);
   }
   else
   {
      if(ppt.x>=Oldpt.x)
      {
      	pt.x=Oldpt.x;
         npt.x=(short)(ppt.x-Oldpt.x);
      }
      else
      {
      	pt.x=ppt.x;
         npt.x=0;
      }
      if(ppt.y>=Oldpt.y)
      {
      	pt.y=Oldpt.y;
         npt.y=(short)(ppt.y-pt.y);
      }
      else
      {
      	pt.y=ppt.y;
         npt.y=0;
      }

      BitBlt(hTempDC,0,0,2*iWidth,2*iHeight,hBkgDC,pt.x,pt.y,SRCCOPY);
      BitBlt(hTempDC,npt.x,npt.y,iWidth,iHeight,hMaskDC,0,0,SRCAND);
      BitBlt(hTempDC,npt.x,npt.y,iWidth,iHeight,hMemDC,0,0,SRCINVERT);
      BitBlt(hDC,pt.x,pt.y,2*iWidth,2*iHeight,hTempDC,0,0,SRCCOPY);
   }

   DeleteDC(hMemDC);
   DeleteDC(hBkgDC);
   DeleteDC(hTempDC);
   DeleteDC(hMaskDC);
   ReleaseDC(hWnd,hDC);
}

BOOL SaveBmp(HBITMAP hbm,LPCTSTR szFilename)
{
	BITMAPFILEHEADER bmfh;
   BITMAPINFOHEADER bmih;
   BITMAPINFO 		  bmif;
	BITMAP bm;
   andyFile rf = szFilename;
   LPBYTE lpbData;
   HDC hDC;

   if(GetObject(hbm,sizeof(BITMAP),&bm)==0)
      return FALSE;

   ZeroMemory(&bmfh,sizeof(BITMAPFILEHEADER));
   bmfh.bfType = 0x4d42;
   bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bm.bmWidth * bm.bmHeight * 3;
   bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

   if(!rf.Create(RF_NEW))
      return FALSE;

   if(!rf.Write(&bmfh,sizeof(BITMAPFILEHEADER),TRUE))
   {
      rf.Close();
      return FALSE;
   }

   ZeroMemory(&bmih,sizeof(BITMAPINFOHEADER));
   bmih.biSize = sizeof(BITMAPINFOHEADER);
   bmih.biWidth = bm.bmWidth;
   bmih.biHeight = bm.bmHeight;
   bmih.biPlanes = 1;
   bmih.biBitCount = 24;
   bmih.biCompression = BI_RGB;

   if(!rf.Write(&bmih,sizeof(BITMAPINFOHEADER),TRUE))
      return FALSE;

   lpbData = (LPBYTE)New(bm.bmWidth*bm.bmHeight*3);
   if(lpbData == NULL)
   {
      rf.Close();
      return FALSE;
   }

   bmif.bmiHeader = bmih;

   hDC = GetDC(HWND_DESKTOP);
   GetDIBits(hDC,hbm,0,bm.bmHeight,lpbData,&bmif,DIB_RGB_COLORS);
   ReleaseDC(HWND_DESKTOP,hDC);

   if(!rf.Write(lpbData,bm.bmWidth*bm.bmHeight*3,TRUE))
   {
      Del(lpbData);
      rf.Close();
      return FALSE;
   }

   Del(lpbData);
   rf.Close();
   return TRUE;
}

BOOL DoDraw(HWND hWnd,HBITMAP hbm,int x,int y,int width,int height,int xPos,int yPos,DWORD dwRop)
{
	HDC hDC,hMemDC;

   hDC = GetDC(hWnd);
   hMemDC = CreateCompatibleDC(hDC);

   SelectObject(hMemDC,hbm);
   BitBlt(hDC,x,y,width,height,hMemDC,xPos,yPos,dwRop);
   DeleteDC(hMemDC);
   ReleaseDC(hWnd,hDC);

   return TRUE;
}

BOOL DoPaint(HWND hWnd,HBITMAP hbm,int x,int y,int width,int height,int xPos,int yPos,DWORD dwRop)
{
	HDC hMemDC;
   PAINTSTRUCT ps;

   BeginPaint(hWnd,&ps);
   hMemDC=CreateCompatibleDC(ps.hdc);
   SelectObject(hMemDC,hbm);
   BitBlt(ps.hdc,x,y,width,height,hMemDC,xPos,yPos,dwRop);
   DeleteDC(hMemDC);
   EndPaint(hWnd,&ps);

   return TRUE;
}

BOOL DoDrawTrans(HWND hWnd,HBITMAP hbm,HBITMAP hbmMask,int x,int y,int width,int height,int xPos,int yPos)
{
	HDC hDC,hMemDC,hMaskDC;

   hDC = GetDC(hWnd);
   hMemDC = CreateCompatibleDC(hDC);
   hMaskDC = CreateCompatibleDC(hDC);

   SelectObject(hMemDC,hbm);
   SelectObject(hMaskDC,hbmMask);
   BitBlt(hDC,x,y,width,height,hMaskDC,xPos,yPos,SRCAND);
   BitBlt(hDC,x,y,width,height,hMemDC,xPos,yPos,SRCINVERT);
   DeleteDC(hMaskDC);
   DeleteDC(hMemDC);
   ReleaseDC(hWnd,hDC);

   return TRUE;
}
*/
