#if !defined(AFX_ASMLIST_H__7B12F7EB_4F43_4932_BD80_0311921ED679__INCLUDED_)
#define AFX_ASMLIST_H__7B12F7EB_4F43_4932_BD80_0311921ED679__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AsmList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAsmList window

class CAsmList : public CListBox
{
// Construction
public:
	CAsmList();

// Attributes
public:
	HFONT m_Font;
	

// Operations
public:
	void InitDCStuff(HDC hDC);
	void SubDraw(HDC hDC,char *buf,RECT *where);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAsmList)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAsmList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAsmList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASMLIST_H__7B12F7EB_4F43_4932_BD80_0311921ED679__INCLUDED_)
