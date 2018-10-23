#if !defined(AFX_DIPSETTINGS_H__66C31E28_3DDE_4C13_B978_E7BACC81B0F3__INCLUDED_)
#define AFX_DIPSETTINGS_H__66C31E28_3DDE_4C13_B978_E7BACC81B0F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DIPSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDIPSettings dialog

class CDIPSettings : public CDialog
{
// Construction
public:
	CDIPSettings(CWnd* pParent = NULL);   // standard constructor

	void DIPSWDesc();
	void UnPackSettings();
	DWORD m_DIP1;
	DWORD m_DIP2;

// Dialog Data
	//{{AFX_DATA(CDIPSettings)
	enum { IDD = IDD_DIPSettings };
	CString	m_DIP1SW1;
	CString	m_DIP1SW2;
	CString	m_DIP1SW3;
	CString	m_DIP1SW4;
	CString	m_DIP1SW5;
	CString	m_DIP1SW6;
	CString	m_DIP1SW7;
	CString	m_DIP1SW8;
	CString	m_DIP2SW1;
	CString	m_DIP2SW2;
	CString	m_DIP2SW3;
	CString	m_DIP2SW4;
	CString	m_DIP2SW5;
	CString	m_DIP2SW6;
	CString	m_DIP2SW7;
	CString	m_DIP2SW8;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDIPSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDIPSettings)
	virtual void OnOK();
	virtual void OnCancel();
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
	afx_msg void OnDefault();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIPSETTINGS_H__66C31E28_3DDE_4C13_B978_E7BACC81B0F3__INCLUDED_)
