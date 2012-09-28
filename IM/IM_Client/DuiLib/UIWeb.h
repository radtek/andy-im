/********************************************************************
 @created: 2010/01/16
 @version: 0.1
 @filename:  WebBrowser.h
 @file base: WebBrowser
 @file ext: h
 @author: andywei
 
 @purpose: Declaration for class CWebBrowser, a wrapper of
    IWebBrowser2.
 @remark: To use this class, include 'Exdisp.h' before this file.
    Besides, use OleInitialize instead of CoInitialize, and
    do not forget OleUninitialize.
*********************************************************************/

#ifndef __WEBBROWSER_H__
#define __WEBBROWSER_H__

#include <Exdisp.h>  /* Defines of stuff like IWebBrowser2. This is an include file with Visual C 6 and above */
#include <Mshtml.h>  /* Defines of stuff like IHTMLDocument2. This is an include file with Visual C 6 and above */
/* If you gets some error when compling under VS2005, move mshtml.h to top*/
#include <crtdbg.h>  /* for _ASSERT() */

#include <tchar.h>
#include <atlbase.h>
#include <atlcom.h>
#include <ExDispid.h>
namespace DuiLib {
//#include "andydows.h"

// This is for a simple C++ wrapper. There are lots more things we can control about the browser object,
// but we don't do it all in this. _Many_ of the functions we provide for the browser to call, will
// never actually be called by the browser in our sample. Why? Because we don't do certain things
// with the browser that would require it to call those functions (even though we need to provide
// at least some stub for all of the functions).
//
// So, for these "dummy functions" that we don't expect the browser to call, we'll just stick in some
// assembly code that causes a debugger breakpoint and tells the browser object that we don't support
// the functionality. That way, if we try to do more things with the browser object, and it starts
// calling these "dummy functions", we'll know which ones we should add more meaningful code to.
#define NOTIMPLEMENTED _ASSERT(0); return(E_NOTIMPL)

//web事件消息。用于发送到父窗口
#define WM_NEW_IEVIEW		  WM_USER+100
#define WM_DOCUMENT_COMPLETE  WM_USER+101
#define WM_BEFORENAVIGATE2    WM_USER+102
#define WM_TITLECHANGE        WM_USER+103
#define WM_NAVIGATECOMPLETE   WM_USER+104
#define WM_STATUSTEXTCHANGE   WM_USER+105

//最大标签个数
#define  MAX_TAB_NUM           32  

//通知消息结构
typedef struct tagWEBNITIFY{
	NMHDR		hdr;
	char		url[2048];					//url or other text
	//int		iSubItem;				//保留
	//HWND	  	hWndChild;				//保留
	//LPARAM	lParam;					//保留
	//LPARAM	lParamSubItem;			//保留
}WEBNITIFY,*LPWEBNITIFY;
//class tab //
class CWBStorage;
class CWBClientSite;
class CWBDispatch;
class CWebBrowserUI;
class CWebWnd;
//=============================================================================
/*
 @class   CWebBrowser
 @author 
 @brief   A wrapper class of IWebBrowser2 interface.
*/
//=============================================================================

class UILIB_API CWebBrowserUI : public CControlUI, public IMessageFilterUI
{
	friend CWebWnd;
public:
 // Constructor
 CWebBrowserUI();
 // Destructor
 virtual ~CWebBrowserUI();
   CWBDispatch   *m_pDispatch;
private:
 // Data members
  IOleObject  *m_pBrowserObj;
  CWBClientSite  *m_pClientSite;
public:
	POINT	m_ptMouseDown;//last mouse down point
	//创建/关联操作
	BOOL CreateWebWnd();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetVisible(bool bVisible = true);
	void SetInternVisible(bool bVisible = true);
	void SetPos(RECT rc);
	void DoPaint(HDC hDC, const RECT& rcPaint);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);


	//SIZE EstimateSize(SIZE szAvailable);
	//void DoEvent(TEventUI& event);

	void DoPageAction(DWORD action);

 void UnEmbedBrowserObject(HWND /*hwnd*/);

 
 long EmbedBrowserObject(HWND hwnd);

 
 long DisplayHTMLPage(LPCTSTR webPageName);
 
 //long DisplayHTMLStr(HWND /*hwnd*/, LPCTSTR string);
 long DisplayHTMLStr(LPCTSTR string);
//resize the browserview
 //void ResizeBrowser(HWND hwnd, DWORD x,DWORD y,DWORD width, DWORD height);
 void ResizeBrowser(DWORD x,DWORD y,DWORD width, DWORD height);
//the web window get the focus,so we can get mouse event
 void Onfocus();
 void  SetParent(HWND hwd){m_ParentWnd = hwd;};

 LPCTSTR GetPointUrl(/*POINT pt*/);
protected:
	//LRESULT AndydowProc(HWND,ANDYPARAMS);
private:
 // Methods
   //父窗口，用于发送消息
	HWND m_ParentWnd;

	CWebWnd* m_pWindow;
};
class CWBDispatch:public IDispatch
{
public:
	CWBDispatch()
	{

	}
	CWBDispatch(HWND hwnd)
	{
		m_hWindow = hwnd;
	}
	~CWBDispatch()
	{
		Exit();
	}
	HRESULT ConnectEvents();
	void Exit();
	public:
	HWND    m_hWindow;
	CComPtr<IConnectionPointContainer> m_pCPContainer;
	CComPtr<IConnectionPoint> m_pConnectionPoint;
	IWebBrowser2	*m_pWebBrowser2;
	DWORD dwCookie;
	public:
	ULONG m_uRef;
	// IUnknown接口实现 
	STDMETHODIMP QueryInterface( REFIID iid, void **ppvObject ) 
	{

		*ppvObject=this;
		return S_OK;


	} 
	ULONG STDMETHODCALLTYPE AddRef() 
	{

		m_uRef++;
		return m_uRef;

	} 
	ULONG STDMETHODCALLTYPE Release() 
	{

		ULONG u = m_uRef--;
		if ( 0 == m_uRef ) 
		{

			delete this;

		} 
		return u;

	} 
	// IDispatch接口实现 
	STDMETHODIMP GetTypeInfoCount( UINT *pctinfo ) 
	{

		return E_NOTIMPL;

	} 
	STDMETHODIMP GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) 
	{

		return E_NOTIMPL;

	} 
	STDMETHODIMP GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) 
	{

		return E_NOTIMPL;

	} 

	//virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID *ppvObj);    
	//virtual ULONG   STDMETHODCALLTYPE AddRef(void)                                    { return 1; }
	//virtual ULONG   STDMETHODCALLTYPE Release(void)                                    { return 1; }
	//virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(unsigned int FAR* pctinfo)    { return E_NOTIMPL; }
	//virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo FAR* FAR* ppTInfo){ return E_NOTIMPL; }
	//virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, OLECHAR FAR* FAR* rgszNames, unsigned int cNames, LCID lcid, DISPID FAR* rgDispId){ return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID  dispIdMember, REFIID  riid, LCID  lcid, WORD  wFlags, DISPPARAMS FAR*  pDispParams, VARIANT FAR*  pVarResult, EXCEPINFO FAR*  pExcepInfo, unsigned int FAR*  puArgErr);
};
class CWBStorage :
	public IStorage
{
public:
	// IUnknown methods
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

	virtual ULONG STDMETHODCALLTYPE AddRef( void);

	virtual ULONG STDMETHODCALLTYPE Release( void);

	// IStorage methods
	virtual HRESULT STDMETHODCALLTYPE CreateStream(
		/* [string][in] */ const OLECHAR *pwcsName,
		/* [in] */ DWORD grfMode,
		/* [in] */ DWORD reserved1,
		/* [in] */ DWORD reserved2,
		/* [out] */ IStream **ppstm);

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE OpenStream(
		/* [string][in] */ const OLECHAR *pwcsName,
		/* [unique][in] */ void *reserved1,
		/* [in] */ DWORD grfMode,
		/* [in] */ DWORD reserved2,
		/* [out] */ IStream **ppstm);

	virtual HRESULT STDMETHODCALLTYPE CreateStorage(
		/* [string][in] */ const OLECHAR *pwcsName,
		/* [in] */ DWORD grfMode,
		/* [in] */ DWORD reserved1,
		/* [in] */ DWORD reserved2,
		/* [out] */ IStorage **ppstg);

	virtual HRESULT STDMETHODCALLTYPE OpenStorage(
		/* [string][unique][in] */ const OLECHAR *pwcsName,
		/* [unique][in] */ IStorage *pstgPriority,
		/* [in] */ DWORD grfMode,
		/* [unique][in] */ SNB snbExclude,
		/* [in] */ DWORD reserved,
		/* [out] */ IStorage **ppstg);

	virtual HRESULT STDMETHODCALLTYPE CopyTo(
		/* [in] */ DWORD ciidExclude,
		/* [size_is][unique][in] */ const IID *rgiidExclude,
		/* [unique][in] */ SNB snbExclude,
		/* [unique][in] */ IStorage *pstgDest);

	virtual HRESULT STDMETHODCALLTYPE MoveElementTo(
		/* [string][in] */ const OLECHAR *pwcsName,
		/* [unique][in] */ IStorage *pstgDest,
		/* [string][in] */ const OLECHAR *pwcsNewName,
		/* [in] */ DWORD grfFlags);

	virtual HRESULT STDMETHODCALLTYPE Commit(
		/* [in] */ DWORD grfCommitFlags);

	virtual HRESULT STDMETHODCALLTYPE Revert( void);

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE EnumElements(
		/* [in] */ DWORD reserved1,
		/* [size_is][unique][in] */ void *reserved2,
		/* [in] */ DWORD reserved3,
		/* [out] */ IEnumSTATSTG **ppenum);

	virtual HRESULT STDMETHODCALLTYPE DestroyElement(
		/* [string][in] */ const OLECHAR *pwcsName);

	virtual HRESULT STDMETHODCALLTYPE RenameElement(
		/* [string][in] */ const OLECHAR *pwcsOldName,
		/* [string][in] */ const OLECHAR *pwcsNewName);

	virtual HRESULT STDMETHODCALLTYPE SetElementTimes(
		/* [string][unique][in] */ const OLECHAR *pwcsName,
		/* [unique][in] */ const FILETIME *pctime,
		/* [unique][in] */ const FILETIME *patime,
		/* [unique][in] */ const FILETIME *pmtime);

	virtual HRESULT STDMETHODCALLTYPE SetClass(
		/* [in] */ REFCLSID clsid);

	virtual HRESULT STDMETHODCALLTYPE SetStateBits(
		/* [in] */ DWORD grfStateBits,
		/* [in] */ DWORD grfMask);

	virtual HRESULT STDMETHODCALLTYPE Stat(
		/* [out] */ STATSTG *pstatstg,
		/* [in] */ DWORD grfStatFlag);

};

class CWBClientSite :
 public IOleClientSite, public IOleInPlaceFrame, public IOleInPlaceSite
{
public:
 // constructors
 CWBClientSite()
 {}

 CWBClientSite(HWND hwnd)
 {
  m_hWindow = hwnd;
 }

 //
 // IUnknown methods
 //
 virtual HRESULT STDMETHODCALLTYPE QueryInterface(
  /* [in] */ REFIID riid,
  /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

 virtual ULONG STDMETHODCALLTYPE AddRef( void);

 virtual ULONG STDMETHODCALLTYPE Release( void);

 //
 // IOleClientSite methods
 //
 virtual HRESULT STDMETHODCALLTYPE SaveObject( void);

 virtual HRESULT STDMETHODCALLTYPE GetMoniker(
  /* [in] */ DWORD dwAssign,
  /* [in] */ DWORD dwWhichMoniker,
  /* [out] */ IMoniker **ppmk);

 virtual HRESULT STDMETHODCALLTYPE GetContainer(
  /* [out] */ IOleContainer **ppContainer);

 virtual HRESULT STDMETHODCALLTYPE ShowObject( void);

 virtual HRESULT STDMETHODCALLTYPE OnShowWindow(
  /* [in] */ BOOL fShow);

 virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout( void);

 //
 // IOleWindow methods
 //
 virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow(
  /* [out] */ HWND *phwnd);

 virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
  /* [in] */ BOOL fEnterMode);

 //
 // IOleInPlaceUIWindow methods
 //
 virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetBorder(
  /* [out] */ LPRECT lprectBorder);

 virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE RequestBorderSpace(
  /* [unique][in] */ LPCBORDERWIDTHS pborderwidths);

 virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetBorderSpace(
  /* [unique][in] */ LPCBORDERWIDTHS pborderwidths);

 virtual HRESULT STDMETHODCALLTYPE SetActiveObject(
  /* [unique][in] */ IOleInPlaceActiveObject *pActiveObject,
  /* [unique][string][in] */ LPCOLESTR pszObjName);

 //
 // IOleInPlaceFrame methods
 //
 virtual HRESULT STDMETHODCALLTYPE InsertMenus(
  /* [in] */ HMENU hmenuShared,
  /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths);

 virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetMenu(
  /* [in] */ HMENU hmenuShared,
  /* [in] */ HOLEMENU holemenu,
  /* [in] */ HWND hwndActiveObject);

 virtual HRESULT STDMETHODCALLTYPE RemoveMenus(
  /* [in] */ HMENU hmenuShared);

 virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetStatusText(
  /* [unique][in] */ LPCOLESTR pszStatusText);

 virtual HRESULT STDMETHODCALLTYPE EnableModeless(
  /* [in] */ BOOL fEnable);

 virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(
  /* [in] */ LPMSG lpmsg,
  /* [in] */ WORD wID);

 //
 // IOleInPlaceSite methods
 //
 virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate( void);

 virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate( void);

 virtual HRESULT STDMETHODCALLTYPE OnUIActivate( void);

 virtual HRESULT STDMETHODCALLTYPE GetWindowContext(
  /* [out] */ IOleInPlaceFrame **ppFrame,
  /* [out] */ IOleInPlaceUIWindow **ppDoc,
  /* [out] */ LPRECT lprcPosRect,
  /* [out] */ LPRECT lprcClipRect,
  /* [out][in] */ LPOLEINPLACEFRAMEINFO lpFrameInfo);

 virtual HRESULT STDMETHODCALLTYPE Scroll(
  /* [in] */ SIZE scrollExtant);

 virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(
  /* [in] */ BOOL fUndoable);

 virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate( void);

 virtual HRESULT STDMETHODCALLTYPE DiscardUndoState( void);

 virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo( void);

 virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(
  /* [in] */ LPCRECT lprcPosRect);
 //virtual HRESULT STDMETHODCALLTYPE InPlace_OnPosRectChange(LPCRECT lprcPosRect);
 /* //////////////////////////////////////////////////
 // We don't want those functions to access global
 // variables, because then we couldn't use more
 // than one browser object. (ie, we couldn't have
 // multiple windows, each with its own embedded
 // browser object to display a different web page).
 //
 // So here is where I added my extra HWND that the
 // CWBIOleInPlaceFrame function Frame_GetWindow() needs
 // to access.
 ////////////////////////////////////////////////// */
 HWND    m_hWindow;
};
} // namespace DuiLib
#endif //__WEBBROWSER_H__
