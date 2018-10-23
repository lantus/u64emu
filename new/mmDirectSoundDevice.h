#ifndef SC_DirectSound_H
#define SC_DirectSound_H

/* Copyright STRiCOR, Inc. 1997,98

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


class mmDirectSoundDevice : public CObject
 {
  public:
    mmDirectSoundDevice();
    ~mmDirectSoundDevice();
    DECLARE_SERIAL(mmDirectSoundDevice);

	LPDIRECTSOUND m_DirectSound;
	LPDIRECTSOUNDBUFFER m_Primary;
	LPDIRECTSOUND3DLISTENER m_3DListener;
	BOOL m_DirectSoundDeviceLocated;
	GUID m_GUID;
	WORD m_ThisMachine;
    
	virtual HRESULT Create(WORD machine);
	BOOL Play();
	void ErrorMessage(HRESULT error,char *string);

  };

#endif