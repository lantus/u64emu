// KIDlg.h : header file
//

#if !defined(AFX_KIDLG_H__D5E734D0_1A92_4959_AEC2_366712D3DE7B__INCLUDED_)
#define AFX_KIDLG_H__D5E734D0_1A92_4959_AEC2_366712D3DE7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EmuObject1.h"
#include "Debugger.h"

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
	

	bool DoesExists(char *filename);
	bool BrowseFile(CString *filename,char *ext);
	void InitSettings();
	void PackSettings();
	void UnPackSettings();
	bool VerifySettings();
	void WriteSettings();
	void ReadSettings();
	void DIPSWDesc();

// Dialog Data
	//{{AFX_DATA(CKIDlg)
	enum { IDD = IDD_KI_DIALOG };
	CComboBox	m_SetName;
	CString	m_DIP1SW1Text;
	CString	m_DIP1SW2Text;
	CString	m_DIP1SW3Text;
	CString	m_DIP1SW4Text;
	CString	m_DIP1SW5Text;
	CString	m_DIP1SW6Text;
	CString	m_DIP1SW7Text;
	CString	m_DIP1SW8Text;
	CString	m_DIP2SW1Text;
	CString	m_DIP2SW2Text;
	CString	m_DIP2SW3Text;
	CString	m_DIP2SW4Text;
	CString	m_DIP2SW5Text;
	CString	m_DIP2SW6Text;
	CString	m_DIP2SW7Text;
	CString	m_DIP2SW8Text;
	BOOL	m_FullScreen;
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
	BOOL	m_LockVSYNC;
	BOOL	m_ScanLines;
	BOOL	m_Skip;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKIDlg)
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
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnBrowse5();
	afx_msg void OnBrowse6();
	afx_msg void OnBrowse7();
	afx_msg void OnBrowse8();
	afx_msg void OnBrowse9();
	afx_msg void OnDip1Sw1();
	afx_msg void OnDip1Sw2();
	afx_msg void OnDip1Sw3();
	afx_msg void OnDip1Sw4();
	afx_msg void OnDip1Sw5();
	afx_msg void OnDip1Sw6();
	afx_msg void OnDip1Sw7();
	afx_msg void OnDip1Sw8();
	afx_msg void OnDip2Sw1();
	afx_msg void OnDip2Sw2();
	afx_msg void OnDip2Sw3();
	afx_msg void OnDip2Sw4();
	afx_msg void OnDip2Sw5();
	afx_msg void OnDip2Sw6();
	afx_msg void OnDip2Sw7();
	afx_msg void OnDip2Sw8();
	afx_msg void OnFullScreen();
	afx_msg void OnHDBrowse();
	afx_msg void OnRes1();
	afx_msg void OnRes2();
	afx_msg void OnRes3();
	afx_msg void OnRes4();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnDefault();
	afx_msg void OnInput();
	afx_msg void OnScanLines();
	afx_msg void OnSelendokSetName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KIDLG_H__D5E734D0_1A92_4959_AEC2_366712D3DE7B__INCLUDED_)
