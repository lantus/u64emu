// KIDlg.h : header file
//

#if !defined(AFX_KIDLG_H__D5E734D0_1A92_4959_AEC2_366712D3DE7B__INCLUDED_)
#define AFX_KIDLG_H__D5E734D0_1A92_4959_AEC2_366712D3DE7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EmuObject1.h"
#include "Debugger.h"
#include "TComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CKIDlg dialog

class CKIDlg : public CDialog
{
// Construction
public:
	CKIDlg(CWnd* pParent = NULL);	// standard constructor

	CFile *m_File;
	DWORD *m_Image;
	CEmuObject *m_EmuObj;
	bool m_Debug;
	CDebugger *m_Debugger;
	bool m_NeedInit;
	int m_DIP1,m_DIP2;
	int m_ScreenRes;
	CString m_LastPath;
	bool m_FullScreen;
	bool m_LockVSYNC;
	bool m_ScanLines;
	bool m_NoHLE;
	DWORD m_FrameRate;
	CBitmap *m_Background;
	CDC *m_BackgroundCDC;
	CString	m_HDImage;
	CString	m_Rom1;
	CString	m_Rom2;
	CString	m_Rom3;
	CString	m_Rom4;
	CString	m_Rom5;
	CString	m_Rom6;
	CString	m_Rom7;
	CString	m_Rom8;
	CString	m_Rom9;
	

	bool DoesExists(char *filename);
	void InitSettings();
	bool VerifySettings();
	void WriteSettings();
	void ReadSettings();
	void DIPSWDesc();

// Dialog Data
	//{{AFX_DATA(CKIDlg)
	enum { IDD = IDD_KI_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKIDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CKIDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDoIt();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnInput();
	afx_msg void OnDIPSwitches();
	afx_msg void OnSettings();
	afx_msg void OnAbout();
	afx_msg void OnRomSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KIDLG_H__D5E734D0_1A92_4959_AEC2_366712D3DE7B__INCLUDED_)
