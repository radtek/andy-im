
// dm_vcDlg.h : header file
//

#pragma once
#include "AndyLib/Andylib.h"
#//include "afxwin.h"
#include "Cdmsoft.h"
#include "afxwin.h"

// Cdm_vcDlg dialog
class Cdm_vcDlg : public CDialog
{
// Construction
public:
	Cdm_vcDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DM_VC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);

public:
	HANDLE m_threadHandle;
	static DWORD WINAPI thrad_main(LPVOID lpParameter);	


private:

	BOOL m_bRun;
	Cdmsoft m_dm;


	void MianProcess();

	void Procedure01();
public:
	afx_msg void OnClose();
	CButton m_btnstop;
	CButton m_btnstart;
};
