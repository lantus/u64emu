/* Copyright MovingModels Inc, 1999

Module:
----------------------------------------------------------
	mmDisplay

Function:
----------------------------------------------------------
	Handles DirectDraw and Direct3D

Revision History:
----------------------------------------------------------
	07/31/99 - jrb


*/



#include "stdafx.h"
#include "math.h"

#include "global.h"
#include "ki.h"
#include "mmDisplay.h"

//#define LOG

static BOOL FAR PASCAL enumDDDeviceFunc(GUID FAR* lpGuid, LPSTR lpDriverDesc,
                                      LPSTR lpDriverName, LPVOID lpContext);

HRESULT CALLBACK D3DEnumPixelFormatsCallback(  LPDDPIXELFORMAT lpDDPixFmt,  
  LPVOID          lpContext    ); 

static HRESULT CALLBACK enumTextureFormatsFunc(LPDDSURFACEDESC lpDdsd,LPVOID lpUserArg);

extern "C"
{
#include "d3dmath.h"
}

#define AR 90
#define AB 90
#define AG 90

#define CULLMODE D3DCULL_CCW
#define NEAR_CLIP_VALUE 0.000001f


#define WIDTH 640
#define HEIGHT 480
#define WIDTH_2 320.f
#define HEIGHT_2 240.f

#define DX6

DWORD CurTime;
DWORD StartTime=1;
extern DWORD dynaVCount;
extern bool iCpuResetVSYNC;
WORD *ColorSwap;
DWORD *ColorSwapD;

mmDisplay::mmDisplay()
{
	m_WindowMode=false;

	m_Identity._11=D3DVAL(1.0); m_Identity._12=D3DVAL(0.0); m_Identity._13=D3DVAL(0.0); m_Identity._14=D3DVAL(0.0);
	m_Identity._21=D3DVAL(0.0); m_Identity._22=D3DVAL(1.0); m_Identity._23=D3DVAL(0.0); m_Identity._24=D3DVAL(0.0);
	m_Identity._31=D3DVAL(0.0); m_Identity._32=D3DVAL(0.0); m_Identity._33=D3DVAL(1.0); m_Identity._34=D3DVAL(0.0);
	m_Identity._41=D3DVAL(0.0); m_Identity._42=D3DVAL(0.0); m_Identity._43=D3DVAL(0.0); m_Identity._44=D3DVAL(1.0);

	m_World=m_Identity;
	m_Viewport=NULL;

	m_Projection._11=D3DVAL(2.0); m_Projection._12=D3DVAL(0.0); m_Projection._13=D3DVAL(0.0); m_Projection._14=D3DVAL(0.0);
	m_Projection._21=D3DVAL(0.0); m_Projection._22=D3DVAL(2.0); m_Projection._23=D3DVAL(0.0); m_Projection._24=D3DVAL(0.0);
	m_Projection._31=D3DVAL(0.0); m_Projection._32=D3DVAL(0.0); m_Projection._33=D3DVAL(1.0); m_Projection._34=D3DVAL(1.0);
	m_Projection._41=D3DVAL(0.0); m_Projection._42=D3DVAL(0.0); m_Projection._43=D3DVAL(-1.0); m_Projection._44=D3DVAL(0.0);
	
	m_View._11=D3DVAL(1.0); m_View._12=D3DVAL(0.0); m_View._13=D3DVAL(0.0); m_View._14=D3DVAL(0.0);
	m_View._21=D3DVAL(0.0); m_View._22=D3DVAL(1.0); m_View._23=D3DVAL(0.0); m_View._24=D3DVAL(0.0);
	m_View._31=D3DVAL(0.0); m_View._32=D3DVAL(0.0); m_View._33=D3DVAL(1.0); m_View._34=D3DVAL(0.0);
	m_View._41=D3DVAL(0.0); m_View._42=D3DVAL(0.0); m_View._43=D3DVAL(10.0); m_View._44=D3DVAL(1.0);

	m_NumD3DDevices=0;
	m_NumTextureFormats=0;

	SetFieldOfView(_PI/3.f);
	m_WorldDirty=true;
	m_ViewDirty=true;
	m_ProjectionDirty=true;
	m_TextureHandle=NULL;
	m_MaterialHandle=NULL;

	m_FrameCount=0;


	//pointers
	m_D3DDevice=NULL;
	m_D3DDevice2=NULL;
	m_D3DDevice3=NULL;
	m_D3DObject=NULL;
	m_DDDevice=NULL;
	m_DDDevice4=NULL;
	m_DDClipper=NULL;
	m_BackBuffer=NULL;
	m_FrontBuffer=NULL;
	m_ZBuffer=NULL;
	m_ScreenBuffer=NULL;

	//allocations
	m_IsOpen=false;
	ColorSwap=NULL;

}

mmDisplay::~mmDisplay()
{
	free(ColorSwap);
}


HRESULT mmDisplay::Create(DWORD CoOpFlags,bool IsWindow)
{
	if(IsWindow)
		m_WindowMode=true;
	else
		m_WindowMode=false;

	m_Identity._11=D3DVAL(1.0); m_Identity._12=D3DVAL(0.0); m_Identity._13=D3DVAL(0.0); m_Identity._14=D3DVAL(0.0);
	m_Identity._21=D3DVAL(0.0); m_Identity._22=D3DVAL(1.0); m_Identity._23=D3DVAL(0.0); m_Identity._24=D3DVAL(0.0);
	m_Identity._31=D3DVAL(0.0); m_Identity._32=D3DVAL(0.0); m_Identity._33=D3DVAL(1.0); m_Identity._34=D3DVAL(0.0);
	m_Identity._41=D3DVAL(0.0); m_Identity._42=D3DVAL(0.0); m_Identity._43=D3DVAL(0.0); m_Identity._44=D3DVAL(1.0);

	m_World=m_Identity;
	m_Viewport=NULL;

	m_Projection._11=D3DVAL(2.0); m_Projection._12=D3DVAL(0.0); m_Projection._13=D3DVAL(0.0); m_Projection._14=D3DVAL(0.0);
	m_Projection._21=D3DVAL(0.0); m_Projection._22=D3DVAL(2.0); m_Projection._23=D3DVAL(0.0); m_Projection._24=D3DVAL(0.0);
	m_Projection._31=D3DVAL(0.0); m_Projection._32=D3DVAL(0.0); m_Projection._33=D3DVAL(1.0); m_Projection._34=D3DVAL(1.0);
	m_Projection._41=D3DVAL(0.0); m_Projection._42=D3DVAL(0.0); m_Projection._43=D3DVAL(-1.0); m_Projection._44=D3DVAL(0.0);
	
	m_View._11=D3DVAL(1.0); m_View._12=D3DVAL(0.0); m_View._13=D3DVAL(0.0); m_View._14=D3DVAL(0.0);
	m_View._21=D3DVAL(0.0); m_View._22=D3DVAL(1.0); m_View._23=D3DVAL(0.0); m_View._24=D3DVAL(0.0);
	m_View._31=D3DVAL(0.0); m_View._32=D3DVAL(0.0); m_View._33=D3DVAL(1.0); m_View._34=D3DVAL(0.0);
	m_View._41=D3DVAL(0.0); m_View._42=D3DVAL(0.0); m_View._43=D3DVAL(10.0); m_View._44=D3DVAL(1.0);

	m_NumD3DDevices=0;
	m_NumTextureFormats=0;

	SetFieldOfView(_PI/3.f);

	m_CoOpFlags= CoOpFlags;
	m_WindowMode=IsWindow;
	m_InScene=false;
	m_NumD3DDevices=0;
	m_NumTextureFormats=0;

	m_WorldDirty=true;
	m_ViewDirty=true;
	m_ProjectionDirty=true;
	m_TextureHandle=NULL;
	m_MaterialHandle=NULL;

	m_FrameCount=0;


	//pointers
	m_D3DDevice=NULL;
	m_D3DDevice2=NULL;
	m_D3DDevice3=NULL;
	m_D3DObject=NULL;
	m_DDDevice=NULL;
	m_DDDevice4=NULL;
	m_DDClipper=NULL;
	m_BackBuffer=NULL;
	m_FrontBuffer=NULL;
	m_ZBuffer=NULL;
	m_ScreenBuffer=NULL;

	m_IsOpen=false;
	return(DD_OK);
}

GUID *lpWindowGUID=NULL;
GUID *lpBackupGUID=NULL;

static BOOL FAR PASCAL enumDDDeviceFunc(GUID FAR* lpGuid, LPSTR lpDriverDesc,
                                      LPSTR lpDriverName, LPVOID lpContext)
{
	LPDIRECTDRAW lpDD;
	DDCAPS DriverCaps, HELCaps;
	LPSTR asdf=lpDriverName;

	HRESULT err;
	if (lpGuid) 
	{
       /*
        * Create the DirectDraw device using this driver.  If it fails,
        * just move on to the next driver.
        */
		err=DirectDrawCreate(lpGuid, &lpDD, NULL);
		if (err!=D3D_OK) 
		{
			theApp.LogMessage("Device Failed Create in Enumeration %s",lpDriverDesc);
			return DDENUMRET_OK;
		}
       /*
        * Get the capabilities of this DirectDraw driver.  If it fails,
        * just move on to the next driver.
        */
		memset(&DriverCaps, 0, sizeof(DDCAPS));
		DriverCaps.dwSize = sizeof(DDCAPS);
		memset(&HELCaps, 0, sizeof(DDCAPS));
		HELCaps.dwSize = sizeof(DDCAPS);
		HELCaps.dwSize = sizeof(DDCAPS);
		if (lpDD->GetCaps(&DriverCaps, &HELCaps)!=D3D_OK) 
		{
			theApp.LogMessage("Device Failed GetCaps %s",lpDriverDesc);
			lpDD->Release();
			return DDENUMRET_OK;
		}
		if (DriverCaps.dwCaps & DDCAPS_3D) 
		{
/*
			if(DriverCaps.dwCaps2&DDCAPS2_CANRENDERWINDOWED)
			{
				lpWindowGUID=lpGuid;
				lpBackupGUID=lpGuid;
			}
			else
*/
			{
				lpBackupGUID=lpGuid;
			}
			((mmDisplay *)lpContext)->m_DDDevice=lpDD;
			theApp.LogMessage("Using D3D driver %s",lpDriverDesc);
//			lpDD->Add
//		    lpDD->Release();
			return DDENUMRET_CANCEL;
		}
		else
		{
			theApp.LogMessage("Found Device without DDCAPS_3D %s",lpDriverDesc);
		}
      lpDD->Release();
  }

    return (D3DENUMRET_OK);
}


HRESULT mmDisplay::Open(
	WORD Width,
	WORD Height,
	WORD Depth,
	HWND hWnd)
{
    HRESULT err;

	m_TextureFormats=(DDPIXELFORMAT *)malloc(sizeof(DDPIXELFORMAT));

	m_hWnd=hWnd;
/*
	if(m_hWnd==NULL)
		m_WindowMode=false;
	else
		m_WindowMode=true;
*/

	if(m_DDDevice==NULL)
	{
			err = DirectDrawCreate(NULL,&m_DDDevice, NULL );
			if(err!=0)
			{
				::MessageBox(0,"No Direct3D Hardware Device Available","Error Creating D3D Device",MB_OK);
				return(err);
			}
	}
	err=m_DDDevice->QueryInterface ((REFIID)IID_IDirectDraw4, (void **)&m_DDDevice4);
	if(err!=D3D_OK)
	{
		theApp.ErrorMessage("%s - %s",ShowError(err),"Draw4 Interface");
		return(err);
	}

    err = m_DDDevice4->SetCooperativeLevel(m_hWnd, m_CoOpFlags );
	if(err!=D3D_OK)
	{
		theApp.ErrorMessage("%s - %s",ShowError(err),"Coop ERROR");
		return(err);
	}

	m_Width=Width;
	m_Height=Height;
	m_Depth=Depth;

	if(!m_WindowMode)
	{
		m_DDDevice->SetDisplayMode(m_Width,m_Height,m_Depth);
	}



    DDSURFACEDESC2 ddsd;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

	if(!m_WindowMode)
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.dwBackBufferCount = 2;
		{
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
								  DDSCAPS_FLIP |
								  DDSCAPS_COMPLEX;
	//							  DDSCAPS_VIDEOMEMORY;
	//							  DDSCAPS_3DDEVICE;
		}

		err = m_DDDevice4->CreateSurface(&ddsd, &m_FrontBuffer, NULL);
		if(err!=D3D_OK)
		{
			ddsd.dwBackBufferCount = 1;
			err = m_DDDevice4->CreateSurface(&ddsd, &m_FrontBuffer, NULL);
			if(err!=D3D_OK)
			{
				theApp.ErrorMessage("%s - %s",ShowError(err),"Create Front buffer");
				return(err);
			}
		}
/*
*/
	}
	else
	{
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE ;//| DDSCAPS_VIDEOMEMORY;
		err = m_DDDevice4->CreateSurface(&ddsd, &m_FrontBuffer, NULL);
		if(err!=DD_OK)
		{
			theApp.ErrorMessage("%s - %s",ShowError(err),"Create Front buffer");
			return(err);
		}
	}

	if((hWnd!=NULL)&&m_WindowMode)
		SetClipper(hWnd);

	if(!m_WindowMode)
	{
		DDSCAPS2 caps;
		caps.dwCaps = DDSCAPS_BACKBUFFER;
		err = m_FrontBuffer->GetAttachedSurface(&caps, &m_BackBuffer);
	}

	MakeScreenBuffer();
	return(D3D_OK);

#ifdef LOG
	theApp.LogMessage("Got Front");
#endif

	if(!m_WindowMode)
	{
		DDSCAPS2 caps;
		caps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE;
		err = m_FrontBuffer->GetAttachedSurface(&caps, &m_BackBuffer);
	}
	else
	{
		DDSCAPS2 caps;
		caps.dwCaps = DDSCAPS_BACKBUFFER;

		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.dwHeight = m_Height;
		ddsd.dwWidth = m_Width;
		ddsd.ddsCaps.dwCaps= DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE; // | DDSCAPS_SYSTEMMEMORY;
		err = m_DDDevice4->CreateSurface(&ddsd, &m_BackBuffer, NULL);

	}

	if(err!=D3D_OK)
	{
		theApp.ErrorMessage("%s - %s",ShowError(err),"Back buffer");
		return(err);
	}
	
#ifdef LOG
	theApp.LogMessage("Got Back");
#endif

#ifdef _SFTWR_RENDER
	
	PALETTEENTRY ThisPal[256];
	for (int i = 0; i < 10; i++) ThisPal[i].peFlags = D3DPAL_READONLY;
	for (i = 10; i < 256 - 10; i++) ThisPal[i].peFlags = D3DPAL_FREE | PC_RESERVED;
	for (i = 256 - 10; i < 256; i++) ThisPal[i].peFlags = D3DPAL_READONLY;
	err=m_DDDevice4->CreatePalette(DDPCAPS_INITIALIZE|DDPCAPS_8BIT|DDPCAPS_ALLOW256,ThisPal,(IDirectDrawPalette **)&m_Palette,NULL);
	err=m_FrontBuffer->SetPalette(m_Palette);
	err=m_BackBuffer->SetPalette(m_Palette);
#endif

    err = m_DDDevice4->QueryInterface(IID_IDirect3D3,(void **)&m_D3DObject);
    if (err != DD_OK)
	{
		theApp.ErrorMessage("%s - %s",ShowError(err),"QI for D3D Object");
        return( err );
	}

#ifdef LOG
	theApp.LogMessage("Got D3D Object");
#endif


	err=CreateZBuffer(m_D3DObject);
    if (err != DD_OK)
	{
		theApp.ErrorMessage("%s - %s",ShowError(err),"Creating Z-Buffer");
        return( err );
	}
#ifdef LOG
	theApp.LogMessage("Got Z");
#endif

    m_NumD3DDevices = 0;

	if(!m_WindowMode)
		err=m_D3DObject->CreateDevice(IID_IDirect3DHALDevice,m_BackBuffer,&m_D3DDevice3,NULL);
	else
		err=m_D3DObject->CreateDevice(IID_IDirect3DHALDevice,m_BackBuffer,&m_D3DDevice3,NULL);
    if (err != DD_OK)
	{
		theApp.ErrorMessage("%s - %s",ShowError(err),"Creating IDirect3D3");
        return( err );
	}

#ifdef LOG
	theApp.LogMessage("Got D3D");
#endif

	err=m_D3DDevice3->QueryInterface(IID_IDirect3DDevice2,(void **)&m_D3DDevice2);
    if (err != DD_OK)
	{
		theApp.ErrorMessage("%s - %s",ShowError(err),"QI for IDirect3D2");
        return( err );
	}

	err=m_D3DDevice2->QueryInterface(IID_IDirect3DDevice,(void **)&m_D3DDevice);
    if (err != DD_OK)
	{
		theApp.ErrorMessage("%s - %s",ShowError(err),"QI for IDirect3D");
        return( err );
	}

#ifdef LOG
	theApp.LogMessage("Got D3D Interfaces");
#endif

	err=CreateViewport(Width,Height);
    if (err != DD_OK)
	{
		theApp.ErrorMessage("%s - %s",ShowError(err),"Creating Viewport");
        return( err );
	}

#ifdef LOG
	theApp.LogMessage("Got Viewport");
#endif

	m_D3DDevice2->EnumTextureFormats((LPD3DENUMTEXTUREFORMATSCALLBACK)enumTextureFormatsFunc, this);

	int count=0;
	m_TextureFormat=m_TextureFormats[0];
	while(count<m_NumTextureFormats)
	{
		if ((m_TextureFormats[count].dwFlags & DDPF_RGB)&&
			(m_TextureFormats[count].dwFlags & DDPF_ALPHAPIXELS)&&
			(m_TextureFormats[count].dwRGBBitCount==16))
		{
			if((m_TextureFormats[count].dwRBitMask|m_TextureFormats[count].dwGBitMask|m_TextureFormats[count].dwBBitMask)==0x0fff)
			{
				m_TextureFormat=m_TextureFormats[count];		// this is what we want, so quit looking
			}
			else if((m_TextureFormats[count].dwRBitMask|m_TextureFormats[count].dwGBitMask|m_TextureFormats[count].dwBBitMask)==0xfff0)
			{
//				m_TextureFormat=m_TextureFormats[count];		// this is what we want, so quit looking
			}
			else if((m_TextureFormats[count].dwRBitMask|m_TextureFormats[count].dwGBitMask|m_TextureFormats[count].dwBBitMask)==0x00ff)
			{
			}
			else if((m_TextureFormats[count].dwRBitMask|m_TextureFormats[count].dwGBitMask|m_TextureFormats[count].dwBBitMask)==0x7fff)
			{
				m_HighColorFormat=m_TextureFormats[count];
			}
		}
		count++;
	}
#ifdef _SFTWR_RENDER
//				m_HighColorFormat=m_TextureFormat;
#endif
//    SetTransform(D3DTRANSFORMSTATE_PROJECTION,&m_Projection);

#ifdef LOG
	theApp.LogMessage("Got TextureFormats");
#endif

	InitRenderState();
	D3DCOLOR ambient=RGBA_MAKE(115,115,115,255);
	SetAmbientLight(ambient);

	D3DVECTOR pos,ori;
	pos.x=0.f; pos.y=0.f; pos.z=0.f;
	ori.x=0.f; ori.y=0.f; ori.z=0.f;
	SetCamera(&pos,&ori);


#ifdef LOG
	theApp.LogMessage("Got Screen Buffer");
#endif

	ClearScreen();
	RenderScene();

	GetCaps();
#ifdef LOG
	theApp.LogMessage("Got Caps & Done");
#endif

	m_IsOpen=true;
    return( err );
}



bool mmDisplay::GetCaps()
{
	D3DDEVICEDESC hwInfo;
	memset(&hwInfo,0,sizeof(hwInfo));
	hwInfo.dwSize=sizeof(hwInfo);
	D3DDEVICEDESC swInfo;
	memset(&swInfo,0,sizeof(swInfo));
	swInfo.dwSize=sizeof(swInfo);
	m_D3DDevice3->GetCaps(&hwInfo,&swInfo);
	if(hwInfo.dwDevCaps&D3DDEVCAPS_SEPARATETEXTUREMEMORIES)
		theApp.LogMessage("Separate Texture Memories");
	if(hwInfo.dpcTriCaps.dwTextureCaps&D3DPTEXTURECAPS_SQUAREONLY)
	{
		theApp.LogMessage("Square Textures only");
		m_SquareOnly=true;
	}
	else
		m_SquareOnly=false;

	if(hwInfo.dwTextureOpCaps&D3DTEXOPCAPS_ADD)
		theApp.LogMessage("Add supported");
	else
		theApp.LogMessage("Add NOT supported");
	theApp.LogMessage("TStages %d",hwInfo.wMaxTextureBlendStages);
	theApp.LogMessage("S-TStages %d",hwInfo.wMaxSimultaneousTextures);
	m_TextureOpCaps=hwInfo.dwTextureOpCaps;
	return(true);
}

void mmDisplay::SetClipper(HWND hWnd)
{
	if(m_WindowMode)
	{
		DirectDrawCreateClipper( 0, &m_DDClipper, NULL );
		m_FrontBuffer->SetClipper( m_DDClipper );
		m_DDClipper->SetHWnd( 0, hWnd );
		SafeReleaseNoCheck(m_DDClipper);
	}
}


HRESULT mmDisplay::Close()
{
	if(m_TextureFormats!=NULL)
		free(m_TextureFormats);

	SafeRelease(m_ScreenBuffer);

//	if(m_IsOpen==false) return(DD_OK);

//	EndScene();

    HRESULT err=m_DDDevice4->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);
    if(err != DD_OK) 
    {
		theApp.ErrorMessage("%s - %s",ShowError(err),"SetCoop - DDSCL_NORMAL");
		return( err );
    }
	err=m_DDDevice4->RestoreDisplayMode();
    if(err != DD_OK) 
    {
		theApp.ErrorMessage("%s - %s",ShowError(err),"Restore DisplayMode");
		return( err );
    }

	SafeReleaseNoCheck(m_Viewport);

	SafeReleaseNoCheck(m_D3DDevice);

	SafeReleaseNoCheck(m_D3DDevice2);

	SafeRelease(m_D3DDevice3);

	SafeReleaseNoCheck(m_ZBuffer);

	SafeReleaseNoCheck(m_FrontBuffer);

	SafeReleaseNoCheck(m_D3DObject);

	SafeRelease(m_DDDevice);

    return( DD_OK );
}

HRESULT CALLBACK D3DEnumPixelFormatsCallback(  LPDDPIXELFORMAT lpDDPixFmt,  
  LPVOID          lpContext    )
{
    if(lpDDPixFmt->dwFlags == DDPF_ZBUFFER)
	{
		memcpy(&((mmDisplay *)lpContext)->m_ZBufferFormat,lpDDPixFmt,sizeof(DDPIXELFORMAT));
		if( lpDDPixFmt->dwZBufferBitDepth == 16 )
			return D3DENUMRET_CANCEL;
	}
	return(D3DENUMRET_OK);
}

HRESULT mmDisplay::CreateZBuffer(LPDIRECT3D3 lpD3D)
{

    DDSURFACEDESC2 ddsd;
#ifdef LOG
	theApp.LogMessage("Start ZEnum");
#endif

	lpD3D->EnumZBufferFormats(IID_IDirect3DHALDevice,D3DEnumPixelFormatsCallback,this);
#ifdef LOG
	theApp.LogMessage("ZEnum Complete");
#endif

    memset(&ddsd, 0 ,sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS |DDSD_PIXELFORMAT;

    if( sizeof(DDPIXELFORMAT) != m_ZBufferFormat.dwSize )
	{
		theApp.LogMessage("ZEnum failed");
	    ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	}


	if(!m_WindowMode)
	{
		ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
	}
	else
	{
	    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
//	    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_SYSTEMMEMORY;
	}
    ddsd.dwHeight = m_Height;
    ddsd.dwWidth = m_Width;
	memcpy(&ddsd.ddpfPixelFormat,&m_ZBufferFormat,sizeof(DDPIXELFORMAT) );

#ifdef LOG
	theApp.LogMessage("ZBuffer Surface Create %X",(DWORD)m_DDDevice4);
#endif

    HRESULT err = m_DDDevice4->CreateSurface(&ddsd,&m_ZBuffer,NULL);
    if(err != DD_OK) 
    {
		theApp.ErrorMessage("%s - %s",ShowError(err),"Create Z ERROR");
		return( err );
    }

#ifdef LOG
	theApp.LogMessage("ZBuffer Surface Created");
#endif

    err =m_BackBuffer->AddAttachedSurface(m_ZBuffer);
    if(err != DD_OK) 
    {
		theApp.ErrorMessage("%s - %s",ShowError(err),"Error Attaching ZBuffer");
		return( err );
    }
#ifdef LOG
	theApp.LogMessage("ZBuffer Surface Attached");
#endif
	return( err );
}

// D3DStuff


HRESULT mmDisplay::InitRenderState()
{
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_ZBIAS,0);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, true);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_ZENABLE, true);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, true);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_LINEAR);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_LINEAR);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_DITHERENABLE, true);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, true);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_FOGENABLE, false);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_FOGCOLOR, RGB_MAKE(0,0,0));
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS,D3DTADDRESS_WRAP);
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_ANTIALIAS, false);
  	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_TRANSLUCENTSORTINDEPENDENT,false);
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_ANISOTROPY,50);
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,true);
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,true);
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_ALPHAREF,0);
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_GREATER);
	m_D3DDevice2->SetRenderState(D3DRENDERSTATE_ZBIAS,0);
    m_D3DDevice2->SetLightState(D3DLIGHTSTATE_FOGMODE, D3DFOG_NONE);

    return D3D_OK;
}


HRESULT mmDisplay::SetAmbientLight(D3DCOLOR AmbientColor)
{
	m_AmbientColor=AmbientColor;
	m_D3DDevice2->SetLightState(D3DLIGHTSTATE_AMBIENT,AmbientColor);
    return D3D_OK;
}

HRESULT mmDisplay::SetFogMode(BOOL Enable,D3DFOGMODE Mode,float Start,float End,D3DCOLOR FogColor)
{
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, Mode);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_FOGTABLESTART, *(unsigned long*)&Start);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_FOGTABLEEND, *(unsigned long*)&End);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_FOGENABLE, Enable);
    m_D3DDevice2->SetRenderState(D3DRENDERSTATE_FOGCOLOR, FogColor);
	return(D3D_OK);
}

HRESULT mmDisplay::CreateViewport(WORD Width, WORD Height)
{
	HRESULT rval;
    D3DMATERIAL MaterialDesc;
    D3DMATERIALHANDLE BackgroundHandle;
	m_ViewWidth=Width;
	m_ViewHeight=Height;

	if(m_Viewport!=NULL)
	{
		m_D3DDevice2->DeleteViewport(m_Viewport);
		SafeDelete(m_Viewport);
	}
    rval = m_D3DObject->CreateViewport(&m_Viewport, NULL);
    if (rval != D3D_OK) 
	{
		return( rval );
    }

	if(m_Viewport==NULL) 
	{
		return(rval);
	}

    rval = m_D3DDevice2->AddViewport(m_Viewport);
    if (rval != D3D_OK) 
	{
		return( rval );
    }

    D3DVIEWPORT2 viewData;
    memset(&viewData, 0, sizeof(D3DVIEWPORT2));
    viewData.dwSize = sizeof(D3DVIEWPORT2);
    viewData.dwX = 0;
	viewData.dwY = 0;
    viewData.dwWidth = Width;
    viewData.dwHeight = Height;
	viewData.dvClipWidth  =  2.f;
	viewData.dvClipHeight =  2.f;
	viewData.dvClipX  =  -1.f;
	viewData.dvClipY  =  1.f;
	viewData.dvMinZ  =  -1.f;
	viewData.dvMaxZ  =  1.f;

	m_Viewport->SetViewport2(&viewData);
	m_D3DDevice2->SetCurrentViewport(m_Viewport);
                                       
    rval=m_D3DObject->CreateMaterial(&m_BackgroundMaterial, NULL);
	if(rval!=D3D_OK)
		return(rval);

    memset(&MaterialDesc, 0, sizeof(D3DMATERIAL));
    MaterialDesc.dwSize = sizeof(D3DMATERIAL);
	MaterialDesc.ambient.r = 0.0f;
	MaterialDesc.ambient.g = 0.0f;
	MaterialDesc.ambient.b = 0.0f;
	MaterialDesc.ambient.a = 1.0f;
	MaterialDesc.specular.r = 0.0f;
	MaterialDesc.specular.g = 0.0f;
	MaterialDesc.specular.b = 0.0f;
	MaterialDesc.specular.a = 1.0f;
	MaterialDesc.emissive.r = 0.0f;
	MaterialDesc.emissive.g = 0.0f;
	MaterialDesc.emissive.b = 0.0f;
	MaterialDesc.emissive.a = 1.0f;
	MaterialDesc.diffuse.r = 0.0f;
	MaterialDesc.diffuse.g = 0.0f;
	MaterialDesc.diffuse.b = 0.0f;
	MaterialDesc.diffuse.a = 1.0f;
    MaterialDesc.dwRampSize = 1;
    m_BackgroundMaterial->SetMaterial(&MaterialDesc);
    m_BackgroundMaterial->GetHandle(m_D3DDevice3,&BackgroundHandle);
    m_Viewport->SetBackground(BackgroundHandle);

	m_D3DDevice2->SetTransform(D3DTRANSFORMSTATE_PROJECTION,&m_Projection);
	m_D3DDevice2->SetTransform(D3DTRANSFORMSTATE_VIEW,&m_View);
	m_D3DDevice2->SetTransform(D3DTRANSFORMSTATE_WORLD,&m_World);

	return( rval );
 }


BOOL mmDisplay::RenderScene()
{
//	if(!m_SceneReady) return false;
/*
	if(theApp.m_BypassGFX)
	{
		theApp.m_BypassGFX=false;
		return true;
	}
*/
	m_FrameCount++;
	if((!m_WindowMode)&&theApp.m_ShowFPS)
	{
		{
			HDC dc;
			m_BackBuffer->GetDC(&dc);
			float fps,hz;
			CurTime=timeGetTime();
			DWORD deltatime=CurTime-StartTime;
			fps=(float)m_FrameCount/((float)(CurTime-StartTime)/1000.f);
			hz=(float)dynaVCount/((float)(CurTime-StartTime)/1000.f);
			if(deltatime>2000) 
			{
				StartTime=CurTime;
				m_FrameCount=0;
				dynaVCount=0;
//				iCpuResetVSYNC=true;
			}


			char text[256];
//			sprintf(text,"%3.2f-Hz  %3.2f-fps Verts: %06d Tris: %06d Dlists: %04d Culled: %02d DMA: %08d NewTex %d",hz,fps,hleRdpNumVerts,hleRdpNumTris,hleRdpDlists,hleRdpDlistsCulled,hleDMABytes,hleRdpTexturesPerFrame);
			ExtTextOut(dc,0,0, ETO_CLIPPED, NULL, text, strlen(text),NULL); 
			m_BackBuffer->ReleaseDC(dc);
		}
/*
*/
		m_FrontBuffer->Flip(NULL,DDFLIP_WAIT);
	}
	else
	{
		RECT Front,Back;
		Front.left=0; 
		Front.right=m_Width;
		Front.top=0; 
		Front.bottom=m_Height;
		Back=Front;
		CPoint Origin;
		Origin.x=0;
		Origin.y=0;
		GetClientRect(m_hWnd,&Front);
		ClientToScreen(m_hWnd,&Origin);
		Front.left=Origin.x;
		Front.top=Origin.y;
		Front.right+=Front.left;
		Front.bottom+=Front.top;

/*
*/
		if(theApp.m_ShowFPS)
		{
			HDC dc;
			m_BackBuffer->GetDC(&dc);
			float fps,hz;
			CurTime=timeGetTime();
			DWORD deltatime=CurTime-StartTime;
			fps=(float)m_FrameCount/((float)(CurTime-StartTime)/1000.f);
			hz=(float)dynaVCount/((float)(CurTime-StartTime)/1000.f);
			if(deltatime>2000) 
			{
				StartTime=CurTime;
				m_FrameCount=0;
				dynaVCount=0;
//				iCpuResetVSYNC=true;
			}

			char text[256];
//			sprintf(text,"%3.2f-Hz  %3.2f-fps",hz,fps);
//			sprintf(text,"%3.2f-Hz  %3.2f-fps Verts: %06d Tris: %06d Dlists: %04d Culled: %02d DMA: %08d NewTex %d",hz,fps,hleRdpNumVerts,hleRdpNumTris,hleRdpDlists,hleRdpDlistsCulled,hleDMABytes,hleRdpTexturesPerFrame);
			ExtTextOut(dc,0,0, ETO_CLIPPED, NULL, text, strlen(text),NULL); 
			m_BackBuffer->ReleaseDC(dc);
		}

		HRESULT Lasterr =m_FrontBuffer->Blt(&Front,m_BackBuffer,&Back,DDBLT_WAIT,NULL);
		if(Lasterr==DDERR_SURFACELOST)
		{
    		m_FrontBuffer->Restore();
    		m_BackBuffer->Restore();
		}
	}
	m_SceneReady=false;
	return true;
}

BOOL mmDisplay::RenderRect(RECT *src)
{
	if(!m_SceneReady) return false;
	if((!m_WindowMode)&&theApp.m_ShowFPS)
	{
		{
			HDC dc;
			m_BackBuffer->GetDC(&dc);
			float fps,hz;
			CurTime=timeGetTime();
			DWORD deltatime=CurTime-StartTime;
			fps=(float)m_FrameCount/((float)(CurTime-StartTime)/1000.f);
			hz=(float)dynaVCount/((float)(CurTime-StartTime)/1000.f);
			if(deltatime>2000) 
			{
				StartTime=CurTime;
				m_FrameCount=0;
				dynaVCount=0;
			}

			char text[256];
			sprintf(text,"%3.2f-Hz  %3.2f-fps",hz,fps);
			ExtTextOut(dc,0,0, ETO_CLIPPED, NULL, text, strlen(text),NULL); 
			m_BackBuffer->ReleaseDC(dc);
		}
/*
*/
		m_FrontBuffer->Flip(NULL,DDFLIP_WAIT);
	}
	else
	{
		RECT Front,Back;
		Front.left=0; 
		Front.right=m_Width;
		Front.top=0; 
		Front.bottom=m_Height;
		Back=Front;
		Back=*src;
		CPoint Origin;
		Origin.x=0;
		Origin.y=0;
		GetClientRect(m_hWnd,&Front);
		ClientToScreen(m_hWnd,&Origin);
		Front.left=Origin.x;
		Front.top=Origin.y;
		Front.right+=Front.left;
		Front.bottom+=Front.top;


		HRESULT Lasterr =m_FrontBuffer->Blt(&Front,m_BackBuffer,&Back,DDBLT_WAIT,NULL);
		if(Lasterr==DDERR_SURFACELOST)
		{
    		m_FrontBuffer->Restore();
    		m_BackBuffer->Restore();
		}
	}
	m_SceneReady=false;
	return true;
}

void mmDisplay::ClearScreen()
{

}

BOOL mmDisplay::SetCamera(
								 D3DVECTOR *Position,
								 D3DVECTOR *Orientation)
{
	m_View=m_Identity;
	m_View._41=Position->x;
	m_View._42=Position->y;
	m_View._43=Position->z;


	if(Orientation->y!=D3DVAL(0.0))
		ConcatenateYRotation(&m_View,Orientation->y);
	if(Orientation->x!=D3DVAL(0.0))
		ConcatenateXRotation(&m_View,Orientation->x);
	if(Orientation->z!=D3DVAL(0.0))
		ConcatenateZRotation(&m_View,Orientation->z);
//	m_View._11*=25.f;
//	m_View._22*=25.f;
//	m_View._33*=25.f;
//	m_View._33*=.15f;
//y,x,z???.. 4/21/98 - yes, this appears correct.  Could have something to do with the fact
// that our camera is not at the center of the boat, and this helps compensate.  Need to fix
// before the release.  jrb
//    if (m_D3DDevice->SetMatrix(m_hView, &m_View) != D3D_OK)
//        return false;


    if (SetTransform(D3DTRANSFORMSTATE_VIEW,&m_View) != D3D_OK)
        return false;

	return( true );
}


HRESULT CALLBACK enumTextureFormatsFunc(LPDDSURFACEDESC lpDdsd,LPVOID lpUserArg)
{
	LPVOID asdf=lpUserArg;

	((mmDisplay *)lpUserArg)->m_TextureFormats =(DDPIXELFORMAT *)realloc(((mmDisplay *)lpUserArg)->m_TextureFormats,sizeof(DDPIXELFORMAT)* (((mmDisplay *)lpUserArg)->m_NumTextureFormats+1));
	memcpy((char *)&((mmDisplay *)lpUserArg)->m_TextureFormats[((mmDisplay *)lpUserArg)->m_NumTextureFormats],(char *)&lpDdsd->ddpfPixelFormat,sizeof(DDPIXELFORMAT));
	((mmDisplay *)lpUserArg)->m_NumTextureFormats++;

	return(D3DENUMRET_OK);
}

void mmDisplay::SetFieldOfView(float FOV)
{
	float FOV2;
	float zn,zf,a;
	FOV2=FOV/2.f;
	m_FOV=FOV;
	zn=.5f;
	zf=400.f;
	a=(float)sin(FOV2)/(1.f-zn/zf);
	m_Projection._11=(float)cos(FOV2);
	m_Projection._12=m_Projection._13=m_Projection._14=0.f;
	m_Projection._22=(float)cos(FOV2);
	m_Projection._21=m_Projection._23=m_Projection._24=0.f;
	m_Projection._31=m_Projection._32=0.f;
	m_Projection._33=(float)sin(FOV2)/(1.f-zn/zf);
	m_Projection._34=(float)sin(FOV2);
	m_Projection._41=m_Projection._42=m_Projection._44=0.f;
	m_Projection._43=-a*zn;
}


HRESULT mmDisplay::SetTransform(D3DTRANSFORMSTATETYPE dtstTransformStateType,  
  LPD3DMATRIX lpD3DMatrix)
{

	return(m_D3DDevice2->SetTransform(dtstTransformStateType,lpD3DMatrix));

/*
	switch(dtstTransformStateType)
	{
	case D3DTRANSFORMSTATE_WORLD:
		{
			m_WorldTransform=*lpD3DMatrix;
			m_WorldDirty=true;
			MultiplyD3DMATRIX(&m_WorldViewProj,&m_WorldTransform,&m_WorldView);
			break;
		}
	case D3DTRANSFORMSTATE_VIEW:
		{
			m_ViewTransform=*lpD3DMatrix;
			MultiplyD3DMATRIX(&m_WorldView,&m_ViewTransform,&m_Projection);
			break;
		}
	case D3DTRANSFORMSTATE_PROJECTION:
		{
			m_ProjectionTransform=*lpD3DMatrix;
			m_ProjectionDirty=true;
			break;
		}
	}
	return(D3D_OK);
*/
}

HRESULT mmDisplay::SetRenderState(D3DRENDERSTATETYPE RenderStateType,DWORD RenderState)
{
	if(m_RenderStates[RenderStateType&0xff]==RenderState)
		return(0);
	m_RenderStates[RenderStateType&0xff]=RenderState;
	return(m_D3DDevice2->SetRenderState(RenderStateType,RenderState));
}

HRESULT mmDisplay::SetLightState(D3DLIGHTSTATETYPE LightStateType,DWORD LightState)
{
	if(m_LightStates[LightStateType&0xff]==LightState)
		return(0);
	m_LightStates[LightStateType&0xff]=LightState;
	return(m_D3DDevice3->SetLightState(LightStateType,LightState));
}

void mmDisplay::BufferRenderState(D3DRENDERSTATETYPE RenderStateType,DWORD RenderState)
{
	if(m_RenderStates[RenderStateType&0xff]==RenderState)
		return;
	m_RenderStates[RenderStateType&0xff]=RenderState;
	m_RenderStatesDirty[RenderStateType&0xff]=true;
}

void mmDisplay::BufferLightState(D3DLIGHTSTATETYPE LightStateType,DWORD LightState)
{
	if(m_LightStates[LightStateType&0xff]==LightState)
		return;
	m_LightStates[LightStateType&0xff]=LightState;
	m_LightStatesDirty[LightStateType&0xff]=true;
}

void mmDisplay::FlushRenderStates()
{
	for(DWORD i=0;i<255;i++)
	{
		if(m_RenderStatesDirty[i])
		{
			m_D3DDevice2->SetRenderState((D3DRENDERSTATETYPE) i,m_RenderStates[i]);
			m_RenderStatesDirty[i]=false;
		}
	}
}

void mmDisplay::FlushLightStates()
{
	for(DWORD i=0;i<255;i++)
	{
		if(m_LightStatesDirty[i])
		{
			m_D3DDevice2->SetLightState((D3DLIGHTSTATETYPE) i,m_LightStates[i]);
			m_LightStatesDirty[i]=false;
		}
	}
}

void mmDisplay::ClearRenderStates()
{
	for(DWORD i=0;i<255;i++)
	{
		m_RenderStatesDirty[i]=false;
		m_RenderStates[i]=0xffff;
	}
}

void mmDisplay::ClearLightStates()
{
	for(DWORD i=0;i<255;i++)
	{
		m_LightStatesDirty[i]=false;
		m_LightStates[i]=0xffff;
	}
}

BOOL mmDisplay::IsVisible(D3DVERTEX *src)
{
	return(true);
/*
	D3DTLVERTEX *dst=m_TmpVertices;
	DWORD ClipStatus,AllClip;
	AllClip=0xffff;
	for(DWORD i=0;i<8;i++)
	{
		VectorMatrixMultiply(dst,src,&m_WorldViewProj);
		ClipStatus=0;
		float wp=dst->rhw;
		if(dst->sx<=-wp)
			ClipStatus+=4;
		if(dst->sx>wp)
			ClipStatus+=8;
		if(dst->sy<=-wp)
			ClipStatus+=32;
		if(dst->sy>wp)
			ClipStatus+=16;
		if(dst->sz<=0.f)
			ClipStatus+=1;
		if(dst->sz>wp)
			ClipStatus+=2;
		AllClip&=ClipStatus;
		src++;
		dst++;
	}
	return(AllClip==0);
*/
}

void mmDisplay::BeginScene()
{
	if(!m_InScene)
	{
		m_InScene=true;
		m_D3DDevice2->BeginScene();
	}
}

void mmDisplay::EndScene()
{
	if(m_InScene)
	{
		m_InScene=false;
		m_D3DDevice2->EndScene();
	}
}


char *mmDisplay::ShowError(HRESULT hResult)
{
    LPTSTR lpszError;
    char  szMsg[256];

    switch (hResult)
    {
    case DD_OK:
        // The request completed successfully.
        lpszError = TEXT("DD_OK");
        break;

    case DDERR_ALREADYINITIALIZED:
        // The object has already been initialized.
        lpszError = TEXT("DDERR_ALREADYINITIALIZED");
        break;

    case DDERR_BLTFASTCANTCLIP:
        // A DirectDrawClipper object is attached to a source surface 
        // that has passed into a call to the IDirectDrawSurface2::BltFast method. 
        lpszError = TEXT("DDERR_BLTFASTCANTCLIP");
        break;

    case DDERR_CANNOTATTACHSURFACE:
        // A surface cannot be attached to another requested surface. 
        lpszError = TEXT("DDERR_CANNOTATTACHSURFACE");
        break;

    case DDERR_CANNOTDETACHSURFACE:
        // A surface cannot be detached from another requested surface. 
        lpszError = TEXT("DDERR_CANNOTDETACHSURFACE");
        break;

    case DDERR_CANTCREATEDC:
        // Windows cannot create any more device contexts (DCs). 
        lpszError = TEXT("DDERR_CANTCREATEDC");
        break;

    case DDERR_CANTDUPLICATE:
        // Primary and 3D surfaces, or surfaces that are 
        // implicitly created, cannot be duplicated. 
        lpszError = TEXT("DDERR_CANTDUPLICATE");
        break;

    case DDERR_CANTLOCKSURFACE:
        // Access to this surface is refused because an 
        // attempt was made to lock the primary surface without DCI support. 
        lpszError = TEXT("DDERR_CANTLOCKSURFACE"); 
        break;

    case DDERR_CANTPAGELOCK:
        // An attempt to page lock a surface failed. 
        // Page lock will not work on a display-memory 
        // surface or an emulated primary surface.
        lpszError = TEXT("DDERR_CANTPAGELOCK"); 
        break;

    case DDERR_CANTPAGEUNLOCK:
        // An attempt to page unlock a surface failed. 
        // Page unlock will not work on a display-memory 
        // surface or an emulated primary surface. 
        lpszError = TEXT("DDERR_CANTPAGEUNLOCK");
        break;

    case DDERR_CLIPPERISUSINGHWND:
        // An attempt was made to set a clip list for a DirectDrawClipper 
        // object that is already monitoring a window handle. 
        lpszError = TEXT("DDERR_CLIPPERISUSINGHWND");
        break;

    case DDERR_COLORKEYNOTSET:
        // No source color key is specified for this operation
        lpszError = TEXT("DDERR_COLORKEYNOTSET");
        break;

    case DDERR_CURRENTLYNOTAVAIL:
        // No support is currently available. 
        lpszError = TEXT("DDERR_CURRENTLYNOTAVAIL");
        break;

    case DDERR_DCALREADYCREATED:
        // A device context (DC) has already been returned for this surface. 
        // Only one DC can be retrieved for each surface. 
        lpszError = TEXT("DDERR_DCALREADYCREATED");
        break;

    case DDERR_DIRECTDRAWALREADYCREATED:
        // A DirectDraw object representing this driver 
        // has already been created for this process. 
        lpszError = TEXT("DDERR_DIRECTDRAWALREADYCREATED");
        break;

    case DDERR_EXCEPTION:
        // An exception was encountered while 
        // performing the requested operation. 
        lpszError = TEXT("DDERR_EXCEPTION");
        break;

    case DDERR_EXCLUSIVEMODEALREADYSET:
        // An attempt was made to set the cooperative 
        // level when it was already set to exclusive. 
        lpszError = TEXT("DDERR_EXCLUSIVEMODEALREADYSET");
        break;

    case DDERR_GENERIC:
        // There is an undefined error condition. 
        lpszError = TEXT("DDERR_GENERIC");
        break;

    case DDERR_HEIGHTALIGN:
        // The height of the provided rectangle 
        // is not a multiple of the required alignment. 
        lpszError = TEXT("DDERR_HEIGHTALIGN");
        break;

    case DDERR_HWNDALREADYSET:
        // The DirectDraw cooperative level window 
        // handle has already been set. It cannot 
        // be reset while the process has surfaces or palettes created. 
        lpszError = TEXT("DDERR_HWNDALREADYSET");
        break;

    case DDERR_HWNDSUBCLASSED:
        // DirectDraw is prevented from restoring state because the
        // DirectDraw cooperative level window handle has been subclassed. 
        lpszError = TEXT("DDERR_HWNDSUBCLASSED");
        break;

    case DDERR_IMPLICITLYCREATED:
        // The surface cannot be restored because 
        // it is an implicitly created surface. 
        lpszError = TEXT("DDERR_IMPLICITLYCREATED");
        break;
 
    case DDERR_INCOMPATIBLEPRIMARY:
        // The primary surface creation request 
        // does not match with the existing primary surface. 
        lpszError = TEXT("DDERR_INCOMPATIBLEPRIMARY");
        break;

    case DDERR_INVALIDCAPS:
        // One or more of the capability bits 
        // passed to the callback function are incorrect. 
        lpszError = TEXT("DDERR_INVALIDCAPS");
        break;

    case DDERR_INVALIDCLIPLIST:
        // DirectDraw does not support the provided clip list.  
        lpszError = TEXT("DDERR_INVALIDCLIPLIST");
        break;

    case DDERR_INVALIDDIRECTDRAWGUID:
        // The globally unique identifier (GUID) passed to the
        // DirectDrawCreate function is not a valid DirectDraw driver identifier. 
        lpszError = TEXT("DDERR_INVALIDDIRECTDRAWGUID");
        break;

    case DDERR_INVALIDMODE:
        // DirectDraw does not support the requested mode. 
        lpszError = TEXT("DDERR_INVALIDMODE");
        break;

    case DDERR_INVALIDOBJECT:
        // DirectDraw received a pointer that was an invalid DirectDraw object. 
        lpszError = TEXT("DDERR_INVALIDOBJECT");
        break;

    case DDERR_INVALIDPARAMS:
        // One or more of the parameters passed to the method are incorrect. 
        lpszError = TEXT("DDERR_INVALIDPARAMS");
        break;

    case DDERR_INVALIDPIXELFORMAT:
        // The pixel format was invalid as specified. 
        lpszError = TEXT("DDERR_INVALIDPIXELFORMAT");
        break;

    case DDERR_INVALIDPOSITION:
        // The position of the overlay on the destination is no longer legal. 
        lpszError = TEXT("DDERR_INVALIDPOSITION");
        break;

    case DDERR_INVALIDRECT:
        // The provided rectangle was invalid. 
        lpszError = TEXT("DDERR_INVALIDRECT");
        break;

    case DDERR_INVALIDSURFACETYPE:
        // The requested operation could not be performed
        // because the surface was of the wrong type. 
        lpszError = TEXT("DDERR_INVALIDSURFACETYPE");
        break;

    case DDERR_LOCKEDSURFACES:
        // One or more surfaces are locked, 
        // causing the failure of the requested operation. 
        lpszError = TEXT("DDERR_LOCKEDSURFACES");
        break;

    case DDERR_MOREDATA:
        // There is more data available than the specified 
        // buffer size could hold.
        lpszError = TEXT("DDERR_MOREDATA");
        break;

    case DDERR_NO3D:
        // No 3D hardware or emulation is present. 
        lpszError = TEXT("DDERR_NO3D");
        break;

    case DDERR_NOALPHAHW:
        // No alpha acceleration hardware is present or available, 
        // causing the failure of the requested operation. 
        lpszError = TEXT("DDERR_NOALPHAHW");
        break;

    case DDERR_NOBLTHW:
        // No blitter hardware is present. 
        lpszError = TEXT("DDERR_NOBLTHW");
        break;

    case DDERR_NOCLIPLIST:
        // No clip list is available. 
        lpszError = TEXT("DDERR_NOCLIPLIST");
        break;

    case DDERR_NOCLIPPERATTACHED:
        // No DirectDrawClipper object is attached to the surface object. 
        lpszError = TEXT("DDERR_NOCLIPPERATTACHED");
        break;

    case DDERR_NOCOLORCONVHW:
        // The operation cannot be carried out because 
        // no color-conversion hardware is present or available. 
        lpszError = TEXT("DDERR_NOCOLORCONVHW");
        break;

    case DDERR_NOCOLORKEY:
        // The surface does not currently have a color key. 
        lpszError = TEXT("DDERR_NOCOLORKEY");
        break;

    case DDERR_NOCOLORKEYHW:
        // The operation cannot be carried out because there 
        // is no hardware support for the destination color key. 
        lpszError = TEXT("DDERR_NOCOLORKEYHW");
        break;

    case DDERR_NOCOOPERATIVELEVELSET:
        // A create function is called without the 
        // IDirectDraw2::SetCooperativeLevel method being called. 
        lpszError = TEXT("DDERR_NOCOOPERATIVELEVELSET");
        break;

    case DDERR_NODC:
        // No DC has ever been created for this surface. 
        lpszError = TEXT("DDERR_NODC");
        break;

    case DDERR_NODDROPSHW:
        // No DirectDraw raster operation (ROP) hardware is available. 
        lpszError = TEXT("DDERR_NODDROPSHW");
        break;

    case DDERR_NODIRECTDRAWHW:
        // Hardware-only DirectDraw object creation is not possible; 
        // the driver does not support any hardware. 
        lpszError = TEXT("DDERR_NODIRECTDRAWHW");
        break;

    case DDERR_NODIRECTDRAWSUPPORT:
        // DirectDraw support is not possible with the current display driver. 
        lpszError = TEXT("DDERR_NODIRECTDRAWSUPPORT");
        break;

    case DDERR_NOEMULATION: 
        // Software emulation is not available. 
        lpszError = TEXT("DDERR_NOEMULATION");
        break;

    case DDERR_NOEXCLUSIVEMODE:
        // The operation requires the application to have 
        // exclusive mode, but the application does not have exclusive mode. 
        lpszError = TEXT("DDERR_NOEXCLUSIVEMODE");
        break;

    case DDERR_NOFLIPHW: 
        // Flipping visible surfaces is not supported. 
        lpszError = TEXT("DDERR_NOFLIPHW");
        break;

    case DDERR_NOGDI: 
        // No GDI is present. 
        lpszError = TEXT("DDERR_NOGDI");
        break;

    case DDERR_NOHWND: 
        // Clipper notification requires a window handle, 
        // or no window handle has been previously set 
        // as the cooperative level window handle. 
        lpszError = TEXT("DDERR_NOHWND");
        break;

    case DDERR_NOMIPMAPHW: 
        // The operation cannot be carried out because no 
        // mipmap texture mapping hardware is present or available. 
        lpszError = TEXT("DDERR_NOMIPMAPHW");
        break;

    case DDERR_NOMIRRORHW: 
        // The operation cannot be carried out because 
        // no mirroring hardware is present or available. 
        lpszError = TEXT("DDERR_NOMIRRORHW");
        break;

    case DDERR_NONONLOCALVIDMEM: 
        // An attempt was made to allocate non-local video memory
        // from a device that does not support non-local video memory.
        lpszError = TEXT("DDERR_NONONLOCALVIDMEM");
        break;
 
    case DDERR_NOOVERLAYDEST: 
        // The IDirectDrawSurface2::GetOverlayPosition method 
        // is called on an overlay that the IDirectDrawSurface2::UpdateOverlay 
        // method has not been called on to establish a destination. 
        lpszError = TEXT("DDERR_NOOVERLAYDEST");
        break;

    case DDERR_NOOVERLAYHW: 
        // The operation cannot be carried out because 
        // no overlay hardware is present or available. 
        lpszError = TEXT("DDERR_NOOVERLAYHW");
        break;

    case DDERR_NOPALETTEATTACHED: 
        // No palette object is attached to this surface. 
        lpszError = TEXT("DDERR_NOPALETTEATTACHED");
        break;

    case DDERR_NOPALETTEHW: 
        // There is no hardware support for 16- or 256-color palettes. 
        lpszError = TEXT("DDERR_NOPALETTEHW");
        break;

    case DDERR_NORASTEROPHW: 
        // The operation cannot be carried out because 
        // no appropriate raster operation hardware is present or available. 
        lpszError = TEXT("DDERR_NORASTEROPHW");
        break;

    case DDERR_NOROTATIONHW: 
        // The operation cannot be carried out because 
        // no rotation hardware is present or available. 
        lpszError = TEXT("DDERR_NOROTATIONHW");
        break;

    case DDERR_NOSTRETCHHW: 
        // The operation cannot be carried out because 
        // there is no hardware support for stretching. 
        lpszError = TEXT("DDERR_NOSTRETCHHW");
        break;

    case DDERR_NOT4BITCOLOR: 
        // The DirectDrawSurface object is not using a 
        // 4-bit color palette and the requested operation 
        // requires a 4-bit color palette. 
        lpszError = TEXT("DDERR_NOT4BITCOLOR");
        break;

    case DDERR_NOT4BITCOLORINDEX: 
        // The DirectDrawSurface object is not using a 4-bit 
        // color index palette and the requested operation 
        // requires a 4-bit color index palette. 
        lpszError = TEXT("DDERR_NOT4BITCOLORINDEX");
        break;

    case DDERR_NOT8BITCOLOR: 
        // The DirectDrawSurface object is not using an 8-bit 
        // color palette and the requested operation requires 
        // an 8-bit color palette. 
        lpszError = TEXT("DDERR_NOT8BITCOLOR");
        break;

    case DDERR_NOTAOVERLAYSURFACE: 
        // An overlay component is called for a non-overlay surface. 
        lpszError = TEXT("DDERR_NOTAOVERLAYSURFACE");
        break;

    case DDERR_NOTEXTUREHW: 
        // The operation cannot be carried out because no 
        // texture-mapping hardware is present or available. 
        lpszError = TEXT("DDERR_NOTEXTUREHW");
        break;

    case DDERR_NOTFLIPPABLE: 
        // An attempt has been made to flip a surface that cannot be flipped. 
        lpszError = TEXT("DDERR_NOTFLIPPABLE");
        break;

    case DDERR_NOTFOUND: 
        // The requested item was not found. 
        lpszError = TEXT("DDERR_NOTFOUND");
        break;

    case DDERR_NOTINITIALIZED: 
        // An attempt was made to call an interface method of a DirectDraw object 
        // created by CoCreateInstance before the object was initialized. 
        lpszError = TEXT("DDERR_NOTINITIALIZED");
        break;

    case DDERR_NOTLOCKED: 
        // An attempt is made to unlock a surface that was not locked. 
        lpszError = TEXT("DDERR_NOTLOCKED");
        break;

    case DDERR_NOTPAGELOCKED: 
        // An attempt is made to page unlock a surface 
        // with no outstanding page locks. 
        lpszError = TEXT("DDERR_NOTPAGELOCKED");
        break;

    case DDERR_NOTPALETTIZED: 
        // The surface being used is not a palette-based surface. 
        lpszError = TEXT("DDERR_NOTPALETTIZED");
        break;

    case DDERR_NOVSYNCHW: 
        // The operation cannot be carried out because 
        // there is no hardware support for vertical blank synchronized operations. 
        lpszError = TEXT("DDERR_NOVSYNCHW");
        break;

    case DDERR_NOZBUFFERHW: 
        // The operation to create a z-buffer in display memory 
        // or to perform a blit using a z-buffer cannot be carried 
        // out because there is no hardware support for z-buffers. 
        lpszError = TEXT("DDERR_NOZBUFFERHW");
        break;

    case DDERR_NOZOVERLAYHW: 
        // The overlay surfaces cannot be z-layered based 
        // on the z-order because the hardware does not 
        // support z-ordering of overlays. 
        lpszError = TEXT("DDERR_NOZOVERLAYHW");
        break;

    case DDERR_OUTOFCAPS: 
        // The hardware needed for the requested operation has already been allocated. 
        lpszError = TEXT("DDERR_OUTOFCAPS");
        break;

    case DDERR_OUTOFMEMORY: 
        // DirectDraw does not have enough memory to perform the operation. 
        lpszError = TEXT("DDERR_OUTOFMEMORY");
        break;

    case DDERR_OUTOFVIDEOMEMORY: 
        // DirectDraw does not have enough display memory to perform the operation. 
        lpszError = TEXT("DDERR_OUTOFVIDEOMEMORY");
        break;

    case DDERR_OVERLAYCANTCLIP: 
        // The hardware does not support clipped overlays. 
        lpszError = TEXT("DDERR_OVERLAYCANTCLIP");
        break;

    case DDERR_OVERLAYCOLORKEYONLYONEACTIVE: 
        // An attempt was made to have more than one color key active on an overlay. 
        lpszError = TEXT("DDERR_OVERLAYCOLORKEYONLYONEACTIVE");
        break;

    case DDERR_OVERLAYNOTVISIBLE: 
        // The IDirectDrawSurface2::GetOverlayPosition method is called on a hidden overlay. 
        lpszError = TEXT("DDERR_OVERLAYNOTVISIBLE");
        break;

    case DDERR_PALETTEBUSY: 
        // Access to this palette is refused 
        // because the palette is locked by another thread. 
        lpszError = TEXT("DDERR_PALETTEBUSY");
        break;

    case DDERR_PRIMARYSURFACEALREADYEXISTS: 
        // This process has already created a primary surface. 
        lpszError = TEXT("DDERR_PRIMARYSURFACEALREADYEXISTS");
        break;

    case DDERR_REGIONTOOSMALL: 
        // The region passed to the 
        // IDirectDrawClipper::GetClipList method is too small. 
        lpszError = TEXT("DDERR_REGIONTOOSMALL");
        break;

    case DDERR_SURFACEALREADYATTACHED: 
        // An attempt was made to attach a surface to 
        // another surface to which it is already attached. 
        lpszError = TEXT("DDERR_SURFACEALREADYATTACHED");
        break;

    case DDERR_SURFACEALREADYDEPENDENT: 
        // An attempt was made to make a surface a dependency 
        // of another surface to which it is already dependent. 
        lpszError = TEXT("DDERR_SURFACEALREADYDEPENDENT");
        break;

    case DDERR_SURFACEBUSY: 
        // Access to the surface is refused because the 
        // surface is locked by another thread. 
        lpszError = TEXT("DDERR_SURFACEBUSY");
        break;

    case DDERR_SURFACEISOBSCURED: 
        // Access to the surface is refused 
        // because the surface is obscured. 
        lpszError = TEXT("DDERR_SURFACEISOBSCURED");
        break;

    case DDERR_SURFACELOST: 
        // Access to the surface is refused because the 
        // surface memory is gone. The DirectDrawSurface 
        // object representing this surface should have 
        // the IDirectDrawSurface2::Restore method called on it. 
        lpszError = TEXT("DDERR_SURFACELOST");
        break;

    case DDERR_SURFACENOTATTACHED: 
        // The requested surface is not attached. 
        lpszError = TEXT("DDERR_SURFACENOTATTACHED");
        break;

    case DDERR_TOOBIGHEIGHT: 
        // The height requested by DirectDraw is too large. 
        lpszError = TEXT("DDERR_TOOBIGHEIGHT");
        break;

    case DDERR_TOOBIGSIZE: 
        // The size requested by DirectDraw is too large. 
        // However, the individual height and width are OK.
        lpszError = TEXT("DDERR_TOOBIGSIZE");
        break;

    case DDERR_TOOBIGWIDTH: 
        // The width requested by DirectDraw is too large. 
        lpszError = TEXT("DDERR_TOOBIGWIDTH");
        break;

    case DDERR_UNSUPPORTED: 
        // The operation is not supported. 
        lpszError = TEXT("DDERR_UNSUPPORTED");
        break;

    case DDERR_UNSUPPORTEDFORMAT: 
        // The FourCC format requested is not supported by DirectDraw. 
        lpszError = TEXT("DDERR_UNSUPPORTEDFORMAT");
        break;

    case DDERR_UNSUPPORTEDMASK: 
        // The bitmask in the pixel format requested is not supported by DirectDraw. 
        lpszError = TEXT("DDERR_UNSUPPORTEDMASK");
        break;

    case DDERR_UNSUPPORTEDMODE: 
        // The display is currently in an unsupported mode. 
        lpszError = TEXT("DDERR_UNSUPPORTEDMODE");
        break;

    case DDERR_VERTICALBLANKINPROGRESS: 
        // A vertical blank is in progress. 
        lpszError = TEXT("DDERR_VERTICALBLANKINPROGRESS");
        break;

    case DDERR_WASSTILLDRAWING: 
        // The previous blit operation that is transferring 
        // information to or from this surface is incomplete. 
        lpszError = TEXT("DDERR_WASSTILLDRAWING");
        break;

    case DDERR_WRONGMODE:
        // This surface cannot be restored because it was created in a different mode. 
        lpszError = TEXT("DDERR_WRONGMODE");
        break;

    case DDERR_XALIGN:
        // The provided rectangle was not horizontally aligned on a required boundary. 
        lpszError = TEXT("DDERR_XALIGN");
        break;

    case DDERR_VIDEONOTACTIVE:
        // The video port is not active
        lpszError = TEXT("DDERR_VIDEONOTACTIVE");
        break;


        //
        // D3D Immediate Mode Errors
        //

    case D3DERR_BADMAJORVERSION:
        // ???
        lpszError = TEXT("D3DERR_BADMAJORVERSION");
        break;

    case D3DERR_BADMINORVERSION:
        // ???
        lpszError = TEXT("D3DERR_BADMINORVERSION");
        break;

    case D3DERR_INVALID_DEVICE:
        // ???
        lpszError = TEXT("D3DERR_INVALID_DEVICE");
        break;

    case D3DERR_INITFAILED:
        // ???
        lpszError = TEXT("D3DERR_INITFAILED");
        break;

    case D3DERR_EXECUTE_CREATE_FAILED:
        // ???
        lpszError = TEXT("D3DERR_EXECUTE_CREATE_FAILED");
        break;

    case D3DERR_EXECUTE_DESTROY_FAILED:
        // ???
        lpszError = TEXT("D3DERR_EXECUTE_DESTROY_FAILED");
        break;

    case D3DERR_EXECUTE_LOCK_FAILED:
        // ???
        lpszError = TEXT("D3DERR_EXECUTE_LOCK_FAILED");
        break;

    case D3DERR_EXECUTE_UNLOCK_FAILED:
        // ???
        lpszError = TEXT("D3DERR_EXECUTE_UNLOCK_FAILED");
        break;

    case D3DERR_EXECUTE_LOCKED:
        // ???
        lpszError = TEXT("D3DERR_EXECUTE_LOCKED");
        break;

    case D3DERR_EXECUTE_NOT_LOCKED:
        // ???
        lpszError = TEXT("D3DERR_EXECUTE_NOT_LOCKED");
        break;

    case D3DERR_EXECUTE_FAILED:
        // ???
        lpszError = TEXT("D3DERR_EXECUTE_FAILED");
        break;

    case D3DERR_EXECUTE_CLIPPED_FAILED:
        // ???
        lpszError = TEXT("D3DERR_CLIPPED_FAILED");
        break;

    case D3DERR_TEXTURE_NO_SUPPORT:
        // ???
        lpszError = TEXT("D3DERR_TEXTURE_NO_SUPPORT");
        break;

    case D3DERR_TEXTURE_CREATE_FAILED:
        // ???
        lpszError = TEXT("D3DERR_TEXTURE_CREATE_FAILED");
        break;

    case D3DERR_TEXTURE_DESTROY_FAILED:
        // ???
        lpszError = TEXT("D3DERR_TEXTURE_DESTROY_FAILED");
        break;

    case D3DERR_TEXTURE_LOCK_FAILED:
        // ???
        lpszError = TEXT("D3DERR_TEXTURE_LOCK_FAILED");
        break;

    case D3DERR_TEXTURE_UNLOCK_FAILED:
        // ???
        lpszError = TEXT("D3DERR_TEXTURE_UNLOCK_FAILED");
        break;

    case D3DERR_TEXTURE_LOAD_FAILED:
        // ???
        lpszError = TEXT("D3DERR_LOAD_FAILED");
        break;

    case D3DERR_TEXTURE_SWAP_FAILED:
        // ???
        lpszError = TEXT("D3DERR_SWAP_FAILED");
        break;

    case D3DERR_TEXTURE_LOCKED:
        // ???
        lpszError = TEXT("D3DERR_TEXTURE_LOCKED");
        break;

    case D3DERR_TEXTURE_NOT_LOCKED:
        // ???
        lpszError = TEXT("D3DERR_TEXTURE_NOT_LOCKED");
        break;

    case D3DERR_TEXTURE_GETSURF_FAILED:
        // ???
        lpszError = TEXT("D3DERR_TEXTURE_GETSURF_FAILED");
        break;

    case D3DERR_MATRIX_CREATE_FAILED:
        // ???
        lpszError = TEXT("D3DERR_MATRIX_CREATE_FAILED");
        break;

    case D3DERR_MATRIX_DESTROY_FAILED:
        // ???
        lpszError = TEXT("D3DERR_MATRIX_DESTROY_FAILED");
        break;

    case D3DERR_MATRIX_SETDATA_FAILED:
        // ???
        lpszError = TEXT("D3DERR_MATRIX_SETDATA_FAILED");
        break;

    case D3DERR_MATRIX_GETDATA_FAILED:
        // ???
        lpszError = TEXT("D3DERR_MAXTRIX_GETDATA_FAILED");
        break;

    case D3DERR_SETVIEWPORTDATA_FAILED:
        // ???
        lpszError = TEXT("D3DERR_SETVIEWPORTDATA_FAILED");
        break;

    case D3DERR_INVALIDCURRENTVIEWPORT:
        // ???
        lpszError = TEXT("D3DERR_INVALIDCURRENTVIEWPORT");
        break;
	
    case D3DERR_INVALIDPRIMITIVETYPE:
        // ???
        lpszError = TEXT("D3DERR_INVALIDPRIMITIVETYPE");
        break;

    case D3DERR_INVALIDVERTEXTYPE:
        // ???
        lpszError = TEXT("D3DERR_INVALIDVERTEXTYPE");
        break;

    case D3DERR_TEXTURE_BADSIZE:
        // ???
        lpszError = TEXT("D3DERR_TEXTURE_BADSIZE");
        break;

    case D3DERR_MATERIAL_CREATE_FAILED:
        // ???
        lpszError = TEXT("D3DERR_MATERIAL_CREATE_FAILED");
        break;

    case D3DERR_MATERIAL_DESTROY_FAILED:
        // ???
        lpszError = TEXT("D3DERR_MATERIAL_DESTROY_FAILED");
        break;

    case D3DERR_MATERIAL_SETDATA_FAILED:
        // ???
        lpszError = TEXT("D3DERR_MATERIAL_SETDATA_FAILED");
        break;

    case D3DERR_MATERIAL_GETDATA_FAILED:
        // ???
        lpszError = TEXT("D3DERR_MATERIAL_GETDATA_FAILED");
        break;

    case D3DERR_INVALIDPALETTE:
        // ???
        lpszError = TEXT("D3DERR_INVALIDPALETTE");
        break;

    case D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY:
        // ???
        lpszError = TEXT("D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY");
        break;

    case D3DERR_ZBUFF_NEEDS_VIDEOMEMORY:
        // ???
        lpszError = TEXT("D3DERR_ZBUFF_NEEDS_VIDEOMEMORY");
        break;

    case D3DERR_SURFACENOTINVIDMEM:
        // ???
        lpszError = TEXT("D3DERR_SURFACENOTINVIDMEM");
        break;

    case D3DERR_LIGHT_SET_FAILED:
        // ???
        lpszError = TEXT("D3DERR_LIGHT_SET_FAILED");
        break;

    case D3DERR_LIGHTHASVIEWPORT:
        // ???
        lpszError = TEXT("D3DERR_LIGHTHASVIEWPORT");
        break;

    case D3DERR_LIGHTNOTINTHISVIEWPORT:
        // ???
        lpszError = TEXT("D3DERR_LIGHTNOTINTHISVIEWPORT");
        break;

    case D3DERR_SCENE_IN_SCENE:
        // ???
        lpszError = TEXT("D3DERR_SCENE_IN_SCENE");
        break;

    case D3DERR_SCENE_NOT_IN_SCENE:
        // ???
        lpszError = TEXT("D3DERR_SCENE_NOT_IN_SCENE");
        break;

    case D3DERR_SCENE_BEGIN_FAILED:
        // ???
        lpszError = TEXT("D3DERR_SCENE_BEGIN_FAILED");
        break;

    case D3DERR_SCENE_END_FAILED:
        // ???
        lpszError = TEXT("D3DERR_SCENE_END_FAILED");
        break;

    case D3DERR_INBEGIN:
        // ???
        lpszError = TEXT("D3DERR_INBEGIN");
        break;

    case D3DERR_NOTINBEGIN:
        // ???
        lpszError = TEXT("D3DERR_NOTINBEGIN");
        break;

    case D3DERR_NOVIEWPORTS:
        // ???
        lpszError = TEXT("D3DERR_NOVIEWPORTS");
        break;

    case D3DERR_VIEWPORTDATANOTSET:
        // ???
        lpszError = TEXT("D3DERR_VIEWPORTDATANOTSET");
        break;

    case D3DERR_VIEWPORTHASNODEVICE:
        // ???
        lpszError = TEXT("D3DERR_VIEWPORTHASNODEVICE");
        break;


    default:
        // Unknown DD/D3D/App Error
        wsprintf (szMsg, "Unknown Error #%ld", (DWORD)(hResult & 0x0000FFFFL));
        lpszError = szMsg;
        break;
    }

	return(lpszError);
} 

void MakeColorSwap16BG()
{
	ColorSwap=(WORD *)malloc(0x8000*sizeof(WORD));
	WORD p,r,b,g;
	for(WORD i=0;i<0x8000;i++)
	{
		r=i&0x1f;
		g=i&0x3e0;
		b=i&0x7c00;
		if(theApp.m_ScanLines)
		{
			r+=3;
			if(r>0x1f) r=0x1f;
			g+=(3<<5);
			if(g>0x3e0) g=0x3e0;
			b+=(3<<10);
			if(b>0x7c00) b=0x7c00;
		}

		p=g<<1;
		p|=(r<<11);
		p|=(b>>10);
		ColorSwap[i]=p;
	}
}

void MakeColorSwap16()
{
	ColorSwap=(WORD *)malloc(0x8000*sizeof(WORD));
	WORD p,r,b,g;
	for(WORD i=0;i<0x8000;i++)
	{
		r=i&0x1f;
		g=i&0x3e0;
		b=i&0x7c00;
		if(theApp.m_ScanLines)
		{
			r+=3;
			if(r>0x1f) r=0x1f;
			g+=(3<<5);
			if(g>0x3e0) g=0x3e0;
			b+=(3<<10);
			if(b>0x7c00) b=0x7c00;
		}

		p=g;
		p|=(r<<10);
		p|=(b>>10);
		ColorSwap[i]=p|0x8000;
	}
}

void MakeColorSwap32()
{
	ColorSwapD=(DWORD *)malloc(0x8000*sizeof(DWORD));
	DWORD p,r,b,g;
	for(WORD i=0;i<0x8000;i++)
	{
		r=i&0x1f;
		g=i&0x3e0;
		b=i&0x7c00;
		if(theApp.m_ScanLines)
		{
			r+=3;
			if(r>0x1f) r=0x1f;
			g+=(3<<5);
			if(g>0x3e0) g=0x3e0;
			b+=(3<<10);
			if(b>0x7c00) b=0x7c00;
		}

		p=g<<6;
		p|=(r<<19);
		p|=(b>>7);
		ColorSwapD[i]=p;
	}
	ColorSwap=(WORD *)ColorSwapD;
}

void mmDisplay::UpdateScreenBuffer(CWnd *view,char *source,WORD Depth)
{
	DDSURFACEDESC2 ddsd;
	HRESULT err;

	m_FrameCount++;

	memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	err = m_ScreenBuffer->Lock(NULL, &ddsd, 0, NULL);
	if(err!=DD_OK) 
	{
//		theApp.LogMessage(ShowError(err));
//		m_ScreenBuffer->Unlock(NULL);
		return;
	}
	
	DWORD *src=(DWORD *)source;
	DWORD *dst=(DWORD *)ddsd.lpSurface;
	WORD *wdst=(WORD *)ddsd.lpSurface;
	WORD *whold;
	DWORD *dhold;
	DWORD *dst2,*dst3;
	WORD *wdst2,*wdst3;
	DWORD wpitch=ddsd.lPitch/2;
	DWORD dpitch=ddsd.lPitch/4;
	DWORD argb;
	WORD wargb;
	DWORD p1,p2;

	WORD r,g,b,a;
	int x,y;

	if(ColorSwap==NULL)
	{
		if(ddsd.ddpfPixelFormat.dwRGBBitCount!=32)
		{
			DWORD gm=ddsd.ddpfPixelFormat.dwGBitMask;
			if(gm==0x7e0)
				MakeColorSwap16BG();
			else
				MakeColorSwap16();
		}
		else
		{
			MakeColorSwap32();
		}
	}


	dst2=dst+dpitch;
	wdst2=wdst+wpitch;
	if(theApp.m_ScanLines)
	{
		if(ddsd.ddpfPixelFormat.dwRGBBitCount!=32)
		{
			for(y=0;y<240;y++)
			{
				dhold=dst;
				for(x=0;x<160;x++)
				{
					p2=*(src++);
					p1=p2&0x7fff;
					p2>>=16;
					p2&=0x7fff;
					*(dst++)=ColorSwap[p1]|ColorSwap[p2]<<16;
				}
				dst=dhold+(dpitch*2);
			}
		}
		else
		{
			for(y=0;y<240;y++)
			{
				dhold=dst;
				for(x=0;x<160;x++)
				{
					p2=*(src++);
					p1=p2&0x7fff;
					p2>>=16;
					p2&=0x7fff;
					*(dst++)=ColorSwapD[p1];
					*(dst++)=ColorSwapD[p2];
				}
				dst=dhold+(dpitch*2);
			}
		}
	}
	else
	{
		if(ddsd.ddpfPixelFormat.dwRGBBitCount!=32)
		{
			for(y=0;y<240;y++)
			{
				dhold=dst;
				for(x=0;x<160;x++)
				{
					p2=*(src++);
					p1=p2&0x7fff;
					p2>>=16;
					p2&=0x7fff;
					*(dst++)=ColorSwap[p1]|ColorSwap[p2]<<16;
				}
				dst=dhold+(dpitch*1);
			}
		}
		else
		{
			for(y=0;y<240;y++)
			{
				dhold=dst;
				for(x=0;x<160;x++)
				{
					p2=*(src++);
					p1=p2&0x7fff;
					p2>>=16;
					p2&=0x7fff;
					*(dst++)=ColorSwapD[p1];
					*(dst++)=ColorSwapD[p2];
				}
				dst=dhold+(dpitch*1);
			}
		}
	}
/*
*/
/*

	if(theApp.m_ScanLines)
	{
		if(gm==0x7e0)
		{
			for(y=0;y<240;y++)
			{
				dhold=dst;
				whold=wdst;
				for(x=0;x<160;x++)
				{
					p2=*(src++);
					p1=p2&0xffff;
					p2>>=16;
					if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
					{
						argb=(p1& 0x7c00)>>7;
						argb|=(p1&0x03e0)<<6;
						argb|=(p1&0x001f)<<19;
						*(dst++)=argb;
						*(dst2++)=argb;
					}
					else
					{
						r=(WORD)((p1&0x7c00)>>10);
						g=(WORD)((p1&0x03e0)<<1);
						b=(WORD)((p1&0x001f)<<11);
						wargb=r|g|b;
						*(wdst++)=wargb;
						*(wdst2++)=wargb;
					}
					if(p1==p2)
					{
						if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
						{
							*(dst++)=argb;
							*(dst2++)=argb;
						}
						else
						{
							*(wdst++)=wargb;
							*(wdst2++)=wargb;
						}
					}
					else
					{
						if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
						{
							argb=( p2&0x7c00)>>7;
							argb|=(p2&0x03e0)<<6;
							argb|=(p2&0x001f)<<19;
							*(dst++)=argb;
							*(dst2++)=argb;
						}
						else
						{
							r=(WORD)((p2&0x7c00)>>10);
							g=(WORD)((p2&0x03e0)<<1);
							b=(WORD)((p2&0x001f)<<11);
							wargb=r|g|b;
							*(wdst++)=wargb;
							*(wdst2++)=wargb;
						}
					}
				}
				dst=dhold+(dpitch*3);
				wdst=whold+(wpitch*3);
				dst2=dhold+(dpitch*4);
				wdst2=whold+(wpitch*4);

			 }
		}
		else
		{
			for(y=0;y<240;y++)
			{
				dhold=dst;
				whold=wdst;
				for(x=0;x<160;x++)
				{
					p2=*(src++);
					p1=p2&0xffff;
					p2>>=16;
					if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
					{
						argb=(p1& 0x7c00)>>7;
						argb|=(p1&0x03e0)<<6;
						argb|=(p1&0x001f)<<19;
						*(dst++)=argb;
						*(dst2++)=argb;
					}
					else
					{
						r=(WORD)((p1&0x7c00)>>10);
						g=(WORD)((p1&0x03e0)<<0);
						b=(WORD)((p1&0x001f)<<10);
						wargb=r|g|b;
						*(wdst++)=wargb;
						*(wdst2++)=wargb;
					}
					if(p1==p2)
					{
						if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
						{
							*(dst++)=argb;
							*(dst2++)=argb;
						}
						else
						{
							*(wdst++)=wargb;
							*(wdst2++)=wargb;
						}
					}
					else
					{
						if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
						{
							argb=( p2&0x7c00)>>7;
							argb|=(p2&0x03e0)<<6;
							argb|=(p2&0x001f)<<19;
							*(dst++)=argb;
							*(dst2++)=argb;
						}
						else
						{
							r=(WORD)((p2&0x7c00)>>10);
							g=(WORD)((p2&0x03e0)<<0);
							b=(WORD)((p2&0x001f)<<10);
							wargb=r|g|b;
							*(wdst++)=wargb;
							*(wdst2++)=wargb;
						}
					}
				}
				dst=dhold+(dpitch*3);
				wdst=whold+(wpitch*3);
				dst2=dhold+(dpitch*4);
				wdst2=whold+(wpitch*4);
			 }
		}
	}
	else
	{
		if(gm==0x7e0)
		{
			for(y=0;y<240;y++)
			{
				dhold=dst;
				whold=wdst;
				for(x=0;x<160;x++)
				{
					p2=*(src++);
					p1=p2&0xffff;
					p2>>=16;
					if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
					{
						argb=(p1& 0x7c00)>>7;
						argb|=(p1&0x03e0)<<6;
						argb|=(p1&0x001f)<<19;
						*(dst++)=argb;
					}
					else
					{
						r=(WORD)((p1&0x7c00)>>10);
						g=(WORD)((p1&0x03e0)<<1);
						b=(WORD)((p1&0x001f)<<11);
						wargb=r|g|b;
						*(wdst++)=wargb;
					}
					if(p1==p2)
					{
						if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
						{
							*(dst++)=argb;
						}
						else
						{
							*(wdst++)=wargb;
						}
					}
					else
					{
						if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
						{
							argb=( p2&0x7c00)>>7;
							argb|=(p2&0x03e0)<<6;
							argb|=(p2&0x001f)<<19;
							*(dst++)=argb;
						}
						else
						{
							r=(WORD)((p2&0x7c00)>>10);
							g=(WORD)((p2&0x03e0)<<1);
							b=(WORD)((p2&0x001f)<<11);
							wargb=r|g|b;
							*(wdst++)=wargb;
						}
					}
				}
				dst=dhold+(dpitch*1);
				wdst=whold+(wpitch*1);
			 }
		}
		else
		{
			for(y=0;y<240;y++)
			{
				dhold=dst;
				whold=wdst;
				for(x=0;x<160;x++)
				{
					p2=*(src++);
					p1=p2&0xffff;
					p2>>=16;
					if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
					{
						argb=(p1& 0x7c00)>>7;
						argb|=(p1&0x03e0)<<6;
						argb|=(p1&0x001f)<<19;
						*(dst++)=argb;
					}
					else
					{
						r=(WORD)((p1&0x7c00)>>10);
						g=(WORD)((p1&0x03e0)<<0);
						b=(WORD)((p1&0x001f)<<10);
						wargb=r|g|b;
						*(wdst++)=wargb;
					}
					if(p1==p2)
					{
						if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
						{
							*(dst++)=argb;
						}
						else
						{
							*(wdst++)=wargb;
						}
					}
					else
					{
						if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
						{
							argb=( p2&0x7c00)>>7;
							argb|=(p2&0x03e0)<<6;
							argb|=(p2&0x001f)<<19;
							*(dst++)=argb;
						}
						else
						{
							r=(WORD)((p2&0x7c00)>>10);
							g=(WORD)((p2&0x03e0)<<0);
							b=(WORD)((p2&0x001f)<<10);
							wargb=r|g|b;
							*(wdst++)=wargb;
						}
					}
				}
				dst=dhold+(dpitch*1);
				wdst=whold+(wpitch*1);
			 }
		}
	}
/*
*/
	m_ScreenBuffer->Unlock(NULL);
	if(theApp.m_ShowFPS)
	{
		HDC dc;
		m_ScreenBuffer->GetDC(&dc);
		float hz,fps;
		CurTime=timeGetTime();
		DWORD deltatime=CurTime-StartTime;
		hz=(float)dynaVCount/((float)(CurTime-StartTime)/1000.f);
		fps=(float)m_FrameCount/((float)(CurTime-StartTime)/1000.f);
		if(deltatime>4000) 
		{
			StartTime=CurTime;
			m_FrameCount=0;
			dynaVCount=0;
		}

		char text[256];
		sprintf(text,"%3.2fHz %3.2ffps",hz,fps);
		if(theApp.m_ScanLines)
			ExtTextOut(dc,0,460, ETO_CLIPPED, NULL, text, strlen(text),NULL); 
		else
			ExtTextOut(dc,0,220, ETO_CLIPPED, NULL, text, strlen(text),NULL); 
		m_ScreenBuffer->ReleaseDC(dc);
	}

	RECT Front,Back;
	Front.left=0; 
	Front.right=m_Width;
	Front.top=0; 
	Front.bottom=m_Height;
	Back=Front;
	Back.left=0; Back.top=0;
	Back.right=320; 
	if(theApp.m_ScanLines)
	{
		Back.bottom=480;
	}
	else
		Back.bottom=240;

	CPoint Origin;
	Origin.x=0;
	Origin.y=0;
	view->GetClientRect(&Front);
	view->ClientToScreen(&Origin);
	Front.left=Origin.x;
	Front.top=Origin.y;
	Front.right+=Front.left;
	Front.bottom+=Front.top;
	if(m_WindowMode)
	{
		HRESULT Lasterr =m_FrontBuffer->Blt(&Front,m_ScreenBuffer,&Back,DDBLT_WAIT,NULL);
		if(Lasterr==DDERR_SURFACELOST)
		{
    		m_FrontBuffer->Restore();
    		m_BackBuffer->Restore();
		}
	}
	else
	{
		Front.left=0; Front.top=0;
		Front.right=m_Width; Front.bottom=m_Height;
		Back.left=0; Back.top=0;
		Back.right=320; 
		if(theApp.m_ScanLines)
		{
			Back.bottom=480;
/*
			if(m_FrameCount&1)
			{
				Back.top++;
				Back.bottom++;
			}
*/
		}
		else
			Back.bottom=240;
		HRESULT Lasterr =m_FrontBuffer->Blt(&Front,m_ScreenBuffer,&Back,DDBLT_WAIT,NULL);
//		m_FrontBuffer->Flip(NULL,0);
	}

}

void mmDisplay::MakeScreenBuffer()
{
	DDSURFACEDESC2 ddsd;
	DDPIXELFORMAT pf;
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwFlags = DDSD_CAPS;
	ddsd.dwSize = sizeof(ddsd);
    ddsd.ddsCaps.dwCaps = 0; //DDSCAPS_VIDEOMEMORY;
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;// | DDSD_PIXELFORMAT;
	if(theApp.m_ScanLines)
		ddsd.dwHeight = 480;
	else
		ddsd.dwHeight = 240;
	ddsd.dwWidth = 320;
	pf.dwSize=sizeof(DDPIXELFORMAT);
	m_FrontBuffer->GetPixelFormat(&pf);
	ddsd.ddpfPixelFormat=pf;;
	HRESULT err = m_DDDevice4->CreateSurface(&ddsd, &m_ScreenBuffer, NULL);
	if(err!=DD_OK)
	{
	    ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
		HRESULT err = m_DDDevice4->CreateSurface(&ddsd, &m_ScreenBuffer, NULL);
		if(err!=DD_OK)
		{
			theApp.ErrorMessage("%s - %s",ShowError(err),"Error Creating Screen Surface");
		}
	}
	err = m_ScreenBuffer->Lock(NULL, &ddsd, 0, NULL);
	if(err!=DD_OK) 
	{
		return;
	}
	DWORD *dst=(DWORD *)ddsd.lpSurface;
	DWORD size=320*240*2;
	if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
		size*=2;
	if(theApp.m_ScanLines)
		size*=2;
	memset(dst,0,size);
	m_ScreenBuffer->Unlock(NULL);
}

IMPLEMENT_SERIAL(mmDisplay,CObject,1);
