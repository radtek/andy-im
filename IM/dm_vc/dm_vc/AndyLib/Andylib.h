/**********************************************************************
//
//

//
//							AndySDK��� andylib.lib
//���ߣ�
//
//�汾��1.0
//

//
//ԭ�ļ�����andylib.h
//
//������
//    ȫ�ֺ���
//		andyStr  		-- �ַ���������
//		andyFile 		-- �ļ�������
//		andyReg  		-- ע��������
//		andyINI  		-- �����ļ�������
//		andyDateTime	-- ����/ʱ����
//
//�ڴ��������New��Del
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

//δ��װPlatform SDK����Ҫ���¶���
#ifndef IS_INTRESOURCE
#define IS_INTRESOURCE(_r) (((DWORD)(_r) >> 16) == 0)
#endif

//A-Z,0-9�ļ�ֵ��BC,VCͷ�ļ��о�δ����
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

//�Զ��崰����Ϣ
#define MYWM_ICON			(WM_APP+100)		//������ͼ����
#define MYWM_COMMAND		(WM_APP+101)		//���̼䴫��������
#define MYWM_SHAREDATA	(WM_APP+102)		//����䴫��������
#define MYWM_ERROR		(WM_APP+103)		//��⵽��������Ϣ

//ע����������ֵ
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
//������������ַ���
#define MAX_CLASSNAME	(80)

typedef struct tagREGVALINFO
{
	LPCTSTR szValName;
	DWORD	dwType;
	DWORD	cbData;
	DWORD	dwData;
	LPVOID	lpData;
}REGVALINFO,*LPREGVALINFO;


//���ڳߴ磨���ã�
#define WINVAR_DESKTOP_X		(GetSystemMetrics(SM_CXSCREEN))		//��Ļ��
#define WINVAR_DESKTOP_Y		(GetSystemMetrics(SM_CYSCREEN))       //��Ļ��
#define WINVAR_FRAME_X 			(GetSystemMetrics(SM_CXSIZEFRAME))    //���ڱ߿���
#define WINVAR_FRAME_Y 			(GetSystemMetrics(SM_CYSIZEFRAME))    //���ڱ߿�߶�
#define WINVAR_MENU_Y			(GetSystemMetrics(SM_CYMENU))         //�˵��߶�
#define WINVAR_CAPTION_Y		(GetSystemMetrics(SM_CYCAPTION))      //�������߶�
#define WINVAR_EDGE_X			(GetSystemMetrics(SM_CXEDGE))			//3D�߿��
#define WINVAR_EDGE_Y			(GetSystemMetrics(SM_CYEDGE))			//3D�߿��
#define WINVAR_BORDER_X			(GetSystemMetrics(SM_CXBORDER))		//�߿��
#define WINVAR_BORDER_Y			(GetSystemMetrics(SM_CYBORDER))		//�߿��
#define WINVAR(var)				(GetSystemMetrics(var))

//�ڽ�����ʵ������������ھ��
//extern HINSTANCE	m_hInst;
//extern HWND			m_hWnd;

//�ļ����˱�־
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

//����ʼ��
void InitAndyLib(HWND hWnd,HINSTANCE hInst = NULL);
HINSTANCE GetInstance();
HWND GetMainWnd();

//////////////////////////////////////////////////////////////////////////////////////
//�ڴ����/�ͷ�,����ʧ��ʱ�Զ�����
/*#ifdef _DEBUG

// ���䡢�ͷŹ��Ӻ���ԭ��
typedef void (WINAPI* _RMemAllocHook)(int type, DWORD size, char *file, int line);

_RMemAllocHook _SetNewAllocHook(_RMemAllocHook newhook);

const unsigned long		_DBGMEMAGIC = 0x4D454D52;		// �ڴ��ǩ-'RMEM'
const unsigned long		_DBGMEMARP  = 0xFDFDFDFD;		// ����������
const unsigned char		_DBGFILLCNT = 0xCD;				// ������
const unsigned int		_DBGMEMLSIZE= 256;				// �������ߴ�ļ���


// �ڴ�ڵ�ṹ
struct _tagDBGMEMNODE;

typedef struct _tagDBGMEMNODE
{
	unsigned long	magic;								// �ڴ��ǩ
	unsigned long	clisize;							// �û������ʵ���ڴ�ߴ�
	char			*pfilename;							// ������ڴ���Դ��λ��
	int				line;								//
	struct _tagDBGMEMNODE	*prev;						// ǰһ���ڴ���λ��
	struct _tagDBGMEMNODE	*next;						// ��һ���ڴ���λ��
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
//������ͼ�����
BOOL AddInTaskBar(HWND hWnd,HICON hIcon,LPCTSTR s,UINT uId = 0);
BOOL ModiInTaskBar(HWND hWnd,HICON hIcon,LPCTSTR s,UINT uId = 0);
BOOL DelInTaskBar(HWND hWnd,UINT uId = 0);



//////////////////////////////////////////////////////////////////////////////////////
//��Ϣ��
void Errmsg(LPCTSTR format,...);		//��ʾ������Ϣ
BOOL Confirm(LPCTSTR format,...);   //ѯ��ȷ��
void MsgBox(LPCTSTR szTitle,LPCTSTR format, ...);	//���MAX_PATH����
int MsgBoxEx(LPCTSTR szTitle,UINT uType,DWORD dwMsgLength,LPCTSTR format, ...);	//����������
void ShowHelpInfo(LPCTSTR szText,LPCTSTR szCaption,LPCTSTR lpszIcon);	//������Ϣ����ͼ��


//////////////////////////////////////////////////////////////////////////////////////
//��Ŀ¼���ɶ༶Ŀ¼һ�ν�����
BOOL MkDir(LPCTSTR szPath);
//ִ���ļ���������ִ���ļ����ĵ���
HINSTANCE andyExec(LPCTSTR lpFile,LPCTSTR lpDir=NULL,LPCTSTR lpParam=NULL,
						 LPCTSTR lpOp=NULL,INT nShowCmd=SW_SHOW);
//ִ���ļ�������CreateProcess��
int RunProcess(LPCTSTR lpFile,LPCTSTR lpDir=NULL,LPCTSTR lpParam=NULL,
							DWORD dwWait = 3000,int nShowCmd=SW_SHOW,
							DWORD dwCreateFlag=CREATE_DEFAULT_ERROR_MODE,
							LPPROCESS_INFORMATION lpProcessInformation = NULL);
//����ϵͳ�Ի���ѡ��·��
BOOL SelectDir(LPTSTR lpDir,LPCTSTR lpszTitle=LANSTR_SELDIR,
					UINT uFlag=BIF_RETURNONLYFSDIRS,BFFCALLBACK lpfn=NULL,
					LPARAM lParam=0,int iImage=0);
INT CALLBACK SelDirSpecInitProc(HWND hwnd,UINT uMsg,LPARAM lp,LPARAM pData);
//�õ����ļ��ĵ�ǰ�ļ�ָ��
DWORD GetFilePointer(HANDLE hFile);
DWORDLONG GetPathSize(LPCTSTR lpszPathName,int* lpnFileCnt = NULL,int* lpnPathCnt = NULL);
//BOOL CheckCDROM(LPTSTR szCdrom);



//////////////////////////////////////////////////////////////////////////////////////
//����/�������
int WinWidth(HWND hWnd);
int WinHeight(HWND hWnd);
int ClientWidth(HWND hWnd);
int ClientHeight(HWND hWnd);
//�ڴ�������λ���ƶ�����
void EasyMove(HWND hWnd);
//�޸�ע����趨�����Զ�����
BOOL SetAutoRun(LPCTSTR szKeyName,LPCTSTR szParam = NULL);
BOOL DelAutoRun(LPCTSTR szKeyName);
//ע��MUTEX������NULL���⵽��ͬһʵ������ͬһ���򲻿���������
HANDLE RegOnlyWindow(LPCTSTR szMutex);
//����䴫������
BOOL SendShareData(HWND hWnd,LPCTSTR szName,LPVOID lpData,DWORD dwSize);
//���շ��͵�����
BOOL GetShareData(LPCTSTR szName,LPVOID* lpData,DWORD dwSize);

//�ջ��Ʋ���
void PaintNull(HWND hWnd);



//////////////////////////////////////////////////////////////////////////////////////
//������ϵͳ����
int OSType();
HANDLE NewThread(LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParam);
//����/��ֹӦ�ó���Ȩ��
BOOL EnableNTRight(LPCTSTR lpName,BOOL bEnable=TRUE);
//ת���������
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
//ͻ��1024�ֽ����Ƶ�wsprintf��չ��
void exwsprintf(LPTSTR s,LPCTSTR format,...);
}

enum
{
	RSTR_MAX					= 1024,
	RSTR_INIT				= 80,										//��ʼ����
	RSTR_STEP				= MAX_PATH,								//���Ӳ���
	RSTR_INNER				= 1,										//SubStringʱָʾ�����޶�ͷβ�ַ����м���ַ���
	RSTR_OUTER				= 2,										//SubStringʱָʾ���ذ����޶�ͷβ�ַ������ַ���
	RSTR_IGNORERIGHT		= 4,										//SubStringʱָʾ���Ҳ����޶�β�ַ����򷵻ص���β

	RSTR_WINPATH			= -32768,								//windows·��
	RSTR_SYSPATH			= -32767,       						//ϵͳ·��
	RSTR_TEMPPATH			= -32766,								//��ʱĿ¼
	RSTR_CURRPATH			= -32765,								//ִ���ļ�����·������ϵͳ��ǰ·��
	RSTR_MYFILENAME		= -32764,								//ִ���ļ���·���ļ���
	//�¶����ڴ˼���,���뱣֤��ֵ������ȷ���� RSTR_WINPATH �� RSTR_PCNAME ��ֵ����
	//...
	RSTR_PCNAME = -32600									//�������
};


//////////////////////////////////////////////////////////////////////
//
//�ַ���������
//		˵�������ಢδ����ȫ����������磺���������ַ�����ת���ȣ���ʵ��
//		����õ��Ĺ��ܲ���ĳЩ����������ǿ�����������˵õ�ϵͳ·����ϵ
//		ͳ�����ĸ��ƹ��캯����
//		ʵ����ANSI��UNICODE�汾��ͨ�ô��룬δʵ��UNICODE�汾ר�ô��룬
//		����Ԥ���ӿڡ�
//
//////////////////////////////////////////////////////////////////////
class andyStr
{
public:
	//���죬���ƹ��캯��
	andyStr():m_data(NULL),m_size(0),m_wData(NULL),m_wSize(0),m_len(0){memset(x,0,4);}
	andyStr(LPCTSTR str,DWORD size=0);
	//andyStr(LPSTR format,...);
	andyStr(int size);		//size:��ʼ���ȣ�<0����RSTR_WINPATH~RSTR_PCNAME��Χ��Ϊ�����ִ�
	andyStr(const andyStr& src);
	~andyStr();
		
	void Clear();
	
	int Load(UINT ident,HINSTANCE hInst = GetInstance());
	//�������⺬���ַ�
	BOOL LoadSpecial(int nFlag);
	//UTF8�ַ���֧��
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
	//ȡƥ����м��ִ�������"---{123456}---"
	//SubString("{","}"),����"123456",nIndex�������}��λ�ã�uFlag���ָ��RSTR_OUTER���򷵻�"{123456}"
	andyStr SubString(LPCTSTR szStart,LPCTSTR szEnd,int nStart=0,int* nIndex=NULL,UINT uFlag=RSTR_INNER) const;

	//ntimes:��ntimes��ƥ�䷵��(��1��ʼ����)
	int indexOf(LPCTSTR str,int start = 0,int ntimes = 1) const;
	int lastIndexOf(LPCTSTR str,int ntimes = 1) const;
	//���㺬�ж���str�Ӵ�
	int count(LPCTSTR str) const;

	andyStr Insert(const andyStr& str, int index);
	int Delete(int index,int count=1);
	
	void toLower();
	void toUpper();
	void Trim();
	void LTrim();
	void RTrim();
	//���count��c�ַ�����β
	void Append(TCHAR c,int count);
	//���lpstr�����ɸ�c�ַ�����β��������ַ���Ϊcount
	//���lpstr���ȴ���count���ض�lpstrΪcount���ַ�
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
//ע��������
class andyReg
{
public:
	andyReg();
   ~andyReg();

   void Close();

   //ĳ������ȡֵ��ʼ�����رոü������������������Զ��رգ�
	BOOL Get(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,
				LPVOID lpData,DWORD dwDataSize,LPDWORD lpdwType=NULL,LPDWORD lpDataSize=NULL);
   DWORD GetInt(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,DWORD dwDef = 0);	//dwDef:ʧ��ʱ���ص�Ĭ��ֵ
   //���Ѵ򿪵ļ���ȡֵ�������������������ڵ���������������֮һ�����
   BOOL Get(LPCTSTR szKey,LPVOID lpData,DWORD dwDataSize,LPDWORD lpdwType=NULL,LPDWORD lpDataSize=NULL);
   DWORD GetInt(LPCTSTR szKey,DWORD dwDef = 0);	//dwDef:ʧ��ʱ���ص�Ĭ��ֵ
   //����ֵ����
   BOOL Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,LPCTSTR szVal);
   BOOL Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,DWORD dwVal);
   BOOL Set(HKEY hKey,LPCTSTR szSubKey,LPCTSTR szKey,LPVOID lpData,DWORD dwType,DWORD cbData);
   //���ֵ���ã�Add���м������Set
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
   DWORD m_dwKeyCnt;			//���ü�����
   DWORD m_dwKeyMax;			//���ü������
   HKEY m_hKey;
};

//�����ļ�������
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

// ָ��Ŀ¼��ǩ
typedef enum _tagRFSPath
{
	RFSP_MYFILENAME = 0,								//ִ���ļ���·���ļ���
	RFSP_OSCURRPATH,									//����ϵͳ�����ϵĵ�ǰ·��
	RFSP_CURRPATH,										//ִ���ļ�����·������ϵͳ��ǰ·��
	RFSP_WINPATH,										//windows·��
	RFSP_SYSPATH,		       							//ϵͳ·��
	RFSP_TEMPPATH,										//��ʱĿ¼
	//RFSP_XXXX											//�������ڴ˼���
	RFSP_MAX
} RFSPATH, *LPRFSPATH;

// ��д�������ǩ
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

//�ļ�״̬��־��ע�⣺�޸Ķ���ֵ���밴���еĴ�С����
//��: RFA_VOID < RFA_FILE < RFA_PATH < RFA_NOTEXIST < RFA_BADFILENAME
#define RFA_VOID				12000
#define RFA_FILE				12001
#define RFA_PATH				12002
#define RFA_NOTEXIST			12003
#define RFA_BADFILENAME			12004
//for ReadAll������ļ��ߴ���ڸóߴ������
#define RF_READALL_SIZE		102400000

//�ļ���д�������
#define RFE_EOF				(-1)
#define RFE_LINEOVERFLOW	(-2)
#define RFE_ERROR				(-3)
#define RFE_FILETOOBIG		(-4)
#define RFE_MEMERROR			(-5)
//�ļ�������
class andyFile
{
public:
	// Ԥ��������
	andyFile();
	andyFile(LPCTSTR szFilename);
	andyFile& operator=(LPCTSTR szFilename){SetFile(szFilename);return *this;}

	void SetFile(LPCTSTR format, ...);
	void SetFile(HANDLE hFile);
	void SetFileName(LPCTSTR szFile);
	int  Select(LPCTSTR szFileType,LPCTSTR szTitle = NULL,BOOL bSave=FALSE,
					HWND hWnd=GetMainWnd(),LPCTSTR dlgEx=NULL,LPOFNHOOKPROC hookproc=NULL,
					LONG lCustData=0);

	// ʵ��������
	andyFile(HANDLE hFile){Init();SetFile(hFile);}
	//dwShare:Ĭ�ϲ�����Ҫ��Create�����ڲ�CreateFile��share����һ�£��������Create
	//��δ�ر��ļ�ʱ��һ�������Open��ͬһ�ļ���ʧ��
	BOOL Open(DWORD dwAction=RF_READ,DWORD dwShare=RF_SHARE_READ|FILE_SHARE_WRITE,DWORD dwAttr=FILE_ATTRIBUTE_NORMAL);
	virtual BOOL Open(LPCTSTR lpFilename, DWORD dwAction=RF_READ, DWORD dwShare=RF_SHARE_READ,DWORD dwAttr=FILE_ATTRIBUTE_NORMAL);
	BOOL Create(DWORD dwCreate = RF_EDIT,DWORD dwAttr=FILE_ATTRIBUTE_NORMAL);
	virtual BOOL Create(LPCTSTR lpFilename, DWORD dwCreate = RF_EDIT);
	BOOL CreateTemp(LPCTSTR prefix=NULL);
	
	// ���������
	virtual void Close();	
	virtual ~andyFile();
	
	// �����������
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
	
	// ͨ�÷���
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
	BOOL		SelectDir(LPTSTR lpDir,LPCTSTR lpszTitle=_T("ѡ��·��"),
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
	HANDLE m_hFile;							//�ļ����
	
private:
	TCHAR  m_szFilename[MAX_PATH];//�ļ���(��ȫ·��)
	TCHAR  m_szDrive[4];			//��������(eg: c:\da\a.txt--return "c:")
	LPTSTR m_lpszPath;				//�ļ�·��(eg: c:\da\a.txt--return "C:\da")
	LPTSTR m_lpszFilename;			//�ļ���(eg: c:\da\a.txt--return "a.txt")
	LPTSTR m_lpszTitlename;		//�ļ���(eg: c:\da\a.txt--return "a")
	LPTSTR m_lpszExtname;			//�ļ���չ��(eg: aa.txt--return "txt")
	
	LPVOID m_lpFilebuf;					//�ļ�ȫ�����ݻ�����
	DWORD m_bufSize;						//�������ߴ�
	DWORD  m_dwRWNum;						//�ļ�ʵ�ʶ�д����
	
	int   m_aAttrib;						//��������
	BOOL  m_bExtern;						//m_Handle�Ƿ��ǵ�����

	DWORD m_dwFileAttr;						//�ļ�����
	DWORD m_dwFileSize;						//�ļ��ߴ�
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

//�첽��д�ļ���
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
	HANDLE			m_hFile;							//�ļ����
	HANDLE			m_hEvent;							//��д���

	DWORD			m_dwLoFileSize;						//�ļ��ߴ�
	DWORD			m_dwHiFileSize;

	DWORD			m_dwTrue;							//ʵ�ʵĶ�д�ߴ�

	OVERLAPPED		m_sOverLapped;						//�첽�вνṹ
	int				m_OSType;							//����ϵͳ����
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

	//������һ��Ŀ¼��֧��"\\",".."��ͬDOS��CD����
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

//����/ʱ����
class andyDateTime
{
public:
	andyDateTime();
	virtual ~andyDateTime(){}

	//�󶨵�ϵͳʱ�䣬TRUE:ÿ�ε���year()�Ⱥ���ȡϵͳʱ��
	//����ֱ�ӷ���m_time����Ӧ�ֶΣ����m_timeδ��ʼ����
	//�Զ�ȡϵͳʱ�䡣
	//�ṩ��������Ŀ����Ӧ������/ʱ����Ҫ����������Ӧ��
	//�Ѳ�����ΪFALSE����������һȡ��һ����/ʱ��Σ�����
	//����ͻᱻϵͳʱ������
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
	//��ȡϵͳʱ�䣬�Զ�����GetTime()
	int GetYear();
   int GetMonth();
   int GetDay();
   int GetDayOfWeek();
   int GetHour();
   int GetMinute();
   int GetSecond();
	
	//��ȡ��ǰ�趨ʱ�䣬ֱ�ӷ���ʱ���ֶΣ�������GetTime
	int year(){return (int)m_lptime->wYear;}
   int month(){return (int)m_lptime->wMonth;}
   int day(){return (int)m_lptime->wDay;}
   int dayOfWeek(){return (int)m_lptime->wDayOfWeek;}
   int hour(){return (int)m_lptime->wHour;}
   int minute(){return (int)m_lptime->wMinute;}
   int second(){return (int)m_lptime->wSecond;}
	int ms();			//����

	//����/ʱ���ַ���
	//length������ݣ�=2��Ϊ�̸�ʽ����2004���򷵻�"04"��������
	//��ֵ��Ϊ4λ�ַ���
	//������������/ʱ�䣬=2��Ϊ�̶���λ�ַ���������1����"01"��
	//29����"29"����������ֵ����ݾ�������/ʱ�䷵��1��2λ�ַ���
	//���ֻ��length=2������Ż�����Ӧ���������������Ĭ�ϲ�����
	LPCTSTR year2str(int length=0);
	LPCTSTR month2str(int length=0);
	LPCTSTR day2str(int length=0);
	LPCTSTR hour2str(int length=0);
	LPCTSTR minute2str(int length=0);
	LPCTSTR second2str(int length=0);
	LPCTSTR date2str();
	LPCTSTR time2str();

   //�������ڣ�ʱ�䣬������ȡĬ��ֵ����õ�ǰʱ��
   BOOL SetDate(WORD month = 0,WORD day = 0,WORD year = 0,BOOL bSetSystem=TRUE);
   BOOL SetTime(WORD hour = 61,WORD minute = 61,WORD second = 61,BOOL bSetSystem=TRUE);

	//LPCTSTR FormatString(LPCTSTR szFormat);

	andyDateTime& operator =(const andyDateTime& rdt);
	//�������·�ʽ��ֵ�Զ���m_bReCountΪFALSE��������ϵͳʱ��󶨡�	
	andyDateTime& operator =(const SYSTEMTIME systime);
	andyDateTime& operator =(const FILETIME filetime);
	
	//ת�����ļ�ʱ��
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


#define ANDYFONT_BOLD				1	//����
#define ANDYFONT_ITALIC				2	//б��
#define ANDYFONT_UNDERLINE			4	//�»���
#define ANDYFONT_STRIKEOUT			8	//ɾ����
#define ANDYFONT_PIXEL				16	//�����СΪ����

class andyFont
{
public:
	andyFont();
	andyFont(HFONT hfont):m_crColor(0){SetFont(hfont);}
	andyFont(LPCTSTR szFontname,int size,int nStyle=0,int angle=0);
	~andyFont();
	//nStyle:=LOGFONT�ṹlfWeightֵ+ANDYFONT_xxxһ�������Ļ�|��ֵ
	BOOL SetFont(LPCTSTR szFontname,int size,int nStyle=0,int angle=0);
	BOOL SetFont(LOGFONT* lpLogFont);
	BOOL SetFont(HFONT hfont);
	void SetFontColor(COLORREF cr){m_crColor = cr;}
	COLORREF GetFontColor(){return m_crColor;}
	//����Ϊָ�����ڵ�����
	BOOL SetFont(HWND hWnd);
	void SetWindowSFont(HWND hWnd,LPCTSTR szFontname=NULL,int size=0,int nStyle=0);
	HFONT Select(HWND hWnd = NULL);

	HFONT GetFont(){return m_font;}
	BOOL GetLogFont(LOGFONT* lpLogFont);
	BOOL GetWindowLogFont(HWND hWnd,LOGFONT* lpLogFont);
	//�Զ�����������ʹ����w,hָ���ľ�����������ʾ
	HFONT AutoAdjust(LPCTSTR szText,int w,int h,HDC hDC/* = NULL*/);
	//ָ��������б��
	HFONT SetFontAngle(int nAngle);	
	
	//��ȡ����ߴ磬lpszText���Ϊ�գ�Ϊ�ַ�A�ĳߴ硣lpSize����Ҫ��ȡ�ĳߴ磬���lpSizeΪ�գ�������������߶�
	int GetFontSize(LPCTSTR lpszText=NULL,LPSIZE lpSize=NULL);

private:
	HFONT m_font;
	COLORREF m_crColor;
	LOGFONT m_lpMainFontRec;
	BOOL m_bExtern;
};

/*****************************************************
//
//��Ϣ��ʾ��
//		��������ʱ��Ҫ��¼����ʾһЩ�����������Ϣ��
//		�����ṩ���ֱ�����
//
*****************************************************/
#define ANDYMSG_NONE				0		//������
#define ANDYMSG_LOGFILE			1		//��¼���ļ�
#define ANDYMSG_SENDMESSAGE	2		//������Ϣ
#define ANDYMSG_ALERT			4		//������Ϣ����ʾ
#define ANDYMSG_DEBUGOUT		8		//��ʾ��Debugger����
#define ANDYMSG_RECTIME			0x00010000		//��¼����ʾ����Ϣ��ʱ��
#define ANDYMSG_NORETURN		0xF0000000		//������EDIT�ؼ���ʾ���޻س�

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
	//��¼���ݣ������ڼ�¼��־�ļ�
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
//��Ϣ��ʾ��
//		��AndyMsg���ٴη�װ����AndyMsg�Ķ���ָ��Ϊ��Ա������
//		����Ҫ��Ϣ��ʾ���ܵ����Ա���̳У����������á�
//		�ṩ������Ҫԭ������Щ�������Ҫ��Ϣ��ʾ��Ҳ���ܲ�
//		��Ҫ������̳���AndyMsgs����ܰ������ô�������ݣ�
//		�Ա���̳�ֻ��һ��ָ�룬�����ڴ������ռ�á�
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

//�߳��ٽ���ͬ����
class AndyDataGuard
{
public:
	AndyDataGuard(){InitializeCriticalSection(&m_crs);}
	~AndyDataGuard(){DeleteCriticalSection(&m_crs);}

	void Enter(){EnterCriticalSection(&m_crs);}
	void Leave(){LeaveCriticalSection(&m_crs);}
	//95,98��֧��
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
//֧�ֿ����ͬ���࣬AndyDataGuard������
class AndyDataGuardEx
{
public:
	AndyDataGuardEx();						//������
	AndyDataGuardEx(LPCTSTR szName);		//�����ͬ��
	~AndyDataGuardEx();

	void Enter();
	void Leave();
	BOOL TryEnter(DWORD dwWaitMS = 1000);		//�ȴ�ʱ�䣬��λ������
	BOOL IsSingle() const{return (m_hFileMap == NULL);}

private:
	HANDLE m_hEvent,m_hFileMap;
	LPDGEX_SHAREINFO m_info;
};

//ANDYVCLASS������Ϊת������ĳ�Ա���ݺͳ�Ա������ָ�����ͣ�
//���౾�ɽ����������κζ����ʵ�֣�����BC����������
//VC�����Ϊsizeof(ANDYVCLASS)�쳣����˱�����ʵ��(��{}����)��
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
//ģ��
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
			return FALSE;	//�����߳�����

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
	
	//�˳��̣߳��ú�����ͼ���߳���ֹ��������������趨��ʱ��
	//���룩�߳�δ�˳���ǿ�н����̡߳�Ĭ�ϵȴ�30�롣
	//�ȴ�ʱ�佨�鲻Ҫ���̫�̣������߳��л����Զ�����˳���
	void Quit(int nWaitSecond = 30);
	//�����̵߳��ã����߳����˳������������ȴ��߳��˳����������ء�
	void ThreadQuit(int nWaitSecond = 30);
	
	//�����߳��˳���־������߳��еȴ��ⲿ�¼������������ñ�־Ȼ�������¼���ʹ�߳��˳�
	void SetQuit(){m_bQuit = TRUE;}
	//��ѯ�߳��˳���־
	BOOL IsRunning(){return !m_bQuit;}
	//��ѯ�߳��Ƿ�������
	BOOL IsStarted(){return (BOOL)m_handle;}
	//��ѯ�߳��Ƿ��ڵȴ�״̬
	BOOL IsWaitting(){return (m_nEvCnt == 0);}
	BOOL CanContinue(DWORD dwMilSec = INFINITE);
	DWORD GetExitCode();

	BOOL PostMessage(UINT uMsg,WPARAM wParam,LPARAM lParam);
	
private:
	static DWORD WINAPI ThreadProc(AndyThInfo* obj);
	static DWORD WINAPI QuitProc(AndyThread* obj);
	//֪ͨ�߳̽��������̴߳������˳�ʱ���ã�
	DWORD NotifyThreadQuit();
	DWORD WaitQuit();

private:
	HANDLE m_handle,m_event;
	BOOL m_bQuit;	
	int m_nEvCnt,m_nWaitSecond;
	DWORD m_dwId,m_dwExitCode;
	//ͬ�������ڶ���߳��в�����������Ҫ����ͬ��
	AndyDataGuard m_crl;
	AndyThInfo* m_info;
};

//��������־
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
