#if !defined(AFX_CONTROLCONFIG_H__3098CE1B_55ED_4123_8BFE_312929B88AF4__INCLUDED_)
#define AFX_CONTROLCONFIG_H__3098CE1B_55ED_4123_8BFE_312929B88AF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlConfig.h : header file
//
#include "mmDirectInputDevice.h"

/////////////////////////////////////////////////////////////////////////////
// CControlConfig dialog

class CControlConfig : public CDialog
{
// Construction
public:
	CControlConfig(CWnd* pParent = NULL);   // standard constructor
	mmDirectInputDevice *m_InputDevice;
	DWORD m_Map[22];
	int m_LastSelect;
	void UpdateMap(int index);

// Dialog Data
	//{{AFX_DATA(CControlConfig)
	enum { IDD = IDD_ControllerConfig };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CControlConfig)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeControls();
	virtual BOOL OnInitDialog();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDblclkControls();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLCONFIG_H__3098CE1B_55ED_4123_8BFE_312929B88AF4__INCLUDED_)
