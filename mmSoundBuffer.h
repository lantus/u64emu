#ifndef SC_SoundBuffer_H
#define SC_SoundBuffer_H

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

#include "mmDirectSoundDevice.h"

class mmSoundBuffer : public CObject
 {
  public:
    mmSoundBuffer();
    ~mmSoundBuffer();
    DECLARE_SERIAL(mmSoundBuffer);

	mmDirectSoundDevice *m_DirectSoundDevice;
	LPDIRECTSOUND m_DirectSound;
	LPDIRECTSOUNDBUFFER m_Buffer;
	LPDIRECTSOUND3DBUFFER m_3DBuffer;
	CObject *m_Scene;
	WORD m_ThisMachine;
	WORD m_TotalLength;
	WORD m_BufLength;
	BOOL m_Is3D;
	CFile *m_File;

	HMMIO m_MMIO;
	WAVEFORMATEX *m_WaveFormat;
	MMCKINFO m_ChunkInfo;
    
	virtual BOOL Create(mmDirectSoundDevice *m_SoundDevice);
	BOOL FromFile(char *filename,BOOL Is3D);
	BOOL FromMemory(char *src,DWORD Length,DWORD Rate,WORD Bits,WORD Buffers, BOOL Is3D);
	BOOL FromFileRaw(char *src,DWORD Length,DWORD Rate,WORD Bits, BOOL Is3D);
	BOOL Update(char *src,DWORD Length,WORD pos);
	BOOL Play(DWORD from,DWORD to,DWORD flags);
	BOOL SetVelocity(D3DVECTOR *vel);
	BOOL SetPosition(D3DVECTOR *pos);
	BOOL SetFrequency(DWORD freq);
	BOOL SetVolume(float vol);
	BOOL Stop();
	BOOL IsPlaying();
  };

#endif