// EmuObject.cpp : implementation file
//

#include "stdafx.h"
#include "ki.h"
#include "EmuObject1.h"

#include "iCPU.h"			// Core 4600 emulation routines
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines
#include "adsp2100.h"
#include "hleDSP.h"

extern WORD *ataDataBuffer;
extern DWORD lagging;



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern volatile WORD NewTask;
extern DWORD dynaVCount;

/*
extern DWORD StartTime;
extern DWORD CurTime;
*/
extern WORD globalAudioIncRateWhole;
extern WORD globalAudioIncRatePartial;
extern WORD globalAudioIncRateFrac;
extern BOOL dynaHLEAudio;

extern DWORD aiRate;
extern DWORD aiBufLen;
extern DWORD aiUsed;
extern DWORD aiUsedF;
extern DWORD aiUsedW;
extern DWORD aiUsedA;
extern DWORD aiDataUsed;

#define NUM_BUFFERS 64
DWORD lastTime=0;
DWORD aiWrites=0;
extern DWORD cheat;
/////////////////////////////////////////////////////////////////////////////
// CEmuObject dialog


CEmuObject::CEmuObject( )	 
{
	 
	theApp.m_EmuObj=this;

//	m_Display=NULL;
	m_LastTime=0;
	m_LastInstruction=0;
	m_FirstVSYNCTime=0;
	m_NumVSYNCs=0;
//	m_Audio=NULL;
	m_BadAudio=true;
	m_3DActive=false;
	m_Debug=false;
	m_IsWaveraceSE=false;
	m_Debug=false;
}

CEmuObject::~CEmuObject()
{
	StopEmulation();
}

void CEmuObject::StopEmulation()
{

	if(m_Open)
	{
 

		iMainStopCPU(); 
		iMainDestruct();

		//SafeDelete(m_Audio);

		//if(m_Display!=NULL)
//			m_Display->Close();
		//SafeDelete(m_Display);

		//m_InputDevice->Close();
		//SafeDelete(m_InputDevice);
		m_Open=false;


		//SafeDelete(m_DirectSoundDevice);
	}
}

HRESULT CEmuObject::OpenFullScreen()
{
	 
}

HRESULT CEmuObject::OpenWindowed()
{
 
}

HRESULT CEmuObject::Init()
{
//	m_Display=(mmDisplay *)new mmDisplay();
//	HRESULT result;
 
	// init gfx here
 

//	m_Display->ClearScreen();
//	m_Display->RenderScene();

	//m_DirectSoundDevice=(mmDirectSoundDevice *)new mmDirectSoundDevice();
	//result=m_DirectSoundDevice->Create(0);
	//if(result!=DS_OK)
	//{
//		SafeDelete(m_DirectSoundDevice);
//	}

//	m_InputDevice=(mmDirectInputDevice *)new mmDirectInputDevice();
//	theApp.m_InputDevice=m_InputDevice;
//	m_InputDevice->Create(DISCL_FOREGROUND,this->m_hWnd);
//	m_InputDevice->Open();

	m_Open=true;
	return(0);
}
 

/////////////////////////////////////////////////////////////////////////////
// CEmuObject message handlers
extern DWORD CatchMe;
 
void CEmuObject::OnClose() 
{
	StopEmulation();
	//DestroyWindow();
}

bool CEmuObject::UpdateDisplay()
{
	if(!m_Open)
		return false;

	static char * ImageOffset;
	static WORD screen=0;
	static bool first3D=false;
	DWORD width,height;
	width=640;
	height=480;

	DWORD flags=0;
	DWORD offset;
	if(gRomSet==KI2)
		offset=m->atReg[0x98];
	else
		offset=m->atReg[0x80];

//	if(CatchMe==2)
//		return(true);
	char *source=(char *)SRAM+((offset)*320*128+0x30000); //+0x12c00);

	m_NumVSYNCs++;
	if(m_NumVSYNCs>90)
	{
		m_NumVSYNCs=1;
		iCpuNextVSYNC=svcGetSystemTick()+16;
	}

//	if(lagging&&theApp.m_Skip)
	{
//		m_Display->m_FrameCount++;
	}
//	else
	{
//		m_Display->UpdateScreenBuffer(this,source,(flags&0x03));		TODO:FIX
	}
	if(CatchMe)
		CatchMe=2;
	return true;
}



void CEmuObject::UpdateAudio(DWORD offset)
{
	
	/*
	DWORD length,rate;

#ifdef LOG_STUFF
		theApp.LogMessage("Update Audio");
#endif
	if(m_Audio!=NULL)
	{
		m_AudioBufferPosition++;
		m_AudioBufferPosition&=(NUM_BUFFERS-1);

		m_Audio->Update((char *)&m->dspDMem[offset*2],0x1e0,m_AudioBufferPosition);
		if(!m_AudioReady)
		{
			if(m_AudioBufferPosition>6)
			{
				m_Audio->Play(0,0,DSBPLAY_LOOPING);
				m_AudioReady=TRUE;
			}
		}
		return;
	}
	else
	{
		char *src;
		WORD bits;
		src=(char *)&m->dspDMem[dspAutoBase*2];
		length=0x1e0;
		rate=32000;
		bits=16;

		m_Audio=(mmSoundBuffer *) new mmSoundBuffer();
		m_Audio->Create(m_DirectSoundDevice);
		if(!m_Audio->FromMemory(src,length,rate,bits,NUM_BUFFERS,FALSE))
		{
			m_BadAudio=true;
		}
		else
		{
			m_BadAudio=false;
		}
		m_AudioBufferPosition=0;
		m_AudioReady=FALSE;

	} */
}

void CEmuObject::Emulate(char *filename)
{
	 
	m_LastTime=0;
	m_LastInstruction=0;
	m_FirstVSYNCTime=0;
	m_NumVSYNCs=0;
	
	//m_Audio=NULL;			TODO:FIX
	//m_BadAudio=true;
	
//	m_3DActive=true;

	strcpy(m_FileName,filename);

//	hleRdpDisplay=m_Display;
//	hleRdpConstruct();
//	hleRdpCreate();
	 

	iMainConstruct((char *)m_FileName);
	iRomReadImage((char *)m_FileName);
	theApp.LogMessage("about to iMemCopyBootCode()");
	iMemCopyBootCode();
	theApp.LogMessage("about to iMainReset()");
	iMainReset();
	theApp.LogMessage("about to iMainStartCPU()");
	iMainStartCPU();

 

}	
 
 
