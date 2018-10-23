/* Copyright STRiCOR, Inc. 1997,98

Module:
----------------------------------------------------------
	mmSoundBuffer

Function:
----------------------------------------------------------
	A class to handle the DirectX Sound API as related to
	buffers.

Revision History:
----------------------------------------------------------
	03/03/98 - created - jrb


*/
#include "stdafx.h"
#include "math.h"

#include "global.h"
#include "wave.h"

#include "mmSoundBuffer.h"

#define CREATE_DBG_FILEn

mmSoundBuffer::mmSoundBuffer()
{
	m_Buffer=NULL;
	m_3DBuffer=NULL;
	m_Is3D=FALSE;
}

mmSoundBuffer::~mmSoundBuffer()
{
#ifdef CREATE_DBG_FILE
	m_File->Close();
	delete m_File;
#endif

	if(m_Buffer!=NULL)
	{
		m_Buffer->Stop();
		m_Buffer->Release();
		m_Buffer=NULL;
	}
	if(m_3DBuffer!=NULL)
	{
		m_3DBuffer->Release();
		m_3DBuffer=NULL;
	}
}

BOOL mmSoundBuffer::Create(mmDirectSoundDevice *dsdevice)
{
	m_DirectSoundDevice=dsdevice;
	m_ThisMachine=m_DirectSoundDevice->m_ThisMachine;


	m_DirectSound=m_DirectSoundDevice->m_DirectSound;
	return(TRUE);
}

BOOL mmSoundBuffer::FromFile(char *filename, BOOL Is3D)
{
	MMCKINFO                        ckInRiff;
	MMCKINFO                        ckIn;
	int error;

	error=WaveOpenFile(filename,&m_MMIO,&m_WaveFormat,&ckInRiff);

	if ((error = WaveStartDataRead(&m_MMIO, &ckIn, &ckInRiff)) != 0)
	{
		goto ERROR_LOADING;
	}
	
				 
	PCMWAVEFORMAT pcmwf; 
    DSBUFFERDESC dsbdesc; 
    HRESULT hr; 
    // Set up wave format structure. 
    memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT)); 
    pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM; 
    pcmwf.wf.nChannels = m_WaveFormat->nChannels; 
    pcmwf.wf.nSamplesPerSec = m_WaveFormat->nSamplesPerSec;
    pcmwf.wf.nBlockAlign = m_WaveFormat->nBlockAlign;
    pcmwf.wf.nAvgBytesPerSec = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign; 
    pcmwf.wBitsPerSample = m_WaveFormat->wBitsPerSample;

    // Set up DSBUFFERDESC structure. 
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out. 
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    // Need default controls (pan, volume, frequency). 
    dsbdesc.dwFlags =  DSBCAPS_LOCHARDWARE | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
	if(Is3D)
	{
		dsbdesc.dwFlags|= DSBCAPS_CTRL3D;
	}
    // 3-second buffer. 
    dsbdesc.dwBufferBytes = ckIn.cksize; 
    dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf; 
    // Create buffer. 
    hr = m_DirectSound->CreateSoundBuffer(&dsbdesc,&m_Buffer, NULL); 
	if(hr!=DS_OK)
	{
		m_Buffer=NULL;
//		char msg[256];
//		m_DirectSoundDevice->ErrorMessage(hr,msg);
//		::MessageBox(0,msg,"Create Sound Buffer",MB_OK);
		dsbdesc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
		if(Is3D)
		{
			dsbdesc.dwFlags|= DSBCAPS_CTRL3D;
		}
		hr = m_DirectSound->CreateSoundBuffer(&dsbdesc,&m_Buffer, NULL); 
		if(hr!=DS_OK)
		{
			m_Buffer=NULL;
//			::MessageBox(0,"Gee, I tried two different methods but still..","Error Creating Sound Buffer",MB_OK);
			// 3-second buffer. 
 			return(FALSE);
		}
	}
	void *lpBuf1,*lpBuf2;
	DWORD Buf1Len,Buf2Len;
	unsigned int cbActualRead;

	m_Buffer->Lock(0,ckIn.cksize,&lpBuf1,&Buf1Len,&lpBuf2,&Buf2Len,DSBLOCK_ENTIREBUFFER);
	WaveReadFile(m_MMIO, ckIn.cksize,(BYTE *)lpBuf1, &ckIn, &cbActualRead);
	WaveCloseReadFile(&m_MMIO,&m_WaveFormat);
	m_Buffer->Unlock(lpBuf1,ckIn.cksize,lpBuf2,0);

	if(Is3D)
	{
		hr = m_Buffer->QueryInterface(IID_IDirectSound3DBuffer,(void **)&m_3DBuffer);
		if(hr!=DS_OK)
		{
			m_3DBuffer=NULL;
			return(FALSE);
		}
		m_3DBuffer->SetMinDistance(10.f,DS3D_DEFERRED);
		m_3DBuffer->SetMaxDistance(700.f,DS3D_DEFERRED);
	}
	return TRUE; 

ERROR_LOADING:
	return FALSE;
}

BOOL mmSoundBuffer::Play(DWORD from,DWORD to,DWORD flags)
{

	m_Buffer->Play(from,to,flags);
	return(TRUE);
}

BOOL mmSoundBuffer::Stop()
{
	m_Buffer->Stop();
	return(TRUE);
}

BOOL mmSoundBuffer::SetFrequency(DWORD freq)
{
	m_Buffer->SetFrequency(freq);
	return(TRUE);
}

BOOL mmSoundBuffer::SetPosition(D3DVECTOR *pos)
{
	if(m_3DBuffer==NULL) return(TRUE);
	m_3DBuffer->SetPosition(pos->x,pos->y,pos->z,DS3D_DEFERRED);
	return(TRUE);
}


BOOL mmSoundBuffer::SetVelocity(D3DVECTOR *vel)
{
	if(m_3DBuffer==NULL) return(TRUE);
	m_3DBuffer->SetVelocity(vel->x,vel->y,vel->z,DS3D_DEFERRED);
	return(TRUE);
}

BOOL mmSoundBuffer::SetVolume(float vol)
{
	int volume;
	volume=(int)((1.0-vol)*(float)DSBVOLUME_MIN);
	m_Buffer->SetVolume(volume);
	return(TRUE);
}

BOOL mmSoundBuffer::IsPlaying()
{
	DWORD status;
	m_Buffer->GetStatus(&status);
	return(status&DSBSTATUS_PLAYING);
}


/*
HRESULT SCDirectSound::Create3DBuffer()
{
// LPDIRECTSOUNDBUFFER lpDsbSecondary;
// The buffer has been created with DSBCAPS_CTRL3D. 
LPDIRECTSOUND3DBUFFER lpDs3dBuffer;

HRESULT hr = lpDsbSecondary->QueryInterface(IID_IDirectSound3DBuffer, 
                                            &lpDs3dBuffer); 
if (SUCCEEDED(hr)) 
  { 
  // Set 3-D parameters of this sound. 
    . 
    . 
    . 
  } 
}

typedef struct { 
    DWORD     dwSize; 
    D3DVECTOR  vPosition; 
    D3DVECTOR  vVelocity; 
    DWORD     dwInsideConeAngle; 
    DWORD     dwOutsideConeAngle; 
    D3DVECTOR  vConeOrientation; 
    LONG      lConeOutsideVolume; 
    D3DVALUE     flMinDistance; 
    D3DVALUE     flMaxDistance; 
    DWORD     dwMode; 
} DS3DBUFFER, *LPDS3DBUFFER;
 
typedef const DS3DBUFFER *LPCDS3DBUFFER;
HRESULT GetAllParameters(
  LPDS3DBUFFER lpDs3dBuffer  
);
 
HRESULT SetVelocity(
  D3DVALUE x,    
  D3DVALUE y,    
  D3DVALUE z,    
  DWORD dwApply  
);
HRESULT SetPosition(
  D3DVALUE x,    
  D3DVALUE y,    
  D3DVALUE z,    
  DWORD dwApply  
);
 DS3D_DEFERRED 	Settings are not applied until the application calls the IDirectSound3DListener::CommitDeferredSettings method. This allows the application to change several settings and generate a single recalculation. 
DS3D_IMMEDIATE 	Settings are applied immediately, causing the system to recalculate the 3-D coordinates for all 3-D sound buffers. 


// LPDIRECTSOUNDBUFFER lpDsbPrimary;
// The buffer has been created with DSBCAPS_CTRL3D. 
LPDIRECTSOUND3DLISTENER lpDs3dListener;

HRESULT hr = lpDsbPrimary->QueryInterface(IID_IDirectSound3DListener, 
                                          &lpDs3dListener); 
 
if (SUCCEEDED(hr)) 
  { 
  // Perform 3-D operations. 
  . 
  . 
  . 
  } 
 HRESULT SetOrientation(
  D3DVALUE xFront,  
  D3DVALUE yFront,  
  D3DVALUE zFront,  
  D3DVALUE xTop,    
  D3DVALUE yTop,    
  D3DVALUE zTop,    
  DWORD dwApply     
);
*/

BOOL mmSoundBuffer::FromMemory(char *src,DWORD Length,DWORD Rate,WORD Bits,WORD Buffers, BOOL Is3D)
{
	int error;

	Is3D=false;
	PCMWAVEFORMAT pcmwf; 
    DSBUFFERDESC dsbdesc; 
    HRESULT hr; 
    // Set up wave format structure. 
    memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT)); 
    pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM; 
    pcmwf.wf.nChannels = 1; 
    pcmwf.wf.nSamplesPerSec = Rate;
    pcmwf.wf.nBlockAlign = pcmwf.wf.nChannels*2;
    pcmwf.wf.nAvgBytesPerSec = Rate*pcmwf.wf.nBlockAlign; 
    pcmwf.wBitsPerSample = Bits;

#ifdef CREATE_DBG_FILE
	m_File=(CFile *)new CFile();
	m_File->Open("debug.wav",CFile::modeCreate|CFile::modeReadWrite);
	m_File->Write(&pcmwf,sizeof(PCMWAVEFORMAT));
#endif

    // Set up DSBUFFERDESC structure. 
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out. 
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    // Need default controls (pan, volume, frequency). 
    dsbdesc.dwFlags =  DSBCAPS_LOCHARDWARE | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
	if(Is3D)
	{
		dsbdesc.dwFlags|= DSBCAPS_CTRL3D;
	}
    // 3-second buffer. 
    dsbdesc.dwBufferBytes = Length*Buffers; 
    dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf; 
	m_TotalLength=Length*Buffers;
	m_BufLength=Length;
    // Create buffer. 
    hr = m_DirectSound->CreateSoundBuffer(&dsbdesc,&m_Buffer, NULL); 
	if(hr!=DS_OK)
	{
		m_Buffer=NULL;
		char msg[256];
		dsbdesc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
		if(Is3D)
		{
			dsbdesc.dwFlags|= DSBCAPS_CTRL3D;
		}
		hr = m_DirectSound->CreateSoundBuffer(&dsbdesc,&m_Buffer, NULL); 
		if(hr!=DS_OK)
		{
			m_Buffer=NULL;
//			::MessageBox(0,"Gee, I tried two different methods but still..","Error Creating Sound Buffer",MB_OK);
			// 3-second buffer. 
 			return(FALSE);
		}
	}
	void *lpBuf1,*lpBuf2;
	DWORD Buf1Len,Buf2Len;
	unsigned int cbActualRead;

	m_Buffer->Lock(0,Length,&lpBuf1,&Buf1Len,&lpBuf2,&Buf2Len,DSBLOCK_FROMWRITECURSOR);
	memcpy((char *)lpBuf1,src,Length);
	m_Buffer->Unlock(lpBuf1,Length,lpBuf2,0);

	if(Is3D)
	{
		hr = m_Buffer->QueryInterface(IID_IDirectSound3DBuffer,(void **)&m_3DBuffer);
		if(hr!=DS_OK)
		{
			m_3DBuffer=NULL;
			return(FALSE);
		}
		m_3DBuffer->SetMinDistance(10.f,DS3D_DEFERRED);
		m_3DBuffer->SetMaxDistance(700.f,DS3D_DEFERRED);
	}

	m_Buffer->Lock(0,m_TotalLength,&lpBuf1,&Buf1Len,&lpBuf2,&Buf2Len,0);
	memset(lpBuf1,0,Buf1Len);
	if(lpBuf2!=NULL)
		memset(lpBuf2,0,Buf2Len);
	m_Buffer->Unlock(lpBuf1,Buf1Len,lpBuf2,0);


	return TRUE; 

ERROR_LOADING:
	return FALSE;
}

BOOL mmSoundBuffer::Update(char *src,DWORD Length,WORD pos)
{
#ifdef CREATE_DBG_FILE
	m_File->Write(src,Length);
#endif
//return true;
	int error;

	void *lpBuf1,*lpBuf2;
	DWORD Buf1Len,Buf2Len;
	unsigned int cbActualRead;
	DWORD off=pos*Length;

	m_Buffer->Lock(off,Length,&lpBuf1,&Buf1Len,&lpBuf2,&Buf2Len,0);
	memcpy((char *)lpBuf1,src,Buf1Len);
	if(lpBuf2!=NULL)
		memcpy((char *)lpBuf2,src+Buf1Len,Buf2Len);
	m_Buffer->Unlock(lpBuf1,Buf1Len,lpBuf2,0);

	return TRUE; 
}

BOOL mmSoundBuffer::FromFileRaw(char *src,DWORD Length,DWORD Rate,WORD Bits, BOOL Is3D)
{
	int error;

	PCMWAVEFORMAT pcmwf; 
    DSBUFFERDESC dsbdesc; 
    HRESULT hr; 
    // Set up wave format structure. 
    memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT)); 
    pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM; 
    pcmwf.wf.nChannels = 1; 
    pcmwf.wf.nSamplesPerSec = Rate;
    pcmwf.wf.nBlockAlign = pcmwf.wf.nChannels*2;
    pcmwf.wf.nAvgBytesPerSec = Rate*pcmwf.wf.nBlockAlign; 
    pcmwf.wBitsPerSample = Bits;

#ifdef CREATE_DBG_FILE
	m_File=(CFile *)new CFile();
	m_File->Open("debug.raw",CFile::modeReadWrite);
	m_File->Read(&pcmwf,sizeof(PCMWAVEFORMAT));
	Length=m_File->GetLength();
	char *tmp=(char *)malloc(Length);
	m_File->Read(tmp,Length);
#endif


    pcmwf.wf.nChannels = 2; 
    pcmwf.wf.nSamplesPerSec = Rate;
    pcmwf.wf.nBlockAlign = pcmwf.wf.nChannels*2;
    pcmwf.wf.nAvgBytesPerSec = Rate*pcmwf.wf.nBlockAlign; 
    pcmwf.wBitsPerSample = Bits;


    // Set up DSBUFFERDESC structure. 
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out. 
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    // Need default controls (pan, volume, frequency). 
    dsbdesc.dwFlags =  DSBCAPS_LOCHARDWARE | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
	if(Is3D)
	{
		dsbdesc.dwFlags|= DSBCAPS_CTRL3D;
	}
    // 3-second buffer. 
    dsbdesc.dwBufferBytes = Length; 
    dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf; 
    // Create buffer. 
    hr = m_DirectSound->CreateSoundBuffer(&dsbdesc,&m_Buffer, NULL); 
	if(hr!=DS_OK)
	{
		m_Buffer=NULL;
		char msg[256];
		m_DirectSoundDevice->ErrorMessage(hr,msg);
		::MessageBox(0,msg,"Create Sound Buffer",MB_OK);
		dsbdesc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
		if(Is3D)
		{
			dsbdesc.dwFlags|= DSBCAPS_CTRL3D;
		}
		hr = m_DirectSound->CreateSoundBuffer(&dsbdesc,&m_Buffer, NULL); 
		if(hr!=DS_OK)
		{
			m_Buffer=NULL;
			::MessageBox(0,"Failed","Creating Sound Buffer",MB_OK);
			// 3-second buffer. 
 			return(FALSE);
		}
	}
	void *lpBuf1,*lpBuf2;
	DWORD Buf1Len,Buf2Len;
	unsigned int cbActualRead;

	m_Buffer->Lock(0,Length,&lpBuf1,&Buf1Len,&lpBuf2,&Buf2Len,DSBLOCK_ENTIREBUFFER);
	memset((char *)lpBuf1,0,Length);
	m_Buffer->Unlock(lpBuf1,Length,lpBuf2,0);

#ifdef CREATE_DBG_FILE
	free(tmp);
#endif

	if(Is3D)
	{
		hr = m_Buffer->QueryInterface(IID_IDirectSound3DBuffer,(void **)&m_3DBuffer);
		if(hr!=DS_OK)
		{
			m_3DBuffer=NULL;
			return(FALSE);
		}
		m_3DBuffer->SetMinDistance(10.f,DS3D_DEFERRED);
		m_3DBuffer->SetMaxDistance(700.f,DS3D_DEFERRED);
	}
	return TRUE; 

ERROR_LOADING:
	return FALSE;
}


IMPLEMENT_SERIAL(mmSoundBuffer,CObject,1);
