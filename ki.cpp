// ki.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ki.h"

#include <stdarg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define RELEASE_BUILD
/////////////////////////////////////////////////////////////////////////////
// CKIApp
 
/////////////////////////////////////////////////////////////////////////////
// CKIApp construction

void BootKI1(void);
void BootKI2(void);

bool bQuitSignal;
int gameisrunning = 0;

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
 
	gAllowHLE=0;
	gRomSet=KI2;

 
}

void CKIApp::LogMessage(char *fmt, ...)
{
#ifndef RELEASE_BUILD

	char ach[1280];

    va_list va;
    va_start( va, fmt );
    vsprintf( ach, fmt, va );
    va_end( va );

	ofstream *LogFile=(ofstream *)new ofstream();
	char filename[256];
	sprintf(filename,"log.dat",m_Path);
	LogFile->open(filename,ios::binary | ios::app);
	 
	LogFile->seekp(0,ios::end);
	LogFile->write((char *)ach,strlen(ach));
	LogFile->write((char *)"\r\n",2);
	LogFile->close();
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
	ofstream *LogFile=(ofstream *)new ofstream();
	char filename[256];
	sprintf(filename,"log.dat");
	LogFile->open(filename,ios::binary | ios::app);
	LogFile->seekp(0,ios::end);	 
	LogFile->write("::Error::",strlen("::Error::"));
	LogFile->write(ach,strlen(ach));
	LogFile->write("\r\n",2);
	LogFile->close();	 
	delete LogFile;
}

void CKIApp::LoadGame()
{
 
}

void CKIApp::SaveGame()
{
}

void CKIApp::RSPMessage(char *fmt, ...)
{
//	return;
#ifndef RELEASE_BUILD
 

#endif
}

void CKIApp::NIMessage(char *fmt, ...)
{
#ifndef RELEASE_BUILD
 

#endif
}

void CKIApp::VectorMessage(char *fmt, ...)
{
//	return;
#ifndef RELEASE_BUILD
 

#endif
}

void CKIApp::BPMessage(char *fmt, ...)
{
	if(!m_LogGFX)
		return;
//return;
#ifndef RELEASE_BUILD
 

#endif
}


int CKIApp::ExitInstance() 
{
 
}

extern "C"
{
	DWORD gRomSet;
	DWORD gAllowHLE;
}


int main()
{
	BootKI1();
	
	return 0;
}

void BootKI1(void)
{
	CEmuObject e;

 

	theApp.m_UCode		= 0;
	theApp.m_BootCode	= 0x3f;
	theApp.m_DIPS		= 3584;
	theApp.m_LockOn		= true;
	theApp.m_FrameDelay	= (DWORD)(1000.f / 60.f + .3f);
	theApp.m_VTraceInc	= 49999980 / 60;
 
	gAllowHLE	= true;
	bQuitSignal	= false;

	strcpy(theApp.m_HDImage, "ki.img");

	strcpy(theApp.m_ARom1, "u10-l1");
	strcpy(theApp.m_ARom2, "u11-l1");
	strcpy(theApp.m_ARom3, "u12-l1");
	strcpy(theApp.m_ARom4, "u13-l1");
	strcpy(theApp.m_ARom5, "u33-l1");
	strcpy(theApp.m_ARom6, "u34-l1");
	strcpy(theApp.m_ARom7, "u35-l1");
	strcpy(theApp.m_ARom8, "u36-l1");
  
	gRomSet = KI1;
	e.Init();
	e.Emulate("u98-l15d");
	theApp.m_UCode = 0;

	while (!bQuitSignal);
	e.StopEmulation();
	 
}


void BootKI2(void)
{
	CEmuObject e;
 

	theApp.m_UCode		= 0;
 	theApp.m_BootCode	= 0x3f;
	theApp.m_DIPS		= 3584;
	theApp.m_LockOn		= true;
	theApp.m_FrameDelay	= (DWORD)(1000.f / 60.f + .3f);
	theApp.m_VTraceInc	= 49999980 / 60;

	gAllowHLE	= true;
	bQuitSignal	= false;
 
	strcpy(theApp.m_HDImage,"d:\\KI2\\ki2.img");

	strcpy(theApp.m_ARom1,  "d:\\KI2\\ki2_l1.u10");
	strcpy(theApp.m_ARom2,  "d:\\KI2\\ki2_l1.u11");
	strcpy(theApp.m_ARom3,  "d:\\KI2\\ki2_l1.u12");
	strcpy(theApp.m_ARom4,  "d:\\KI2\\ki2_l1.u13");
	strcpy(theApp.m_ARom5,  "d:\\KI2\\ki2_l1.u33");
	strcpy(theApp.m_ARom6,  "d:\\KI2\\ki2_l1.u34");
	strcpy(theApp.m_ARom7,  "d:\\KI2\\ki2_l1.u35");
	strcpy(theApp.m_ARom8,  "d:\\KI2\\ki2_l1.u36");
  
	gRomSet = KI2;
	e.Init();
	e.Emulate("d:\\KI2\\ki2-l11.u98");
	theApp.m_UCode = 0;

	while (!bQuitSignal);

	e.StopEmulation();
	 
}