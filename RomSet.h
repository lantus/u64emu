#if !defined(AFX_ROMSET_H__959299CA_FA05_472E_8610_A9D5EA5CE2D5__INCLUDED_)
#define AFX_ROMSET_H__959299CA_FA05_472E_8610_A9D5EA5CE2D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RomSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRomSet dialog

class CRomSet : public CDialog
{
// Construction
public:
	CRomSet(CWnd* pParent = NULL);   // standard constructor
	WORD m_SetNum;
	bool BrowseFile(CString *filename,char *ext);

// Dialog Data
	//{{AFX_DATA(CRomSet)
	enum { IDD = IDD_RomSet };
	CComboBox	m_SetName;
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
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRomSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRomSet)
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnBrowse5();
	afx_msg void OnBrowse6();
	afx_msg void OnBrowse7();
	afx_msg void OnBrowse8();
	afx_msg void OnBrowse9();
	afx_msg void OnHDBrowse();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokSetName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROMSET_H__959299CA_FA05_472E_8610_A9D5EA5CE2D5__INCLUDED_)
