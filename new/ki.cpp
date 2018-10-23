// ki.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ki.h"
#include "KIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RELEASE_BUILD
/////////////////////////////////////////////////////////////////////////////
// CKIApp

BEGIN_MESSAGE_MAP(CKIApp, CWinApp)
	//{{AFX_MSG_MAP(CKIApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKIApp construction

CKIApp::CKIApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CKIApp object

CKIApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CKIApp initialization

BOOL CKIApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

/*
#ifdef KI2
	SetRegistryKey(_T("U64Emu2"));
#else
	SetRegistryKey(_T("U64Emu"));
#endif
*/
	SetRegistryKey(_T("U64Emu"));

	CoInitialize(NULL);

	gAllowHLE=0;
	gRomSet=KI2;

	CKIDlg dlg;
	m_pMainWnd = &dlg;

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	m_Auto=false;
   if (m_lpCmdLine[0] != '\0')   
   {
	   if(strcmp(m_lpCmdLine,"-auto")==0)
	   {
		   m_Auto=true;
	   }
   }

	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	return FALSE;
}

void CKIApp::LogMessage(char *fmt, ...)
{
#ifndef RELEASE_BUILD

	char ach[1280];

    va_list va;
    va_start( va, fmt );
    vsprintf( ach, fmt, va );
    va_end( va );

	CFile *LogFile=(CFile *)new CFile();
	char filename[256];
	sprintf(filename,"log.dat",m_Path);
	BOOL result=LogFile->Open(filename,CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
	if(!result)
	{
		delete LogFile;
		return;
	}
	LogFile->Seek(0,CFile::end);
	LogFile->Write(ach,strlen(ach));
	LogFile->Write("\r\n",2);
	LogFile->Close();
	delete LogFile;

#endif
}


void CKIApp::ErrorMessage(char *fmt, ...)
{
    char ach[128];
    va_list va;
    va_start( va, fmt );
    vsprintf( ach, fmt, va );
    va_end( va );
	CFile *LogFile=(CFile *)new CFile();
	char filename[256];
	sprintf(filename,"log.dat");
	LogFile->Open(filename,CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
	LogFile->Seek(0,CFile::end);
	CTime LogTime=CTime::GetCurrentTime();
	CString s = LogTime.Format( "%A, %B %d, %Y, %H:%M:%S " );
	LogFile->Write(s.GetBuffer(256),s.GetLength());
	LogFile->Write("::Error::",strlen("::Error::"));
	LogFile->Write(ach,strlen(ach));
	LogFile->Write("\r\n",2);
	LogFile->Close();
	::MessageBox(0,ach,"General Application Error",MB_OK);
	delete LogFile;
}

void CKIApp::LoadGame()
{
//	m_EmuObj->m_CPU->LoadGame();
}

void CKIApp::SaveGame()
{
}

void CKIApp::RSPMessage(char *fmt, ...)
{
//	return;
#ifndef RELEASE_BUILD

	char ach[1280];

    va_list va;
    va_start( va, fmt );
    vsprintf( ach, fmt, va );
    va_end( va );

/*
	CTime LogTime=CTime::GetCurrentTime();
	CString s = LogTime.Format( "%A, %B %d, %Y, %H:%M:%S " );
	WORD len=s.GetLength();
	m_RDPFile->Write(s.GetBuffer(len),len);
	m_RDPFile->Write("::",2);
*/
	m_RDPFile->Write(ach,strlen(ach));
	m_RDPFile->Write("\r\n",2);

#endif
}

void CKIApp::NIMessage(char *fmt, ...)
{
#ifndef RELEASE_BUILD

	char ach[1280];

    va_list va;
    va_start( va, fmt );
    vsprintf( ach, fmt, va );
    va_end( va );

	CTime LogTime=CTime::GetCurrentTime();
	CString s = LogTime.Format( "%A, %B %d, %Y, %H:%M:%S " );
	WORD len=s.GetLength();
	m_InsFile->Write(s.GetBuffer(len),len);
	m_InsFile->Write("::",2);
	m_InsFile->Write(ach,strlen(ach));
	m_InsFile->Write("\r\n",2);

#endif
}

void CKIApp::VectorMessage(char *fmt, ...)
{
//	return;
#ifndef RELEASE_BUILD

	char ach[1280];

    va_list va;
    va_start( va, fmt );
    vsprintf( ach, fmt, va );
    va_end( va );

	CTime LogTime=CTime::GetCurrentTime();
	CString s = LogTime.Format( "%A, %B %d, %Y, %H:%M:%S " );
	WORD len=s.GetLength();
	m_InsFile->Write(s.GetBuffer(len),len);
	m_InsFile->Write("::",2);
	m_InsFile->Write(ach,strlen(ach));
	m_InsFile->Write("\r\n",2);

#endif
}

void CKIApp::BPMessage(char *fmt, ...)
{
	if(!m_LogGFX)
		return;
//return;
#ifndef RELEASE_BUILD

	char ach[1280];

    va_list va;
    va_start( va, fmt );
    vsprintf( ach, fmt, va );
    va_end( va );

	m_BPFile->Write(ach,strlen(ach));
	m_BPFile->Write("\r\n",2);

#endif
}


int CKIApp::ExitInstance() 
{
	CoUninitialize(); 
/*
	m_InsFile->Close();
	m_RDPFile->Close();
	m_BPFile->Close();
	m_ABFile->Close();
	delete m_InsFile;
	delete m_RDPFile;
	delete m_BPFile;
	delete m_ABFile;
*/
	
	return CWinApp::ExitInstance();
}

extern "C"
{
	DWORD gRomSet;
	DWORD gAllowHLE;
}