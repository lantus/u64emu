// ki.h : main header file for the KI application
//

#if !defined(AFX_KI_H__9738CF13_3B69_40CA_AF0D_63E2E5ACB0D3__INCLUDED_)
#define AFX_KI_H__9738CF13_3B69_40CA_AF0D_63E2E5ACB0D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "global.h"
#include "EmuObject1.h"

#define KI1 1
#define KI2 2
/////////////////////////////////////////////////////////////////////////////
// CKIApp:
// See ki.cpp for the implementation of this class
//

class CKIApp : public CWinApp
{
public:
	CKIApp();

	char m_Path[256];

	WORD m_DIPS;

	WORD m_UCode;
	DWORD m_BootCode;
	bool m_Auto;
	bool m_Skip;
	bool m_ScanLines;
	bool m_FakeCOP1;
	bool m_PalShift;
	bool m_ShowFPS;
	bool m_BypassGFX;
	bool m_BypassAFX;
	bool m_LogGFX;
	bool m_StartLog;
	bool m_WireFrame;
	bool m_LockOn;
	WORD m_Indent;
	int m_FullScreen;
	int m_ScreenRes;
	int m_DeviceNum[4];
	bool m_FitToWindow;
	int m_IgnoreSecondary;
	int m_DisableSound;
	int m_3dfx;
	char m_LastBrowsePath[256];
	char m_HomeDirectory[256];
	char m_HDImage[256];
	char m_ARom1[256];
	char m_ARom2[256];
	char m_ARom3[256];
	char m_ARom4[256];
	char m_ARom5[256];
	char m_ARom6[256];
	char m_ARom7[256];
	char m_ARom8[256];

	bool m_MFCHack;
	bool m_UpdateAudio;
	void LogMessage(char *ftm, ...);
	void ErrorMessage(char *ftm, ...);
	CFile *m_BPFile,*m_ABFile,*m_RDPFile,*m_InsFile;
	void RSPMessage(char *ftm, ...);
	void NIMessage(char *ftm, ...);
	void VectorMessage(char *ftm, ...);
	void BPMessage(char *ftm, ...);
	void ABMessage(char *ftm, ...);
	void LoadGame();
	void SaveGame();
	CEmuObject *m_EmuObj;
	mmDirectInputDevice *m_InputDevice;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKIApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKIApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CKIApp theApp;

extern "C" {
	extern DWORD gRomSet;
	extern DWORD gAllowHLE;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KI_H__9738CF13_3B69_40CA_AF0D_63E2E5ACB0D3__INCLUDED_)
