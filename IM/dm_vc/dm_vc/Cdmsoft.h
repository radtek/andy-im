// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

//#import "E:\\zfgj\\zfgj\\dm.dll" no_namespace
#import "dm.dll" no_namespace
// Cdmsoft wrapper class

class Cdmsoft : public COleDispatchDriver
{
public:
	Cdmsoft(){} // Calls COleDispatchDriver default constructor
	Cdmsoft(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Cdmsoft(const Cdmsoft& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	// Attributes
public:

	// Operations
public:


	// Idmsoft methods
public:
	CString Ver()
	{
		CString result;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	CString GetBasePath()
	{
		CString result;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long SetPath(LPCTSTR path)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, parms, path);
		return result;
	}
	long UnBindWindow()
	{
		long result;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SetDictPwd(LPCTSTR pwd)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, parms, pwd);
		return result;
	}
	long UseDict(long index)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, parms, index);
		return result;
	}
	long SetExportDict(LPCTSTR dict_name)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_I4, (void*)&result, parms, dict_name);
		return result;
	}
	long SetExportDict1(LPCTSTR dict_name)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, dict_name);
		return result;
	}
	long SetExportDict2(LPCTSTR dict_name)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_I4, (void*)&result, parms, dict_name);
		return result;
	}
	CString Ocr(long x1, long y1, long x2, long y2, LPCTSTR color, double sim)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_R8 ;
		InvokeHelper(0xa, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, color, sim);
		return result;
	}
	long FindStr(long x1, long y1, long x2, long y2, LPCTSTR str, LPCTSTR color, double sim, VARIANT * x, VARIANT * y)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_R8 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0xb, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x1, y1, x2, y2, str, color, sim, x, y);
		return result;
	}
	CString FindStrEx(long x1, long y1, long x2, long y2, LPCTSTR str, LPCTSTR color, double sim)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_R8 ;
		InvokeHelper(0xc, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, str, color, sim);
		return result;
	}
	CString OcrInFile(long x1, long y1, long x2, long y2, LPCTSTR pic_name, LPCTSTR color, double sim)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_R8 ;
		InvokeHelper(0xd, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, pic_name, color, sim);
		return result;
	}
	long GetResultCount(LPCTSTR str)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xe, DISPATCH_METHOD, VT_I4, (void*)&result, parms, str);
		return result;
	}
	long GetResultPos(LPCTSTR str, long index, VARIANT * x, VARIANT * y)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0xf, DISPATCH_METHOD, VT_I4, (void*)&result, parms, str, index, x, y);
		return result;
	}
	long SetMinRowGap(long row_gap)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x10, DISPATCH_METHOD, VT_I4, (void*)&result, parms, row_gap);
		return result;
	}
	long SetMinColGap(long col_gap)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x11, DISPATCH_METHOD, VT_I4, (void*)&result, parms, col_gap);
		return result;
	}
	long SetWordLineHeight(long line_height)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x12, DISPATCH_METHOD, VT_I4, (void*)&result, parms, line_height);
		return result;
	}
	long SetWordGap(long word_gap)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x13, DISPATCH_METHOD, VT_I4, (void*)&result, parms, word_gap);
		return result;
	}
	long SetRowGapNoDict(long row_gap)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x14, DISPATCH_METHOD, VT_I4, (void*)&result, parms, row_gap);
		return result;
	}
	long SetColGapNoDict(long col_gap)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x15, DISPATCH_METHOD, VT_I4, (void*)&result, parms, col_gap);
		return result;
	}
	long SetWordLineHeightNoDict(long line_height)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x16, DISPATCH_METHOD, VT_I4, (void*)&result, parms, line_height);
		return result;
	}
	long SetWordGapNoDict(long word_gap)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x17, DISPATCH_METHOD, VT_I4, (void*)&result, parms, word_gap);
		return result;
	}
	long GetWordResultCount(LPCTSTR str)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x18, DISPATCH_METHOD, VT_I4, (void*)&result, parms, str);
		return result;
	}
	long GetWordResultPos(LPCTSTR str, long index, VARIANT * x, VARIANT * y)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x19, DISPATCH_METHOD, VT_I4, (void*)&result, parms, str, index, x, y);
		return result;
	}
	CString GetWordResultStr(LPCTSTR str, long index)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0x1a, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, str, index);
		return result;
	}
	CString GetWords(long x1, long y1, long x2, long y2, LPCTSTR color, double sim)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_R8 ;
		InvokeHelper(0x1b, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, color, sim);
		return result;
	}
	CString GetWordsNoDict(long x1, long y1, long x2, long y2, LPCTSTR color)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR ;
		InvokeHelper(0x1c, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, color);
		return result;
	}
	long StrStr(LPCTSTR s, LPCTSTR str)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x1d, DISPATCH_METHOD, VT_I4, (void*)&result, parms, s, str);
		return result;
	}
	long KeyPress(long vk)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1e, DISPATCH_METHOD, VT_I4, (void*)&result, parms, vk);
		return result;
	}
	long KeyDown(long vk)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1f, DISPATCH_METHOD, VT_I4, (void*)&result, parms, vk);
		return result;
	}
	long KeyUp(long vk)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x20, DISPATCH_METHOD, VT_I4, (void*)&result, parms, vk);
		return result;
	}
	long LeftClick()
	{
		long result;
		InvokeHelper(0x21, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long RightClick()
	{
		long result;
		InvokeHelper(0x22, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long MiddleClick()
	{
		long result;
		InvokeHelper(0x23, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long LeftDoubleClick()
	{
		long result;
		InvokeHelper(0x24, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long LeftDown()
	{
		long result;
		InvokeHelper(0x25, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long LeftUp()
	{
		long result;
		InvokeHelper(0x26, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long RightDown()
	{
		long result;
		InvokeHelper(0x27, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long RightUp()
	{
		long result;
		InvokeHelper(0x28, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long MoveTo(long x, long y)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x29, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x, y);
		return result;
	}
	long MoveR(long rx, long ry)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x2a, DISPATCH_METHOD, VT_I4, (void*)&result, parms, rx, ry);
		return result;
	}
	long Capture(long x1, long y1, long x2, long y2, LPCTSTR file)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR ;
		InvokeHelper(0x2b, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x1, y1, x2, y2, file);
		return result;
	}
	CString GetColor(long x, long y)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x2c, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x, y);
		return result;
	}
	CString GetColorBGR(long x, long y)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x2d, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x, y);
		return result;
	}
	CString RGB2BGR(LPCTSTR rgb_color)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x2e, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, rgb_color);
		return result;
	}
	CString BGR2RGB(LPCTSTR bgr_color)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x2f, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, bgr_color);
		return result;
	}
	long CmpColor(long x, long y, LPCTSTR color, double sim)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_BSTR VTS_R8 ;
		InvokeHelper(0x30, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x, y, color, sim);
		return result;
	}
	long FindColor(long x1, long y1, long x2, long y2, LPCTSTR color, double sim, long dir, VARIANT * x, VARIANT * y)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_R8 VTS_I4 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x31, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x1, y1, x2, y2, color, sim, dir, x, y);
		return result;
	}
	CString FindColorEx(long x1, long y1, long x2, long y2, LPCTSTR color, double sim, long dir)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_R8 VTS_I4 ;
		InvokeHelper(0x32, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, color, sim, dir);
		return result;
	}
	CString GetAveRGB(long x1, long y1, long x2, long y2)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x33, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2);
		return result;
	}
	CString GetAveHSV(long x1, long y1, long x2, long y2)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x34, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2);
		return result;
	}
	CString GetColorHSV(long x, long y)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x35, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x, y);
		return result;
	}
	long ClientToScreen(long hwnd, VARIANT * x, VARIANT * y)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x36, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x, y);
		return result;
	}
	long ScreenToClient(long hwnd, VARIANT * x, VARIANT * y)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x37, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x, y);
		return result;
	}
	long ShowScrMsg(long x1, long y1, long x2, long y2, LPCTSTR msg, LPCTSTR color)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x38, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x1, y1, x2, y2, msg, color);
		return result;
	}
	long GetForegroundWindow()
	{
		long result;
		InvokeHelper(0x39, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetForegroundFocus()
	{
		long result;
		InvokeHelper(0x3a, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetMousePointWindow()
	{
		long result;
		InvokeHelper(0x3b, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetPointWindow(long x, long y)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x3c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x, y);
		return result;
	}
	CString EnumWindow(long parent, LPCTSTR title, LPCTSTR class_name, long filter)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 ;
		InvokeHelper(0x3d, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, parent, title, class_name, filter);
		return result;
	}
	long MoveWindow(long hwnd, long x, long y)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x3e, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x, y);
		return result;
	}
	long GetWindowState(long hwnd, long flag)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x3f, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, flag);
		return result;
	}
	long GetWindow(long hwnd, long flag)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x40, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, flag);
		return result;
	}
	long GetSpecialWindow(long flag)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x41, DISPATCH_METHOD, VT_I4, (void*)&result, parms, flag);
		return result;
	}
	long SetWindowText(long hwnd, LPCTSTR text)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x42, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, text);
		return result;
	}
	long SetWindowSize(long hwnd, long width, long height)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x43, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, width, height);
		return result;
	}
	long GetClientSize(long hwnd, VARIANT * width, VARIANT * height)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x44, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, width, height);
		return result;
	}
	long GetWindowRect(long hwnd, VARIANT * x1, VARIANT * y1, VARIANT * x2, VARIANT * y2)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x45, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x1, y1, x2, y2);
		return result;
	}
	CString GetWindowTitle(long hwnd)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x46, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, hwnd);
		return result;
	}
	CString GetWindowClass(long hwnd)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x47, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, hwnd);
		return result;
	}
	long SetWindowState(long hwnd, long flag)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x48, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, flag);
		return result;
	}
	long SendCommand(LPCTSTR cmd)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x49, DISPATCH_METHOD, VT_I4, (void*)&result, parms, cmd);
		return result;
	}
	long SetShowErrorMsg(long show)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x4a, DISPATCH_METHOD, VT_I4, (void*)&result, parms, show);
		return result;
	}
	long CreateFoobarRect(long hwnd, long x, long y, long w, long h)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x4b, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x, y, w, h);
		return result;
	}
	long CreateFoobarRoundRect(long hwnd, long x, long y, long w, long h, long rw, long rh)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x4c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x, y, w, h, rw, rh);
		return result;
	}
	long CreateFoobarEllipse(long hwnd, long x, long y, long w, long h)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x4d, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x, y, w, h);
		return result;
	}
	long CreateFoobarCustom(long hwnd, long x, long y, LPCTSTR pic, LPCTSTR trans_color, double sim)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_R8 ;
		InvokeHelper(0x4e, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x, y, pic, trans_color, sim);
		return result;
	}
	long FoobarFillRect(long hwnd, long x1, long y1, long x2, long y2, LPCTSTR color)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR ;
		InvokeHelper(0x4f, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x1, y1, x2, y2, color);
		return result;
	}
	long FoobarDrawText(long hwnd, long x, long y, long w, long h, LPCTSTR text, LPCTSTR color, long align)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 ;
		InvokeHelper(0x50, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x, y, w, h, text, color, align);
		return result;
	}
	long FoobarDrawPic(long hwnd, long x, long y, LPCTSTR pic, LPCTSTR trans_color)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x51, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x, y, pic, trans_color);
		return result;
	}
	long FoobarUpdate(long hwnd)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x52, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd);
		return result;
	}
	long FoobarLock(long hwnd)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x53, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd);
		return result;
	}
	long FoobarUnlock(long hwnd)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x54, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd);
		return result;
	}
	long FoobarSetFont(long hwnd, LPCTSTR font_name, long size, long flag)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_I4 VTS_I4 ;
		InvokeHelper(0x55, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, font_name, size, flag);
		return result;
	}
	long FoobarTextRect(long hwnd, long x, long y, long w, long h)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x56, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, x, y, w, h);
		return result;
	}
	long FoobarPrintText(long hwnd, LPCTSTR text, LPCTSTR color)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x57, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, text, color);
		return result;
	}
	long FoobarClearText(long hwnd)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x58, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd);
		return result;
	}
	long FoobarTextLineGap(long hwnd, long gap)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x59, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, gap);
		return result;
	}
	long Play(LPCTSTR file)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x5a, DISPATCH_METHOD, VT_I4, (void*)&result, parms, file);
		return result;
	}
	long FaqCapture(long x1, long y1, long x2, long y2, long quality, long delay, long time)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x5b, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x1, y1, x2, y2, quality, delay, time);
		return result;
	}
	long FaqRelease(long handle)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x5c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, handle);
		return result;
	}
	CString FaqSend(LPCTSTR server, long handle, long request_type, long time_out)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x5d, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, server, handle, request_type, time_out);
		return result;
	}
	long Beep(long fre, long delay)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x5e, DISPATCH_METHOD, VT_I4, (void*)&result, parms, fre, delay);
		return result;
	}
	long FoobarClose(long hwnd)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x5f, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd);
		return result;
	}
	long MoveDD(long dx, long dy)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x60, DISPATCH_METHOD, VT_I4, (void*)&result, parms, dx, dy);
		return result;
	}
	long FaqGetSize(long handle)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x61, DISPATCH_METHOD, VT_I4, (void*)&result, parms, handle);
		return result;
	}
	long LoadPic(LPCTSTR pic_name)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x62, DISPATCH_METHOD, VT_I4, (void*)&result, parms, pic_name);
		return result;
	}
	long FreePic(LPCTSTR pic_name)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x63, DISPATCH_METHOD, VT_I4, (void*)&result, parms, pic_name);
		return result;
	}
	long GetScreenData(long x1, long y1, long x2, long y2)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x64, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x1, y1, x2, y2);
		return result;
	}
	long FreeScreenData(long handle)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x65, DISPATCH_METHOD, VT_I4, (void*)&result, parms, handle);
		return result;
	}
	long WheelUp()
	{
		long result;
		InvokeHelper(0x66, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long WheelDown()
	{
		long result;
		InvokeHelper(0x67, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SetMouseDelay(LPCTSTR type, long delay)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0x68, DISPATCH_METHOD, VT_I4, (void*)&result, parms, type, delay);
		return result;
	}
	long SetKeypadDelay(LPCTSTR type, long delay)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0x69, DISPATCH_METHOD, VT_I4, (void*)&result, parms, type, delay);
		return result;
	}
	CString GetEnv(long index, LPCTSTR name)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x6a, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, index, name);
		return result;
	}
	long SetEnv(long index, LPCTSTR name, LPCTSTR value)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x6b, DISPATCH_METHOD, VT_I4, (void*)&result, parms, index, name, value);
		return result;
	}
	long SendString(long hwnd, LPCTSTR str)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x6c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, str);
		return result;
	}
	long PostString(long hwnd, LPCTSTR str)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x6d, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, str);
		return result;
	}
	long DelEnv(long index, LPCTSTR name)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x6e, DISPATCH_METHOD, VT_I4, (void*)&result, parms, index, name);
		return result;
	}
	CString GetPath()
	{
		CString result;
		InvokeHelper(0x6f, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long SetDict(long index, LPCTSTR dict_name)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x70, DISPATCH_METHOD, VT_I4, (void*)&result, parms, index, dict_name);
		return result;
	}
	long FindPic(long x1, long y1, long x2, long y2, LPCTSTR pic_name, LPCTSTR delta_color, double sim, long dir, VARIANT * x, VARIANT * y)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_R8 VTS_I4 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x71, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x1, y1, x2, y2, pic_name, delta_color, sim, dir, x, y);
		return result;
	}
	CString FindPicEx(long x1, long y1, long x2, long y2, LPCTSTR pic_name, LPCTSTR delta_color, double sim, long dir)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_R8 VTS_I4 ;
		InvokeHelper(0x72, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, pic_name, delta_color, sim, dir);
		return result;
	}
	long SetClientSize(long hwnd, long width, long height)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x73, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, width, height);
		return result;
	}
	long ReadInt(long hwnd, LPCTSTR addr, long type)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_I4 ;
		InvokeHelper(0x74, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, addr, type);
		return result;
	}
	float ReadFloat(long hwnd, LPCTSTR addr)
	{
		float result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x75, DISPATCH_METHOD, VT_R4, (void*)&result, parms, hwnd, addr);
		return result;
	}
	double ReadDouble(long hwnd, LPCTSTR addr)
	{
		double result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x76, DISPATCH_METHOD, VT_R8, (void*)&result, parms, hwnd, addr);
		return result;
	}
	CString FindInt(long hwnd, LPCTSTR addr_range, long int_value_min, long int_value_max, long type)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x77, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, hwnd, addr_range, int_value_min, int_value_max, type);
		return result;
	}
	CString FindFloat(long hwnd, LPCTSTR addr_range, float float_value_min, float float_value_max)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_R4 VTS_R4 ;
		InvokeHelper(0x78, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, hwnd, addr_range, float_value_min, float_value_max);
		return result;
	}
	CString FindDouble(long hwnd, LPCTSTR addr_range, double double_value_min, double double_value_max)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_R8 VTS_R8 ;
		InvokeHelper(0x79, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, hwnd, addr_range, double_value_min, double_value_max);
		return result;
	}
	CString FindString(long hwnd, LPCTSTR addr_range, LPCTSTR string_value, long type)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 ;
		InvokeHelper(0x7a, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, hwnd, addr_range, string_value, type);
		return result;
	}
	long GetModuleBaseAddr(long hwnd, LPCTSTR module_name)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x7b, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, module_name);
		return result;
	}
	long MoveToEx(long x, long y, long w, long h)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x7c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x, y, w, h);
		return result;
	}
	CString MatchPicName(LPCTSTR pic_name)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x7d, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, pic_name);
		return result;
	}
	long AddDict(long index, LPCTSTR dict_info)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x7e, DISPATCH_METHOD, VT_I4, (void*)&result, parms, index, dict_info);
		return result;
	}
	long EnterCri()
	{
		long result;
		InvokeHelper(0x7f, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long LeaveCri()
	{
		long result;
		InvokeHelper(0x80, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long WriteInt(long hwnd, LPCTSTR addr, long type, long v)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_I4 VTS_I4 ;
		InvokeHelper(0x81, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, addr, type, v);
		return result;
	}
	long WriteFloat(long hwnd, LPCTSTR addr, float v)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_R4 ;
		InvokeHelper(0x82, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, addr, v);
		return result;
	}
	long WriteDouble(long hwnd, LPCTSTR addr, double v)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_R8 ;
		InvokeHelper(0x83, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, addr, v);
		return result;
	}
	long WriteString(long hwnd, LPCTSTR addr, long type, LPCTSTR v)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x84, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, addr, type, v);
		return result;
	}
	long AsmAdd(LPCTSTR asm_ins)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x85, DISPATCH_METHOD, VT_I4, (void*)&result, parms, asm_ins);
		return result;
	}
	long AsmClear()
	{
		long result;
		InvokeHelper(0x86, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long AsmCall(long hwnd, long mode)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x87, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, mode);
		return result;
	}
	long FindMultiColor(long x1, long y1, long x2, long y2, LPCTSTR first_color, LPCTSTR offset_color, double sim, long dir, VARIANT * x, VARIANT * y)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_R8 VTS_I4 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x88, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x1, y1, x2, y2, first_color, offset_color, sim, dir, x, y);
		return result;
	}
	CString FindMultiColorEx(long x1, long y1, long x2, long y2, LPCTSTR first_color, LPCTSTR offset_color, double sim, long dir)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_R8 VTS_I4 ;
		InvokeHelper(0x89, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, first_color, offset_color, sim, dir);
		return result;
	}
	CString AsmCode(long base_addr)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x8a, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, base_addr);
		return result;
	}
	CString Assemble(LPCTSTR asm_ins, long base_addr, long is_upper)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 ;
		InvokeHelper(0x8b, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, asm_ins, base_addr, is_upper);
		return result;
	}
	long SetWindowTransparent(long hwnd, long v)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x8c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, v);
		return result;
	}
	CString ReadData(long hwnd, LPCTSTR addr, long len)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_I4 ;
		InvokeHelper(0x8d, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, hwnd, addr, len);
		return result;
	}
	long WriteData(long hwnd, LPCTSTR addr, LPCTSTR data)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x8e, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, addr, data);
		return result;
	}
	CString FindData(long hwnd, LPCTSTR addr_range, LPCTSTR data)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x8f, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, hwnd, addr_range, data);
		return result;
	}
	long SetPicPwd(LPCTSTR pwd)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x90, DISPATCH_METHOD, VT_I4, (void*)&result, parms, pwd);
		return result;
	}
	long Log(LPCTSTR info)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x91, DISPATCH_METHOD, VT_I4, (void*)&result, parms, info);
		return result;
	}
	CString FindStrE(long x1, long y1, long x2, long y2, LPCTSTR str, LPCTSTR color, double sim)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_R8 ;
		InvokeHelper(0x92, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, str, color, sim);
		return result;
	}
	CString FindColorE(long x1, long y1, long x2, long y2, LPCTSTR color, double sim, long dir)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_R8 VTS_I4 ;
		InvokeHelper(0x93, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, color, sim, dir);
		return result;
	}
	CString FindPicE(long x1, long y1, long x2, long y2, LPCTSTR pic_name, LPCTSTR delta_color, double sim, long dir)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_R8 VTS_I4 ;
		InvokeHelper(0x94, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, pic_name, delta_color, sim, dir);
		return result;
	}
	CString FindMultiColorE(long x1, long y1, long x2, long y2, LPCTSTR first_color, LPCTSTR offset_color, double sim, long dir)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_R8 VTS_I4 ;
		InvokeHelper(0x95, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, first_color, offset_color, sim, dir);
		return result;
	}
	long SetExactOcr(long exact_ocr)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x96, DISPATCH_METHOD, VT_I4, (void*)&result, parms, exact_ocr);
		return result;
	}
	CString ReadString(long hwnd, LPCTSTR addr, long type, long len)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_I4 VTS_I4 ;
		InvokeHelper(0x97, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, hwnd, addr, type, len);
		return result;
	}
	long FoobarTextPrintDir(long hwnd, long dir)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x98, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, dir);
		return result;
	}
	CString OcrEx(long x1, long y1, long x2, long y2, LPCTSTR color, double sim)
	{
		CString result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_R8 ;
		InvokeHelper(0x99, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, x1, y1, x2, y2, color, sim);
		return result;
	}
	long SetDisplayInput(LPCTSTR mode)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x9a, DISPATCH_METHOD, VT_I4, (void*)&result, parms, mode);
		return result;
	}
	long GetTime()
	{
		long result;
		InvokeHelper(0x9b, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetScreenWidth()
	{
		long result;
		InvokeHelper(0x9c, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetScreenHeight()
	{
		long result;
		InvokeHelper(0x9d, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long BindWindowEx(long hwnd, LPCTSTR display, LPCTSTR mouse, LPCTSTR keypad, LPCTSTR public_desc, long mode)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 ;
		InvokeHelper(0x9e, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, display, mouse, keypad, public_desc, mode);
		return result;
	}
	CString GetDiskSerial()
	{
		CString result;
		InvokeHelper(0x9f, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	CString Md5(LPCTSTR str)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xa0, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, str);
		return result;
	}
	CString GetMac()
	{
		CString result;
		InvokeHelper(0xa1, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long ActiveInputMethod(long hwnd, LPCTSTR id)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0xa2, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, id);
		return result;
	}
	long CheckInputMethod(long hwnd, LPCTSTR id)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0xa3, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, id);
		return result;
	}
	long FindInputMethod(LPCTSTR id)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xa4, DISPATCH_METHOD, VT_I4, (void*)&result, parms, id);
		return result;
	}
	long GetCursorPos(VARIANT * x, VARIANT * y)
	{
		long result;
		static BYTE parms[] = VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0xa5, DISPATCH_METHOD, VT_I4, (void*)&result, parms, x, y);
		return result;
	}
	long BindWindow(long hwnd, LPCTSTR display, LPCTSTR mouse, LPCTSTR keypad, long mode)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 ;
		InvokeHelper(0xa6, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hwnd, display, mouse, keypad, mode);
		return result;
	}
	long FindWindow(LPCTSTR class_name, LPCTSTR title_name)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR ;
		InvokeHelper(0xa7, DISPATCH_METHOD, VT_I4, (void*)&result, parms, class_name, title_name);
		return result;
	}
	long GetScreenDepth()
	{
		long result;
		InvokeHelper(0xa8, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long SetScreen(long width, long height, long depth)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0xa9, DISPATCH_METHOD, VT_I4, (void*)&result, parms, width, height, depth);
		return result;
	}
	long ExitOs(long type)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xaa, DISPATCH_METHOD, VT_I4, (void*)&result, parms, type);
		return result;
	}
	CString GetDir(long type)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xab, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, type);
		return result;
	}
	long GetOsType()
	{
		long result;
		InvokeHelper(0xac, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long FindWindowEx(long parent, LPCTSTR class_name, LPCTSTR title_name)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR ;
		InvokeHelper(0xad, DISPATCH_METHOD, VT_I4, (void*)&result, parms, parent, class_name, title_name);
		return result;
	}

	// Idmsoft properties
public:

};
