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
#include "mmInputDevice.h"


#define KEYBOARD 0
#define JOYSTICK 1

class mmInputDevice
{
	public:
		mmInputDevice();
		~mmInputDevice();
 
		bool m_NeedInit[8];
		DWORD m_Map[22];
		WORD m_NumDevices;		 
		char m_DeviceName[8][64];
		WORD m_DeviceNumber[8];
		WORD m_DeviceType[8];
		WORD m_Mapping[8][16];


 		HRESULT Create();
		HRESULT Open();
		HRESULT Close();
		DWORD GetData(WORD ControllerNum);
		DWORD GetKeyboardData(WORD ControllerNum,WORD *Values);
		void PickDevices();
		void KeyToText(int code,char *str);
		DWORD Scan(WORD MapValue,WORD *Device,WORD *Index,char *Text);
		DWORD MultiScan(WORD *Values);
 
 };

#endif
