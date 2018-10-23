#if !defined(AFX_EMUOBJECT_H__7627060B_55CA_11D3_854F_00A0C9AFD347__INCLUDED_)
#define AFX_EMUOBJECT_H__7627060B_55CA_11D3_854F_00A0C9AFD347__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EmuObject.h : header file
//
#include "global.h"
#include "mmDisplay.h"
#include "mmSoundBuffer.h"
#include "mmDirectInputDevice.h"

#include "iMain.h"
/////////////////////////////////////////////////////////////////////////////
// CEmuObject dialog

class CEmuObject : public CDialog
{
// Construction
public:
	CEmuObject(CWnd* pParent = NULL);   // standard constructor
	~CEmuObject();
	bool m_Open;
	bool m_Debug;
	bool m_IsWaveraceSE;

// file we will emulate
	char m_FileName[256];

// our input device
	mmDirectInputDevice *m_InputDevice;

// our 2d/3d display
	mmDisplay *m_Display;

// emu stuff
//	N64RomFile *m_RomFile;
//	N64CPU *m_CPU;

// display info stuff
	DWORD m_LastTime;
	DWORD m_NumVSYNCs;
	DWORD m_LastInstruction;
	DWORD m_FirstVSYNCTime;
	bool  m_3DActive;

// audio stuff
	BOOL m_BadAudio;
	mmDirectSoundDevice *m_DirectSoundDevice;
	mmSoundBuffer *m_Audio;
	DWORD m_AudioBufferPosition;
	BOOL m_AudioReady;
	DWORD m_AudioMisses;
	DWORD m_AudioHits;

// Dialog Data
	//{{AFX_DATA(CEmuObject)
	enum { IDD = IDD_EmuObject };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	HRESULT Init();
	HRESULT OpenFullScreen();
	HRESULT OpenWindowed();
	void Emulate(char *filename);
	bool UpdateDisplay();
	void UpdateAudio(DWORD offset);
	void UpdateInfo();
	void StopEmulation();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmuObject)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEmuObject)
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCancel();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMUOBJECT_H__7627060B_55CA_11D3_854F_00A0C9AFD347__INCLUDED_)
