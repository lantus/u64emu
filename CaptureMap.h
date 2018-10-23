#if !defined(AFX_CAPTUREMAP_H__B1D37F0B_40A8_42C2_96BD_BD323893BEBB__INCLUDED_)
#define AFX_CAPTUREMAP_H__B1D37F0B_40A8_42C2_96BD_BD323893BEBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaptureMap.h : header file
//
#include "mmDirectInputDevice.h"

/////////////////////////////////////////////////////////////////////////////
// CCaptureMap dialog

class CCaptureMap : public CDialog
{
// Construction
public:
	CCaptureMap(CWnd* pParent = NULL);   // standard constructor
	mmDirectInputDevice *m_InputDevice;
	DWORD m_LastMap;
	char m_KeyText[256];

// Dialog Data
	//{{AFX_DATA(CCaptureMap)
	enum { IDD = IDD_Remap };
	CString	m_MapText;
	CString	m_ModifyText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaptureMap)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCaptureMap)
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPTUREMAP_H__B1D37F0B_40A8_42C2_96BD_BD323893BEBB__INCLUDED_)
