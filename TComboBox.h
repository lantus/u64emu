#if !defined(AFX_TCOMBOBOX_H__13147740_26BC_4100_9369_CCE711E15668__INCLUDED_)
#define AFX_TCOMBOBOX_H__13147740_26BC_4100_9369_CCE711E15668__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TComboBox window

class TComboBox : public CComboBox
{
// Construction
public:
	TComboBox();
	void InitDCStuff(HDC hDC);
	HFONT m_Font;
	CBrush *m_BackBrush;
	DWORD m_BackColor;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TComboBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~TComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(TComboBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDrawClipboard();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TCOMBOBOX_H__13147740_26BC_4100_9369_CCE711E15668__INCLUDED_)
