#if !defined(AFX_CONTROLLERCONFIG_H__3313B300_76BA_11D3_854F_00A0C9AFD347__INCLUDED_)
#define AFX_CONTROLLERCONFIG_H__3313B300_76BA_11D3_854F_00A0C9AFD347__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControllerConfig.h : header file
//

#include "mmDirectInputDevice.h"

/////////////////////////////////////////////////////////////////////////////
// CControllerConfig dialog

class CControllerConfig : public CDialog
{
// Construction
public:
	CControllerConfig(CWnd* pParent = NULL);   // standard constructor

	WORD m_ControllerNum;
	WORD m_Mapping[16];
// Dialog Data
	//{{AFX_DATA(CControllerConfig)
	enum { IDD = IDD_ControllerConfig };
	CComboBox	m_Controller;
	CComboBox	m_Device;
	CComboBox	m_DeviceMappings;
	CListBox	m_N64Controls;
	BOOL	m_Mempack;
	BOOL	m_Plugged;
	BOOL	m_RumblePack;
	float	m_Y;
	float	m_X;
	BOOL	m_Button1;
	BOOL	m_Button2;
	BOOL	m_Button3;
	BOOL	m_Button4;
	BOOL	m_Button5;
	BOOL	m_Button6;
	BOOL	m_Button7;
	BOOL	m_Button8;
	DWORD	m_POV1;
	DWORD	m_POV2;
	DWORD	m_POV3;
	float	m_Z;
	CString	m_KeyboardValue;
	//}}AFX_DATA


	mmDirectInputDevice *m_InputDevice;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControllerConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CControllerConfig)
	virtual void OnOK();
	afx_msg void OnSelchangeN64Keys();
	afx_msg void OnSelendokController();
	afx_msg void OnSelendokDevice();
	afx_msg void OnSelendcancelDeviceMapping();
	afx_msg void OnSelendokDeviceMapping();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLLERCONFIG_H__3313B300_76BA_11D3_854F_00A0C9AFD347__INCLUDED_)
