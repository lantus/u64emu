/* Copyright STRiCOR, Inc. 1997,99

Module:
----------------------------------------------------------
	mmInputDevice

Function:
----------------------------------------------------------
	NX class manager

Revision History:
----------------------------------------------------------
	02/16/98 - created - jrb


*/
 
#include "math.h"
#include <switch.h>
 
#include "global.h"
#include "imemory.h"
#include "mmInputDevice.h"
#include "ki.h"

extern FILE *ataMFile;
extern bool logJAL;
extern bool nDiagnostics;
 
const int DEADZONE = 0x1000;

unsigned long KIMap[32] = { 0x00000001,0x00000002,0x00000004,0x00000008,
							0x00000010,0x00000020,0x00000040,0x00000080,
							0x00000100,0x00000200,0x00000400,0x00000800,
							0x00001000,0x00002000,0x00004000,0x00008000,
							0x00010000,0x00020000,0x00040000,0x00080000,
							0x00100000,0x00200000,0x00400000,0x00800000,
							0x01000000,0x02000000,0x04000000,0x08000000,
							0x10000000,0x20000000,0x40000000,0x80000000};

unsigned long KeyDown[32] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
 
#define KEYDOWN(name,key) (name[key] & 0x80)

extern volatile WORD NewTask;

BOOL          hasEnhanced;  
 
mmInputDevice::mmInputDevice()
{
	m_NumDevices = 2;
}

mmInputDevice::~mmInputDevice()
{
 
}
 

HRESULT mmInputDevice::Create()
{
	return 1;
}

HRESULT mmInputDevice::Open()
{
	return 1;
}


HRESULT mmInputDevice::Close()
{
	return 1;
}
 

DWORD mmInputDevice::Scan(WORD MapValue,WORD *Device,WORD *Index,char *Text)
{
 
	return(0xffffffff);
}

DWORD mmInputDevice::GetKeyboardData(WORD ControllerNum,WORD *Values)
{
 
	return 0;
}

void mmInputDevice::PickDevices()
{
 
}

 
DWORD mmInputDevice::MultiScan(WORD *Values)
{
 
	DWORD state=0;
	DWORD mask=0;
	
	hidScanInput();
	
	int k = hidKeysDown(CONTROLLER_P1_AUTO);
    int h = hidKeysHeld(CONTROLLER_P1_AUTO);	
	
	JoystickPosition joyPositionLeft, joyPositionRight;

    hidJoystickRead(&joyPositionLeft, CONTROLLER_P1_AUTO, JOYSTICK_LEFT);
    hidJoystickRead(&joyPositionRight, CONTROLLER_P1_AUTO, JOYSTICK_RIGHT);
 
/* second player - fix later 
	if (k & KEY_PLUS)
	{
		mask=(1<<10);
		state|=mask;
	}

	if (g_Gamepads[1].bAnalogButtons[m_GamepadConfig->GetFireMapping(1,0)] > 64)
	{
		mask=(1<<0);
		state|=mask;
	}

	if (g_Gamepads[1].bAnalogButtons[m_GamepadConfig->GetFireMapping(1,1)] > 64)
	{
		mask=(1<<1);
		state|=mask;
	}

	if (g_Gamepads[1].bAnalogButtons[m_GamepadConfig->GetFireMapping(1,2)] > 64)
	{
		mask=(1<<2);
		state|=mask;
	}

	if (g_Gamepads[1].bAnalogButtons[m_GamepadConfig->GetFireMapping(1,3)] > 64)
	{
		mask=(1<<3);
		state|=mask;
	}

	if (g_Gamepads[1].bAnalogButtons[m_GamepadConfig->GetFireMapping(1,4)] > 64)
	{
		mask=(1<<4);
		state|=mask;
	}

	if (g_Gamepads[1].bAnalogButtons[m_GamepadConfig->GetFireMapping(1,5)] > 64)
	{
		mask=(1<<5);
		state|=mask;
	}

	if ((k & KEY_DUP ) ||
	    (g_Gamepads[1].sThumbLY >  DEADZONE))
	{
		mask=(1<<6);
		state|=mask;
	}

	if ((k & KEY_DDOWN) ||
		(g_Gamepads[1].sThumbLY <  -DEADZONE))
	{
		mask=(1<<7);
		state|=mask;
	}

	if ((k & KEY_DLEFT) ||
		(g_Gamepads[1].sThumbLX < -DEADZONE))
	{
		mask=(1<<8);
		state|=mask;
	}

	if ((k & KEY_DRIGHT) ||
		(g_Gamepads[1].sThumbLX > DEADZONE))
	{
		mask=(1<<9);
		state|=mask;
	}

	if (g_Gamepads[1].wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
	{
		InGameMenu();
	}
	

	if (nDiagnostics)
	{
		state|=(0x1000<<16);
		nDiagnostics = 0;
	} */
	
	
	if (k & KEY_PLUS)
	{
		mask=(1<<21);
		mask<<=5;
		state|=mask;
		
		svcOutputDebugString("PLUS",20);
	}

	// quick punch
	if (k & KEY_A)
	{
		mask=(1<<11);
		mask<<=5;
		state|=mask;
		
		svcOutputDebugString("A",20);
	}

	// mid punch
	if (k & KEY_B)
	{
		mask=(1<<12);
		mask<<=5;
		state|=mask;
		
		svcOutputDebugString("B",20);
	}

	// fierce punch
	if (k & KEY_X)
	{
		mask=(1<<13);
		mask<<=5;
		state|=mask;
		
		svcOutputDebugString("X",20);
	}

	// quick kick
	if (k & KEY_Y)
	{
		mask=(1<<14);
		mask<<=5;
		state|=mask;
		
		svcOutputDebugString("Y",20);
	}

	// mid kick
	if (k & KEY_L)
	{
		mask=(1<<15);
		mask<<=5;
		state|=mask;
	}

	// fierce kick
	if (k & KEY_R)
	{
		mask=(1<<16);
		mask<<=5;
		state|=mask;
	}

	if ((h & KEY_DUP) ||
	    (joyPositionLeft.dy >  DEADZONE))
	{
		mask=(1<<17);
		mask<<=5;
		state|=mask;
	}

	if ((h & KEY_DDOWN) ||
		(joyPositionLeft.dy <  -DEADZONE))
	{
		mask=(1<<18);
		mask<<=5;
		state|=mask;
	}

	if ((h & KEY_DLEFT) ||
		(joyPositionLeft.dx < -DEADZONE))
	{
		mask=(1<<19);
		mask<<=5;
		state|=mask;
	}

	if ((h & KEY_DRIGHT) ||
		(joyPositionLeft.dx > DEADZONE))
	{
		mask=(1<<20);
		mask<<=5;
		state|=mask;
	}

/*	if (g_Gamepads[0].wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
	{
		InGameMenu();
	}*/
 

	*(Values++)=~(state&0xffff);
	*(Values++)=~((state>>16)&0xffff);
	state=0;
	*(Values++)=~(state&0xffff);
 
	return(state);
}
 

