#ifndef SC_INPUT
#define SC_INPUT

/* Copyright STRiCOR, Inc. 1997,99

Module:
----------------------------------------------------------
	mmDirectInputDevice

Function:
----------------------------------------------------------
	DirectInput class manager

Revision History:
----------------------------------------------------------
	02/16/98 - created - jrb


*/

#include "global.h"
#include "mmDirectInputDevice.h"


#define KEYBOARD 0
#define JOYSTICK 1

class mmDirectInputDevice : public CObject
{
	public:
		mmDirectInputDevice();
		~mmDirectInputDevice();
	    DECLARE_SERIAL(mmDirectInputDevice);

		DIJOYSTATE m_OldJoyData[8];
		bool m_NeedInit[8];
		DWORD m_Map[22];
		WORD m_NumDevices;
		HWND m_hWnd;
		DIDEVICEINSTANCE m_DeviceInfo[8];
		LPDIRECTINPUT m_DInput;
		LPDIRECTINPUTDEVICE m_Device[8];		// up to 8 possible devices
		LPDIRECTINPUTDEVICE2 m_Device2[8];		// up to 8 possible devices
		LPDIRECTINPUTDEVICE2 m_Controller[8];	// for mapping to controller
		char m_DeviceName[8][64];
		WORD m_DeviceNumber[8];
		WORD m_DeviceType[8];
		WORD m_Mapping[8][16];


		HRESULT Create(DWORD CoOpFlags,HWND hWnd);
		HRESULT Open();
		HRESULT Close();
		DWORD GetData(WORD ControllerNum);
		DWORD GetKeyboardData(WORD ControllerNum,WORD *Values);
		void PickDevices();
		void KeyToText(int code,char *str);
		DWORD Scan(WORD MapValue,WORD *Device,WORD *Index,char *Text);
		DWORD MultiScan(WORD *Values);
		void SetCoop(HWND hWnd);
 };

#endif
