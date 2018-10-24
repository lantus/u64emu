#if !defined(AFX_EMUOBJECT_H__7627060B_55CA_11D3_854F_00A0C9AFD347__INCLUDED_)
#define AFX_EMUOBJECT_H__7627060B_55CA_11D3_854F_00A0C9AFD347__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EmuObject.h : header file
//
#include "global.h"
#include "mmDisplay.h"
//#include "mmSoundBuffer.h"
//#include "mmDirectInputDevice.h"

#include "iMain.h"
/////////////////////////////////////////////////////////////////////////////
// CEmuObject dialog

class CEmuObject  
{
// Construction
public:
	CEmuObject();   // standard constructor
	~CEmuObject();
	bool m_Open;
	bool m_Debug;
	bool m_IsWaveraceSE;

// file we will emulate
	char m_FileName[256];

// our input device
//	mmDirectInputDevice *m_InputDevice;

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
//	mmDirectSoundDevice *m_DirectSoundDevice;
//	mmSoundBuffer *m_Audio;
	DWORD m_AudioBufferPosition;
	BOOL m_AudioReady;
	DWORD m_AudioMisses;
	DWORD m_AudioHits;

 
	HRESULT Init();
	HRESULT OpenFullScreen();
	HRESULT OpenWindowed();
	void Emulate(char *filename);
	bool UpdateDisplay();
	void UpdateAudio(DWORD offset);
	void UpdateInfo();
	void StopEmulation();
	void OnClose();
 
protected:
 
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMUOBJECT_H__7627060B_55CA_11D3_854F_00A0C9AFD347__INCLUDED_)
