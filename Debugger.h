#if !defined(AFX_DEBUGGER_H__E01FF5F5_A228_44A1_A6C2_8417A6E0EB6E__INCLUDED_)
#define AFX_DEBUGGER_H__E01FF5F5_A228_44A1_A6C2_8417A6E0EB6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Debugger.h : header file
//
#include "AsmList.h"

/////////////////////////////////////////////////////////////////////////////
// CDebugger dialog

class CDebugger : public CDialog
{
// Construction
public:
	CDebugger(CWnd* pParent = NULL);   // standard constructor
	~CDebugger();
	CEmuObject *m_EmuObject;
	char *m_InsText,*m_DspText;
	DWORD m_InsStart,m_InsEnd,m_InsCur;
	DWORD m_InsStartDSP,m_InsEndDSP,m_InsCurDSP;
	WORD m_RegSet;
	int m_LastCursor,m_LastCursorDSP;
	sQWORD m_CPUReg[32];
	sQWORD m_CPR0Reg[32];
	sQWORD m_CCR0Reg[32];
	sQWORD m_RSPReg[32];
	DWORD dspBPoint[32];
	DWORD dspNumBPoints;

// Dialog Data
	//{{AFX_DATA(CDebugger)
	enum { IDD = IDD_Debugger };
	CAsmList	m_AsmList;
	CAsmList	m_RegList;
	CTabCtrl	m_RegSelect;
	CAsmList	m_DSPList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugger)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void DisAsmRange(DWORD start,DWORD end);
	void DisAsmRangeDSP(DWORD start,DWORD end);
	void Init(CEmuObject *emuobj);
	void InitRegisters();
	void InitCPURegisters();
	void UpdateCPURegisters();
	void InitCPR0Registers();
	void InitCCR0Registers();
	void AddBP(DWORD address);
	void RemoveBP(DWORD address);
	bool CheckBP(DWORD address);
	void Break(DWORD address);
	void BreakDSP(DWORD address);
	void UpdateDSPRegisters();
	void InitDSPRegisters();
	void AddBPDSP(DWORD address);
	void RemoveBPDSP(DWORD address);
	bool CheckBPDSP(DWORD address);

protected:

	// Generated message map functions
	//{{AFX_MSG(CDebugger)
	afx_msg void OnSelchangeAsmListing();
	afx_msg void OnDblclkAsmListing();
	afx_msg void OnBreak();
	afx_msg void OnBreakDSP();
	afx_msg void OnBreakPoints();
	afx_msg void OnBreakPointsDSP();
	afx_msg void OnSelchangeDSPListing();
	afx_msg void OnDblclkDSPListing();
	afx_msg void OnGo();
	afx_msg void OnGoDSP();
	afx_msg void OnStep();
	afx_msg void OnStepDSP();
	afx_msg void OnStepInto();
	afx_msg void OnStepIntoDSP();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeRegisters(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGGER_H__E01FF5F5_A228_44A1_A6C2_8417A6E0EB6E__INCLUDED_)
