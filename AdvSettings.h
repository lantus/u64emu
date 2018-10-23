#if !defined(AFX_ADVSETTINGS_H__7A0D72AF_2E3F_4202_B611_D12CC1DD8904__INCLUDED_)
#define AFX_ADVSETTINGS_H__7A0D72AF_2E3F_4202_B611_D12CC1DD8904__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdvSettings dialog

class CAdvSettings : public CDialog
{
// Construction
public:
	CAdvSettings(CWnd* pParent = NULL);   // standard constructor
	DWORD m_ScreenRes;

// Dialog Data
	//{{AFX_DATA(CAdvSettings)
	enum { IDD = IDD_Settings };
	CComboBox	m_ScreenResolution;
	int		m_FrameRate;
	BOOL	m_FullScreen;
	BOOL	m_NoHLE;
	BOOL	m_ScanLines;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvSettings)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGS_H__7A0D72AF_2E3F_4202_B611_D12CC1DD8904__INCLUDED_)
