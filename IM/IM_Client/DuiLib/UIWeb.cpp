/********************************************************************
author:caoziwei
time:2010-12-12 win32 web
*********************************************************************/


#define MAKE_SELF_LIB
#include "StdAfx.h"
#include <strstream>
//#include "andyWeb.h"
#include <comutil.h>
namespace DuiLib {
//#include <ExDispid.h>
// #include "WBClientsite.h"
// 
// #include "WebBrowser.h"
// #include "Global.h"
#pragma comment(lib, "comsupp.lib")
/* This is used by DisplayHTMLStr(). It can be global because we never change it. */
static const SAFEARRAYBOUND ArrayBound = {1, 0};
#define WEBPAGE_GOBACK		0
#define WEBPAGE_GOFORWARD	1
#define WEBPAGE_GOHOME		2
#define WEBPAGE_SEARCH		3
#define WEBPAGE_REFRESH		4
#define WEBPAGE_STOP		5
//extern CWebBrowser *pweb = NULL;
////////////////////////////////////////////

///cwebwindows//////////////////////////////////////////////////////////////////////////
class CWebWnd : public CWindowWnd
{

public:
	HWND Init(CWebBrowserUI* pOwner, HWND hWndParent);

	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void DoVerb(LONG iVerb);

	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	CWebBrowserUI* m_pOwner;
};
HWND CWebWnd::Init(CWebBrowserUI* pOwner, HWND hWndParent)
{
	m_pOwner = pOwner;
	UINT uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	Create(hWndParent, _T("UIActiveX"), uStyle, 0L, 0,0,0,0, NULL);
	return m_hWnd;
}

LPCTSTR CWebWnd::GetWindowClassName() const
{
	return _T("WebWnd");
}

void CWebWnd::OnFinalMessage(HWND hWnd)
{
	//delete this; // 这里不需要清理，CActiveXUI会清理的
}

void CWebWnd::DoVerb(LONG iVerb)
{
	/*if( m_pOwner == NULL ) return;
	if( m_pOwner->m_pOwner == NULL ) return;
	IOleObject* pUnk = NULL;
	m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
	if( pUnk == NULL ) return;
	CSafeRelease<IOleObject> RefOleObject = pUnk;
	IOleClientSite* pOleClientSite = NULL;
	m_pOwner->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
	CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
	pUnk->DoVerb(iVerb, NULL, pOleClientSite, 0, m_hWnd, &m_pOwner->m_pOwner->GetPos());*/
}

LRESULT CWebWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
case WM_PAINT:         lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
case WM_SETFOCUS:      lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
case WM_ERASEBKGND:    lRes = OnEraseBkgnd(uMsg, wParam, lParam, bHandled); break;
case WM_MOUSEACTIVATE: lRes = OnMouseActivate(uMsg, wParam, lParam, bHandled); break;
default:
	bHandled = FALSE;
	}
	if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CWebWnd::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	/*if( m_pOwner->m_pViewObject == NULL ) bHandled = FALSE;*/
	return 1;
}

LRESULT CWebWnd::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//IOleObject* pUnk = NULL;
	//m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
	//if( pUnk == NULL ) return 0;
	//CSafeRelease<IOleObject> RefOleObject = pUnk;
	//DWORD dwMiscStatus = 0;
	//pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
	//if( (dwMiscStatus & OLEMISC_NOUIACTIVATE) != 0 ) return 0;
	//if( !m_pOwner->m_bInPlaceActive ) DoVerb(OLEIVERB_INPLACEACTIVATE);
	bHandled = FALSE;
	return 0;
}

LRESULT CWebWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	//m_pOwner->m_bFocused = true;
	//if( !m_pOwner->m_bUIActivated ) DoVerb(OLEIVERB_UIACTIVATE);
	return 0;
}

LRESULT CWebWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	m_pOwner->m_bFocused = false;
	return 0;
}

LRESULT CWebWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	::BeginPaint(m_hWnd, &ps);
	::EndPaint(m_hWnd, &ps);
	return 1;
}



CWebBrowserUI::CWebBrowserUI()
{
	m_pBrowserObj = NULL;
	m_pWindow = NULL;
	//m_windowType = ITIS_EDIT;	
	OleInitialize(NULL);
}

CWebBrowserUI::~CWebBrowserUI()
{
	OleUninitialize();
}
LPCTSTR CWebBrowserUI::GetClass() const
{
	return _T("WebBrowserUI");
}

LPVOID CWebBrowserUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("WebBrowser")) == 0 ) return static_cast<CWebBrowserUI*>(this);
	return CControlUI::GetInterface(pstrName);
}
void CWebBrowserUI::SetVisible(bool bVisible)
{
	CControlUI::SetVisible(bVisible);
	if( m_ParentWnd != NULL) 
		::ShowWindow(m_ParentWnd, IsVisible() ? SW_SHOW : SW_HIDE);
}

void CWebBrowserUI::SetInternVisible(bool bVisible)
{
	CControlUI::SetInternVisible(bVisible);
	if( m_ParentWnd != NULL  ) 
		::ShowWindow(m_ParentWnd, IsVisible() ? SW_SHOW : SW_HIDE);
}

void CWebBrowserUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	ASSERT(m_pWindow);
	if (m_pWindow)
	{
		::MoveWindow(*m_pWindow, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, TRUE);
	}	
}

void CWebBrowserUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

	//if( m_pControl != NULL && m_pControl->m_bWindowless && m_pControl->m_pViewObject != NULL )
	//{
	//	m_pControl->m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &m_rcItem, (RECTL*) &m_rcItem, NULL, NULL); 
	//}
}

void CWebBrowserUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	/*if( _tcscmp(pstrName, _T("clsid")) == 0 ) CreateControl(pstrValue);
	else if( _tcscmp(pstrName, _T("modulename")) == 0 ) SetModuleName(pstrValue);
	else if( _tcscmp(pstrName, _T("delaycreate")) == 0 ) SetDelayCreate(_tcscmp(pstrValue, _T("true")) == 0);*/
	/*else */CControlUI::SetAttribute(pstrName, pstrValue);
}

LRESULT CWebBrowserUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( m_pWindow == NULL ) return 0;
	if( !IsMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) return 0;
	bool bWasHandled = true;
	if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR ) {
		// Mouse message only go when captured or inside rect
		//DWORD dwHitResult = m_pWindow->m_bCaptured ? HITRESULT_HIT : HITRESULT_OUTSIDE;
		//if( dwHitResult == HITRESULT_OUTSIDE && m_pControl->m_pViewObject != NULL ) {
		//	IViewObjectEx* pViewEx = NULL;
		//	m_pControl->m_pViewObject->QueryInterface(IID_IViewObjectEx, (LPVOID*) &pViewEx);
		//	if( pViewEx != NULL ) {
		//		POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		//		pViewEx->QueryHitPoint(DVASPECT_CONTENT, &m_rcItem, ptMouse, 0, &dwHitResult);
		//		pViewEx->Release();
		//	}
		//}
		//if( dwHitResult != HITRESULT_HIT ) return 0;
		if( uMsg == WM_SETCURSOR ) bWasHandled = false;
	}
	else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST ) {
		// Keyboard messages just go when we have focus
		if( !IsFocused() ) return 0;
	}
	else {
		switch( uMsg ) {
		case WM_HELP:
		case WM_CONTEXTMENU:
			bWasHandled = false;
			break;
		default:
			return 0;
		}
	}
	LRESULT lResult = 0;
//	HRESULT Hr = m_pControl->m_pInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lResult);
	//if( Hr == S_OK ) bHandled = bWasHandled;
	return lResult;
}
BOOL CWebBrowserUI::CreateWebWnd()
{
	InitCommonControls();
//	pweb = this;
	//if(AndyControls::Create(dwStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|ES_READONLY,dwExStyle,szInitText,uId,x,y,width,height))
	//{
	//	//去除CS_HREDRAW|CS_VREDRAW，否则调整尺寸引起窗口闪烁
	//	LONG lc = GetClassLong(m_hWnd,GCL_STYLE);
	//	lc &= ~(CS_HREDRAW|CS_VREDRAW);
	//	SetClassLong(m_hWnd,GCL_STYLE,lc);
	//	EmbedBrowserObject(m_hWnd);
	//	return TRUE;
	//}
	//else
	//{
	//	EmbedBrowserObject(m_hWnd);
	//	return FALSE;
	//}
	if( m_pWindow != NULL ) return TRUE;
	m_pWindow = new CWebWnd;
	if( m_pWindow == NULL ) return FALSE;
	m_ParentWnd = m_pWindow->Init(this, GetManager()->GetPaintWindow());

	if (m_pWindow)
	{
		EmbedBrowserObject(m_ParentWnd);
	}
	return S_OK;

}
void CWebBrowserUI::Onfocus()
{
	HRESULT hr;
	//IDispatch *pDisp = m_pDispatch->;
	//ASSERT( pDisp ); //if NULL, we failed
	IWebBrowser2	*webBrowser2;
	LPDISPATCH  lpDispatch;
	IHTMLDocument2 *pDocument = NULL;
	IHTMLElement *pBody = NULL;
	IHTMLElement2 *pElement = NULL;
	IHTMLTextContainer *pCont = NULL;

	IHTMLWindow2   *pWindow; 

	//	IWebBrowser2	*webBrowser2;
	//IOleObject		*browserObject;

	// Retrieve the browser object's pointer we stored in our window's GWL_USERDATA when
	// we initially attached the browser object to this window.
	m_pBrowserObj = *((IOleObject **)GetWindowLong(m_ParentWnd, GWL_USERDATA));

	// We want to get the base address (ie, a pointer) to the IWebBrowser2 object embedded within the browser
	// object, so we can call some of the functions in the former's table.
	if (!m_pBrowserObj->QueryInterface(IID_IWebBrowser2, (void**)&webBrowser2))
	{
		if (!webBrowser2->get_Document(&lpDispatch))
		{
			// We want to get a pointer to the IHTMLDocument2 object embedded within the DISPATCH
			// object, so we can call some of the functions in the former's table.
			if (!lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pDocument))
			{


				if (!pDocument-> get_parentWindow   (&pWindow))
				{
					pWindow->focus();
				}

			}
		}
	}


	//	// 获得Html文档指针
	//	IHTMLDocument2 *pDocument = NULL;
	//	hr = m_pDispatch->QueryInterface( IID_IHTMLDocument2, (void**)&pDocument );
	////	ASSERT( SUCCEEDED( hr ) );
	//	//ASSERT( pDocument );
	//
	//	IHTMLElement *pBody = NULL;
	//	hr = pDocument->get_body( &pBody );
	//	//ASSERT( SUCCEEDED( hr ) );
	//	//ASSERT( pBody );
	//
	//	// 从body获得IHTMLElement2接口指针，用以访问滚动条
	//	IHTMLElement2 *pElement = NULL;
	//	hr = pBody->QueryInterface(IID_IHTMLElement,(void**)&pElement);
	//	//ASSERT(SUCCEEDED(hr));
	//	//ASSERT( pElement );
	//
	//	IHTMLTextContainer *pCont;   
	//	pBody->QueryInterface(IID_IHTMLTextContainer, (LPVOID *)(&pCont));   
	//	//ASSERT(pCont);   
	//
	//
	//	// 向下滚动100个像素
	//		pElement->put_scrollTop( 100 );  
	//
	//
	//	// 获得滚动条高度
	//	long scroll_height;  
	//	pCont->get_scrollHeight( &scroll_height );
	//
	//	// 获得滚动条宽度
	//	//long scroll_width;  
	//	//if(FAILED(pCont->get_scrollWidth( &scroll_width )))
	////		AfxMessageBox("get scrollWidth failed.");
	//
	//	// 获得滚动条位置，从顶端开始
	//	//long scroll_top;
	//	//if(FAILED(pCont->get_scrollTop( &scroll_top )))
	//	//	AfxMessageBox("get scrollWidth failed.");
	//	// 获得滚动条位置，从左端开始
	//	//long scroll_left;
	//	//if(FAILED(pCont->get_scrollLeft( &scroll_left )))
	//	//	AfxMessageBox("get scrollWidth failed.");
	//
	//	//CString str;
	//	//str.Format("%d,%d,%d,%d",scroll_height,scroll_width,scroll_top,scroll_left);
	//	//AfxMessageBox(str);
	//
	if (pCont!=NULL)
	{
		pCont->Release();
	}
	if (pElement!=NULL)
	{
		pElement->Release();
	}
	if (pBody!=NULL)
	{
		pBody->Release();
	}
	if (pDocument!=NULL)
	{
		pDocument->Release();
	}
	if (m_pDispatch!= NULL)
	{
		m_pDispatch->Release();
	}


}
void CWebBrowserUI::DoPageAction(DWORD action)
{	
	IWebBrowser2	*webBrowser2;
	//IOleObject		*browserObject;

	// Retrieve the browser object's pointer we stored in our window's GWL_USERDATA when
	// we initially attached the browser object to this window.
	m_pBrowserObj = *((IOleObject **)GetWindowLong(m_ParentWnd, GWL_USERDATA));

	// We want to get the base address (ie, a pointer) to the IWebBrowser2 object embedded within the browser
	// object, so we can call some of the functions in the former's table.
	if (!m_pBrowserObj->QueryInterface(IID_IWebBrowser2, (void**)&webBrowser2))
	{
		// Ok, now the pointer to our IWebBrowser2 object is in 'webBrowser2', and so its VTable is
		// webBrowser2->lpVtbl.

		// Call the desired function
		switch (action)
		{
		case WEBPAGE_GOBACK:
			{
				// Call the IWebBrowser2 object's GoBack function.
				webBrowser2->GoBack();
				break;
			}

		case WEBPAGE_GOFORWARD:
			{
				// Call the IWebBrowser2 object's GoForward function.
				webBrowser2->GoForward();
				break;
			}

		case WEBPAGE_GOHOME:
			{
				// Call the IWebBrowser2 object's GoHome function.
				webBrowser2->GoHome();
				break;
			}

		case WEBPAGE_SEARCH:
			{
				// Call the IWebBrowser2 object's GoSearch function.
				webBrowser2->GoSearch();
				break;
			}

		case WEBPAGE_REFRESH:
			{
				// Call the IWebBrowser2 object's Refresh function.
				webBrowser2->Refresh();
			}

		case WEBPAGE_STOP:
			{
				// Call the IWebBrowser2 object's Stop function.
				webBrowser2->Stop();
			}
		}

		// Release the IWebBrowser2 object.
		webBrowser2->Release();
	}
}

void CWebBrowserUI::UnEmbedBrowserObject(HWND /*hwnd*/)
{
 //////////////////////////////////////////////////////////////////////////
 //
 // Not implemented for multiple browsing...
 //
 //IOleObject **ppBrowserHandle;
 //IOleObject *pBrowserObject;
 //
 //// Retrieve the browser object's pointer we stored in our window's GWL_USERDATA when
 //// we initially attached the browser object to this window.
 //if ((ppBrowserHandle = (IOleObject **)GetWindowLong(hwnd, GWL_USERDATA)))
 //{
 // // Unembed the browser object, and release its resources.
 // pBrowserObject = *ppBrowserHandle;
 // pBrowserObject->Close(OLECLOSE_NOSAVE);
 // pBrowserObject->Release();
 //}
 //////////////////////////////////////////////////////////////////////////

 if (m_pBrowserObj)
 {
  // Unembed the browser object, and release its resources.
  m_pBrowserObj->Close(OLECLOSE_NOSAVE);
  m_pBrowserObj->Release();

  return;
 }

 // You must have called this for a window that wasn't successfully passed to EmbedBrowserObject().
 // Bad boy!
 _ASSERT(0);
}

long CWebBrowserUI::DisplayHTMLStr(LPCTSTR string)
{ 
 IWebBrowser2 *pWebBrowser2;
 LPDISPATCH  lpDispatch;
 IHTMLDocument2 *pHtmlDoc2;
 SAFEARRAY  *psfArray;
 VARIANT   varMyURL;
 VARIANT   *pVar;
 BSTR   bstr;

 // Assume an error.
 bstr = 0;

 //////////////////////////////////////////////////////////////////////////
 //
 // Not implemented for multiple browsing...
 //
 //IOleObject **ppBrowserHandle;
 //IOleObject *pBrowserObject;
 //
 //// Retrieve the browser object's pointer we stored in our window's GWL_USERDATA when
 //// we initially attached the browser object to this window.
 //if ((ppBrowserHandle = (IOleObject **)GetWindowLong(hwnd, GWL_USERDATA)))
 //{
 // // Unembed the browser object, and release its resources.
 // pBrowserObject = *ppBrowserHandle;
 //}
 //////////////////////////////////////////////////////////////////////////

 // We want to get the base address (ie, a pointer) to the IWebBrowser2 object embedded within the browser
 // object, so we can call some of the functions in the former's table.
 if (!m_pBrowserObj->QueryInterface(IID_IWebBrowser2, (void**)&pWebBrowser2))
 {
  // Before we can get_Document(), we actually need to have some HTML page loaded in the browser. So,
  // let's load an empty HTML page. Then, once we have that empty page, we can get_Document() and
  // write() to stuff our HTML string into it.
  VariantInit(&varMyURL);
  varMyURL.vt = VT_BSTR;
  varMyURL.bstrVal = SysAllocString(L"about:blank");

  // Call the Navigate2() function to actually display the page.
  pWebBrowser2->Navigate2(&varMyURL, 0, 0, 0, 0);

  // Free any resources (including the BSTR).
  VariantClear(&varMyURL);

  // Call the IWebBrowser2 object's get_Document so we can get its DISPATCH object. I don't know why you
  // don't get the DISPATCH object via the browser object's QueryInterface(), but you don't.
  if (!pWebBrowser2->get_Document(&lpDispatch))
  {
   // We want to get a pointer to the IHTMLDocument2 object embedded within the DISPATCH
   // object, so we can call some of the functions in the former's table.
   if (!lpDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pHtmlDoc2))
   {
    // Our HTML must be in the form of a BSTR. And it must be passed to write() in an
    // array of "VARIENT" structures. So let's create all that.
    if ((psfArray = SafeArrayCreate(VT_VARIANT, 1, (SAFEARRAYBOUND *)&ArrayBound)))
    {
     if (!SafeArrayAccessData(psfArray, (void**)&pVar))
     {
      pVar->vt = VT_BSTR;
#ifndef UNICODE
      {
       wchar_t  *buffer;
       DWORD  size;

       size = MultiByteToWideChar(CP_ACP, 0, string, -1, 0, 0);
       if (!(buffer = (wchar_t *)GlobalAlloc(GMEM_FIXED, sizeof(wchar_t) * size))) goto bad;
       MultiByteToWideChar(CP_ACP, 0, string, -1, buffer, size);
       bstr = SysAllocString(buffer);
       GlobalFree(buffer);
      }
#else
      bstr = SysAllocString(string);
#endif
      // Store our BSTR pointer in the VARIENT.
      if ((pVar->bstrVal = bstr))
      {
       // Pass the VARIENT with its BSTR to write() in order to shove our desired HTML string
       // into the body of that empty page we created above.
       pHtmlDoc2->write(psfArray);

       // Normally, we'd need to free our BSTR, but SafeArrayDestroy() does it for us
       //       SysFreeString(bstr);
      }
     }

     // Free the array. This also frees the VARIENT that SafeArrayAccessData created for us,
     // and even frees the BSTR we allocated with SysAllocString
     SafeArrayDestroy(psfArray);
    }

    // Release the IHTMLDocument2 object.
#ifndef UNICODE
bad:   
#endif
    pHtmlDoc2->Release();
   }

   // Release the DISPATCH object.
   lpDispatch->Release();
  }

  // Release the IWebBrowser2 object.
  pWebBrowser2->Release();
 }

 // No error?
 if (bstr) return 0;

 // An error
 return -1;
}

long CWebBrowserUI::DisplayHTMLPage(LPCTSTR webPageName)
{
 IWebBrowser2 *pWebBrowser2;
 VARIANT   varMyURL;

 //////////////////////////////////////////////////////////////////////////
 //
 // Not implemented for multiple browsing...
 //
 //IOleObject **ppBrowserHandle;
 //IOleObject *pBrowserObject;
 //
 //// Retrieve the browser object's pointer we stored in our window's GWL_USERDATA when
 //// we initially attached the browser object to this window.
 //if ((ppBrowserHandle = (IOleObject **)GetWindowLong(hwnd, GWL_USERDATA)))
 //{
 // // Unembed the browser object, and release its resources.
 // pBrowserObject = *ppBrowserHandle;
 //}
 //////////////////////////////////////////////////////////////////////////

 // We want to get the base address (ie, a pointer) to the IWebBrowser2 object embedded within the browser
 // object, so we can call some of the functions in the former's table.
 if (!m_pBrowserObj->QueryInterface(IID_IWebBrowser2, (void**)&pWebBrowser2))
 {
  // Our URL (ie, web address, such as "http://www.microsoft.com" or an HTM filename on disk
  // such as "c:\myfile.htm") must be passed to the IWebBrowser2's Navigate2() function as a BSTR.
  // A BSTR is like a pascal version of a double-byte character string. In other words, the
  // first unsigned short is a count of how many characters are in the string, and then this
  // is followed by those characters, each expressed as an unsigned short (rather than a
  // char). The string is not nul-terminated. The OS function SysAllocString can allocate and
  // copy a UNICODE C string to a BSTR. Of course, we'll need to free that BSTR after we're done
  // with it. If we're not using UNICODE, we first have to convert to a UNICODE string.
  //
  // What's more, our BSTR needs to be stuffed into a VARIENT struct, and that VARIENT struct is
  // then passed to Navigate2(). Why? The VARIENT struct makes it possible to define generic
  // 'datatypes' that can be used with all languages. Not all languages support things like
  // nul-terminated C strings. So, by using a VARIENT, whose first field tells what sort of
  // data (ie, string, float, etc) is in the VARIENT, COM interfaces can be used by just about
  // any language.
  VariantInit(&varMyURL);
  varMyURL.vt = VT_BSTR;

#ifndef UNICODE
  {
   wchar_t  *buffer;
   DWORD  size;

   size = MultiByteToWideChar(CP_ACP, 0, webPageName, -1, 0, 0);
   if (!(buffer = (wchar_t *)GlobalAlloc(GMEM_FIXED, sizeof(wchar_t) * size))) goto badalloc;
   MultiByteToWideChar(CP_ACP, 0, webPageName, -1, buffer, size);
   varMyURL.bstrVal = SysAllocString(buffer);
   GlobalFree(buffer);
  }
#else
  varMyURL.bstrVal = SysAllocString(webPageName);
#endif
  if (!varMyURL.bstrVal)
  {
#ifndef UNICODE
badalloc:
#endif
   pWebBrowser2->Release();
   return -6;
  }

  // Call the Navigate2() function to actually display the page.
  pWebBrowser2->Navigate2(&varMyURL, 0, 0, 0, 0);

  // Free any resources (including the BSTR we allocated above).
  VariantClear(&varMyURL);

  // We no longer need the IWebBrowser2 object (ie, we don't plan to call any more functions in it,
  // so we can release our hold on it). Note that we'll still maintain our hold on the browser
  // object.
  pWebBrowser2->Release();

  // Success
  return 0;
 }

 // failed
 return -5;
}

long CWebBrowserUI::EmbedBrowserObject(HWND hwnd)
{
 IWebBrowser2  *pWebBrowser2;
 RECT    rect;
 CWBStorage   Storage;
/* CWBClientSite  **/m_pClientSite = new CWBClientSite(hwnd);

/* CWBDispatch   **/m_pDispatch = new CWBDispatch(hwnd);

 // Get a pointer to the browser object and lock it down (so it doesn't "disappear" while we're using
 // it in this program). We do this by calling the OS function OleCreate().
 // 
 // NOTE: We need this pointer to interact with and control the browser. With normal WIN32 controls such as a
 // Static, Edit, Combobox, etc, you obtain an HWND and send messages to it with SendMessage(). Not so with
 // the browser object. You need to get a pointer to its base class (as returned by OleCreate()).
 //
 // For example, the browser object happens to have a SetHostNames() function we want to call. So, after we
 // retrieve the pointer to the browser object (in a local we'll name 'm_pBrowserObj'), then we can call that
 // function, and pass it args, as so:
 //
 // m_pBrowserObj->SetHostNames(SomeString, SomeString);

 if (!OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, 0, (IOleClientSite *)m_pClientSite, &Storage, (void**)&m_pBrowserObj))
 {
  //////////////////////////////////////////////////////////////////////////
  //
  // Not implemented for multiple browsing...
  //
  // Ok, we now have the pointer to the browser object in 'browserObject'. Let's save this in the
  // memory block we allocated above, and then save the pointer to that whole thing in our window's
  // USERDATA field. That way, if we need multiple windows each hosting its own browser object, we can
  // call EmbedBrowserObject() for each one, and easily associate the appropriate browser object with
  // its matching window and its own objects containing per-window data.
    IOleObject **ppObj = &m_pBrowserObj;
    SetWindowLong(hwnd, GWL_USERDATA, (LONG)ppObj);
  //////////////////////////////////////////////////////////////////////////

  // We can now call the browser object's SetHostNames function. SetHostNames lets the browser object know our
  // application's name and the name of the document in which we're embedding the browser. (Since we have no
  // document name, we'll pass a 0 for the latter). When the browser object is opened for editing, it displays
  // these names in its titlebar.
  // 
  // We are passing 2 args to SetHostNames. Oh yeah, the L is because we need UNICODE strings.
  // And BTW, the host and document names can be anything you want.

  m_pBrowserObj->SetHostNames(L"My Host Name", 0);

  GetClientRect(m_ParentWnd,&rect);

  // Let browser object know that it is embedded in an OLE container.
  if (!OleSetContainedObject((IUnknown *)m_pBrowserObj, TRUE))
  {
   // Set the display area of our browser control the same as our window's size
   // and actually put the browser object into our window.
   if (!m_pBrowserObj->DoVerb(OLEIVERB_SHOW, NULL, (IOleClientSite *)m_pClientSite, -1, hwnd, &rect))
   {
    // We want to get the base address (ie, a pointer) to the IWebBrowser2 object embedded within the browser
    // object, so we can call some of the functions in the former's table. For example, one IWebBrowser2 function
    // we intend to call below will be Navigate2(). So we call the browser object's QueryInterface to get our
    // pointer to the IWebBrowser2 object.
    if (!m_pBrowserObj->QueryInterface(IID_IWebBrowser2, (void**)&pWebBrowser2))
    {
     // Let's call several functions in the IWebBrowser2 object to position the browser display area
     // in our window. The functions we call are put_Left(), put_Top(), put_Width(), and put_Height().
     pWebBrowser2->put_Left(0);
     pWebBrowser2->put_Top(0);
     pWebBrowser2->put_Width(rect.right);
     pWebBrowser2->put_Height(rect.bottom);


	 m_pDispatch->ConnectEvents();
     // We no longer need the IWebBrowser2 object (ie, we don't plan to call any more functions in it
     // right now, so we can release our hold on it). Note that we'll still maintain our hold on the
     // browser object until we're done with that object.
     pWebBrowser2->Release();

     // Success
     return 0;
    }
// Something went wrong!
    // QueryInterface failed
   }
   // DoVerb Failed
  }
  // OleSetContainedObject failed

  if (m_pClientSite)
  {
   delete m_pClientSite;
  }
  if (m_pDispatch)
  {
	  delete m_pDispatch;
  }
  UnEmbedBrowserObject(hwnd);
  return -3;
 }
 return -2;
}
void CWebBrowserUI::ResizeBrowser(DWORD x,DWORD y,DWORD width, DWORD height)
{
	IWebBrowser2	*pWebBrowser2;
	//IOleObject		*browserObject;

	// Retrieve the browser object's pointer we stored in our window's GWL_USERDATA when
	// we initially attached the browser object to this window.
	//browserObject = *((IOleObject **)GetWindowLong(hwnd, GWL_USERDATA));

	// We want to get the base address (ie, a pointer) to the IWebBrowser2 object embedded within the browser
	// object, so we can call some of the functions in the former's table.
	//if (!browserObject->lpVtbl->QueryInterface(browserObject, &IID_IWebBrowser2, (void**)&webBrowser2))
	//{
		// Ok, now the pointer to our IWebBrowser2 object is in 'webBrowser2', and so its VTable is
		// webBrowser2->lpVtbl.

		// Call are put_Width() and put_Height() to set the new width/height.
	//	webBrowser2->lpVtbl->put_Left(webBrowser2,x);
		//webBrowser2->lpVtbl->put_Top(webBrowser2,y);
		//webBrowser2->lpVtbl->put_Width(webBrowser2, width);
		//webBrowser2->lpVtbl->put_Height(webBrowser2, height);

		// We no longer need the IWebBrowser2 object (ie, we don't plan to call any more functions in it,
		// so we can release our hold on it). Note that we'll still maintain our hold on the browser
		// object.
		//webBrowser2->lpVtbl->Release(webBrowser2);
	//}

	if (!m_pBrowserObj->QueryInterface(IID_IWebBrowser2, (void**)&pWebBrowser2))
	{
		pWebBrowser2->put_Left(x);
		pWebBrowser2->put_Top(y);
		pWebBrowser2->put_Width(width);
		pWebBrowser2->put_Height(height);

		pWebBrowser2->Release();
	}

}
//LPCTSTR  CWebBrowser::GetPointUrl(/*POINT pt*/)
//{
//	LPCTSTR strUrl;
//	//LPDISPATCH pDisp; 
//	IHTMLDocument2* pHTMLDoc=NULL;
//	IHTMLElement* pElem = NULL;
//	IHTMLElement* pElemParent = NULL;
//
//	try{
//		//pDisp = GetHtmlDocument();
//		if(m_pDispatch!=NULL)
//		{
//			if(SUCCEEDED(m_pDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDoc)))
//			{
//				if(pHTMLDoc!=NULL)
//				{
//					POINT pt;
//					GetCursorPos(&pt);
//					ScreenToClient(m_hWnd,&pt);
//					if(SUCCEEDED(pHTMLDoc->elementFromPoint(pt.x, pt.y, &pElem)))
//					{
//						BSTR bstrSrc;
//						while(pElem)
//						{
//							pElem->get_tagName(&bstrSrc);
//							strUrl = bstrSrc;
//							SysFreeString(bstrSrc);
//							if (strUrl!=_T("A"))
//							{
//								pElem->get_parentElement(&pElemParent);
//								pElem->Release();
//								pElem = pElemParent;
//							}
//							else
//							{
//								pElem->toString(&bstrSrc);
//								strUrl=bstrSrc;
//								SysFreeString(bstrSrc);
//								break;
//							}
//						}
//					}
//				}
//			}
//		}
//	}catch(...){}
//
//	try{
//		if (pElem)
//		{
//			pElem->Release();
//		}
//       if (pHTMLDoc)
//       {
//		  pHTMLDoc->Release();
//       }
//// 		if (pDisp)
//// 		{
//// 				pDisp->Release();
//// 		}
//
//	}catch(...){}
//
//		return strUrl;
//}
//LRESULT CWebBrowserUI::AndydowProc(HWND hWnd,ANDYPARAMS param)
//{
//	switch(param.uMsg)
//	{
//	
//		case WM_LBUTTONDOWN:
//		{
//			//m_ptMouseDown = param.mousept;
//			m_ptMouseDown.x = param.mousept.x;
//			m_ptMouseDown.y = param.mousept.y;
//			break;
//		}
//		case WM_PAINT:
//		{
//			PAINTSTRUCT ps;
//			BeginPaint(m_hWnd,&ps);
//			SetBkMode(ps.hdc,TRANSPARENT);
//			param.wParam = (WPARAM)ps.hdc;
//			LRESULT res = DefaultProc(param);
//			EndPaint(m_hWnd,&ps);
//			return res;
//		}
//	  //  case DISPID_NEWWINDOW2:
//			//{
//			//	int i = 1;
//			//	OutputDebugStr(_T("NEWWINDOW2"));
//			//}
//			//break;
//		case  WM_TITLECHANGE:
//			{
//				WEBNITIFY nm;
//				nm.hdr.code = WM_TITLECHANGE;
//				memset(nm.url,0,2048);
//				sprintf(nm.url,"%s",(char*)param.lParam);
//				nm.hdr.hwndFrom = m_hWnd;
//				nm.hdr.idFrom = GetId();
//				SendMessage(m_parent->Handle(),WM_NOTIFY,GetId(),(LPARAM)&nm);
//				break;
//			}
//	case WM_NEW_IEVIEW:
//		{
//			WEBNITIFY nm;
//			nm.hdr.code = WM_NEW_IEVIEW;
//			//nm.url = (char*)(param.lParam);
//			memset(nm.url,0,2048);
//            sprintf(nm.url,"%s",(char*)param.lParam);
//			nm.hdr.hwndFrom = m_hWnd;
//			nm.hdr.idFrom = GetId();
//			//NMHDR nm;
//			//nm.code = WM_NEW_IEVIEW;
//			//nm.hwndFrom = m_hWnd;
//			//nm.idFrom = GetId();
//			SendMessage(m_parent->Handle(),WM_NOTIFY,GetId(),(LPARAM)&nm);
//			/*IUnknown*   pUnk   =  m_pBrowserObj->GetControlUnknown();
//			if   (pUnk)
//			{
//				IDispatch*   pDisp   =   NULL;
//				HRESULT   hr   =   pUnk-> QueryInterface(IID_IDispatch,   (void**)&pDisp);
//
//				if   (SUCCEEDED(hr)   &&   pDisp)
//					*ppDisp   =   pDisp;
//			}*/
//		//} 
//			break;
//
//		}
//		case WM_ERASEBKGND:
//		{			
//			//if(m_brush)
//			//{
//			//	RECT rc;
//			//	::GetClientRect(m_hWnd,&rc);
//			//	FillRect(param.hdc,&rc,m_brush);
//			//	return TRUE;
//			//}
//			//else
//			//	break;//FillRect(param.hdc,&rc,(HBRUSH)GetStockObject(18/DC_BRUSH///));	
//			break;
//		}
//		case  WM_SIZE:
//		{
//			RECT rc;
//			::GetClientRect(m_hWnd,&rc);
//			ResizeBrowser(rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top);
//			OutputDebugStr(_T("WM_SIZE"));
//			break;
//
//		}
//		
//	}
//	return AndyControls::AndydowProc(m_hWnd,param);
//}
//


HRESULT CWBDispatch::ConnectEvents()
{
	IOleObject		*browserObject;
//	IOleInPlaceObject    *inplace;
	browserObject = *((IOleObject **)GetWindowLong(m_hWindow, GWL_USERDATA));
	browserObject->QueryInterface(IID_IWebBrowser2, (void**)&m_pWebBrowser2);
	  HRESULT hr = S_OK;
	if (FAILED(hr = browserObject->QueryInterface(IID_IConnectionPointContainer, (void**)&m_pCPContainer)))
		return hr;
	if (FAILED(hr = m_pCPContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pConnectionPoint)))
		return hr;
	if (FAILED(hr = m_pConnectionPoint->Advise(static_cast<IDispatch *>(this), &dwCookie)))
		return hr;
	return hr;
}

void CWBDispatch::Exit()
{
	// Step 5: Unadvise. 注意m_pConnectionPoint 应当在CSomeClass的析构函数中释放
	//
	if (m_pConnectionPoint)
	{
		HRESULT hr = m_pConnectionPoint->Unadvise(dwCookie);
		if (FAILED(hr))
		{
			//::MessageBox(NULL, _T("Failed to Unadvise"),
			//	_T("C++ Event Sink"), MB_OK);
		}
	}
}
//HRESULT STDMETHODCALLTYPE CWBDispatch::QueryInterface(REFIID riid, LPVOID *ppvObj)
//{
//	{
//		HRESULT ret = S_OK;
//		if(riid == IID_IUnknown)
//			*ppvObj = static_cast<IUnknown *>(this);
//		else if(riid == IID_IDispatch)
//			*ppvObj = static_cast<IDispatch *>(this);
//		else
//			*ppvObj = 0, ret = E_NOINTERFACE;;
//		return ret;                
//	}
//}
HRESULT STDMETHODCALLTYPE CWBDispatch::Invoke(DISPID  dispIdMember, REFIID  riid, LCID  lcid, WORD  wFlags, DISPPARAMS FAR*  pDispParams, VARIANT FAR*  pVarResult, EXCEPINFO FAR*  pExcepInfo, unsigned int FAR*  puArgErr)
{

	//		    MSG				msg;
	/*  
	参数pDispParams->rgvarg是一个数组，其中值如下
	//0 : bstrUrl, 1 : bstrContext, 2 : dwFlags, 3 : *bCancel, 4 : **pDisp
	你可以根据第4个参数来得到你要的值
	*/

	//IOleObject		*browserObject;
	//browserObject = *((IOleObject **)GetWindowLong(m_hWindow, GWL_USERDATA));
	//browserObject->QueryInterface(IID_IWebBrowser2, (void**)&pWebBrowser2);
	if (!pDispParams)
		return E_INVALIDARG;

	switch (dispIdMember)
	{
	case DISPID_STATUSTEXTCHANGE:
		{
			BSTR lpStatusText;
			IOleObject		*browserObject;
			IWebBrowser2	*pWebBrowser2;
			browserObject = *((IOleObject **)GetWindowLong(m_hWindow, GWL_USERDATA));
			browserObject->QueryInterface(IID_IWebBrowser2, (void**)&pWebBrowser2);
	    

			if (pWebBrowser2 != NULL)
			{
			    pWebBrowser2->get_StatusText(&lpStatusText);
				//if (!strcmp(lpStatusText, ""))
				//{
					_bstr_t b = lpStatusText;
					char* lpszText2 = b;
				//}
			}

			break;
		}
	case DISPID_NAVIGATECOMPLETE:
		{
			break;
		}
	case DISPID_TITLECHANGE:
		{

			if (pDispParams->cArgs > 0 && pDispParams->rgvarg[0].vt == VT_BSTR)
			{
				BSTR   context= pDispParams->rgvarg[0].bstrVal;
				_bstr_t b = context;
				char* lpszText2 = b;
				SendMessage(m_hWindow,WM_TITLECHANGE,0,(LPARAM)lpszText2);
			}
			else
			{
				
			}
			break;
		}
	case DISPID_DOCUMENTCOMPLETE:
		{
			SendMessage(m_hWindow,WM_DOCUMENT_COMPLETE,0,0);
			break;
		}
	case DISPID_NEWWINDOW2:
		{
			*V_BOOLREF(&pDispParams->rgvarg[0]) = VARIANT_TRUE;
			break;
		}
	case DISPID_NEWWINDOW3:
		{
		if (pDispParams->cArgs > 0 && pDispParams->rgvarg[0].vt == VT_BSTR)
		  {
			BSTR   url   =   pDispParams-> rgvarg[0].bstrVal; 
			_bstr_t b = url;
			char* lpszText2 = b;
			SendMessage(m_hWindow,WM_NEW_IEVIEW,0,(LPARAM)lpszText2);
		  }
			break;
		}
	case DISPID_BEFORENAVIGATE2:
		{
			CComBSTR url = NULL;
			m_pWebBrowser2->get_LocationURL(&url);

			if (url ==_T("http://www.163.com"))
			{
				//CComPtr<IWebBrowser2> spBrowser;
				//CComPtr<IDispatch> spDisp = ((*pDispParams).rgvarg)[6].pdispVal;
				//spDisp->QueryInterface(IID_IWebBrowser2, (void**)&spBrowser);
				//spBrowser->Stop();
				//CComBSTR newURL = _T("http://www.baidu.com");
				//spBrowser->Navigate(newURL, NULL, NULL, NULL, NULL);
				//((*pDispParams).rgvarg)[0].boolVal = TRUE;
			}
			break;
		}
	default:
		break;
		// 		if (dispIdMember== DISPID_STATUSTEXTCHANGE)
		// 		{
		// 		}
		//if (dispIdMember== DISPID_NAVIGATECOMPLETE)
		//{
		//}
		/*if (dispIdMember == DISPID_TITLECHANGE)
		{
		SendMessage(m_hWindow,WM_TIMECHANGE,0,0);
		}*/
		//if (dispIdMember == DISPID_DOCUMENTCOMPLETE)
		//{
		//	//PostThreadMessage(dwCurrentThreadID, WM_QUIT, 0, 0);
		//	//TRACE("\n       DISPID_DOCUMENTCOMPLETE ");
		//	//OutputDebugStr(_T("DISPID_DOCUMENTCOMPLETE"));
		//	SendMessage(m_hWindow,WM_DOCUMENT_COMPLETE,0,0);
		//}
		//if (dispIdMember ==  DISPID_NEWWINDOW2)
		//	//TRACE("\n       DISPID_BEFORENAVIGATE2 ");
		//{
		//	//pDispParams->rgvarg[0].pvarVal->vt = VT_BOOL;
		//	//pDispParams->rgvarg[0].pvarVal->boolVal = VARIANT_TRUE;


		//	*V_BOOLREF(&pDispParams->rgvarg[0]) = VARIANT_TRUE;


		////	LPCTSTR url = pweb->GetPointUrl();
		//}
		//		if (dispIdMember==DISPID_NEWWINDOW3)
		//		{
		//			BSTR   url   =   pDispParams-> rgvarg[0].bstrVal; 
		//
		//			//BSTR bstrText = ::SysAllocString(url);
		//
		//			//char* lpszText2 = _com_util::ConvertBSTRToString(bstrText);
		//
		////
		//			_bstr_t b = url;
		//		    char* lpszText2 = b;
		//
		//
		//			//char*    char1=AnsiString(url).c_str(); 
		//
		//
		//			//char* lpszText2 = _com_util::ConvertBSTRToString(url);
		//			SendMessage(m_hWindow,WM_NEW_IEVIEW,0,(LPARAM)lpszText2);
		//		   // SysFreeString(url); // 用完释
		//		   // delete[] lpszText2;
		//
		//		}
		//      if (dispIdMember==DISPID_BEFORENAVIGATE2)
		//      {
		//	//CComBSTR url = ((*pDispParams).rgvarg)[5].pvarVal->bstrVal;

		//	CComBSTR url = NULL;
		//	m_pWebBrowser2->get_LocationURL(&url);
		//	
		//	if (url ==_T("http://www.163.com"))
		//	{
		//		//CComPtr<IWebBrowser2> spBrowser;
		//		//CComPtr<IDispatch> spDisp = ((*pDispParams).rgvarg)[6].pdispVal;
		//		//spDisp->QueryInterface(IID_IWebBrowser2, (void**)&spBrowser);
		//		//spBrowser->Stop();
		//		//CComBSTR newURL = _T("http://www.baidu.com");
		//		//spBrowser->Navigate(newURL, NULL, NULL, NULL, NULL);
		//		//((*pDispParams).rgvarg)[0].boolVal = TRUE;
		//	}
		//}
	}
	return S_OK;

}
//类CWBStorage：
////////////////////////////////////// CWBStorage functions  /////////////////////////////////////////
// NOTE: The browser object doesn't use the IStorage functions, so most of these are us just returning
// E_NOTIMPL so that anyone who *does* call these functions knows nothing is being done here.

HRESULT STDMETHODCALLTYPE CWBStorage::QueryInterface(
 /* [in] */ REFIID riid,
 /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
 NOTIMPLEMENTED;
}

ULONG STDMETHODCALLTYPE CWBStorage::AddRef( void)
{
 return 1;
}

ULONG STDMETHODCALLTYPE CWBStorage::Release( void)
{
 return 0;
}


HRESULT STDMETHODCALLTYPE CWBStorage::CreateStream(
 /* [string][in] */ const OLECHAR *pwcsName,
 /* [in] */ DWORD grfMode,
 /* [in] */ DWORD reserved1,
 /* [in] */ DWORD reserved2,
 /* [out] */ IStream **ppstm)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::OpenStream(
 /* [string][in] */ const OLECHAR *pwcsName,
 /* [unique][in] */ void *reserved1,
 /* [in] */ DWORD grfMode,
 /* [in] */ DWORD reserved2,
 /* [out] */ IStream **ppstm)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::CreateStorage(
 /* [string][in] */ const OLECHAR *pwcsName,
 /* [in] */ DWORD grfMode,
 /* [in] */ DWORD reserved1,
 /* [in] */ DWORD reserved2,
 /* [out] */ IStorage **ppstg)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::OpenStorage(
 /* [string][unique][in] */ const OLECHAR *pwcsName,
 /* [unique][in] */ IStorage *pstgPriority,
 /* [in] */ DWORD grfMode,
 /* [unique][in] */ SNB snbExclude,
 /* [in] */ DWORD reserved,
 /* [out] */ IStorage **ppstg)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::CopyTo(
 /* [in] */ DWORD ciidExclude,
 /* [size_is][unique][in] */ const IID *rgiidExclude,
 /* [unique][in] */ SNB snbExclude,
 /* [unique][in] */ IStorage *pstgDest)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::MoveElementTo(
 /* [string][in] */ const OLECHAR *pwcsName,
 /* [unique][in] */ IStorage *pstgDest,
 /* [string][in] */ const OLECHAR *pwcsNewName,
 /* [in] */ DWORD grfFlags)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::Commit(
 /* [in] */ DWORD grfCommitFlags)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::Revert( void)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::EnumElements(
 /* [in] */ DWORD reserved1,
 /* [size_is][unique][in] */ void *reserved2,
 /* [in] */ DWORD reserved3,
 /* [out] */ IEnumSTATSTG **ppenum)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::DestroyElement(
 /* [string][in] */ const OLECHAR *pwcsName)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::RenameElement(
 /* [string][in] */ const OLECHAR *pwcsOldName,
 /* [string][in] */ const OLECHAR *pwcsNewName)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::SetElementTimes(
 /* [string][unique][in] */ const OLECHAR *pwcsName,
 /* [unique][in] */ const FILETIME *pctime,
 /* [unique][in] */ const FILETIME *patime,
 /* [unique][in] */ const FILETIME *pmtime)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::SetClass(
 /* [in] */ REFCLSID clsid)
{
 //NOTIMPLEMENTED;
 return(S_OK);
}

HRESULT STDMETHODCALLTYPE CWBStorage::SetStateBits(
 /* [in] */ DWORD grfStateBits,
 /* [in] */ DWORD grfMask)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBStorage::Stat(
   /* [out] */ STATSTG *pstatstg,
   /* [in] */ DWORD grfStatFlag)
{
 NOTIMPLEMENTED;
}

//类CWBClientSite：

/********************************************************************
@created: 2006/01/16
@version: 0.1
@filename:  WBClientSite.h
@file base: WBClientSite
@file ext: h
@author: Pomelo Wu

@purpose: Declaration for class CWBClientSite, a implemetation of
   interface IOleClientSite, IOleInPlaceFrame and IOleInPlaceSite
@remark: NULL

@revision history:
-- Date   Ver. Modifier Description--
2006/01/16  0.1  Pomelo  Create
********************************************************************
@created: 2006/01/16
@version: 0.1
@filename:  WBClientSite.cpp
@file base: WBClientSite
@file ext: cpp
@author: Pomelo Wu

@purpose: Implementation for class CWBClientSite
@remark: NULL

@revision history:
-- Date   Ver. Modifier Description--
2006/01/16  0.1  Pomelo  Create
*********************************************************************/

// Implementation of CWBClientSite, a derived class of IOleClientSite, IOleInPlaceFrame, and IOleInPlaceSite

HRESULT STDMETHODCALLTYPE CWBClientSite::QueryInterface(
 /* [in] */ REFIID riid,
 /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
 // query the interface of IUnknown
 if (!memcmp(&riid, &IID_IUnknown, sizeof(GUID)))
 {
  *ppvObject = reinterpret_cast<IUnknown *> (this);
 }
 // query the interface of IOleClientSite
 else if (!memcmp(&riid, &IID_IOleClientSite, sizeof(GUID)))
 {
  *ppvObject = (IOleClientSite *)this;
 }
 // query the interface of IOleInPlaceSite
 else if (!memcmp(&riid, &IID_IOleInPlaceSite, sizeof(GUID)))
 {
  *ppvObject = (IOleInPlaceSite *)this;
 }
 // For other types of objects the browser wants, just report that we don't have any such objects.
 // NOTE: If you want to add additional functionality to your browser hosting, you may need to
 // provide some more objects here. You'll have to investigate what the browser is asking for
 // (ie, what REFIID it is passing).
 else
 {
  *ppvObject = 0;
  return(E_NOINTERFACE);
 }

 return(S_OK);
}

ULONG STDMETHODCALLTYPE CWBClientSite::AddRef( void)
{
 return 1;
}

ULONG STDMETHODCALLTYPE CWBClientSite::Release( void)
{
 return 0;
}

// IOleClientSite methods
HRESULT STDMETHODCALLTYPE CWBClientSite::SaveObject( void)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::GetMoniker(
 /* [in] */ DWORD dwAssign,
 /* [in] */ DWORD dwWhichMoniker,
 /* [out] */ IMoniker **ppmk)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::GetContainer(
 /* [out] */ IOleContainer **ppContainer)
{
 // Tell the browser that we are a simple object and don't support a container
 *ppContainer = 0;

 return(E_NOINTERFACE);
}

HRESULT STDMETHODCALLTYPE CWBClientSite::ShowObject( void)
{
 return(NOERROR);
}

HRESULT STDMETHODCALLTYPE CWBClientSite::OnShowWindow(
 /* [in] */ BOOL fShow)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::RequestNewObjectLayout( void)
{
 NOTIMPLEMENTED;
}

// IOleWindow methods
HRESULT STDMETHODCALLTYPE CWBClientSite::GetWindow(
 /* [out] */ HWND *phwnd)
{
 *phwnd = m_hWindow;
 return S_OK;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::ContextSensitiveHelp(
 /* [in] */ BOOL fEnterMode)
{
 NOTIMPLEMENTED;
}

// IOleInPlaceUIWindow methods
HRESULT STDMETHODCALLTYPE CWBClientSite::GetBorder(
         /* [out] */ LPRECT lprectBorder)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::RequestBorderSpace(
 /* [unique][in] */ LPCBORDERWIDTHS pborderwidths)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::SetBorderSpace(
 /* [unique][in] */ LPCBORDERWIDTHS pborderwidths)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::SetActiveObject(
 /* [unique][in] */ IOleInPlaceActiveObject *pActiveObject,
 /* [unique][string][in] */ LPCOLESTR pszObjName)
{
 return S_OK;
}

// IOleInPlaceFrame methods
HRESULT STDMETHODCALLTYPE CWBClientSite::InsertMenus(
 /* [in] */ HMENU hmenuShared,
 /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::SetMenu(
 /* [in] */ HMENU hmenuShared,
 /* [in] */ HOLEMENU holemenu,
 /* [in] */ HWND hwndActiveObject)
{
 return S_OK;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::RemoveMenus(
 /* [in] */ HMENU hmenuShared)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::SetStatusText(
 /* [unique][in] */ LPCOLESTR pszStatusText)
{
 return S_OK;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::EnableModeless(
 /* [in] */ BOOL fEnable)
{
 return S_OK;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::TranslateAccelerator(
 /* [in] */ LPMSG lpmsg,
 /* [in] */ WORD wID)
{
 NOTIMPLEMENTED;
}

// IOleInPlaceSite methods
HRESULT STDMETHODCALLTYPE CWBClientSite::CanInPlaceActivate( void)
{
 return S_OK;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::OnInPlaceActivate( void)
{
 return S_OK;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::OnUIActivate( void)
{
 return S_OK;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::GetWindowContext(
 /* [out] */ IOleInPlaceFrame **ppFrame,
 /* [out] */ IOleInPlaceUIWindow **ppDoc,
 /* [out] */ LPRECT lprcPosRect,
 /* [out] */ LPRECT lprcClipRect,
 /* [out][in] */ LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
 *ppFrame = (IOleInPlaceFrame *)this; // maybe incorrect

 // We have no OLEINPLACEUIWINDOW
 *ppDoc = 0;

 // Fill in some other info for the browser
 lpFrameInfo->fMDIApp = FALSE;
 lpFrameInfo->hwndFrame = m_hWindow;  // maybe incorrect
 lpFrameInfo->haccel = 0;
 lpFrameInfo->cAccelEntries = 0;

 // Give the browser the dimensions of where it can draw. We give it our entire window to fill
 GetClientRect(lpFrameInfo->hwndFrame, lprcPosRect);
 GetClientRect(lpFrameInfo->hwndFrame, lprcClipRect);

 return S_OK;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::Scroll(
 /* [in] */ SIZE scrollExtant)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::OnUIDeactivate(
 /* [in] */ BOOL fUndoable)
{
 return S_OK;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::OnInPlaceDeactivate( void)
{
 return S_OK;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::DiscardUndoState( void)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::DeactivateAndUndo( void)
{
 NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE CWBClientSite::OnPosRectChange(
 /* [in] */ LPCRECT lprcPosRect)
{
	IOleObject		*browserObject;
	IOleInPlaceObject    *inplace;

	browserObject = *((IOleObject **)GetWindowLong(m_hWindow, GWL_USERDATA));
	if (!browserObject->QueryInterface(IID_IOleInPlaceObject, (void**)&inplace))
	{
		// Give the browser the dimensions of where it can draw.
		inplace->SetObjectRects(lprcPosRect, lprcPosRect);
		inplace->Release();
	}


 return S_OK;
}
} // namespace DuiLib
////调用示例：
//
//// Sample.cpp : Defines the entry point for the application.
////
//
//#include "stdafx.h"
//#include "Resource.h"
//
//#include <mshtml.h>  // Defines of stuff like IHTMLDocument2. This is an include file with Visual C 6 and above
//#include <exdisp.h>  // Defines of stuff like IWebBrowser2. This is an include file with Visual C 6 and above
//
//#include "WebBrowser.h"
//#pragma comment (lib, "webbrowser.lib")
//
//CWebBrowser *g_pWebBrowser = NULL;
//
//#define MAX_LOADSTANDY 100
//
//// Global Variables:
//HINSTANCE hInst;        // current instance
//TCHAR szTitle[MAX_LOADSTANDY];     // The title bar text
//TCHAR szWindowClass[MAX_LOADSTANDY];   // the main window class name
//
//// Forward declarations of functions included in this code module:
//ATOM    MyRegisterClass(HINSTANCE hInstance);
//BOOL    InitInstance(HINSTANCE, int);
//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
//
//int APIENTRY _tWinMain(HINSTANCE hInstance,
//                     HINSTANCE hPrevInstance,
//                     LPTSTR    lpCmdLine,
//                     int       nCmdShow)
//{
// UNREFERENCED_PARAMETER(hPrevInstance);
// UNREFERENCED_PARAMETER(lpCmdLine);
//
//  // TODO: Place code here.
// MSG msg;
// HACCEL hAccelTable;
//
// // COM initialization
// OleInitialize(NULL);
//
// // Initialize global strings
// LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTANDY);
// LoadString(hInstance, IDC_IEMINI, szWindowClass, MAX_LOADSTANDY);
// MyRegisterClass(hInstance);
//
// // Perform application initialization:
// if (!InitInstance (hInstance, nCmdShow))
// {
//  return FALSE;
// }
//
// hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IEMINI));
//
// // Main message loop:
// while (GetMessage(&msg, NULL, 0, 0))
// {
//  if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//  {
//   TranslateMessage(&msg);
//   DispatchMessage(&msg);
//  }
// }
//
// OleUninitialize();
//
// return (int) msg.wParam;
//}
//
////
////  FUNCTION: MyRegisterClass()
////
////  PURPOSE: Registers the window class.
////
////  COMMENTS:
////
////    This function and its usage are only necessary if you want this code
////    to be compatible with Win32 systems prior to the 'RegisterClassEx'
////    function that was added to Windows 95. It is important to call this function
////    so that the application will get 'well formed' small icons associated
////    with it.
////
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
// WNDCLASSEX wcex;
//
// wcex.cbSize = sizeof(WNDCLASSEX);
//
// wcex.style   = CS_HREDRAW | CS_VREDRAW;
// wcex.lpfnWndProc = WndProc;
// wcex.cbClsExtra  = 0;
// wcex.cbWndExtra  = 0;
// wcex.hInstance  = hInstance;
// wcex.hIcon   = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IEMINI));
// wcex.hCursor  = LoadCursor(NULL, IDC_ARROW);
// wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
// wcex.lpszMenuName = MAKEINTRESOURCE(IDC_IEMINI);
// wcex.lpszClassName = szWindowClass;
// wcex.hIconSm  = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
//
// return RegisterClassEx(&wcex);
//}
//
////
////   FUNCTION: InitInstance(HINSTANCE, int)
////
////   PURPOSE: Saves instance handle and creates main window
////
////   COMMENTS:
////
////        In this function, we save the instance handle in a global variable and
////        create and display the main program window.
////
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   HWND hWnd;
//
//   hInst = hInstance; // Store instance handle in our global variable
//
//   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
//
//   if (!hWnd)
//   {
//      return FALSE;
//   }
//
//   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
//
//   return TRUE;
//}
//
////
////  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
////
////  PURPOSE:  Processes messages for the main window.
////
////  WM_COMMAND - process the application menu
////  WM_PAINT - Paint the main window
////  WM_DESTROY - post a quit message and return
////
////
//
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
// int wmId, wmEvent;
// PAINTSTRUCT ps;
// HDC hdc;
//
// switch (message)
// {
// case WM_COMMAND:
//  wmId    = LOWORD(wParam);
//  wmEvent = HIWORD(wParam);
//  // Parse the menu selections:
//  switch (wmId)
//  {
//  case IDM_ABOUT:
//   //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//   if (g_pWebBrowser)
//   {
//    g_pWebBrowser->DisplayHTMLPage(hWnd, L"http://www.microsoft.com");
//   }
//   break;
//  case IDM_EXIT:
//   DestroyWindow(hWnd);
//   break;
//  default:
//   return DefWindowProc(hWnd, message, wParam, lParam);
//  }
//  break;
// case WM_PAINT:
//  hdc = BeginPaint(hWnd, &ps);
//  // TODO: Add any drawing code here...
//  EndPaint(hWnd, &ps);
//  break;
// case WM_DESTROY:
//  PostQuitMessage(0);
//  if (g_pWebBrowser)
//  {
//   g_pWebBrowser->UnEmbedBrowserObject(hWnd);
//   delete g_pWebBrowser;
//  }
//  break;
//
// case WM_CREATE:
//  g_pWebBrowser = new CWebBrowser;
//  if (g_pWebBrowser)
//   g_pWebBrowser->EmbedBrowserObject(hWnd);
//  break;
//
// default:
//  return DefWindowProc(hWnd, message, wParam, lParam);
// }
// return 0;
//}
//
//// Message handler for about box.
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
// UNREFERENCED_PARAMETER(lParam);
// switch (message)
// {
// case WM_INITDIALOG:
//  return (INT_PTR)TRUE;
//
// case WM_COMMAND:
//  if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//  {
//   EndDialog(hDlg, LOWORD(wParam));
//   return (INT_PTR)TRUE;
//  }
//  break;
// }
// return (INT_PTR)FALSE;
//}
