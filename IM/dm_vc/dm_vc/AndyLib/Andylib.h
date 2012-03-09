/**********************************************************************
//
//

//
//							AndySDK类库 andylib.lib
//作者：
//
//版本：1.0
//

//
//原文件名：andylib.h
//
//包含：
//    全局函数
//		andyStr  		-- 字符串操作类
//		andyFile 		-- 文件操作类
//		andyReg  		-- 注册表操作类
//		andyINI  		-- 配置文件操作类
//		andyDateTime	-- 日期/时间类
//
//内存操作函数New和Del
//
**********************************************************************/
#ifndef _INC_ANDYSDK
#define _INC_ANDYSDK

#if (!defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0400))
#define _WIN32_WINNT 0x0400
#endif

//#pragma warning(disable:4089)

//#ifndef _INC_WINDOWS
#include <windows.h>
#include <commdlg.h>
#include <assert.h>
//#endif
#include <stdio.h>
#include <tchar.h>
#include <shlobj.h>
#include "lang_string.h"

class andyStr;
class andyDateTime;
class andyFont;
class andyFile;

//未安装Platform SDK则需要以下定义
#ifndef IS_INTRESOURCE
#define IS_INTRESOURCE(_r) (((DWORD)(_r) >> 16) == 0)
#endif

//A-Z,0-9的键值，BC,VC头文件中均未定义
#define VK_A	0x41
#define VK_B	0x42
#define VK_C	0x43
#define VK_D	0x44
#define VK_E	0x45
#define VK_F	0x46
#define VK_G	0x47
#define VK_H	0x48
#define VK_I	0x49
#define VK_J	0x4A
#define VK_K	0x4B
#define VK_L	0x4C
#define VK_M	0x4D
#define VK_N	0x4E
#define VK_O	0x4F
#define VK_P	0x50
#define VK_Q	0x51
#define VK_R	0x52
#define VK_S	0x53
#define VK_T	0x54
#define VK_U	0x55
#define VK_V	0x56
#define VK_W	0x57
#define VK_X	0x58
#define VK_Y	0x59
#define VK_Z	0x5A
#define VK_0    0x30
#define VK_1    0x31
#define VK_2    0x32
#define VK_3    0x33
#define VK_4    0x34
#define VK_5    0x35
#define VK_6    0x36
#define VK_7    0x37
#define VK_8    0x38
#define VK_9    0x39

//自定义窗口消息
#define MYWM_ICON			(WM_APP+100)		//任务栏图标用
#define MYWM_COMMAND		(WM_APP+101)		//进程间传递命令用
#define MYWM_SHAREDATA	(WM_APP+102)		//程序间传递数据用
#define MYWM_ERROR		(WM_APP+103)		//检测到错误后的消息

//注册表操作返回值
#define REG_KEYADDED		10
#define REG_KEYCHANGED		11
#define REG_ERROR			12
#define REG_SUCCESS			13

#define THREADPROC			LPTHREAD_START_ROUTINE

#define OST_UNKNOWN		3000
#define OST_WIN31			1992
#define OST_WINNT			1994
#define OST_WIN95			1995
#define OST_WIN98			1998
#define OST_WIN2KPRO		2000
#define OST_WIN2KSERVER	2001
#define OST_WIN2K			2002
#define OST_WINXP			2003

#define WIN9X				OST_WIN98
#define NT					OST_WINNT
//窗口类名最大字符数
#define MAX_CLASSNAME	(80)

typedef struct tagREGVALINFO
{
	LPCTSTR szValName;
	DWORD	dwType;
	DWORD	cbData;
	DWORD	dwData;
	LPVOID	lpData;
}REGVALINFO,*LPREGVALINFO;


//窗口尺寸（常用）
#define WINVAR_DESKTOP_X		(GetSystemMetrics(SM_CXSCREEN))		//屏幕宽
#define WINVAR_DESKTOP_Y		(GetSystemMetrics(SM_CYSCREEN))       //屏幕高
#define WINVAR_FRAME_X 			(GetSystemMetrics(SM_CXSIZEFRAME))    //窗口边框宽度
#define WINVAR_FRAME_Y 			(GetSystemMetrics(SM_CYSIZEFRAME))    //窗口边框高度
#define WINVAR_MENU_Y			(GetSystemMetrics(SM_CYMENU))         //菜单高度
#define WINVAR_CAPTION_Y		(GetSystemMetrics(SM_CYCAPTION))      //标题栏高度
#define WINVAR_EDGE_X			(GetSystemMetrics(SM_CXEDGE))			//3D边框宽
#define WINVAR_EDGE_Y			(GetSystemMetrics(SM_CYEDGE))			//3D边框高
#define WINVAR_BORDER_X			(GetSystemMetrics(SM_CXBORDER))		//边框宽
#define WINVAR_BORDER_Y			(GetSystemMetrics(SM_CYBORDER))		//边框高
#define WINVAR(var)				(GetSystemMetrics(var))

//内建程序实例句柄，主窗口句柄
//extern HINSTANCE	m_hInst;
//extern HWND			m_hWnd;

//文件过滤标志
#define FILTER_FOLDER         FILE_ATTRIBUTE_DIRECTORY
#define FILTER_ALBUM          0x10000000
#define FILTER_PIC            0x20000000
#define FILTER_MEDIA          0x40000000
#define FILTER_OTHER          0x80000000
#define FILTER_HIDDEN			FILE_ATTRIBUTE_HIDDEN

#define THUMB_MASK				0x10000000
#define SIZE_MASK					0x0FFFFFFF


#define xchg(a,b) (b)^=(a)^=(b)^=(a)

#define MASK_MATCH(check,mask)	(((check) & (mask)) == (mask))

extern "C" {

//类库初始化
void InitAndyLib(HWND hWnd,HINSTANCE hInst = NULL);
HINSTANCE GetInstance();
HWND GetMainWnd();

//////////////////////////////////////////////////////////////////////////////////////
//内存分配/释放,函数失败时自动防护
/*#ifdef _DEBUG

// 分配、释放钩子函数原型
typedef void (WINAPI* _RMemAllocHook)(int type, DWORD size, char *file, int line);

_RMemAllocHook _SetNewAllocHook(_RMemAllocHook newhook);

const unsigned long		_DBGMEMAGIC = 0x4D454D52;		// 内存标签-'RMEM'
const unsigned long		_DBGMEMARP  = 0xFDFDFDFD;		// 防护区内容
const unsigned char		_DBGFILLCNT = 0xCD;				// 填充废料
const unsigned int		_DBGMEMLSIZE= 256;				// 废料填充尺寸的极限


// 内存节点结构
struct _tagDBGMEMNODE;

typedef struct _tagDBGMEMNODE
{
	unsigned long	magic;								// 内存标签
	unsigned long	clisize;							// 用户请求的实际内存尺寸
	char			*pfilename;							// 分配此内存块的源码位置
	int				line;								//
	struct _tagDBGMEMNODE	*prev;						// 前一个内存块的位置
	struct _tagDBGMEMNODE	*next;						// 下一个内存块的位置
} _DBGMEMNODE, *_LPDBGMEMNODE;


#define New(dwSize)		(_New(dwSize, __FILE__, __LINE__))
#define Del(lpv)		(_Del(lpv, __FILE__, __LINE__))

LPVOID  _New(DWORD dwSize, char *file, int line);
LPVOID  _Del(LPVOID lpv, char *file, int line);

BOOL	RL_IsValidMemAddr(LPVOID pmem);
DWORD	RL_GetTotalMemUsed(void);
DWORD	RL_GetTotalMemCnt(void);
BOOL	RL_MemCheck(void);
void	RL_CheckMemLeak(void);
DWORD	RL_GetMemSize(void *pv);

#else	// _DEBUG
*/
LPVOID  New(DWORD dwSize);
LPVOID  Del(LPVOID lpv);

#define RL_IsValidMemAddr(pmem)		(TRUE)
#define	RL_GetTotalMemUsed()		(0)
#define RL_GetTotalMemCnt()			(0)
#define RL_MemCheck()				(TRUE)
#define RL_CheckMemLeak()			((void*)0)
#define RL_GetMemSize(pv)			(0)

//#endif

DWORD  GetFreeMem(void);



//////////////////////////////////////////////////////////////////////////////////////
//任务栏图标控制
BOOL AddInTaskBar(HWND hWnd,HICON hIcon,LPCTSTR s,UINT uId = 0);
BOOL ModiInTaskBar(HWND hWnd,HICON hIcon,LPCTSTR s,UINT uId = 0);
BOOL DelInTaskBar(HWND hWnd,UINT uId = 0);



//////////////////////////////////////////////////////////////////////////////////////
//消息框
void Errmsg(LPCTSTR format,...);		//显示出错信息
BOOL Confirm(LPCTSTR format,...);   //询问确认
void MsgBox(LPCTSTR szTitle,LPCTSTR format, ...);	//最多MAX_PATH字数
int MsgBoxEx(LPCTSTR szTitle,UINT uType,DWORD dwMsgLength,LPCTSTR format, ...);	//带字数控制
void ShowHelpInfo(LPCTSTR szText,LPCTSTR szCaption,LPCTSTR lpszIcon);	//帮助信息，带图标


//////////////////////////////////////////////////////////////////////////////////////
//建目录（可多级目录一次建立）
BOOL MkDir(LPCTSTR szPath);
//执行文件（包括可执行文件和文档）
HINSTANCE andyExec(LPCTSTR lpFile,LPCTSTR lpDir=NULL,LPCTSTR lpParam=NULL,
						 LPCTSTR lpOp=NULL,INT nShowCmd=SW_SHOW);
//执行文件（调用CreateProcess）
int RunProcess(LPCTSTR lpFile,LPCTSTR lpDir=NULL,LPCTSTR lpParam=NULL,
							DWORD dwWait = 3000,int nShowCmd=SW_SHOW,
							DWORD dwCreateFlag=CREATE_DEFAULT_ERROR_MODE,
							LPPROCESS_INFORMATION lpProcessInformation = NULL);
//调用系统对话框选择路径
BOOL SelectDir(LPTSTR lpDir,LPCTSTR lpszTitle=LANSTR_SELDIR,
					UINT uFlag=BIF_RETURNONLYFSDIRS,BFFCALLBACK lpfn=NULL,
					LPARAM lParam=0,int iImage=0);
INT CALLBACK SelDirSpecInitProc(HWND hwnd,UINT uMsg,LPARAM lp,LPARAM pData);
//得到打开文件的当前文件指针
DWORD GetFilePointer(HANDLE hFile);
DWORDLONG GetPathSize(LPCTSTR lpszPathName,int* lpnFileCnt = NULL,int* lpnPathCnt = NULL);
//BOOL CheckCDROM(LPTSTR szCdrom);



//////////////////////////////////////////////////////////////////////////////////////
//窗口/程序相关
int WinWidth(HWND hWnd);
int WinHeight(HWND hWnd);
int ClientWidth(HWND hWnd);
int ClientHeight(HWND hWnd);
//在窗口任意位置移动窗口
void EasyMove(HWND hWnd);
//修改注册表设定程序自动运行
BOOL SetAutoRun(LPCTSTR szKeyName,LPCTSTR szParam = NULL);
BOOL DelAutoRun(LPCTSTR szKeyName);
//注册MUTEX，返回NULL则检测到有同一实例，即同一程序不可运行两次
HANDLE RegOnlyWindow(LPCTSTR szMutex);
//程序间传递数据
BOOL SendShareData(HWND hWnd,LPCTSTR szName,LPVOID lpData,DWORD dwSize);
//接收发送的数据
BOOL GetShareData(LPCTSTR szName,LPVOID* lpData,DWORD dwSize);

//空绘制操作
void PaintNull(HWND hWnd);



//////////////////////////////////////////////////////////////////////////////////////
//检测操作系统类型
int OSType();
HANDLE NewThread(LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParam);
//允许/禁止应用程序权限
BOOL EnableNTRight(LPCTSTR lpName,BOOL bEnable=TRUE);
//转换错误代码
int GetErrMessage(DWORD dwErrcode,LPTSTR lpstr,int nLen);

//BOOL DoDraw(HWND hWnd,HBITMAP hbm,int x,int y,int width,int height,int xPos,int yPos,DWORD dwRop);
//BOOL DoPaint(HWND hWnd,HBITMAP hbm,int x,int y,int width,int height,int xPos,int yPos,DWORD dwRop);
//BOOL DoDrawTrans(HWND hWnd,HBITMAP hbm,HBITMAP hbmMask,int x,int y,int width,int height,int xPos,int yPos);
//
//void MoveImage(HWND hWnd,HBITMAP hBkg,HBITMAP hBmp,HBITMAP hTemp,int iWidth,int iHeight,POINTS Oldpt,LONG lParam);
//void MoveTransImage(HWND hWnd,HBITMAP hBkg,HBITMAP hBmp,HBITMAP hMaskBmp,HBITMAP hTemp,int iWidth,int iHeight,POINTS Oldpt,LONG lParam);
//
//BOOL CreateMaskBitmap(HBITMAP hbmSource,HBITMAP hbmDest,COLORREF crKey);

//HANDLE Open(LPSTR szFilename,DWORD dwCreate);
//BOOL FailOpen(HANDLE hFile);
BOOL ChangeGray(BYTE *pSrcDib, int width, int height);
BOOL SaveBmp(HDC   hDC,HBITMAP hbm,LPCTSTR szFilename);
BOOL SaveGrayBmp(HDC   hDC,HBITMAP hBitmap,LPCTSTR szFilename);

BOOL GetBmpSize(HBITMAP hbm,SIZE *size);
//突破1024字节限制的wsprintf扩展版
void exwsprintf(LPTSTR s,LPCTSTR format,...);
}

enum
{
	RSTR_MAX					= 1024,
	RSTR_INIT				= 80,										//初始长度
	RSTR_STEP				= MAX_PATH,								//增加步长
	RSTR_INNER				= 1,										//SubString时指示返回限定头尾字符串中间的字符串
	RSTR_OUTER				= 2,										//SubString时指示返回包含限定头尾字符串的字符串
	RSTR_IGNORERIGHT		= 4,										//SubString时指示如找不到限定尾字符串则返回到结尾

	RSTR_WINPATH			= -32768,								//windows路径
	RSTR_SYSPATH			= -32767,       						//系统路径
	RSTR_TEMPPATH			= -32766,								//临时目录
	RSTR_CURRPATH			= -32765,								//执行文件所在路径，非系统当前路径
	RSTR_MYFILENAME		= -32764,								//执行文件带路径文件名
	//新定义在此加入,必须保证数值排序正确，即 RSTR_WINPATH 到 RSTR_PCNAME 数值递增
	//...
	RSTR_PCNAME = -32600									//计算机名
};


//////////////////////////////////////////////////////////////////////
//
//字符串操作类
//		说明：该类并未考虑全部情况，例如：浮点数到字符串的转换等，但实现
//		了最常用到的功能并在某些方面有所加强，例如增加了得到系统路径等系
//		统参数的复制构造函数。
//		实现了ANSI与UNICODE版本的通用代码，未实现UNICODE版本专用代码，
//		但已预留接口。
//
//////////////////////////////////////////////////////////////////////
class andyStr
{
public:
	//构造，复制构造函数
	andyStr():m_data(NULL),m_size(0),m_wData(NULL),m_wSize(0),m_len(0){memset(x,0,4);}
	andyStr(LPCTSTR str,DWORD size=0);
	//andyStr(LPSTR format,...);
	andyStr(int size);		//size:初始长度，<0且在RSTR_WINPATH~RSTR_PCNAME范围内为特殊字串
	andyStr(const andyStr& src);
	~andyStr();
		
	void Clear();
	
	int Load(UINT ident,HINSTANCE hInst = GetInstance());
	//载入特殊含义字符
	BOOL LoadSpecial(int nFlag);
	//UTF8字符串支持
	BOOL AssignUTF8Str(LPCTSTR str);

	int printf(LPCTSTR format, ...); // Returns formatted length
	int strcat(LPCTSTR format, ...); // Returns formatted length
	
	operator LPCTSTR() const{return string();}

	const andyStr& operator =(LPCTSTR str);
	//const andyStr& operator =(LPCWSTR str);
	const andyStr& operator =(TCHAR ch);
//	const andyStr& operator =(const _TUCHAR * psz);
	const andyStr& operator =(const andyStr& str);
	const andyStr& operator =(const int number);
	const andyStr& operator +=(const int number);
	const andyStr& operator +=(const andyStr& str);
	const andyStr& operator +=(LPCTSTR lpstr);
	const andyStr& operator +=(TCHAR ch);

	andyStr operator +(const andyStr& rhs) const;
	andyStr operator +(TCHAR ch) const;
	friend andyStr operator +(LPCTSTR lhs, const andyStr& rhs);
	friend andyStr operator +(TCHAR ch, const andyStr& rhs);

//#ifdef _UNICODE
//	const andyStr& operator=(char ch);
//	const andyStr& operator +=(char ch);
//	friend andyStr operator +(char ch, const andyStr& rhs);
//#endif	
	// Comparisons
	BOOL operator ==(const andyStr& str) const;
	BOOL operator ==(LPCTSTR str) const;
	BOOL operator !=(const andyStr& str) const;
	BOOL operator !=(LPCTSTR str) const;
	BOOL operator <(const andyStr& str) const;
	BOOL operator >(const andyStr& str) const;
	BOOL operator <=(const andyStr& str) const;
	BOOL operator >=(const andyStr& str) const;
	
	TCHAR& operator [](int index);
	
	int cmp(const andyStr& str) const;
	int cmpi(const andyStr& str) const; //ignorecase
	
	int GetLength();
	int len() const{return m_len;}
	LPCTSTR string() const {return (m_data)?m_data:_T("");}

	LPCTSTR UTF8String(int* nLen=NULL);
	WCHAR* wstring(int* nLen=NULL);
	DWORD size() const{return m_size;}
	BOOL isEmpty() const{return (m_data == NULL);}
	BOOL isClear() const{return (m_data == NULL || *m_data == '\0');}
	
	DWORD SetLength(DWORD size,BOOL bClear = FALSE);
	
	andyStr replace(LPCTSTR str1,LPCTSTR str2,BOOL bAll = TRUE) const;
	BOOL replace(andyStr& temp,LPCTSTR str1,LPCTSTR str2,BOOL bAll = TRUE) const;
	void replace(TCHAR c1,TCHAR c2,BOOL bAll = TRUE);
	andyStr SubString(int start,int count = 0) const;
	//取匹配的中间字串，例："---{123456}---"
	//SubString("{","}"),返回"123456",nIndex返回最后}的位置，uFlag如果指定RSTR_OUTER，则返回"{123456}"
	andyStr SubString(LPCTSTR szStart,LPCTSTR szEnd,int nStart=0,int* nIndex=NULL,UINT uFlag=RSTR_INNER) const;

	//ntimes:第ntimes次匹配返回(从1开始计数)
	int indexOf(LPCTSTR str,int start = 0,int ntimes = 1) const;
	int lastIndexOf(LPCTSTR str,int ntimes = 1) const;
	//计算含有多少str子串
	int count(LPCTSTR str) const;

	andyStr Insert(const andyStr& str, int index);
	int Delete(int index,int count=1);
	
	void toLower();
	void toUpper();
	void Trim();
	void LTrim();
	void RTrim();
	//添加count个c字符到串尾
	void Append(TCHAR c,int count);
	//添加lpstr和若干个c字符到串尾，总添加字符数为count
	//如果lpstr长度大于count，截断lpstr为count个字符
	void AppendSize(LPCTSTR lpstr,TCHAR c,int count);
     //modifide unicode 
#ifdef _UNICODE
	int toInt() const{return m_data?_wtoi(m_data):0;}
	double toFloat() const{return m_data?_wtof(m_data):0;}
#else
	int toInt() const{return m_data?atoi(m_data):0;}
	double toFloat() const{return m_data?atof(m_data):0;}
#endif

	
private:
	void resizeBuf(LPCTSTR str);
	void init();
	void Release();
	
private:
	DWORD m_size,m_len,m_wSize;
	LPTSTR m_data;
	WCHAR* m_wData;
	TCHAR x[4];
};

extern andyStr operator +(LPTSTR,const andyStr& rhs);


//////////////////////////////////////////////////////////////////////////////////////
//注册表操作类
class andyReg
{
public:
	andyReg();
   ~andyReg();

   void Close();

   //某个键下取值开始（不关闭该键，调用其他函数会自动关闭）
	BOOL Get(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,
				LPVOID lpData,DWORD dwDataSize,LPDWORD lpdwType=NULL,LPDWORD lpDataSize=NULL);
   DWORD GetInt(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,DWORD dwDef = 0);	//dwDef:失败时返回的默认值
   //在已打开的键下取值，以下两个函数必须在调用上面两个函数之一后调用
   BOOL Get(LPCTSTR szKey,LPVOID lpData,DWORD dwDataSize,LPDWORD lpdwType=NULL,LPDWORD lpDataSize=NULL);
   DWORD GetInt(LPCTSTR szKey,DWORD dwDef = 0);	//dwDef:失败时返回的默认值
   //单个值设置
   BOOL Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,LPCTSTR szVal);
   BOOL Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,DWORD dwVal);
   BOOL Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,LPVOID lpData,DWORD dwType,DWORD cbData);
   //多个值设置，Add所有键后调用Set
   BOOL Add(LPCTSTR szKey,LPVOID lpData,DWORD dwType,DWORD cbData,DWORD dwNum=25);
   BOOL Add(LPCTSTR szKey,DWORD dwData,DWORD dwNum=25);
   BOOL Add(LPCTSTR szKey,LPCTSTR szData,DWORD dwNum=25);
   BOOL Set(HKEY hKey,LPCTSTR szSubKey);

   BOOL Delete(HKEY hKey,LPCTSTR szSubKey);
   BOOL Delete(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey);

private:
	BOOL Check(DWORD dwNum);
   BOOL error(HKEY hKey,LPCTSTR szSubKey);
   BOOL DelNT(HKEY hKey,LPCTSTR szSubKey);
   void Destroy();

private:
	LPREGVALINFO m_lprv;
   DWORD m_dwKeyCnt;			//设置键记数
   DWORD m_dwKeyMax;			//设置键最大数
   HKEY m_hKey;
};

//配置文件操作类
class andyINI
{
public:
	andyINI();
   andyINI(LPCTSTR szFile);
   virtual ~andyINI();

   void SetFile(LPCTSTR format,...);
   int GetInt(LPCTSTR section,LPCTSTR key,int nDefault = 0);
   DWORD GetString(LPCTSTR section,LPCTSTR key,LPTSTR szBuf,int nBufSize,LPCTSTR szDefault=NULL);
   BOOL Write(LPCTSTR section,LPCTSTR key,LPCTSTR szBuf);
   BOOL Write(LPCTSTR section,LPCTSTR key,int nVal);
	BOOL DeleteKey(LPCTSTR section,LPCTSTR szKey);
	BOOL DeleteSection(LPCTSTR section);
	DWORD GetSections(LPTSTR lpBuffer,int nSize);
	DWORD GetKeys(LPCTSTR szSec,LPTSTR lpBuffer,int nSize);

   LPCTSTR GetIniFileName()const{return m_filename;}
   BOOL IsInited() {return bInited;}

private:
	TCHAR m_filename[MAX_PATH];
   BOOL bInited;
};

// 指定目录标签
typedef enum _tagRFSPath
{
	RFSP_MYFILENAME = 0,								//执行文件带路径文件名
	RFSP_OSCURRPATH,									//操作系统意义上的当前路径
	RFSP_CURRPATH,										//执行文件所在路径，非系统当前路径
	RFSP_WINPATH,										//windows路径
	RFSP_SYSPATH,		       							//系统路径
	RFSP_TEMPPATH,										//临时目录
	//RFSP_XXXX											//新类型在此加入
	RFSP_MAX
} RFSPATH, *LPRFSPATH;

// 读写及共享标签
#define RF_READ					GENERIC_READ
#define RF_WRITE					GENERIC_WRITE
#define RF_READWRITE				(GENERIC_READ | GENERIC_WRITE)
#define RF_SHARE_READ			FILE_SHARE_READ
#define RF_SHARE_WRITE			FILE_SHARE_WRITE
#define RF_SHARE_READWRITE		(FILE_SHARE_READ | FILE_SHARE_WRITE)
#define RF_SHARE_NONE			(0)
#define RF_NEW						(50100)
#define RF_APPEND					(50101)
#define RF_EDIT					OPEN_ALWAYS

//文件状态标志，注意：修改定义值必须按现有的大小排序，
//即: RFA_VOID < RFA_FILE < RFA_PATH < RFA_NOTEXIST < RFA_BADFILENAME
#define RFA_VOID				12000
#define RFA_FILE				12001
#define RFA_PATH				12002
#define RFA_NOTEXIST			12003
#define RFA_BADFILENAME			12004
//for ReadAll，如果文件尺寸大于该尺寸则放弃
#define RF_READALL_SIZE		102400000

//文件读写错误代码
#define RFE_EOF				(-1)
#define RFE_LINEOVERFLOW	(-2)
#define RFE_ERROR				(-3)
#define RFE_FILETOOBIG		(-4)
#define RFE_MEMERROR			(-5)
//文件操作类
class andyFile
{
public:
	// 预关联操作
	andyFile();
	andyFile(LPCTSTR szFilename);
	andyFile& operator=(LPCTSTR szFilename){SetFile(szFilename);return *this;}

	void SetFile(LPCTSTR format, ...);
	void SetFile(HANDLE hFile);
	void SetFileName(LPCTSTR szFile);
	int  Select(LPCTSTR szFileType,LPCTSTR szTitle = NULL,BOOL bSave=FALSE,
					HWND hWnd=GetMainWnd(),LPCTSTR dlgEx=NULL,LPOFNHOOKPROC hookproc=NULL,
					LONG lCustData=0);

	// 实关联操作
	andyFile(HANDLE hFile){Init();SetFile(hFile);}
	//dwShare:默认参数需要与Create函数内部CreateFile的share参数一致，否则调用Create
	//但未关闭文件时另一对象调用Open打开同一文件会失败
	BOOL Open(DWORD dwAction=RF_READ,DWORD dwShare=RF_SHARE_READ|FILE_SHARE_WRITE,DWORD dwAttr=FILE_ATTRIBUTE_NORMAL);
	virtual BOOL Open(LPCTSTR lpFilename, DWORD dwAction=RF_READ, DWORD dwShare=RF_SHARE_READ,DWORD dwAttr=FILE_ATTRIBUTE_NORMAL);
	BOOL Create(DWORD dwCreate = RF_EDIT,DWORD dwAttr=FILE_ATTRIBUTE_NORMAL);
	virtual BOOL Create(LPCTSTR lpFilename, DWORD dwCreate = RF_EDIT);
	BOOL CreateTemp(LPCTSTR prefix=NULL);
	
	// 解关联操作
	virtual void Close();	
	virtual ~andyFile();
	
	// 关联对象操作
	virtual BOOL Read(LPVOID lpBuf,DWORD dwNum,BOOL bCheckReadNum=FALSE,LPDWORD lpTrueReadNum=NULL);
	int ReadLine(LPVOID lpBuf,DWORD dwNum);
	int ReadAll(TCHAR **ppbuf=NULL,UINT uMaxSize=RF_READALL_SIZE);
	
	virtual BOOL Write(const LPVOID lpBuf,DWORD dwNum,BOOL bCheckWriteNum=FALSE,LPDWORD lpTrueWriteNum=NULL);
	BOOL WriteLine(LPCTSTR lpBuf)
	{
		if(Write((LPVOID)lpBuf,_tcsclen(lpBuf)))
			return Write(_T("\r\n"),2);
		else
			return FALSE;
	}
	
	virtual DWORD ReadWriteNum() const {return m_dwRWNum;}
	
	virtual void Flush(void);
	virtual void LockRWArea(DWORD dwPos, DWORD dwCount);
	virtual void UnlockRWArea(DWORD dwPos, DWORD dwCount);
	
	DWORD Seek(LONG lDistance,DWORD dwMethod=FILE_BEGIN) const;
	DWORD SeekToBegin(void) const;
	DWORD SeekToEnd(void) const;
	BOOL SetEndOfFile(int pos = -1);

	DWORD GetRWLocation() const;
	void  SetSize(DWORD dwNewLen);
	DWORD Size(void);

	HANDLE Handle(){return m_hFile;}

	LPCTSTR Fullname(void) const;
	LPCTSTR Drivename(void) const;
	LPCTSTR Pathname(void) const;
	LPCTSTR Filename(void) const;
	LPCTSTR Titlename(void) const;
	LPCTSTR Extname(void) const;
	
	// 通用服务
	BOOL		isExist(LPCTSTR lpPathFilename=NULL);
	BOOL		isFile(LPCTSTR lpPathFilename=NULL);

	BOOL		copyTo(LPCTSTR lpNewName,BOOL bForceOverWrite=FALSE);
	BOOL		moveTo(LPCTSTR lpNewName,BOOL bForceOverWrite=FALSE);
	BOOL		delIt();
	BOOL		reName(LPCTSTR lpNewName,BOOL bForceOverWrite=FALSE);

	DWORD		GetAttrib(LPCTSTR lpszfilename = NULL);
	BOOL		SetAttrib(DWORD dwFileAttributes);
	BOOL		GetFileTime(LPFILETIME lpCreate, LPFILETIME lpLast, LPFILETIME lpModify);
	BOOL		SetFileTime(LPFILETIME lpCreate, LPFILETIME lpLast, LPFILETIME lpModify);

	BOOL		mdir(LPCTSTR szPath);
	HINSTANCE	exec(LPCTSTR lpFile,LPCTSTR lpDir=NULL,LPCTSTR lpParam=NULL,LPCTSTR lpOp=NULL,INT nShowCmd=SW_SHOW);
	BOOL		SelectDir(LPTSTR lpDir,LPCTSTR lpszTitle=_T("选择路径"),
								UINT uFlag=BIF_RETURNONLYFSDIRS,BFFCALLBACK lpfn=NULL,
								LPARAM lParam=0,int iImage=0);
	
	static unsigned __int64 GetDiskFreeSize(LPTSTR lpRoot);

	LPVOID Buffer(){return m_lpFilebuf;}

private:
	void Init(void);
	//void SplitFilename(void);
	int  check(void);
	void Destroy(void);
	//BOOL isValidName(void);
	void ClearFileName();
	void PrepareDestFilename(LPCTSTR szFilename,LPTSTR szRet);
	//static BOOL is_dir_sep(TCHAR ch){return (ch == '\\' || ch == '/');}
	
protected:
	HANDLE m_hFile;							//文件句柄
	
private:
	TCHAR  m_szFilename[MAX_PATH];//文件名(带全路径)
	TCHAR  m_szDrive[4];			//驱动器名(eg: c:\da\a.txt--return "c:")
	LPTSTR m_lpszPath;				//文件路径(eg: c:\da\a.txt--return "C:\da")
	LPTSTR m_lpszFilename;			//文件名(eg: c:\da\a.txt--return "a.txt")
	LPTSTR m_lpszTitlename;		//文件名(eg: c:\da\a.txt--return "a")
	LPTSTR m_lpszExtname;			//文件扩展名(eg: aa.txt--return "txt")
	
	LPVOID m_lpFilebuf;					//文件全读数据缓冲区
	DWORD m_bufSize;						//缓冲区尺寸
	DWORD  m_dwRWNum;						//文件实际读写长度
	
	int   m_aAttrib;						//关联属性
	BOOL  m_bExtern;						//m_Handle是否是导入句柄

	DWORD m_dwFileAttr;						//文件属性
	DWORD m_dwFileSize;						//文件尺寸
};

class andyMappingFile:public andyFile
{
public:
	andyMappingFile():m_hMapping(NULL){}
	andyMappingFile(HANDLE hFile):andyFile(hFile),m_hMapping(NULL){}
	virtual ~andyMappingFile();

	HANDLE CreateMapping(DWORD dwProtectFlag,DWORD dwMaxSizeLow=0,DWORD dwMaxSizeHigh=0,             // low-order DWORD of size
								LPCTSTR lpName = NULL);
	LPBYTE CreateView(DWORD dwAccess,DWORD dwNumber,HANDLE hMap=NULL,DWORD dwOffsetLow=0,DWORD dwOffsetHigh=0);
	BOOL DeleteView(LPVOID lpv);
	BOOL UnMap(HANDLE hMap=NULL);

private:
	HANDLE m_hMapping;
};

//异步读写文件类
class andyAsynFile
{
public:
	andyAsynFile();
	~andyAsynFile();

	virtual BOOL	Create(LPCTSTR lpFilename);
	virtual BOOL	Open(LPCTSTR lpFilename);
	virtual void	Close(void);
	
	virtual int		Read(LPVOID lpBuf,DWORD dwNum,DWORD offset=0);
	virtual int		Write(const LPVOID lpBuf,DWORD dwNum,DWORD offset=0);

	BOOL			ConfirmOver(BOOL wait=FALSE);
	BOOL			QuestionResult(LPDWORD dwRWTure=NULL, LPDWORD errCode=NULL);

	DWORD			GetFileLength(void);

	
private:
	HANDLE			m_hFile;							//文件句柄
	HANDLE			m_hEvent;							//读写完成

	DWORD			m_dwLoFileSize;						//文件尺寸
	DWORD			m_dwHiFileSize;

	DWORD			m_dwTrue;							//实际的读写尺寸

	OVERLAPPED		m_sOverLapped;						//异步行参结构
	int				m_OSType;							//操作系统类型
};


class andyDir
{
public:
	andyDir();
   ~andyDir();
	andyDir(LPCTSTR szDir);

   void SetDir(LPCTSTR format,...);
	LPCTSTR Select(LPCTSTR lpInitDir=NULL,LPTSTR lpDir=NULL);
	LPCTSTR Getdir(){return m_szDir;}
   BOOL IsExist();
	BOOL Create();

	//进入下一级目录，支持"\\",".."，同DOS的CD命令
	void cd(LPCTSTR lpDir);

   BOOL BeginFindFile(LPCTSTR szFilename=NULL,LPTSTR lpBuf=NULL);
	BOOL FindNext(LPTSTR lpBuf);
	BOOL FindClose();
	WIN32_FIND_DATA* GetCurrFindData(){return &m_data;}

private:
   HANDLE m_handle;
	WIN32_FIND_DATA m_data;
	TCHAR m_szDir[MAX_PATH],m_szCurrDir[MAX_PATH];
};

//日期/时间类
class andyDateTime
{
public:
	andyDateTime();
	virtual ~andyDateTime(){}

	//绑定到系统时间，TRUE:每次调用year()等函数取系统时间
	//否则直接返回m_time的相应字段，如果m_time未初始化，
	//自动取系统时间。
	//提供本函数的目的是应付日期/时间需要计算的情况，应该
	//把参数设为FALSE，否则计算后一取任一日期/时间段，计算
	//结果就会被系统时间冲掉。
	void BindRealTime(BOOL bValid)
	{
		m_bReCount = bValid;
		GetLocalTime(&m_time);
		GetLocalTime(&m_tmptime);
		if(bValid)
			m_lptime = &m_time;
		else
			m_lptime = &m_tmptime;
	}

	void GetTime(){GetLocalTime(&m_time);}
	//获取系统时间，自动调用GetTime()
	int GetYear();
   int GetMonth();
   int GetDay();
   int GetDayOfWeek();
   int GetHour();
   int GetMinute();
   int GetSecond();
	
	//获取当前设定时间，直接返回时间字段，不调用GetTime
	int year(){return (int)m_lptime->wYear;}
   int month(){return (int)m_lptime->wMonth;}
   int day(){return (int)m_lptime->wDay;}
   int dayOfWeek(){return (int)m_lptime->wDayOfWeek;}
   int hour(){return (int)m_lptime->wHour;}
   int minute(){return (int)m_lptime->wMinute;}
   int second(){return (int)m_lptime->wSecond;}
	int ms();			//毫秒

	//日期/时间字符串
	//length对于年份，=2则为短格式（如2004年则返回"04"），其余
	//数值则为4位字符串
	//对于其他日期/时间，=2则为固定两位字符串，（如1返回"01"，
	//29返回"29"），其余数值则根据具体日期/时间返回1或2位字符串
	//因此只有length=2的情况才会有相应操作，其余均采用默认操作。
	LPCTSTR year2str(int length=0);
	LPCTSTR month2str(int length=0);
	LPCTSTR day2str(int length=0);
	LPCTSTR hour2str(int length=0);
	LPCTSTR minute2str(int length=0);
	LPCTSTR second2str(int length=0);
	LPCTSTR date2str();
	LPCTSTR time2str();

   //设置日期，时间，参数如取默认值则采用当前时间
   BOOL SetDate(WORD month = 0,WORD day = 0,WORD year = 0,BOOL bSetSystem=TRUE);
   BOOL SetTime(WORD hour = 61,WORD minute = 61,WORD second = 61,BOOL bSetSystem=TRUE);

	//LPCTSTR FormatString(LPCTSTR szFormat);

	andyDateTime& operator =(const andyDateTime& rdt);
	//采用以下方式赋值自动置m_bReCount为FALSE，即不与系统时间绑定。	
	andyDateTime& operator =(const SYSTEMTIME systime);
	andyDateTime& operator =(const FILETIME filetime);
	
	//转换到文件时间
	BOOL GetFileTime(LPFILETIME lpft);

	SYSTEMTIME* IncMonth(int nStep,SYSTEMTIME* lptime = NULL);
	
protected:
	LPCTSTR formatStr(LPTSTR s,int nval,int length);

protected:
	SYSTEMTIME m_time,m_tmptime,*m_lptime;
	BOOL m_bReCount;
	//andyStr m_str;
	TCHAR m_str[24];
};


#define ANDYFONT_BOLD				1	//黑体
#define ANDYFONT_ITALIC				2	//斜体
#define ANDYFONT_UNDERLINE			4	//下划线
#define ANDYFONT_STRIKEOUT			8	//删除线
#define ANDYFONT_PIXEL				16	//字体大小为像素

class andyFont
{
public:
	andyFont();
	andyFont(HFONT hfont):m_crColor(0){SetFont(hfont);}
	andyFont(LPCTSTR szFontname,int size,int nStyle=0,int angle=0);
	~andyFont();
	//nStyle:=LOGFONT结构lfWeight值+ANDYFONT_xxx一个或多个的或（|）值
	BOOL SetFont(LPCTSTR szFontname,int size,int nStyle=0,int angle=0);
	BOOL SetFont(LOGFONT* lpLogFont);
	BOOL SetFont(HFONT hfont);
	void SetFontColor(COLORREF cr){m_crColor = cr;}
	COLORREF GetFontColor(){return m_crColor;}
	//设置为指定窗口的字体
	BOOL SetFont(HWND hWnd);
	void SetWindowSFont(HWND hWnd,LPCTSTR szFontname=NULL,int size=0,int nStyle=0);
	HFONT Select(HWND hWnd = NULL);

	HFONT GetFont(){return m_font;}
	BOOL GetLogFont(LOGFONT* lpLogFont);
	BOOL GetWindowLogFont(HWND hWnd,LOGFONT* lpLogFont);
	//自动调整字体宽度使能在w,h指定的矩形中完整显示
	HFONT AutoAdjust(LPCTSTR szText,int w,int h,HDC hDC/* = NULL*/);
	//指定字体倾斜度
	HFONT SetFontAngle(int nAngle);	
	
	//获取字体尺寸，lpszText如果为空，为字符A的尺寸。lpSize返回要获取的尺寸，如果lpSize为空，函数返回字体高度
	int GetFontSize(LPCTSTR lpszText=NULL,LPSIZE lpSize=NULL);

private:
	HFONT m_font;
	COLORREF m_crColor;
	LOGFONT m_lpMainFontRec;
	BOOL m_bExtern;
};

/*****************************************************
//
//信息显示类
//		程序中有时需要记录或显示一些操作或错误信息，
//		本类提供该种便利。
//
*****************************************************/
#define ANDYMSG_NONE				0		//不处理
#define ANDYMSG_LOGFILE			1		//记录到文件
#define ANDYMSG_SENDMESSAGE	2		//发送消息
#define ANDYMSG_ALERT			4		//弹出消息框显示
#define ANDYMSG_DEBUGOUT		8		//显示到Debugger窗口
#define ANDYMSG_RECTIME			0x00010000		//记录该显示该信息的时间
#define ANDYMSG_NORETURN		0xF0000000		//仅用于EDIT控件显示，无回车

class AndyMsgClass;

class AndyMsg
{
public:
	AndyMsg();
	virtual ~AndyMsg();

	void SetMsgMode(DWORD mode,BOOL bOverWrite=FALSE);
	DWORD GetMsgMode(){return m_msgMode;}

	void SetMsgFileSize(int maxSize);
	BOOL SetMsgFile(LPCTSTR szfilename,int maxSize=-1);
	BOOL ClearMsgFile();
	BOOL DeleteMsgFile();
	int GetMsgFileSize();
	
	void SetMsgMessage(HWND hWnd,UINT msg,BOOL bWithLine=TRUE);
	//记录数据，仅用于记录日志文件
	BOOL DataOut(LPVOID lpBuf,DWORD dwSize);

	BOOL MsgFormatOut(DWORD dwMsgId);
	void MsgOut(LPCTSTR format,...);
	void MsgOutFast(LPCTSTR format);
	void ErrOut(int errCode,LPCTSTR szErr);

	void SendMsgMessage(LPCTSTR lpMsg,int nErr=0,BOOL bError=FALSE);
	void PostMsgMessage(LPCTSTR lpMsg,int nErr=0,BOOL bError=FALSE);
	void OutFile(LPCTSTR lpMsg);

	LPCTSTR GetLastMsg(){return m_str;}
	DWORD GetLastErrCode(){return m_dwLastErrorCode;}

private:
	DWORD m_msgMode;
	CRITICAL_SECTION msg_critical;
	andyFile* m_lprf;
	int m_nMaxFileSize;
	HWND m_hWnd;
	UINT m_msg;
	LPTSTR m_str;
	DWORD m_dwLastErrorCode;
	andyDateTime m_rdt;
	
	friend class AndyMsgClass;
};

/*****************************************************
//
//信息显示类
//		对AndyMsg的再次封装，以AndyMsg的对象指针为成员函数，
//		供需要信息显示功能的类自本类继承，以灵活方便配置。
//		提供该类主要原因是有些类可能需要信息显示，也可能不
//		需要，如果继承自AndyMsgs则可能包含无用代码和数据，
//		自本类继承只有一个指针，减少内存和数据占用。
//
*****************************************************/
class AndyMsgClass
{
public:
	AndyMsgClass():m_msg(NULL){}
	virtual ~AndyMsgClass(){delete m_msg;}
		
	AndyMsg* GetMsg()
	{
		if(m_msg == NULL)
			m_msg = new AndyMsg;
		return m_msg;
	}

	void SetMsgMode(DWORD mode,BOOL bOverWrite=FALSE)
	{
		try
		{
			GetMsg()->SetMsgMode(mode,bOverWrite);
		}
		catch(...)
		{
		}
	}

	BOOL SetMsgFile(LPCTSTR szfilename,int maxSize=-1)
	{
		try
		{
			return GetMsg()->SetMsgFile(szfilename,maxSize);
		}
		catch(...)
		{
		}
		return FALSE;
	}

	void SetMsgMessage(HWND hWnd,UINT msg,BOOL bWithLine=TRUE)
	{
		try
		{
			GetMsg()->SetMsgMessage(hWnd,msg,bWithLine);
		}
		catch(...)
		{
		}		
	}

protected:
	AndyMsg* m_msg;
};

/*/#if(_WIN32_WINNT < 0x0400)
WINBASEAPI
BOOL
WINAPI
TryEnterCriticalSection(LPCRITICAL_SECTION);
//#endif /* _WIN32_WINNT >= 0x0400 */

//线程临界区同步类
class AndyDataGuard
{
public:
	AndyDataGuard(){InitializeCriticalSection(&m_crs);}
	~AndyDataGuard(){DeleteCriticalSection(&m_crs);}

	void Enter(){EnterCriticalSection(&m_crs);}
	void Leave(){LeaveCriticalSection(&m_crs);}
	//95,98不支持
	BOOL TryEnter();//{return TryEnterCriticalSection(&m_crs);}

private:
	CRITICAL_SECTION m_crs;
};

typedef struct 
{
	int  m_nLockCount;
	int m_nEnterCount;
	DWORD m_dwThreadId;	
}DGEX_SHAREINFO,*LPDGEX_SHAREINFO;
//支持跨进程同步类，AndyDataGuard升级版
class AndyDataGuardEx
{
public:
	AndyDataGuardEx();						//单进程
	AndyDataGuardEx(LPCTSTR szName);		//跨进程同步
	~AndyDataGuardEx();

	void Enter();
	void Leave();
	BOOL TryEnter(DWORD dwWaitMS = 1000);		//等待时间，单位：毫秒
	BOOL IsSingle() const{return (m_hFileMap == NULL);}

private:
	HANDLE m_hEvent,m_hFileMap;
	LPDGEX_SHAREINFO m_info;
};

//ANDYVCLASS类作用为转换各类的成员数据和成员函数的指针类型，
//该类本可仅声明，无任何定义和实现，但在BC运行正常，
//VC则表现为sizeof(ANDYVCLASS)异常，因此必须有实现(加{}就行)。
class ANDYVCLASS
{
public:
	ANDYVCLASS(){}
	~ANDYVCLASS(){}
};

class AndyThread;

template <typename T>
struct AndyThreadInfo
{
	typedef DWORD (T::*THREADFUNC1)(LONG);
	T* m_obj;
	AndyThread* m_thread;
	LONG m_param;
	THREADFUNC1 m_lpthreadFunc;	
};
//模板
typedef AndyThreadInfo<ANDYVCLASS> AndyThInfo;
typedef AndyThreadInfo<ANDYVCLASS>::THREADFUNC1 THREADFUNC;

class AndyThread
{
public:
	AndyThread();
	~AndyThread();

	template<typename tname>
	BOOL Start(tname* obj,THREADFUNC func,LONG param = 0)
	{		
		if(m_handle || obj == NULL || func == NULL)
			return FALSE;	//已有线程运行

		AndyThreadInfo<tname>* lpv;
		if(m_info)
		{
			if((LONG)m_info->m_obj == (LONG)obj)
				lpv = (AndyThreadInfo<tname>*)m_info;
			else
			{
				delete m_info;
				lpv = new AndyThreadInfo<tname>;
			}
		}
		else
			lpv = new AndyThreadInfo<tname>;
		lpv->m_obj = obj;
		lpv->m_thread = this;
		lpv->m_param = param;
		lpv->m_lpthreadFunc = (AndyThreadInfo<tname>::THREADFUNC1)func;
		
		m_info = (AndyThreadInfo<ANDYVCLASS>*)lpv;
		m_handle = NewThread((THREADPROC)ThreadProc,m_info);
		return (BOOL)m_handle;
	}
	BOOL Start(THREADPROC proc,LONG param = 0);

	void Wait(BOOL bForceWait=FALSE);
	void Continue();
	void Pause();
	void Resume();
	BOOL SetPriority(int nPriority);
	
	//退出线程，该函数试图让线程终止，如果超过参数设定的时间
	//（秒）线程未退出，强行结束线程。默认等待30秒。
	//等待时间建议不要设得太短，以让线程有机会自动完成退出。
	void Quit(int nWaitSecond = 30);
	//可由线程调用，在线程内退出，本函数不等待线程退出，立即返回。
	void ThreadQuit(int nWaitSecond = 30);
	
	//设置线程退出标志，如果线程有等待外部事件，可以先设置标志然后设置事件以使线程退出
	void SetQuit(){m_bQuit = TRUE;}
	//查询线程退出标志
	BOOL IsRunning(){return !m_bQuit;}
	//查询线程是否已启动
	BOOL IsStarted(){return (BOOL)m_handle;}
	//查询线程是否处于等待状态
	BOOL IsWaitting(){return (m_nEvCnt == 0);}
	BOOL CanContinue(DWORD dwMilSec = INFINITE);
	DWORD GetExitCode();

	BOOL PostMessage(UINT uMsg,WPARAM wParam,LPARAM lParam);
	
private:
	static DWORD WINAPI ThreadProc(AndyThInfo* obj);
	static DWORD WINAPI QuitProc(AndyThread* obj);
	//通知线程结束（由线程处理函数退出时调用）
	DWORD NotifyThreadQuit();
	DWORD WaitQuit();

private:
	HANDLE m_handle,m_event;
	BOOL m_bQuit;	
	int m_nEvCnt,m_nWaitSecond;
	DWORD m_dwId,m_dwExitCode;
	//同步对象，在多个线程中操作本对象需要进行同步
	AndyDataGuard m_crl;
	AndyThInfo* m_info;
};

//服务器日志
class AndyServLogFile
{
public:
	AndyServLogFile();
	~AndyServLogFile();
	
	void SetLogFile(LPCTSTR lpszLogfile,int nMaxSize,BOOL bWithDate=FALSE);
	BOOL WriteLog(LPCTSTR lpBuf,LPCTSTR lpBufNext=NULL);
	BOOL WriteLog(LPVOID lpBuf,int nSize);
	BOOL WriteLogLine(LPCTSTR lpBuf,LPCTSTR lpBufNext=NULL);
	BOOL WriteError(DWORD dwErrCode);
	
	DWORD MonDateProc(LONG);
	
	LPCTSTR GetLogFileName(){return (m_rf?m_rf->Filename():_T(""));}
	BOOL IsWithDate(){return (BOOL)m_thread;}
	DWORD GetMaxSize(){return m_nMaxFileSize;}

private:
	BOOL WriteLog(LPVOID lpBuf,int nSize1,LPVOID lpBufNext,int nSize2,BOOL bSetNewLine,BOOL bAddWrap);
	void OnDateChange();
	
private:
	DWORD m_nMaxFileSize;
	int m_nCurrDay;
	BOOL m_bNewLine;
	LPTSTR m_lpszErr;
	TCHAR m_szFilePrefix[MAX_PATH],m_szDate[32];
	AndyDataGuard m_ctl;	
	AndyThread* m_thread;
	andyDateTime m_rdt;
	andyFile* m_rf;
};

//#ifndef MAKE_SELF_LIB
////#pragma comment(lib, "oleaut32.lib")
//#ifdef _DEBUG
//#   ifdef _UNICODE
//#       pragma comment(lib, "andylibud.lib")
//#   else
//#       pragma comment(lib, "andylibd.lib")
//#   endif
//#else
//#   ifdef _UNICODE
//#       pragma comment(lib, "andylibu.lib")
//#   else
//#      pragma comment(lib, "andylib.lib")
//#   endif
//#endif
//#pragma comment(linker,"/NODEFAULTLIB:libc.lib")
//#endif

#endif

//#include "inc_lib.h"
