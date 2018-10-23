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


CEmuObject::CEmuObject(CWnd* pParent /*=NULL*/)
	: CDialog(CEmuObject::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEmuObject)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(IDD_EmuObject,pParent);

	theApp.m_EmuObj=this;

	m_Display=NULL;
	m_LastTime=0;
	m_LastInstruction=0;
	m_FirstVSYNCTime=0;
	m_NumVSYNCs=0;
	m_Audio=NULL;
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
		if(theApp.m_FullScreen)
			ReleaseCapture();
/*
		m_CPU->m_DMA->SavePack();
		m_CPU->m_DMA->SaveEEPROM("doh.eep");

		m_CPU->StopVTrace();
		m_CPU->StopCPUThread();
*/
		KillTimer(1002);

		iMainStopCPU();

//		hleRdpDestruct();

		iMainDestruct();

		SafeDelete(m_Audio);

		if(m_Display!=NULL)
			m_Display->Close();
		SafeDelete(m_Display);

		m_InputDevice->Close();
		SafeDelete(m_InputDevice);
		m_Open=false;


		SafeDelete(m_DirectSoundDevice);
	}
}

HRESULT CEmuObject::OpenFullScreen()
{
	HRESULT result=m_Display->Create(DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN,false);
	if(result!=DD_OK)
		return(result);
	switch(theApp.m_ScreenRes)
	{
	case 0:
		{
			result=m_Display->Open(320,240,16,this->m_hWnd);
			break;
		}
	case 1:
		{
			result=m_Display->Open(640,480,16,this->m_hWnd);
			break;
		}
	case 2:
		{
			result=m_Display->Open(800,600,16,this->m_hWnd);
			break;
		}
	case 3:
		{
			result=m_Display->Open(1024,768,16,this->m_hWnd);
			break;
		}
	case 4:
		{
			result=m_Display->Open(1280,1024,16,this->m_hWnd);
			break;
		}
	}
	return(result);
}

HRESULT CEmuObject::OpenWindowed()
{
#define FW 32
#define FH 32
	HRESULT result=m_Display->Create(DDSCL_NORMAL,true);
	if(result!=DD_OK)
		return(result);
	WINDOWPLACEMENT wPlace;
	GetWindowPlacement(&wPlace);
	switch(theApp.m_ScreenRes)
	{
	case 0:
		{
			result=m_Display->Open(320,240,16,this->m_hWnd);
			wPlace.rcNormalPosition.right=wPlace.rcNormalPosition.left+320+FW;
			wPlace.rcNormalPosition.bottom=wPlace.rcNormalPosition.top+240+FH;
			break;
		}
	case 1:
		{
			result=m_Display->Open(640,480,16,this->m_hWnd);
			wPlace.rcNormalPosition.right=wPlace.rcNormalPosition.left+640+FW;
			wPlace.rcNormalPosition.bottom=wPlace.rcNormalPosition.top+480+FH;
			break;
		}
	case 2:
		{
			result=m_Display->Open(800,600,16,this->m_hWnd);
			wPlace.rcNormalPosition.right=wPlace.rcNormalPosition.left+800+FW;
			wPlace.rcNormalPosition.bottom=wPlace.rcNormalPosition.top+600+FH;
			break;
		}
	case 3:
		{
			result=m_Display->Open(1024,768,16,this->m_hWnd);
			wPlace.rcNormalPosition.right=wPlace.rcNormalPosition.left+1024+FW;
			wPlace.rcNormalPosition.bottom=wPlace.rcNormalPosition.top+768+FH;
			break;
		}
	case 4:
		{
			result=m_Display->Open(1280,1024,16,this->m_hWnd);
			wPlace.rcNormalPosition.right=wPlace.rcNormalPosition.left+1024+FW;
			wPlace.rcNormalPosition.bottom=wPlace.rcNormalPosition.top+768+FH;
			break;
		}
	}
	SetWindowPlacement(&wPlace);
	return(result);
}

HRESULT CEmuObject::Init()
{
	m_Display=(mmDisplay *)new mmDisplay();
	HRESULT result;
	if(theApp.m_FullScreen)
	{
		result=OpenFullScreen();

	}
	else
	{
		result=OpenWindowed();
	}

	if(result!=DD_OK)
	{
		return(result);
	}

//	m_Display->ClearScreen();
//	m_Display->RenderScene();

	m_DirectSoundDevice=(mmDirectSoundDevice *)new mmDirectSoundDevice();
	result=m_DirectSoundDevice->Create(0);
	if(result!=DS_OK)
	{
		SafeDelete(m_DirectSoundDevice);
	}

	m_InputDevice=(mmDirectInputDevice *)new mmDirectInputDevice();
	theApp.m_InputDevice=m_InputDevice;
	m_InputDevice->Create(DISCL_FOREGROUND,this->m_hWnd);
	m_InputDevice->Open();

	m_Open=true;
	return(DD_OK);
}

void CEmuObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEmuObject)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEmuObject, CDialog)
	//{{AFX_MSG_MAP(CEmuObject)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmuObject message handlers
extern DWORD CatchMe;
void CEmuObject::OnPaint() 
{
	if(!m_3DActive)
		CPaintDC dc(this); // device context for painting
	
}

void CEmuObject::OnClose() 
{
	StopEmulation();
	DestroyWindow();
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
		iCpuNextVSYNC=timeGetTime()+16;
	}

//	if(lagging&&theApp.m_Skip)
	{
//		m_Display->m_FrameCount++;
	}
//	else
	{
		m_Display->UpdateScreenBuffer(this,source,(flags&0x03));
	}
	if(CatchMe)
		CatchMe=2;
	return true;
}



void CEmuObject::UpdateAudio(DWORD offset)
{
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

	}
}

void CEmuObject::Emulate(char *filename)
{
	if(theApp.m_FullScreen)
		SetCapture();
	m_LastTime=0;
	m_LastInstruction=0;
	m_FirstVSYNCTime=0;
	m_NumVSYNCs=0;
	m_Audio=NULL;
	m_BadAudio=true;
//	m_3DActive=true;

	strcpy(m_FileName,filename);

//	hleRdpDisplay=m_Display;
//	hleRdpConstruct();
//	hleRdpCreate();
	::SetCursor(NULL);

	iMainConstruct((char *)m_FileName);
	iRomReadImage((char *)m_FileName);
	iMemCopyBootCode();
	iMainReset();
	iMainStartCPU();

	char info[256];
	sprintf(info,"%s",m_FileName);
	theApp.LogMessage("Emulating %s",m_FileName);
	SetWindowText(info);
	if(theApp.m_FullScreen)
		::SetCursor(NULL);

}	

void CEmuObject::OnTimer(UINT nIDEvent) 
{
	if(theApp.m_FullScreen)
		::SetCursor(NULL);
	return;

//	UpdateDisplay();
	WORD tmp[4];
//	m_InputDevice->GetKeyboardData(0,tmp);
	if(theApp.m_FullScreen) return;

	char info[512];
//	if(!m_Display->m_WindowMode)
//		return;
	if(!r)
		return;
	if(m_LastTime==0)
	{
		m_LastTime=timeGetTime();
		m_LastInstruction=r->ICount;
		return;

	}

//	sprintf(info,"PC:%08X %d %d",(DWORD)r->PC,dynaVCount,r->ICount);
	sprintf(info,"PC:%08X Compare: %6X r->ICount: %8X %8X %4X %4X",r->PC&0x7fffff,(DWORD)r->CompareCount,(DWORD)r->ICount,r->NextIntCount,((DWORD *)m->miReg)[3],((DWORD *)m->miReg)[2]);
	SetWindowText(info);
	return;
	
	DWORD curtime=timeGetTime();
	float deltatime=(float) (curtime-m_LastTime);
	if(deltatime==0.f) return;
	deltatime/=1000.f;
	m_LastTime=curtime;
	float ips=(float)((sQWORD)r->ICount-m_LastInstruction);
	if(ips==0.0f) return;
	ips/=deltatime;
	m_LastInstruction=r->ICount;
	float vps,fps,aps,hz;
	if(m_NumVSYNCs!=0)
	{
		deltatime=(float)(curtime-m_FirstVSYNCTime);
		if(deltatime==0.f) return;
		deltatime/=1000.f;
		vps=0.f;
		fps=((float)m_Display->m_FrameCount/deltatime);
		aps=(float)aiDataUsed/deltatime;
		hz=(float)dynaVCount/deltatime;
	}
	else
		vps=0.f;
//	sprintf(info,"%s - PC:%llX MIPS:%2.3f VPS:%3.3f FPS:%3.3f NumTextures:%d",m_FileName,r->PC,ips/1000000.f,vps,fps,m_CPU->m_RDP->m_NumTextures);
	sprintf(info,"PC:%llX Compare: %8X r->ICount: %8X %8X %4X %4X",r->PC,(DWORD)r->CompareCount,(DWORD)r->ICount,r->NextIntCount,((DWORD *)m->miReg)[3],((DWORD *)m->miReg)[2]);
//	sprintf(info,"PC:%08X Hz:%3.2f NumTextures:%d",(DWORD)r->PC,hz,m_NumTextures);
	SetWindowText(info);
}

BOOL CEmuObject::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetTimer(1002,1000,NULL);
	char info[256];
	sprintf(info,"%s","Loading");
	SetWindowText(info);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CEmuObject::DestroyWindow() 
{
	StopEmulation();
	
	return CDialog::DestroyWindow();
}


void CEmuObject::OnDestroy() 
{
	StopEmulation();
	CDialog::OnDestroy();
}

void CEmuObject::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
}

void CEmuObject::OnCancel() 
{
	StopEmulation();
	OnClose();	
}

void CEmuObject::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CEmuObject::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	
//	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CEmuObject::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CEmuObject::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
/*
	if(nChar==VK_RETURN)
	{
		DWORD start=timeGetTime();
		m_CPU->Halt();
		while(NewTask!=GAME_HALTED)
		{
			Sleep(300);
			if((timeGetTime()-start)>3000)
			{
				m_CPU->Resume();
				return;
			}
		}

		m_CPU->m_RDP->Halt();

		m_Display->Close();
		theApp.m_FullScreen^=1;
		HRESULT result;
		if(theApp.m_FullScreen)
		{
			result=OpenFullScreen();
		}
		else
		{
			result=OpenWindowed();
		}

		if(result!=DD_OK)
		{
			return;
		}
		m_CPU->m_RDP->Resume();

		m_CPU->Resume();
	}
	else
	CDialog::OnSysKeyDown(nChar, nRepCnt, nFlags);
*/
}

void CEmuObject::OnMouseMove(UINT nFlags, CPoint point) 
{
//	if(!m_3DActive)	
//		CDialog::OnMouseMove(nFlags, point);
}
