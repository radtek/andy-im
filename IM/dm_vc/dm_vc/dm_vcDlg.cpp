
// dm_vcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dm_vc.h"
#include "dm_vcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Cdm_vcDlg dialog

#define  X_MON_OFFSET 30

#define Y_MON_OFFSET   30




Cdm_vcDlg::Cdm_vcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cdm_vcDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cdm_vcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnstop);
	DDX_Control(pDX, IDC_BUTTON_START, m_btnstart);
}

BEGIN_MESSAGE_MAP(Cdm_vcDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &Cdm_vcDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &Cdm_vcDlg::OnBnClickedButtonStop)
	ON_MESSAGE(WM_HOTKEY,&Cdm_vcDlg::OnHotKey)//快捷键消息映射手动加入
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// Cdm_vcDlg message handlers

BOOL Cdm_vcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_bRun=FALSE;

	RegisterHotKey(GetSafeHwnd(),1001,MOD_ALT,'1');//ALT+1键
	RegisterHotKey(GetSafeHwnd(),1002,MOD_ALT,'2');//ALT+2键

	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(OLESTR("dm.dmsoft"),&clsid);
	m_dm.CreateDispatch(clsid);
	CString ver = m_dm.Ver();  
	if(ver.IsEmpty())  
	{  
		AfxMessageBox(_T("创建对象失败,检查系统是否注册了dm。")); 

		return TRUE;
	}  


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cdm_vcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cdm_vcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cdm_vcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cdm_vcDlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here

	int dm_ret;
	long dm_hwn = m_dm.GetMousePointWindow();
	dm_ret = m_dm.BindWindow(dm_hwn,"gdi","normal","normal",0);
	// dm_ret = m_dm.BindWindow(hwnd,"gdi","windows2","normal",0);
	Sleep(1500);
	if(dm_ret=0)
	{
		MessageBox("绑定失败","TISHI",MB_OK);
		return;
	}
	// m_dm.MoveWindow(dm_hwn,0,0);
	// Sleep(500);
	int intX,intY;
	CString s;
	//dm_ret = m_dm.FindStr(114,227,225,279,"","400040-000000",1.0,intX,intY)

	//CString base_path;// = m_dm.GetBasePath();
	CString str;
	str.Format("%s",andyStr(RSTR_CURRPATH).string());
	dm_ret = m_dm.SetPath(str);

	dm_ret = m_dm.SetDict(0,"st9.txt");
	dm_ret = m_dm.SetDict(1,"dm_soft.txt");
	//	s = m_dm.Ocr(0,0,254,199,"400040-000000",1.0);
	//TextOUt(s);
	m_btnstart.EnableWindow(FALSE);
	m_btnstop.EnableWindow(TRUE);
	//m_strList.EnableWindow(FALSE);
	//登录线程
	//TextOUt("Begin Thread");
	m_bRun =TRUE;
	DWORD dwThreadID;
	m_threadHandle= CreateThread(NULL,0,thrad_main, (LPVOID)this,  0,&dwThreadID);

}
void Cdm_vcDlg::MianProcess()
{
	while(m_bRun)
	{
		Procedure01();
		// 		FindItem();
		// 		if (m_iFindCount>4)
		// 		{
		// 			MoveTo();
		// 		}


	}
}

void Cdm_vcDlg::Procedure01()
{
	int distance =-1;
	VARIANT Findx;
	VARIANT Findy;
	int x_find,y_find;
	int dm_ret;
	m_dm.UseDict(0);
	//CString strRestlt;
	//strRestlt = m_dm.Ocr(0,0,1024,768,"000000",1);
	dm_ret= m_dm.FindStr(0,0,1024,768,"目录下","000000",1,&Findx,&Findy);
	int x= Findx.iVal;
	int y=Findy.iVal;
	if(x >0 && y>0)
	{
		x_find = Findx.intVal;
		y_find= Findy.intVal;
		m_dm.MoveTo(x_find+X_MON_OFFSET,y_find+Y_MON_OFFSET);
		Sleep(100);
        m_dm.LeftClick();
		CString str;
		str.Format("FIGHT IN (%d,%d)",x_find+X_MON_OFFSET,y_find+Y_MON_OFFSET);
		//TextOUt(str);
		for (int i=0;i <6;i++)
		{
			VARIANT FindX_new,FindY_new;
			m_dm.FindStr(x_find-10,y_find-10,x_find+100,y_find+50,"目录下","000000",1,&FindX_new,&FindY_new);
			int x_new,y_new;
			x_new=FindX_new.intVal;
			y_new=FindY_new.intVal;
			if (x_new >0 && y_new >0)
			{
				m_dm.MoveTo(x_new+X_MON_OFFSET,y_new+Y_MON_OFFSET);
				Sleep(100);
				m_dm.LeftClick();
				Sleep(1000);
			}
			else
			{
				//TextOUt("killed!");
				break;
			}
		}
	}
	else
	{
		Sleep(100);
	}

}
void Cdm_vcDlg::OnBnClickedButtonStop()
{
	// TODO: Add your control notification handler code here
	m_bRun =FALSE;
	Sleep(1000);
	m_btnstart.EnableWindow(TRUE);
	m_btnstop.EnableWindow(FALSE);
	//m_btn_getApp.EnableWindow(TRUE);
	//m_strList.EnableWindow(TRUE);
	m_dm.UnBindWindow();
	TerminateProcess(m_threadHandle,NULL);
	//TextOUt("Kill Thread");
}
LRESULT Cdm_vcDlg::OnHotKey(WPARAM wParam,LPARAM lParam)
{
	if(wParam ==1001)
	{ 
		//MessageBox("热键Ctrl+1已经按下！");
		OnBnClickedButtonStart();
		//后台测试
		//OnBnClickedButtonTest();
		//这里可以加上你按钮要执行的操作，或者直接调用按钮映射的消息函数
	}
	else if (wParam==1002)
	{
		//MessageBox("热键ctrl+2已经按下！");
		m_bRun = FALSE;
		OnBnClickedButtonStop();
	}
	return 0;
	//关闭对话框时使用
	//	UnregisterHotKey(GetSafeHwnd(),1001);//注销F2键
	//UnregisterHotKey(GetSafeHwnd(),1002);//注销Alt+1键
}
void Cdm_vcDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (m_btnstop.IsWindowEnabled())
	{
		OnBnClickedButtonStop();
	}
	UnregisterHotKey(GetSafeHwnd(),1001);
	UnregisterHotKey(GetSafeHwnd(),1002);//注销
	CDialog::OnClose();
}
DWORD WINAPI Cdm_vcDlg::thrad_main(LPVOID lpParameter)
{
	try
	{

		Cdm_vcDlg &pmain = *(Cdm_vcDlg*)lpParameter;
		if (pmain)
		{

			pmain.MianProcess();

		}
	}
	catch(...)
	{

	}
	return 0;
}