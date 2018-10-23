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

#include "stdafx.h"
#include "math.h"
#include <objbase.h>
//#include <initguid.h>
#include <windowsx.h>

#define DIRECTINPUT_VERSION 0x0500

#include "global.h"
#include "imemory.h"
#include "dinput.h"
#include "mmDirectInputDevice.h"
#include "ki.h"

#include "ControllerConfig.h"
extern CFile *ataMFile;
extern bool logJAL;

extern "C"
{
#include "d3dmath.h"
}

unsigned char PCKey[48] = { DIK_U,DIK_I,DIK_O,DIK_J,
							DIK_K,DIK_L,DIK_UP,DIK_DOWN,
							DIK_LEFT,DIK_RIGHT,DIK_Y,DIK_2,
							DIK_6,DIK_5,DIK_8,DIK_9,
							DIK_W,DIK_E,DIK_R,DIK_S,
							DIK_D,DIK_F,DIK_HOME,DIK_END,
							DIK_DELETE,DIK_NEXT,DIK_Q,DIK_1,
							DIK_7,DIK_3,DIK_4,DIK_G,
							DIK_F1,DIK_F2,DIK_EQUALS,DIK_MINUS,
							DIK_F5,DIK_F6,DIK_F7,DIK_F8,
							DIK_F9,DIK_F10,DIK_F11,DIK_F12,
							DIK_Z,DIK_X,DIK_C,DIK_V};


unsigned long KIMap[32] = { 0x00000001,0x00000002,0x00000004,0x00000008,
							0x00000010,0x00000020,0x00000040,0x00000080,
							0x00000100,0x00000200,0x00000400,0x00000800,
							0x00001000,0x00002000,0x00004000,0x00008000,
							0x00010000,0x00020000,0x00040000,0x00080000,
							0x00100000,0x00200000,0x00400000,0x00800000,
							0x01000000,0x02000000,0x04000000,0x08000000,
							0x10000000,0x20000000,0x40000000,0x80000000};

unsigned long KeyDown[32] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
//Up,Down,Left,Right,Cup,Cdown,Cleft,Cright,A,B,Z,L,R,Start
#define KEYDOWN(name,key) (name[key] & 0x80)

extern volatile WORD NewTask;

BOOL          hasEnhanced;  
BOOL CALLBACK InitJoystickInput(LPCDIDEVICEINSTANCE pdinst, LPVOID pvRef) 
{ 
	LPDIRECTINPUTDEVICE pdev;  
	mmDirectInputDevice *us=(mmDirectInputDevice *)pvRef;

	// create the DirectInput joystick device 
	if (us->m_DInput->CreateDevice(pdinst->guidInstance, &pdev, NULL) != DI_OK)    
	{
		return DIENUM_CONTINUE;    
	} 

	memcpy(&us->m_DeviceInfo[us->m_NumDevices],pdinst,sizeof(DIDEVICEINSTANCE ));
	strcpy(us->m_DeviceName[us->m_NumDevices],pdinst->tszProductName);
	us->m_Device[us->m_NumDevices]=pdev;
	us->m_NumDevices++;

	return DIENUM_CONTINUE; 
} 


mmDirectInputDevice::mmDirectInputDevice()
{
	m_DInput=NULL;
	for(int i=0;i<8;i++)
	{
		m_Device[i]=NULL;
		m_Device2[i]=NULL;
	}
	m_NumDevices=0;
}

mmDirectInputDevice::~mmDirectInputDevice()
{
	for(int i=0;i<8;i++)
	{
		SafeRelease(m_Device[i]);
		SafeRelease(m_Device2[i]);
	}
	SafeRelease(m_DInput);
}

void mmDirectInputDevice::SetCoop(HWND hWnd)
{
	for(int i=0;i<m_NumDevices;i++)
	{
		if(m_DeviceType[i]==JOYSTICK)
		{
			m_Device[i]->SetCooperativeLevel(hWnd,DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
		}
		else
		{
			m_Device[i]->SetCooperativeLevel(hWnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		}
	}
}

HRESULT mmDirectInputDevice::Create(DWORD CoOpFlags,HWND hWnd)
{
	DIPROPDWORD pr;
	pr.diph.dwSize = sizeof(DIPROPDWORD);
	pr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	pr.diph.dwObj = 0;
	pr.diph.dwHow = DIPH_DEVICE;
	pr.dwData = 64;
	m_hWnd=hWnd;
	memset(m_OldJoyData,0,sizeof(DIJOYSTATE)*8);
	memset(m_NeedInit,1,sizeof(bool)*8);

	HRESULT err=DirectInputCreate(AfxGetInstanceHandle(),DIRECTINPUT_VERSION,&m_DInput,NULL);
	if(err!=DD_OK)
	{
		theApp.LogMessage("Error creating DirectInput");
		return(err);
	}
	m_NumDevices=0;


	m_DInput->EnumDevices(DIDEVTYPE_JOYSTICK, InitJoystickInput, (LPVOID) this, DIEDFL_ATTACHEDONLY); 


	for(int i=0;i<m_NumDevices;i++)
	{
		m_Device[i]->SetDataFormat(&c_dfDIJoystick);
		m_Device[i]->SetCooperativeLevel(AfxGetMainWnd()->m_hWnd,DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
		m_DeviceNumber[i]=i;
		m_DeviceType[i]=JOYSTICK;
		m_Device[i]->QueryInterface(IID_IDirectInputDevice2,(void **)&m_Device2[i]);
	}
/*
*/

	err=m_DInput->CreateDevice(GUID_SysKeyboard,&m_Device[m_NumDevices],NULL);
	if(err!=DD_OK)
	{
		theApp.LogMessage("Error creating DirectInputDevice for keyboard");
		return(err);
	}

	err=m_Device[m_NumDevices]->SetDataFormat(&c_dfDIKeyboard);
	if(err!=DD_OK)
	{
		theApp.LogMessage("Error setting data format");
		return(err);
	}

//	err=m_Device[m_NumDevices]->SetCooperativeLevel(AfxGetMainWnd()->m_hWnd,DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	err=m_Device[m_NumDevices]->SetCooperativeLevel(m_hWnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if(err!=DD_OK)
	{
		theApp.LogMessage("Error setting Coop level");
//		return(err);
	}


	err=m_Device[m_NumDevices]->SetProperty( DIPROP_BUFFERSIZE, &pr.diph );
	if(err!=DD_OK)
	{
		theApp.LogMessage("Error setting buffer size");
		return(err);
	}
	strcpy(m_DeviceName[m_NumDevices],"Keyboard");
	m_DeviceType[m_NumDevices]=KEYBOARD;
	m_DeviceNumber[m_NumDevices]=m_NumDevices;
	m_NumDevices++;


/*
	if(m_NumDevices>theApp.m_DeviceNum[0])
	{
		m_DeviceNumber[0]=theApp.m_DeviceNum[0];
		if(m_DeviceNumber[0])
			m_DeviceType[0]=JOYSTICK;
	}
*/
	CFile *tmpFile=(CFile *)new CFile();
	bool results=tmpFile->Open("U64KeyMap.dat",CFile::modeReadWrite);
	if(!results)
	{
		tmpFile->Open("U64KeyMap.dat",CFile::modeReadWrite|CFile::modeCreate);
		m_Map[0]=DIK_U;
		m_Map[1]=DIK_I;
		m_Map[2]=DIK_O;
		m_Map[3]=DIK_J;
		m_Map[4]=DIK_K;
		m_Map[5]=DIK_L;
		m_Map[6]=DIK_UP;
		m_Map[7]=DIK_DOWN;
		m_Map[8]=DIK_LEFT;
		m_Map[9]=DIK_RIGHT;
		m_Map[10]=DIK_Y;

		m_Map[11]=DIK_W;
		m_Map[12]=DIK_E;
		m_Map[13]=DIK_R;
		m_Map[14]=DIK_S;
		m_Map[15]=DIK_D;
		m_Map[16]=DIK_F;
		m_Map[17]=DIK_HOME;
		m_Map[18]=DIK_END;
		m_Map[19]=DIK_DELETE;
		m_Map[20]=DIK_NEXT;
		m_Map[21]=DIK_Q;
		tmpFile->Write(m_Map,sizeof(DWORD)*22);
		tmpFile->Close();
	}
	else
	{
		tmpFile->Read(m_Map,sizeof(DWORD)*22);
		tmpFile->Close();
	}
	
	return(DD_OK);
}

HRESULT mmDirectInputDevice::Open()
{
	for(int i=0;i<m_NumDevices;i++)
	{
		m_Device[i]->Acquire();
	}
    return( DD_OK );
}


HRESULT mmDirectInputDevice::Close()
{
	for(int i=0;i<m_NumDevices;i++)
	{
		m_Device[i]->Unacquire();
	}
    return( DD_OK );
}


DWORD mmDirectInputDevice::GetData(WORD ControllerNum)
{
	static DWORD lastBounce=timeGetTime();
	BYTE buffer[256];
	if(m_DeviceType[ControllerNum]==KEYBOARD)
	{
//		return(GetKeyboardData(ControllerNum));
	}
//	if(ControllerNum>0)
//		return(GetKeyboardData(1));

	DIJOYSTATE joyData;
	WORD dnum=m_DeviceNumber[ControllerNum];
	m_Device2[dnum]->Poll();
	HRESULT err=m_Device2[m_DeviceNumber[ControllerNum]]->GetDeviceState(sizeof(DIJOYSTATE),&joyData);
	float x,y;
	x=1.f-(float)(int)joyData.lX/65536.f;
	y=(float)(int)joyData.lY/65536.f;
	BYTE xx,yy;
	if(x<=.5f)
	{
		xx=127-(x*254.f);
	}
	else
	{
		xx=((1.f-x)*254.f)+128;
	}
	if(y<=.5f)
	{
		yy=127-(y*254.f);
	}
	else
	{
		yy=((1.f-y)*254.f)+128;
	}
	DWORD state=0;
	bool IsAdaptoid=true;
	if(IsAdaptoid)
	{
		state|=(joyData.rgbButtons[8]<<21);		//s
		state|=(joyData.rgbButtons[9]<<22);		//z
		state|=(joyData.rgbButtons[3]<<23);		//b
		state|=(joyData.rgbButtons[0]<<24);		//a
		state|=(joyData.rgbButtons[10]<<20);		//up
		state|=(joyData.rgbButtons[11]<<19);		//down
		state|=(joyData.rgbButtons[12]<<18);		//left
		state|=(joyData.rgbButtons[13]<<17);		//right
		state|=(joyData.rgbButtons[6]<<14);		//panl
		state|=(joyData.rgbButtons[7]<<13);		//panr
		state|=(joyData.rgbButtons[5]<<12);		//cup
		state|=(joyData.rgbButtons[1]<<11);		//cdown
		state|=(joyData.rgbButtons[4]<<10);		//cleft
		state|=(joyData.rgbButtons[2]<< 9);		//cright
		state|=((DWORD)xx<<8);
		state|=yy;
	}
	else
	{
		state|=(joyData.rgbButtons[0]<<21);		//s
		state|=(joyData.rgbButtons[1]<<22);		//z
		state|=(joyData.rgbButtons[2]<<23);		//x
		state|=(joyData.rgbButtons[3]<<24);		//a
		state|=((DWORD)xx<<8);
		state|=yy;
	}
/*
	dnum=1;
	if (m_Device[dnum]->GetDeviceState(sizeof(buffer),(LPVOID)&buffer) != DD_OK)
	{
		m_Device[dnum]->Acquire();
		return(state);
//		m_Device[0]->GetDeviceState(sizeof(buffer),(LPVOID)&buffer);
	}
	if(KEYDOWN(buffer,DIK_F1))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			NewTask=SAVE_GAME;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_F6))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			NewTask=LOAD_GAME;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_F2))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			theApp.m_ShowFPS^=1;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_F3))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			theApp.m_BypassGFX^=1;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_F4))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			theApp.m_StartLog^=1;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_F5))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			theApp.m_BypassAFX^=1;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_W))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			theApp.m_WireFrame^=1;
			lastBounce=timeGetTime();
		}
	}
*/
	return(state);
}

DWORD mmDirectInputDevice::Scan(WORD MapValue,WORD *Device,WORD *Index,char *Text)
{
	BYTE buffer[256];
	WORD LastDevice=m_NumDevices-1;
	if (m_Device[LastDevice]->GetDeviceState(sizeof(buffer),(LPVOID)&buffer) != DD_OK)
	{
		m_Device[LastDevice]->Acquire();
		return(0xffffffff);
	}
	for(int k=0;k<255;k++)
	{
		if(KEYDOWN(buffer,k))
		{
			*Device=0;
			*Index=k;
			KeyToText(k,Text);
			return(k);
		}
	}
	for(int i=0;i<(m_NumDevices-1);i++)
	{
		DIJOYSTATE joyData;
		m_Device2[i]->Poll();
		HRESULT err=m_Device2[i]->GetDeviceState(sizeof(DIJOYSTATE),&joyData);
		if(m_NeedInit[i])
		{
			memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
			m_NeedInit[i]=false;
			return(0xffffffff);
		}
		int dx=joyData.lX;
		int dy=joyData.lY;
		int dz=joyData.lZ;
/*
		if(dx==0) dx=32768;
		if(dy==0) dy=32768;
*/
		if(dz==0) dz=32768;
		if((dx==dy)&&(dx==0))
		{
			dx=32768;
			dy=32768;
		}
		if(dx>48000)
		{
			DWORD res=(256*(i+1))+64+16;
			sprintf(Text,"Joy %d X+ Axis",i);
			memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
			return(res);
		}
		if(dx<16000)
		{
			DWORD res=(256*(i+1))+64+17;
			sprintf(Text,"Joy %d X- Axis",i);
			memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
			return(res);
		}
		if(dy>48000)
		{
			DWORD res=(256*(i+1))+64+18;
			sprintf(Text,"Joy %d Y+ Axis",i);
			memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
			return(res);
		}
		if(dy<16000)
		{
			DWORD res=(256*(i+1))+64+19;
			sprintf(Text,"Joy %d Y- Axis",i);
			memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
			return(res);
		}
		if(dz>48000)
		{
			DWORD res=(256*(i+1))+64+20;
			sprintf(Text,"Joy %d Z+ Axis",i);
			memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
			return(res);
		}
		if(dz<16000)
		{
			DWORD res=(256*(i+1))+64+21;
			sprintf(Text,"Joy %d Z- Axis",i);
			memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
			return(res);
		}
		DWORD state=0;
		for(int b=0;b<32;b++)
		{
			if(joyData.rgbButtons[b]) //!=m_OldJoyData[i].rgbButtons[b])
			{
				DWORD res=(256*(i+1))+b;
				sprintf(Text,"Joy %d Button %d",i,b);
				memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
				return(res);
			}
		}
		for(int p=0;p<4;p++)
		{
			if(joyData.rgdwPOV[p]) //!=m_OldJoyData[i].rgdwPOV[p])
			{
				switch(joyData.rgdwPOV[p])
				{
				case 1:
					{
						DWORD res=(256*(i+1))+p*4+64;
						sprintf(Text,"Joy %d POV[%d] UP",i,p);
						memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
						return(res);
					}
				case 2:
					{
						DWORD res=(256*(i+1))+p*4+65;
						sprintf(Text,"Joy %d POV[%d] LEFT",i,p);
						memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
						return(res);
					}
				case 4:
					{
						DWORD res=(256*(i+1))+p*4+66;
						sprintf(Text,"Joy %d POV[%d] DOWN",i,p);
						memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
						return(res);
					}
				case 8:
					{
						DWORD res=(256*(i+1))+p*4+67;
						sprintf(Text,"Joy %d POV[%d] RIGHT",i,p);
						memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
						return(res);
					}
				}
			}
		}

	}
	return(0xffffffff);
}

DWORD mmDirectInputDevice::GetKeyboardData(WORD ControllerNum,WORD *Values)
{
	WORD state=0;
	static DWORD lastBounce=timeGetTime();
	BYTE buffer[256];
	if (m_Device[ControllerNum]->GetDeviceState(sizeof(buffer),(LPVOID)&buffer) != DD_OK)
	{
		m_Device[ControllerNum]->Acquire();
		return(0);
//		m_Device[0]->GetDeviceState(sizeof(buffer),(LPVOID)&buffer);
	}
	WORD count=0;
	WORD bit;
	for(int j=0;j<3;j++)
	{
		state=0;
		bit=1;
		for (int i=0; i < 16; i++) 
		{
			if (KEYDOWN(buffer,PCKey[count])) 
			{
				state |= bit;
			}
			bit<<=1;
			count++;
		}
		*(Values++)=~state;
	}
	if(KEYDOWN(buffer,DIK_F4))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			NewTask=SAVE_GAME;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_F6))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			NewTask=LOAD_GAME;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_F2))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			theApp.m_ShowFPS^=1;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_RCONTROL))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			theApp.m_IgnoreSecondary^=1;
			lastBounce=timeGetTime();
		}
	}
/*
	else if(KEYDOWN(buffer,DIK_F4))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			theApp.m_StartLog^=1;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_F5))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			theApp.m_BypassAFX^=1;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_W))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			theApp.m_WireFrame^=1;
			lastBounce=timeGetTime();
		}
	}
*/
	return state;
}

void mmDirectInputDevice::PickDevices()
{
/*
	if(m_NumDevices==1)
	{
		MessageBox(0,"There are no DirectInput devices found to configure","Keyboard is only DirectInput Device found",MB_OK);
		return;
	}
	CControllerConfig *config=(CControllerConfig *)new CControllerConfig();
	config->m_InputDevice=this;
	if(config->DoModal()==IDOK)
	{
		theApp.m_DeviceNum[0]=m_DeviceNumber[0];
	}
	delete config;
*/
}

void mmDirectInputDevice::KeyToText(int code,char *str)
{

	switch(code)
	{
	case DIK_ESCAPE:
		{
			strcpy(str,"DIK_ESCAPE");
			break;
		}
	case DIK_1:
		{
			strcpy(str,"DIK_1");
			break;
		}
	case DIK_2:
		{
			strcpy(str,"DIK_2");
			break;
		}
	case DIK_3:
		{
			strcpy(str,"DIK_3");
			break;
		}
	case DIK_4:
		{
			strcpy(str,"DIK_4");
			break;
		}
	case DIK_5:
		{
			strcpy(str,"DIK_5");
			break;
		}
	case DIK_6:
		{
			strcpy(str,"DIK_6");
			break;
		}
	case DIK_7:
		{
			strcpy(str,"DIK_7");
			break;
		}
	case DIK_8:
		{
			strcpy(str,"DIK_8");
			break;
		}
	case DIK_9:
		{
			strcpy(str,"DIK_9");
			break;
		}
	case DIK_0:
		{
			strcpy(str,"DIK_0");
			break;
		}
	case DIK_MINUS:
		{
			strcpy(str,"DIK_MINUS");
			break;
		}
	case DIK_EQUALS:
		{
			strcpy(str,"DIK_EQUALS");
			break;
		}
	case DIK_BACK:
		{
			strcpy(str,"DIK_BACK");
			break;
		}
	case DIK_TAB:
		{
			strcpy(str,"DIK_TAB");
			break;
		}
	case DIK_Q:
		{
			strcpy(str,"DIK_Q");
			break;
		}
	case DIK_W:
		{
			strcpy(str,"DIK_W");
			break;
		}
	case DIK_E:
		{
			strcpy(str,"DIK_E");
			break;
		}
	case DIK_R:
		{
			strcpy(str,"DIK_R");
			break;
		}
	case DIK_T:
		{
			strcpy(str,"DIK_T");
			break;
		}
	case DIK_Y:
		{
			strcpy(str,"DIK_Y");
			break;
		}
	case DIK_U:
		{
			strcpy(str,"DIK_U");
			break;
		}
	case DIK_I:
		{
			strcpy(str,"DIK_I");
			break;
		}
	case DIK_O:
		{
			strcpy(str,"DIK_O");
			break;
		}
	case DIK_P:
		{
			strcpy(str,"DIK_P");
			break;
		}
	case DIK_LBRACKET:
		{
			strcpy(str,"DIK_LBRACKET");
			break;
		}
	case DIK_RBRACKET:
		{
			strcpy(str,"DIK_RBRACKET");
			break;
		}
	case DIK_RETURN:
		{
			strcpy(str,"DIK_RETURN");
			break;
		}
	case DIK_LCONTROL:
		{
			strcpy(str,"DIK_LCONTROL");
			break;
		}
	case DIK_A:
		{
			strcpy(str,"DIK_A");
			break;
		}
	case DIK_S:
		{
			strcpy(str,"DIK_S");
			break;
		}
	case DIK_D:
		{
			strcpy(str,"DIK_D");
			break;
		}
	case DIK_F:
		{
			strcpy(str,"DIK_F");
			break;
		}
	case DIK_G:
		{
			strcpy(str,"DIK_G");
			break;
		}
	case DIK_H:
		{
			strcpy(str,"DIK_H");
			break;
		}
	case DIK_J:
		{
			strcpy(str,"DIK_J");
			break;
		}
	case DIK_K:
		{
			strcpy(str,"DIK_K");
			break;
		}
	case DIK_L:
		{
			strcpy(str,"DIK_L");
			break;
		}
	case DIK_SEMICOLON:
		{
			strcpy(str,"DIK_SEMICOLON");
			break;
		}
	case DIK_APOSTROPHE:
		{
			strcpy(str,"DIK_APOSTROPHE");
			break;
		}
	case DIK_GRAVE:
		{
			strcpy(str,"DIK_GRAVE");
			break;
		}
	case DIK_LSHIFT:
		{
			strcpy(str,"DIK_LSHIFT");
			break;
		}
	case DIK_BACKSLASH:
		{
			strcpy(str,"DIK_BACKSLASH");
			break;
		}
	case DIK_Z:
		{
			strcpy(str,"DIK_Z");
			break;
		}
	case DIK_X:
		{
			strcpy(str,"DIK_X");
			break;
		}
	case DIK_C:
		{
			strcpy(str,"DIK_C");
			break;
		}
	case DIK_V:
		{
			strcpy(str,"DIK_V");
			break;
		}
	case DIK_B:
		{
			strcpy(str,"DIK_B");
			break;
		}
	case DIK_N:
		{
			strcpy(str,"DIK_N");
			break;
		}
	case DIK_M:
		{
			strcpy(str,"DIK_M");
			break;
		}
	case DIK_COMMA:
		{
			strcpy(str,"DIK_COMMA");
			break;
		}
	case DIK_PERIOD:
		{
			strcpy(str,"DIK_PERIOD");
			break;
		}
	case DIK_SLASH:
		{
			strcpy(str,"DIK_SLASH");
			break;
		}
	case DIK_RSHIFT:
		{
			strcpy(str,"DIK_RSHIFT");
			break;
		}
	case DIK_MULTIPLY:
		{
			strcpy(str,"DIK_MULTIPLY");
			break;
		}
	case DIK_LMENU:
		{
			strcpy(str,"DIK_LMENU");
			break;
		}
	case DIK_SPACE:
		{
			strcpy(str,"DIK_SPACE");
			break;
		}
	case DIK_CAPITAL:
		{
			strcpy(str,"DIK_CAPITAL");
			break;
		}
	case DIK_F1:
		{
			strcpy(str,"DIK_F1");
			break;
		}
	case DIK_F2:
		{
			strcpy(str,"DIK_F2");
			break;
		}
	case DIK_F3:
		{
			strcpy(str,"DIK_F3");
			break;
		}
	case DIK_F4:
		{
			strcpy(str,"DIK_F4");
			break;
		}
	case DIK_F5:
		{
			strcpy(str,"DIK_F5");
			break;
		}
	case DIK_F6:
		{
			strcpy(str,"DIK_F6");
			break;
		}
	case DIK_F7:
		{
			strcpy(str,"DIK_F7");
			break;
		}
	case DIK_F8:
		{
			strcpy(str,"DIK_F8");
			break;
		}
	case DIK_F9:
		{
			strcpy(str,"DIK_F9");
			break;
		}
	case DIK_F10:
		{
			strcpy(str,"DIK_F10");
			break;
		}
	case DIK_NUMLOCK:
		{
			strcpy(str,"DIK_NUMLOCK");
			break;
		}
	case DIK_SCROLL:
		{
			strcpy(str,"DIK_SCROLL");
			break;
		}
	case DIK_NUMPAD7:
		{
			strcpy(str,"DIK_NUMPAD7");
			break;
		}
	case DIK_NUMPAD8:
		{
			strcpy(str,"DIK_NUMPAD8");
			break;
		}
	case DIK_NUMPAD9:
		{
			strcpy(str,"DIK_NUMPAD9");
			break;
		}
	case DIK_SUBTRACT:
		{
			strcpy(str,"DIK_SUBTRACT");
			break;
		}
	case DIK_NUMPAD4:
		{
			strcpy(str,"DIK_NUMPAD4");
			break;
		}
	case DIK_NUMPAD5:
		{
			strcpy(str,"DIK_NUMPAD5");
			break;
		}
	case DIK_NUMPAD6:
		{
			strcpy(str,"DIK_NUMPAD6");
			break;
		}
	case DIK_ADD:
		{
			strcpy(str,"DIK_ADD");
			break;
		}
	case DIK_NUMPAD1:
		{
			strcpy(str,"DIK_NUMPAD1");
			break;
		}
	case DIK_NUMPAD2:
		{
			strcpy(str,"DIK_NUMPAD2");
			break;
		}
	case DIK_NUMPAD3:
		{
			strcpy(str,"DIK_NUMPAD3");
			break;
		}
	case DIK_NUMPAD0:
		{
			strcpy(str,"DIK_NUMPAD0");
			break;
		}
	case DIK_DECIMAL:
		{
			strcpy(str,"DIK_DECIMAL");
			break;
		}
	case DIK_F11:
		{
			strcpy(str,"DIK_F11");
			break;
		}
	case DIK_F12:
		{
			strcpy(str,"DIK_F12");
			break;
		}
		
	case DIK_F13:
		{
			strcpy(str,"DIK_F13");
			break;
		}
	case DIK_F14:
		{
			strcpy(str,"DIK_F14");
			break;
		}
	case DIK_F15:
		{
			strcpy(str,"DIK_F15");
			break;
		}
		
	case DIK_KANA:
		{
			strcpy(str,"DIK_KANA");
			break;
		}
	case DIK_CONVERT:
		{
			strcpy(str,"DIK_CONVERT");
			break;
		}
	case DIK_NOCONVERT:
		{
			strcpy(str,"DIK_NOCONVERT");
			break;
		}
	case DIK_YEN:
		{
			strcpy(str,"DIK_YEN");
			break;
		}
	case DIK_NUMPADEQUALS:
		{
			strcpy(str,"DIK_NUMPADEQUALS");
			break;
		}
	case DIK_CIRCUMFLEX:
		{
			strcpy(str,"DIK_CIRCUMFLEX");
			break;
		}
	case DIK_AT:
		{
			strcpy(str,"DIK_AT");
			break;
		}
	case DIK_COLON:
		{
			strcpy(str,"DIK_COLON");
			break;
		}
	case DIK_UNDERLINE:
		{
			strcpy(str,"DIK_UNDERLINE");
			break;
		}
	case DIK_KANJI:
		{
			strcpy(str,"DIK_KANJI");
			break;
		}
	case DIK_STOP:
		{
			strcpy(str,"DIK_STOP");
			break;
		}
	case DIK_AX:
		{
			strcpy(str,"DIK_AX");
			break;
		}
	case DIK_UNLABELED:
		{
			strcpy(str,"DIK_UNLABELED");
			break;
		}
	case DIK_NUMPADENTER:
		{
			strcpy(str,"DIK_NUMPADENTER");
			break;
		}
	case DIK_RCONTROL:
		{
			strcpy(str,"DIK_RCONTROL");
			break;
		}
	case DIK_NUMPADCOMMA:
		{
			strcpy(str,"DIK_NUMPADCOMMA");
			break;
		}
	case DIK_DIVIDE:
		{
			strcpy(str,"DIK_DIVIDE");
			break;
		}
	case DIK_SYSRQ:
		{
			strcpy(str,"DIK_SYSRQ");
			break;
		}
	case DIK_RMENU:
		{
			strcpy(str,"DIK_RMENU");
			break;
		}
	case DIK_HOME:
		{
			strcpy(str,"DIK_HOME");
			break;
		}
	case DIK_UP:
		{
			strcpy(str,"DIK_UP");
			break;
		}
	case DIK_PRIOR:
		{
			strcpy(str,"DIK_PRIOR");
			break;
		}
	case DIK_LEFT:
		{
			strcpy(str,"DIK_LEFT");
			break;
		}
	case DIK_RIGHT:
		{
			strcpy(str,"DIK_RIGHT");
			break;
		}
	case DIK_END:
		{
			strcpy(str,"DIK_END");
			break;
		}
	case DIK_DOWN:
		{
			strcpy(str,"DIK_DOWN");
			break;
		}
	case DIK_NEXT:
		{
			strcpy(str,"DIK_NEXT");
			break;
		}
	case DIK_INSERT:
		{
			strcpy(str,"DIK_INSERT");
			break;
		}
	case DIK_DELETE:
		{
			strcpy(str,"DIK_DELETE");
			break;
		}
	case DIK_LWIN:
		{
			strcpy(str,"DIK_LWIN");
			break;
		}
	case DIK_RWIN:
		{
			strcpy(str,"DIK_RWIN");
			break;
		}
	case DIK_APPS:
		{
			strcpy(str,"DIK_APPS");
			break;
		}
	default:
		{
			strcpy(str,"");
			break;
		}
	}
}


DWORD mmDirectInputDevice::MultiScan(WORD *Values)
{
	static DWORD lastBounce=timeGetTime();
	BYTE buffer[256];
	DWORD state=0;
	WORD LastDevice=m_NumDevices-1;
	if (m_Device[LastDevice]->GetDeviceState(sizeof(buffer),(LPVOID)&buffer) != DD_OK)
	{
		m_Device[LastDevice]->Acquire();
		return(0xffffffff);
	}
	for(int i=0;i<(m_NumDevices-1);i++)
	{
		DIJOYSTATE joyData;
		m_Device2[i]->Poll();
		HRESULT err=m_Device2[i]->GetDeviceState(sizeof(DIJOYSTATE),&joyData);
		memcpy(&m_OldJoyData[i],&joyData,sizeof(DIJOYSTATE));
	}
	DWORD mask=0;
	for(int m=0;m<22;m++)
	{
		if(KEYDOWN(buffer,DIK_F7))
			state|=(0x1000<<15);
		if(KEYDOWN(buffer,DIK_F9))
			state|=(0x2000<<16);
		if(KEYDOWN(buffer,DIK_F10))
			state|=(0x4000<<16);
		if(KEYDOWN(buffer,DIK_F11))
			state|=(0x1000<<16);
		if(KEYDOWN(buffer,DIK_F8))
			state|=(0x2<<10);
		if(m_Map[m]<256)
		{
			if(KEYDOWN(buffer,m_Map[m]))
			{
				mask=(1<<m);
				if(m>10) mask<<=5;
				state|=mask;
			}
		}
		else
		{
			DWORD map=m_Map[m];
			int device=map/256;
			device--;
			map=map%256;
			if(map<64)
			{
				if(m_OldJoyData[device].rgbButtons[map])
				{
					mask=(1<<m);
					if(m>10) mask<<=5;
					state|=mask;
				}
			}
			else if(map<80)
			{
				int pov=(map-64)/4;
				int dir=(map-64)%4;
				switch(dir)
				{
				case 0:
					{
						if(m_OldJoyData[device].rgdwPOV[pov]&1)
						{
							mask=(1<<m);
							if(m>10) mask<<=5;
							state|=mask;
						}
						break;
					}
				case 1:
					{
						if(m_OldJoyData[device].rgdwPOV[pov]&2)
						{
							mask=(1<<m);
							if(m>10) mask<<=5;
							state|=mask;
						}
						break;
					}
				case 2:
					{
						if(m_OldJoyData[device].rgdwPOV[pov]&4)
						{
							mask=(1<<m);
							if(m>10) mask<<=5;
							state|=mask;
						}
						break;
					}
				case 3:
					{
						if(m_OldJoyData[device].rgdwPOV[pov]&8)
						{
							mask=(1<<m);
							if(m>10) mask<<=5;
							state|=mask;
						}
						break;
					}
				}
			}
			else
			{
				switch(map-80)
				{
				case 0:
					{
						if(m_OldJoyData[device].lX>48000)
						{
							mask=(1<<m);
							if(m>10) mask<<=5;
							state|=mask;
						}
						break;
					}
				case 1:
					{
						if(m_OldJoyData[device].lX<16000)
						{
							mask=(1<<m);
							if(m>10) mask<<=5;
							state|=mask;
						}
						break;
					}
				case 2:
					{
						if(m_OldJoyData[device].lY>48000)
						{
							mask=(1<<m);
							if(m>10) mask<<=5;
							state|=mask;
						}
						break;
					}
				case 3:
					{
						if(m_OldJoyData[device].lY<16000)
						{
							mask=(1<<m);
							if(m>10) mask<<=5;
							state|=mask;
						}
						break;
					}
				case 4:
					{
						if(m_OldJoyData[device].lZ>48000)
						{
							mask=(1<<m);
							if(m>10) mask<<=5;
							state|=mask;
						}
						break;
					}
				case 5:
					{
						if(m_OldJoyData[device].lZ<16000)
						{
							mask=(1<<m);
							if(m>10) mask<<=5;
							state|=mask;
						}
						break;
					}
				}
			}
		}
	}
	*(Values++)=~(state&0xffff);
	*(Values++)=~((state>>16)&0xffff);
	state=0;
	if(KEYDOWN(buffer,DIK_MINUS))
	{
		state|=8;
	}
	if(KEYDOWN(buffer,DIK_EQUALS))
	{
		state|=4;
	}
	*(Values++)=~(state&0xffff);
	if(KEYDOWN(buffer,DIK_F4))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			NewTask=SAVE_GAME;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_F6))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			NewTask=LOAD_GAME;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_F5))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			if(DspTask==NORMAL_GAME)
			{
				DspTask=0xff;
				theApp.m_EmuObj->m_Audio->Stop();
			}
			else
			{
				theApp.m_EmuObj->m_Audio->Play(0,0,DSBPLAY_LOOPING);
				DspTask=NORMAL_GAME;
			}
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_F2))
	{
		if((timeGetTime()-lastBounce)>250)
		{
			theApp.m_ShowFPS^=1;
			lastBounce=timeGetTime();
		}
	}
	else if(KEYDOWN(buffer,DIK_RCONTROL))
	{
		if((timeGetTime()-lastBounce)>250)
		{
/*
			ataMFile->Close();
			ataMFile->Open("mirror.dat",CFile::modeReadWrite|CFile::modeCreate);
			theApp.m_IgnoreSecondary^=1;
*/
			lastBounce=timeGetTime();
			logJAL=true;
		}
	}
	return(state);
}


IMPLEMENT_SERIAL(mmDirectInputDevice,CObject,1);


