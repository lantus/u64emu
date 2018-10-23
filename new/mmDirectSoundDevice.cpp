/* Copyright STRiCOR, Inc. 1998

Module:
----------------------------------------------------------
	mmDirectSoundDevice

Function:
----------------------------------------------------------
	A class to handle the DirectX Sound API

Revision History:
----------------------------------------------------------
	03/01/98 - created - jrb



*/

#include "stdafx.h"
#include "math.h"
#include <mmsystem.h>
#include "ki.h"
#include "global.h"
#include "mmDirectSoundDevice.h"
				
BOOL CALLBACK DSEnumProc( LPGUID lpGUID, LPCTSTR lpszDesc,
						 LPCTSTR lpszDrvName, LPVOID lpContext );

				
BOOL CALLBACK DSEnumProc( LPGUID lpGUID, LPCTSTR lpszDesc,
						 LPCTSTR lpszDrvName, LPVOID lpContext )
{
    LPGUID lpTemp = NULL;
	
    if( lpGUID != NULL )
	{
		if(strncmp(lpszDesc,"Dia",3)==0)		// pick the Diamond Monster 3D
		{
			memcpy((void *)&((mmDirectSoundDevice *)lpContext)->m_GUID,(void*)lpGUID,sizeof(GUID));
			((mmDirectSoundDevice *)lpContext)->m_DirectSoundDeviceLocated=TRUE;
			return DDENUMRET_CANCEL;
		}
		if(strncmp(lpszDesc,"AWE",3)==0)		// or pick the AWE64
		{
			memcpy((void *)&((mmDirectSoundDevice *)lpContext)->m_GUID,(void*)lpGUID,sizeof(GUID));
			((mmDirectSoundDevice *)lpContext)->m_DirectSoundDeviceLocated=TRUE;
			return DDENUMRET_CANCEL;
		}
		if(strncmp(lpszDesc,"Cre",3)==0)		// or pick the Creative Labs PCI128
		{
			memcpy((void *)&((mmDirectSoundDevice *)lpContext)->m_GUID,(void*)lpGUID,sizeof(GUID));
			((mmDirectSoundDevice *)lpContext)->m_DirectSoundDeviceLocated=TRUE;
			return DDENUMRET_CANCEL;
		}
	}
    return( TRUE );
}

mmDirectSoundDevice::mmDirectSoundDevice()
{
	m_Primary=NULL;
	m_3DListener=NULL;
	m_DirectSound=NULL;
	m_DirectSoundDeviceLocated=FALSE;
}

mmDirectSoundDevice::~mmDirectSoundDevice()
{
	if(m_Primary!=NULL)
	{
		m_Primary->Stop();
		m_Primary->Release();
		m_Primary=NULL;
	}
	if(m_3DListener!=NULL)
	{
		m_3DListener->Release();
		m_3DListener=NULL;
	}
	if(m_DirectSound!=NULL)
	{
		m_DirectSound->Release();
		m_DirectSound=NULL;
	}
}

HRESULT mmDirectSoundDevice::Create(WORD MachineNum)
{
	DSBUFFERDESC dsbd;

	m_ThisMachine=MachineNum;

	theApp.LogMessage("CoCreateInstance for DirectSound");
    HRESULT hr = CoCreateInstance( CLSID_DirectSound, NULL, CLSCTX_INPROC_SERVER,
								   IID_IDirectSound, (void**)&m_DirectSound);

//	theApp.LogMessage("DirectSound Enumeration started");
//	DirectSoundEnumerate((LPDSENUMCALLBACK)DSEnumProc, this );
	if(!m_DirectSoundDeviceLocated)
	{
		theApp.LogMessage("No Prefered SoundDevice found... trying Primary Driver");
		hr=m_DirectSound->Initialize(NULL);
	}
	else
	{
		theApp.LogMessage("DeviceEnumeration ok, using specified driver");
		hr=m_DirectSound->Initialize(&m_GUID);
	}

	if(hr!=DS_OK)
	{
		theApp.LogMessage("Failed DirectSound Init");
		return(-1);
	}

	CWnd *thisWindow=AfxGetMainWnd();
	m_DirectSound->SetCooperativeLevel(thisWindow->m_hWnd, DSSCL_NORMAL);

	memset(&dsbd, 0, sizeof(DSBUFFERDESC));

	dsbd.dwSize = sizeof(DSBUFFERDESC);
//	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D | DSBCAPS_LOCHARDWARE | DSBCAPS_CTRLVOLUME;
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_LOCHARDWARE | DSBCAPS_CTRLVOLUME;

	hr = m_DirectSound->CreateSoundBuffer(&dsbd, &m_Primary, NULL);
	if(hr!=DS_OK)
	{
		dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;

		hr = m_DirectSound->CreateSoundBuffer(&dsbd, &m_Primary, NULL);
		if(hr!=DS_OK)
		{
			char msg[256];
			ErrorMessage(hr,msg);
//			::MessageBox(0,msg,"DS Create Primary Buffer",MB_OK);
		}
	}
//	m_Primary->SetVolume(-900);
/*
	hr = m_Primary->QueryInterface(IID_IDirectSound3DListener,(void **)&m_3DListener);
	if(hr!=DS_OK)
	{
		::MessageBox(0,"Failed","DS Create 3D Listener",MB_OK);
	}
	m_3DListener->SetDistanceFactor(1.0f,DS3D_DEFERRED);
	m_3DListener->SetDopplerFactor(5.f,DS3D_DEFERRED);
*/
	m_3DListener=NULL;

	return(DS_OK);
}

BOOL mmDirectSoundDevice::Play()
{
	m_Primary->Play(0,0,DSBPLAY_LOOPING );
	return(TRUE);
}

void mmDirectSoundDevice::ErrorMessage(HRESULT error,char *string)
{
	switch (error) 
	{
	case DSERR_ALLOCATED:                   
		{
			strcpy(string,"Requested resources already in use");
			break;
		}
	case DSERR_ALREADYINITIALIZED:  
		{
			strcpy(string,"Object already initialized");
			break;
		}
	case DSERR_BADFORMAT:                   
		{
			strcpy(string,"Wave format not supported");
			break;
		}
	case DSERR_BUFFERLOST:                  
		{
			strcpy(string,"Buffer lost and must be restored");
			break;
		}
	case DSERR_CONTROLUNAVAIL:              
		{
			strcpy(string,"Control requested not available");
			break;
		}
	case DSERR_GENERIC:                             
		{
			strcpy(string,"Undetermined error");
			break;
		}
	case DSERR_INVALIDCALL:                 
		{
			strcpy(string,"Invalid call for object's current state");
			break;
		}
	case DSERR_INVALIDPARAM:                
		{
			strcpy(string,"Invalid parameters passed to object");
			break;
		}
	case DSERR_NOAGGREGATION:               
		{
			strcpy(string,"Object does not support aggregation");
			break;
		}
	case DSERR_NODRIVER:                    
		{
			strcpy(string,"No sound driver available");
			break;
		}
	case DSERR_OUTOFMEMORY:                 
		{
			strcpy(string,"Out of memory");
			break;
		}
	case DSERR_PRIOLEVELNEEDED:             
		{
			strcpy(string,"Caller does not have required priority level");
			break;
		}
	case DSERR_UNSUPPORTED:                 
		{
			strcpy(string,"Unsupported function called");
			break;
		}
	default:                                                
		{
			strcpy(string,"Undetermined error");
			break;
		}
	}
}

 

IMPLEMENT_SERIAL(mmDirectSoundDevice,CObject,1);
