
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "iMain.h"

#include "iCPU.h"			// Core 4600 emulation routines
#include "dynaCompiler.h"
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines
#include "iATA.h"			// KI ATA-2 hard disk emulation

#include "adsp2100.h"
#include "hleDSP.h"

#define DO_NOTHING              0x0000
#define DO_DISPLAY_REFRESH      0x0001
#define DO_RUN_RDP              0x0002
#define UPDATE_miReg           0x0004
#define UPDATE_VI_REG           0x0008
#define UPDATE_PI_REG           0x0010
#define UPDATE_SP_REG           0x0020
#define UPDATE_DP_REG           0x0040
#define CHECK_VI_REG            0x0080
#define UPDATE_AI_REG            0x0100
#define PI_DMA_WRITE            0x0200
#define PIF2DRAM_DMA            0x0400
#define DRAM2PIF_DMA            0x0800
#define IDMEM2DRAM_DMA          0x1000
#define DRAM2IDMEM_DMA          0x2000
#define PI_DMA_READ             0x4000
#define ATA_UPDATE				0x8000

#define LOG_STUFF
//extern QWORD ICount,NextIntCount;
#define LOG_ERRORS
//#define LOG_TLBn
#define HLE_DSP

//#define DOINDIAN

DWORD Patch1=0,Patch2=0,Patch3=0,Patch4=0;


extern WORD *reverse_table = 0;


DWORD hint=0;
DWORD ComRcv;
extern volatile WORD dynaVTRACE;
DWORD cheat;
DWORD cheat2;
WORD iMemToDo=0;
WORD iMemTLBActive=1;
DWORD iMemWriteByteAddress;
DWORD ARangeMin=0xffffffff;
DWORD ARangeMax=0;
DWORD A0RangeMin=0xffffffff;
DWORD A0RangeMax=0;
DWORD A1RangeMin=0xffffffff;
DWORD A1RangeMax=0;
DWORD A2RangeMin=0xffffffff;
DWORD A2RangeMax=0;
DWORD A3RangeMin=0xffffffff;
DWORD A3RangeMax=0;
DWORD A4RangeMin=0xffffffff;
DWORD A4RangeMax=0;
DWORD A5RangeMin=0xffffffff;
DWORD A5RangeMax=0;
DWORD iMemWriteWordAddress;
DWORD iMemWriteDWordAddress;
DWORD iMemWriteQWordAddress;
DWORD iMemReadByteAddress;
DWORD iMemReadWordAddress;
DWORD iMemReadDWordAddress;
DWORD iMemReadQWordAddress;
BYTE *DSPPMem,*DSPDMem;
WORD input1=0;
WORD input2=0;
DWORD input3=0;
DWORD flip=0;
WORD inputs[4]={0,0,0,0};


#define N_SEGMENTS 23


size_t MemSize[N_SEGMENTS] =
{
        8 * 1024 * 1024+(512*1024),        /* rd_ram   */  /*  0 */
        0x100,                  /* sp_dmem  */  /*  1 */
        0x100,					/* sp_imem  */  /*  2 */
        0x7c0,                  /* piRom   */  /*  3 */
        0x40,                   /* pi_ram   */  /*  4 */
        0x40,                   /* pi_ram_w */  /*  5 */
        
        1024,					/* rd_reg   */  /*  6 */
        0x20,                   /* sp_reg   */  /*  7 */
        0x20,                   /* dpc_reg  */  /*  8 */
        0x10,                   /* dps_reg  */  /*  9 */
        0x10,                   /* miReg   */  /* 10 */
        0x10,                   /* miReg_w */  /* 11 */
        0x38,                   /* vi_reg   */  /* 12 */
        0x1000,                 /* ai_reg   */  /* 13 */
        0x34,                   /* pi_reg   */  /* 14 */
        0x34,                   /* pi_reg_w */  /* 15 */
        0x20,                   /* ri_reg   */  /* 16 */
        0x1c,                   /* si_reg   */  /* 17 */
        1024,                   /* NullMem */  /* 18 */
		0x1000,					/* ATA registers  19*/
		0x8000,					/* 2105 program memory 20*/
		0x8000,					/* 2105 data memory 21*/
		4*1024*1024+4096		/* 2105 rom memory 22*/
};

BYTE *iMemAddr[N_SEGMENTS];



// OpCode support

// General Support

void iMemConstruct()
{
	m=NULL;
	iMemToDo=0;
	iMemTLBActive=false;
//	iMemTLBActive=true;
	m=(N64Mem *)malloc(sizeof(N64Mem));
	iMemInit();
	iMemClear();

	iMemWriteByteAddress=(unsigned long)iMemWriteByte;
	iMemWriteWordAddress=(unsigned long)iMemWriteWord;
	iMemWriteDWordAddress=(unsigned long)iMemWriteDWord;
	iMemWriteQWordAddress=(unsigned long)iMemWriteQWord;
	iMemReadByteAddress=(unsigned long)iMemReadByte;
	iMemReadWordAddress=(unsigned long)iMemReadWord;
	iMemReadDWordAddress=(unsigned long)iMemReadDWord;
	iMemReadQWordAddress=(unsigned long)iMemReadQWord;

	iATAConstruct();
	iATAOpen();

	dspBank=0;
	dspIRQClear=1;
	dspAuto=0;
	dspAutoCount=0;
	if(gRomSet==KI2)
	{
		inputs[0]=0;
		inputs[1]=0;
		inputs[2]=0;
		inputs[3]=0;
	}
	else
	{
		inputs[0]=0xffff;
		inputs[1]=0xffff;
		inputs[2]=0xffff;
		inputs[3]=0xffff;
	}
}

void iMemDestruct()
{
#ifdef LOG_STUFF
	theApp.LogMessage("Min ARange %X",ARangeMin);
	theApp.LogMessage("Max ARange %X",ARangeMax);
	theApp.LogMessage("Min A0Range %X",A0RangeMin);
	theApp.LogMessage("Max A0Range %X",A0RangeMax);
	theApp.LogMessage("Min A1Range %X",A1RangeMin);
	theApp.LogMessage("Max A1Range %X",A1RangeMax);
	theApp.LogMessage("Min A2Range %X",A2RangeMin);
	theApp.LogMessage("Max A2Range %X",A2RangeMax);
	theApp.LogMessage("Min A3Range %X",A3RangeMin);
	theApp.LogMessage("Max A3Range %X",A3RangeMax);
	theApp.LogMessage("Min A4Range %X",A4RangeMin);
	theApp.LogMessage("Max A4Range %X",A4RangeMax);
	theApp.LogMessage("Min A5Range %X",A5RangeMin);
	theApp.LogMessage("Max A5Range %X",A5RangeMax);
#endif
	iATAClose();
	iATADestruct();
	iMemFree();
	SafeFree(m);
}

void iMemFinalCheck()
{
    if(iMemToDo)
    {          
		if(iMemToDo & ATA_UPDATE)
				iATAUpdate();
    }                                       
    iMemToDo = DO_NOTHING;
}


BYTE *iMemPhysReadAddr(DWORD VAddr)
{
//	iCpuStepDSP();
	switch(VAddr>>24)
	{
	case 0:	
		{
#ifdef LOG_STUFF
			if(VAddr<A3RangeMin)
				A3RangeMin=VAddr;
			if(VAddr>A3RangeMax)
				A3RangeMax=VAddr;
#endif
			if(NewTask==BOOT_STAGE0)
			{
				return(rom->Image + (VAddr&0x7ffff));
			}
			//very slick... used for palette
			if(VAddr<0x1f0)
				return(m->rdRam + (VAddr&0x7fffff)+0x90000);
			else
				return(m->rdRam + (VAddr&0x7fffff));
			break;
		}
	case 0xa0:	// exception vectors 0x80 0x100 0x180 etc.
		{
#ifdef LOG_STUFF
			if(VAddr<A0RangeMin)
				A0RangeMin=VAddr;
			if(VAddr>A0RangeMax)
				A0RangeMax=VAddr;
#endif
			VAddr&=0xfffff;
			if((VAddr>=0x20000)&&(VAddr<0x80000))
				return((BYTE  *)SRAM+VAddr);
	        return(m->rdRam + (VAddr&0x7fffff));
			break;
		}
	case 0x80:	//video ram
		{
#ifdef LOG_STUFF
			if(VAddr<A1RangeMin)
				A1RangeMin=VAddr;
			if(VAddr>A1RangeMax)
				A1RangeMax=VAddr;
#endif
			VAddr&=0xfffff;
			return((BYTE  *)SRAM+VAddr);
			break;
		}
	case 0x88:	//general ram
		{
#ifdef LOG_STUFF
			if(VAddr<A2RangeMin)
				A2RangeMin=VAddr;
			if(VAddr>A2RangeMax)
				A2RangeMax=VAddr;
#endif
	        return(m->rdRam + (VAddr&0x7fffff));
			break;
		}
	case 0xa8:	//unknown
		{
#ifdef LOG_STUFF
			if(VAddr<ARangeMin)
				ARangeMin=VAddr;
			if(VAddr>ARangeMax)
				ARangeMax=VAddr;
#endif
			VAddr&=0x7fff0;
			return((BYTE  *)SRAM+VAddr);
		}
	case 0xb0:
		{
#ifdef LOG_STUFF
//			if((r->PC&0xfff0000)!=0x3c00000)
//				theApp.LogMessage("B-Range Read %X at PC %X",VAddr&0xfff,r->PC);
#endif
			switch((VAddr&0xffffff8))
			{
			case 0x80:
				{
 	//				DWORD tmp=~theApp.m_EmuObj->m_InputDevice->MultiScan(inputs);
					if(gRomSet==KI2)
					{
						inputs[2]&=~2;
	//					if(dspIRQClear)
							inputs[2]|=2;
						return((BYTE *)&inputs[2]);
					}
					else if(gRomSet==KI1)
					{
						return((BYTE *)&inputs[1]);
					}
					break;
				}
			case 0x88:
				{
					if(gRomSet==KI2)
					{
						Patch2=(~theApp.m_DIPS)&0xffff;
						Patch2|=0xff0000;
						return((BYTE *)&Patch2);
					}
					else if(gRomSet==KI1)
					{
						return((BYTE *)&inputs[0]);
					}
					break;
				}
			case 0x90:	//ready or INT line for DSP communication (i guess)
				{
//			NewTask=ERROR_BREAK;
					if(gRomSet==KI2)
					{
						//for KI2, this is button for player 2
						Patch1=inputs[0]&0xffff;
						return((BYTE *)&Patch1);
					}
					else if(gRomSet==KI1)
					{
						inputs[2]&=~2;
	//					if(dspIRQClear)
							inputs[2]|=2;
						return((BYTE *)&inputs[2]);
					}
					break;
				}
			case 0x98:	//serial port?
				{
					if(gRomSet==KI2)
					{
						//for KI2, this is button for player 1
						Patch3=inputs[1]&0xffff;
						Patch3|=0x1000;
						return((BYTE *)&Patch3);
					}
					else
					{
						return((BYTE *)&inputs[0]);
					}
					break;
				}
			case 0xa0:	//DIP switches
				{
					inputs[3]=~theApp.m_DIPS;
					inputs[4]=0x01;
					return((BYTE *)&inputs[3]);
					break;
				}
			case 0x100:	//data register for ATA interface
				{
					return(iATADataRead());
					break;
				}
			case 0x110:	//ata registers
			case 0x118:
			case 0x120:
			case 0x128:
			case 0x130:
				{
					break;
				}
			case 0x138:
			case 0x170:	//ata registers
				{
					break;
				}
			default:
				{
#ifdef LOG_STUFF
			theApp.LogMessage("B-Range %X",VAddr);
#endif
					break;
				}
			}
			return(m->aiReg+(VAddr&0xfff));
			break;
		}
	case 0x9f:	//rom
	case 0xbf:	//rom
		{
#ifdef LOG_STUFF
			if(VAddr<A4RangeMin)
				A4RangeMin=VAddr;
			if(VAddr>A4RangeMax)
				A4RangeMax=VAddr;
#endif
	        return(rom->Image + (VAddr&0x7ffff));
			break;	
		}
	}
#ifdef LOG_ERRORS
	theApp.LogMessage("PhysReadAddr: No memory at position: %08lx - PC %X\n",VAddr,r->PC);
	NewTask=ERROR_BREAK;
#endif
//	iCpuStepDSP();
	return m->NullMem;
} 
extern bool framespeed;
//int doframe=0;
BYTE *iMemPhysWriteAddr(DWORD VAddr)
{
//	iCpuStepDSP();
	switch(VAddr>>24)
	{
	case 0:	
		{
#ifdef LOG_STUFF
			if(VAddr<A3RangeMin)
				A3RangeMin=VAddr;
			if(VAddr>A3RangeMax)
				A3RangeMax=VAddr;
#endif
			if(NewTask==BOOT_STAGE0)
			{
				return(rom->Image + (VAddr&0x7ffff));
			}
			//very slick... used for palette
			if(VAddr<0x1f0)
				return(m->rdRam + (VAddr&0x7fffff)+0x90000);
			else
				return(m->rdRam + (VAddr&0x7fffff));
			break;
		}
	case 0xa0:	// exception vectors 0x80 0x100 0x180 etc.
		{
#ifdef LOG_STUFF
			if(VAddr<A0RangeMin)
				A0RangeMin=VAddr;
			if(VAddr>A0RangeMax)
				A0RangeMax=VAddr;
#endif
			VAddr&=0xfffff;
			if((VAddr>=0x20000)&&(VAddr<0x80000))
				return((BYTE  *)SRAM+VAddr);
	        return(m->rdRam + (VAddr&0x7fffff));
			break;
		}
	case 0x80:	//video ram
		{
#ifdef LOG_STUFF
			if(VAddr<A1RangeMin)
				A1RangeMin=VAddr;
			if(VAddr>A1RangeMax)
				A1RangeMax=VAddr;
#endif
			VAddr&=0xfffff;
			return((BYTE  *)SRAM+VAddr);
			break;
		}
	case 0x88:	//general ram
		{
#ifdef LOG_STUFF
			if(VAddr<A2RangeMin)
				A2RangeMin=VAddr;
			if(VAddr>A2RangeMax)
				A2RangeMax=VAddr;
#endif
	        return(m->rdRam + (VAddr&0x7fffff));
			break;
		}
	case 0xa8:	//unknown
		{
#ifdef LOG_STUFF
			if(VAddr<ARangeMin)
				ARangeMin=VAddr;
			if(VAddr>ARangeMax)
				ARangeMax=VAddr;
#endif
			VAddr&=0x7fff0;
//			return((BYTE  *)SRAM+VAddr);
			return(m->NullMem);
		}
	case 0xb0:
		{
#ifdef LOG_STUFF
			if((VAddr&0xffff)!=0x98)
				theApp.LogMessage("B-Range Write %X to %X, PC %X",cheat,VAddr&0xffff,r->PC);
#endif
			if(gRomSet==KI2)
			{
				switch(VAddr&0xfff)
				{
					case 0x98:	//page flip register?
						{
/*							doframe++;
							if (doframe==59) doframe=0;
							if (doframe%2==0){**/
							iCpuVSYNC();
						/*	theApp.m_EmuObj->UpdateDisplay();
							DWORD tmp=~theApp.m_EmuObj->m_InputDevice->MultiScan(inputs);//} TODO:FIX */
							*(DWORD *)&m->aiReg[0x98]=inputs[1]&0xffff;
							*(DWORD *)&m->aiReg[0x90]=inputs[0]&0xffff;
							break;

						}
					case 0x88:
						{
							break;
						}
					case 0x80:
						{
/*	#ifndef HLE_DSP
							if(cheat&0x2)
								adsp2105_set_irq_line(ADSP2105_IRQ2, 0);
							else
							{
								dspIRQClear=0;
								adsp2105_set_irq_line(ADSP2105_IRQ2, 1);
							}
	#else*/
							dspIRQClear=1;
//	#endif
							break;
						}
					case 0xA0:
						{
							dspPeek=cheat;
//	#ifdef HLE_DSP
							hleDSPIntService();
//	#endif
							break;
						}
					case 0x100:
						{
							//data register
							return(iATADataRead());
							break;
						}
					case 0x110:
					case 0x118:
					case 0x120:
					case 0x128:
					case 0x130:
					case 0x170:
						{
							return(m->atReg+(VAddr&0xfff));
							break;
						}
					case 0x138:
						{
							iMemToDo=ATA_UPDATE;
							break;
						}
					default:
						{
	#ifdef LOG_STUFFn
							theApp.LogMessage("UNKNOWN B-Range Write %X to %X, PC %X",cheat,VAddr&0xffff,r->PC);
	#endif
							break;
						}
				}
			}
			else if(gRomSet==KI1)
			{
				switch(VAddr&0xfff)
				{
					case 0x80:	//page flip register?
						{
/* TODO:FIX							
if  (framespeed){
	theApp.m_framesToSkip = 2;}
else {
	theApp.m_framesToSkip = 0;
	iCpuVSYNC();}
							theApp.m_EmuObj->UpdateDisplay();

							DWORD tmp=~theApp.m_EmuObj->m_InputDevice->MultiScan(inputs);
							break;
*/
						}
					case 0x88:
						{
							break;
						}
					case 0x90:
						{
/*	#ifndef HLE_DSP
							if(cheat&0x2)
								adsp2105_set_irq_line(ADSP2105_IRQ2, 0);
							else
							{
								dspIRQClear=0;
								adsp2105_set_irq_line(ADSP2105_IRQ2, 1);
							}
	#else*/
							dspIRQClear=1;
//	#endif
							break;
						}
					case 0x98:
						{
							dspPeek=cheat;
//	#ifdef HLE_DSP
							hleDSPIntService();
//	#endif
							break;
						}
					case 0x100:
						{
							//data register
							return(iATADataRead());
							break;
						}
					case 0x110:
					case 0x118:
					case 0x120:
					case 0x128:
					case 0x130:
					case 0x170:
						{
							return(m->atReg+(VAddr&0xfff));
							break;
						}
					case 0x138:
						{
							iMemToDo=ATA_UPDATE;
							break;
						}
					default:
						{
	#ifdef LOG_STUFFn
							theApp.LogMessage("UNKNOWN B-Range Write %X to %X, PC %X",cheat,VAddr&0xffff,r->PC);
	#endif
							break;
						}
				}
			}
			return(m->atReg+(VAddr&0xfff));
			break;
		}
	}

#ifdef LOG_ERRORS
	theApp.LogMessage("PhysWriteAddr: No memory at position: %08lx PC:%X\n",VAddr,r->PC);
//	NewTask=ERROR_BREAK;
#endif

        return(m->NullMem);

} 

BYTE iMemReadByte(DWORD where)
{
#ifdef DOINDIAN
    where ^= 0x03;
#endif
    return(*(BYTE *)iMemPhysReadAddr(where));
} 

WORD iMemReadWord(DWORD where)
{
#ifdef DOINDIAN
    where ^= 0x02;
#endif
    return(*(WORD *)iMemPhysReadAddr(where));
}


DWORD iMemReadDWord(DWORD where)
{
    return(*(DWORD *)iMemPhysReadAddr(where));
}

QWORD iMemReadQWord(DWORD where)
{
    QWORD value;

    value=*(QWORD *)iMemPhysReadAddr(where);
#ifdef DOINDIAN
	return( (value >> 32) + (value << 32) );
#else
	return(value);
#endif
}

void iMemWriteByte(BYTE what,DWORD where)
{
#ifdef LOG_STUFF
	cheat2=0;
#endif
#ifdef DOINDIAN
    where ^= 0x03;
#endif
	cheat=what;
	*(BYTE *)iMemPhysWriteAddr(where)=what;
    iMemFinalCheck();
} 

void iMemWriteWord(WORD what,DWORD where)
{
#ifdef LOG_STUFF
	cheat=what;
	cheat2=0;
#endif
#ifdef DOINDIAN
    where ^= 0x02;
#endif
	cheat=what;
	*(WORD *)iMemPhysWriteAddr(where)=what;
    iMemFinalCheck();
} 

void iMemWriteDWord(DWORD what,DWORD where)
{
#ifdef LOG_STUFF
	cheat=what;
	cheat2=0;
#endif
	cheat=what;
	*(DWORD *)iMemPhysWriteAddr(where)=what;
    iMemFinalCheck();
} 

void iMemWriteQWord(QWORD what,DWORD where)
{
#ifdef DOINDIAN
	QWORD value=(what << 32) + (what >> 32);
#else
	QWORD value=what;
#endif
#ifdef LOG_STUFF
	cheat=what;
	cheat2=(what>>32);
#endif

	cheat=what;
	*(QWORD *)iMemPhysWriteAddr(where)=value;
    iMemFinalCheck();
} 


void iMemInit()
{
	int i;
	for(i=0;i<N_SEGMENTS;i++)
	{
		iMemAddr[i]=(BYTE *)malloc(MemSize[i]);
	}
    m->rdRam      = iMemAddr[0];

    m->spDmem     = iMemAddr[1];
    m->spImem     = iMemAddr[2];
    m->piRom      = iMemAddr[3];
    m->piRam      = iMemAddr[4];
    m->piRamW    = iMemAddr[5];
                 
    m->rdReg      = iMemAddr[6];
    m->spReg      = iMemAddr[7];
    m->dpcReg     = iMemAddr[8];
    m->dpsReg     = iMemAddr[9];
    m->miReg      = iMemAddr[10];
    m->miRegW    = iMemAddr[11];
    m->viReg      = iMemAddr[12];
    m->aiReg      = iMemAddr[13];
    m->piReg      = iMemAddr[14];
    m->piRegW    = iMemAddr[15];
    m->riReg      = iMemAddr[16];
    m->siReg      = iMemAddr[17];
    m->NullMem    = iMemAddr[18];
	m->atReg	  = iMemAddr[19];	// seems to need to read/write here  3.1.99
	m->dspPMem=iMemAddr[20];
	m->dspDMem=iMemAddr[21];
	m->dspRMem=iMemAddr[22];

//    ((DWORD *)m->miReg)[1] = 0x01010101;
    m->spRegPC = 0x04001000;

    m->miRegModeRO=0;
    m->miRegIntrMaskRO=0;
    m->spRegStatusRO=0;
    m->dpcRegStatusRO=0;
    m->piRegStatusRO=0;

} 


void iMemClear()
{
    int i;

/*
    for(i=0; i<N_SEGMENTS; i++)
	{
		memset(iMemAddr[i], 0, MemSize[i]);
    
    } 
	*/

    ((DWORD *)m->miReg)[1] = 0x01010101;
    m->spRegPC = 0x04001000;
	memset(m->NullMem,0x00,1024);
	memset(SRAM,0x00,0x60000);
	memset(m->dspDMem,0,0x8000);
	memset(m->dspPMem,0,0x8000);

    iMemToDo = DO_NOTHING;
	m->atReg[0x80]=0;
	m->atReg[0x98]=0;		//page flip for KI2 - maybe ;)
	m->aiReg[0x170]=0x40;
} 


void iMemFree()
{
    int i;

    for(i=0; i<N_SEGMENTS; i++)
    {
		SafeFree(iMemAddr[i]);
    } 
}


void iMemDoDisplayRefresh()
{
} 


void iMemUpdateAIReg()
{
//	theApp.LogMessage("Audio Length %d",((DWORD *)m->aiReg)[1]);
//	theApp.m_EmuObj->UpdateAudio();
//	theApp.m_UpdateAudio=true;
} 

void iMemUpdateVIReg()
{
    ((DWORD *)m->viReg)[1] &= 0x007fffff;
    ((DWORD *)m->viReg)[2] &= 0x00000fff;
} 

void iMemUpdatePIReg()
{
	DWORD *Status=(DWORD *)m->piReg;
	Status+=4;			// point at status register
	if(*Status &0x01) // reset controler
	{
		//todo
	}
	if(*Status & 0x02)		// clear interrupt
	{
		((DWORD *)m->miReg)[2] &= ~MI_INTR_PI;
		r->DoOrCheckSthg &= ~MI_INTR_PI;
	}
	*Status=0;

} 

void iMemUpdateMIReg()
{
	DWORD miRegMode=((DWORD *)m->miReg)[0];	 

	m->miRegModeRO&=0xfffffff80;		// clear of init length
	m->miRegModeRO|=miRegMode&0x07f;	// add init length

	m->miRegModeRO &= ~((miRegMode & 0x0080)>>0);
	m->miRegModeRO |= ((miRegMode & 0x0100)>>1);
	m->miRegModeRO &= ~((miRegMode & 0x0200)>>1);
	m->miRegModeRO |= ((miRegMode & 0x0400)>>2);
	m->miRegModeRO &= ~((miRegMode & 0x1000)>>3);
	m->miRegModeRO |= ((miRegMode & 0x2000)>>4);

	if(miRegMode & 0x0800)					// MI_CLR_DP_INTR
	{
        ((DWORD *)m->miReg)[2] &= ~MI_INTR_DP;
		r->DoOrCheckSthg &= ~MI_INTR_DP;
	}

	((DWORD *)m->miReg)[0]=m->miRegModeRO; // now overwrite the register
    
	DWORD miRegIntrMask=((DWORD *)m->miReg)[3];

	m->miRegIntrMaskRO &=~((miRegIntrMask&0x0001)>>0);
	m->miRegIntrMaskRO |= ((miRegIntrMask&0x0002)>>1);
	m->miRegIntrMaskRO &=~((miRegIntrMask&0x0004)>>1);
	m->miRegIntrMaskRO |= ((miRegIntrMask&0x0008)>>2);
	m->miRegIntrMaskRO &=~((miRegIntrMask&0x0010)>>2);
	m->miRegIntrMaskRO |= ((miRegIntrMask&0x0020)>>3);
	m->miRegIntrMaskRO &=~((miRegIntrMask&0x0040)>>3);
	m->miRegIntrMaskRO |= ((miRegIntrMask&0x0080)>>4);
	m->miRegIntrMaskRO &=~((miRegIntrMask&0x0100)>>4);
	m->miRegIntrMaskRO |= ((miRegIntrMask&0x0200)>>5);
	m->miRegIntrMaskRO &=~((miRegIntrMask&0x0400)>>5);
	m->miRegIntrMaskRO |= ((miRegIntrMask&0x0800)>>6);

	((DWORD *)m->miReg)[3]=m->miRegIntrMaskRO&0x3f;
#ifdef LOG_STUFF
//	theApp.LogMessage("MIiMemUpdate Mask: %8X",((DWORD *)m->miReg)[3]);
#endif

} 


void iMemUpdateSPReg()
{
	DWORD spRegStatus=((DWORD *)m->spReg)[4];

	m->spRegStatusRO &= ~((spRegStatus & 0x00000001)>>0);
	m->spRegStatusRO |=  ((spRegStatus & 0x00000002)>>1);
	m->spRegStatusRO &= ~((spRegStatus & 0x00000004)>>1);
	m->spRegStatusRO &= ~((spRegStatus & 0x00000020)>>0);
	m->spRegStatusRO |=  ((spRegStatus & 0x00000040)>>1);
	m->spRegStatusRO &= ~((spRegStatus & 0x00000080)>>1);
	m->spRegStatusRO |=  ((spRegStatus & 0x00000100)>>2);
	m->spRegStatusRO &= ~((spRegStatus & 0x00000200)>>2);
	m->spRegStatusRO |=  ((spRegStatus & 0x00000400)>>3);
	m->spRegStatusRO &= ~((spRegStatus & 0x00000800)>>3);
	m->spRegStatusRO |=  ((spRegStatus & 0x00001000)>>4);
	m->spRegStatusRO &= ~((spRegStatus & 0x00002000)>>4);
	m->spRegStatusRO |=  ((spRegStatus & 0x00004000)>>5);
	m->spRegStatusRO &= ~((spRegStatus & 0x00008000)>>5);
	m->spRegStatusRO |=  ((spRegStatus & 0x00010000)>>6);
	m->spRegStatusRO &= ~((spRegStatus & 0x00020000)>>6);
	m->spRegStatusRO |=  ((spRegStatus & 0x00040000)>>7);
	m->spRegStatusRO &= ~((spRegStatus & 0x00080000)>>7);
	m->spRegStatusRO |=  ((spRegStatus & 0x00100000)>>8);
	m->spRegStatusRO &= ~((spRegStatus & 0x00200000)>>8);
	m->spRegStatusRO |=  ((spRegStatus & 0x00400000)>>9);
	m->spRegStatusRO &= ~((spRegStatus & 0x00800000)>>9);
	m->spRegStatusRO |=  ((spRegStatus & 0x01000000)>>10);


    if(spRegStatus & 0x00000008)   /* SP_CLR_INTR       */
	{
        ((DWORD *)m->miReg)[2] &= ~MI_INTR_SP;
		r->DoOrCheckSthg &= ~MI_INTR_SP;
	}

    if(spRegStatus & 0x00000010)   /* SP_SET_INTR       */
    {
        ((DWORD *)m->miReg)[2] |= MI_INTR_SP;
	    r->DoOrCheckSthg = MI_INTR_SP;
//		theApp.m_Interrupts->CheckMI();
    }
	((DWORD *)m->spReg)[4]=m->spRegStatusRO&0x7ff;
/*
*/
/*
	if((((DWORD *)m->spReg)[4]&1)==0)
	{
		if((m->spDmem[0xfc0]<3)) //&&(m->spDmem[0xfc0]==4))
		{
			if(m->spDmem[0xfc0]==1)
			{
				if(NewTask)
				{
					hleRdpBypass();
//					iCpuVSYNC();
				}
			}
			else
			{
					hleRdpBypass();
			}
		}
	}
*/
} 

void iMemUpdateDPReg()
{
	DWORD dpcRegStatus=((DWORD *)m->dpcReg)[3];

	m->dpcRegStatusRO &= ~((dpcRegStatus & 0x001) >> 0);
	m->dpcRegStatusRO |=  ((dpcRegStatus & 0x002) >> 1);
	m->dpcRegStatusRO &= ~((dpcRegStatus & 0x004) >> 1);
	m->dpcRegStatusRO |=  ((dpcRegStatus & 0x008) >> 2);
	m->dpcRegStatusRO &= ~((dpcRegStatus & 0x010) >> 2);
	m->dpcRegStatusRO |=  ((dpcRegStatus & 0x020) >> 3);

	if(dpcRegStatus & 0x0040)
		((DWORD *)m->dpcReg)[7]=0;
	if(dpcRegStatus & 0x0080)
		((DWORD *)m->dpcReg)[6]=0;
	if(dpcRegStatus & 0x0100)
		((DWORD *)m->dpcReg)[5]=0;
	if(dpcRegStatus & 0x0200)
		((DWORD *)m->dpcReg)[4]=0;

	((DWORD *)m->dpcReg)[3]=m->dpcRegStatusRO|0x80;
#ifdef LOG_STUFF
	theApp.LogMessage("DPStatus Verify %X",((DWORD *)m->dpcReg)[3]);
#endif

} 

void iMemCopyBootCode()
{
	DWORD *dst=(DWORD *)m->dspRMem;
	
 
	theApp.LogMessage("size of QWORD is %d", sizeof(QWORD)); 
	theApp.LogMessage("size of DWORD is %d", sizeof(DWORD)); 
	theApp.LogMessage("size of WORD is %d", sizeof(WORD));
	theApp.LogMessage("size of BYTE is %d", sizeof(BYTE));
 

	FILE *arom = NULL;
	arom=fopen(theApp.m_ARom1,"rb");
	fread(dst,512*1024, 1, arom);
	fclose(arom);
	//delete arom;
	dst+=512*256;
	
	arom=NULL;
	arom=fopen(theApp.m_ARom2,"rb");
	fread(dst,512*1024, 1, arom);
	fclose(arom);
	//delete arom;
	dst+=512*256;

	theApp.LogMessage("about to load theApp.m_ARom3");
	arom=NULL;
	arom=fopen(theApp.m_ARom3,"rb");
	fread(dst,512*1024, 1, arom);
	fclose(arom);
	//delete arom;
	dst+=512*256;

	theApp.LogMessage("about to load theApp.m_ARom4");
	arom=NULL;
	arom=fopen(theApp.m_ARom4,"rb");
	fread(dst,512*1024, 1, arom);
	fclose(arom);
	//delete arom;
	dst+=512*256;

	theApp.LogMessage("about to load theApp.m_ARom5");
	arom=NULL;
	arom=fopen(theApp.m_ARom5,"rb");
	fread(dst,512*1024, 1, arom);
	fclose(arom);
	//delete arom;
	dst+=512*256;

	theApp.LogMessage("about to load theApp.m_ARom6");
	arom=NULL;
	arom=fopen(theApp.m_ARom6,"rb");
	fread(dst,512*1024, 1, arom);
	fclose(arom);
	//delete arom;
	dst+=512*256;

	theApp.LogMessage("about to load theApp.m_ARom7");
	arom=NULL;
	arom=fopen(theApp.m_ARom7,"rb");
	fread(dst,512*1024, 1, arom);
	fclose(arom);
	//delete arom;
	dst+=512*256;

	theApp.LogMessage("about to load theApp.m_ARom8");
	arom=NULL;
	arom=fopen(theApp.m_ARom8,"rb");
	fread(dst,512*1024, 1, arom);
	fclose(arom);
	//delete arom;
}

/*
void iMemWriteBootCode()
{
	CFile *boot2=(CFile *)new CFile();
	boot2->Open("boot.rom",CFile::modeReadWrite|CFile::modeCreate);
	boot2->Write(rom->Image,0x1000);
	boot2->Close();
}

void iMemReadBootCode()
{
	return;

	char tmp[0x40];
	for(int i=0;i<0x40;i++)
	{
		tmp[i]=rom->Image[i];
	}
//	DWORD offset=*(DWORD *)(&rom->Image[8]);
	CFile *boot2=(CFile *)new CFile();
	boot2->Open("c:\\n64em\\boot.rom",CFile::modeRead);
	boot2->Read(rom->Image,0x1000);
	boot2->Close();
	*(DWORD *)(&rom->Image[8])=*(DWORD *)(&tmp[8]);
	*(DWORD *)(&rom->Image[0x10])=*(DWORD *)(&tmp[0x10]);
	*(DWORD *)(&rom->Image[0x14])=*(DWORD *)(&tmp[0x14]);

*/

void iMemSave(FILE *tmp)
{
	int i;
	for(i=0;i<N_SEGMENTS;i++)
	{
		fwrite(iMemAddr[i],MemSize[i],1,tmp);
		//tmp->Write(iMemAddr[i],MemSize[i]);
	}

}

void iMemLoad(FILE *tmp)
{
	int i;
	for(i=0;i<N_SEGMENTS;i++)
	{
		fread(iMemAddr[i],MemSize[i],1,tmp);
		//tmp->Read(iMemAddr[i],MemSize[i]);
	}
}

void iMemLoadShort(FILE *tmp)
{
	fread(iMemAddr[0],65536,1, tmp);
	fread(iMemAddr[1],MemSize[1],1,tmp);
	fread(iMemAddr[2],MemSize[2],1,tmp);
	fread(iMemAddr[4],MemSize[4],1,tmp);
	for(int i=7;i<17;i++)
	{
		fread(iMemAddr[i],MemSize[i],1,tmp);
		//tmp->Read(iMemAddr[i],MemSize[i]);
	}
}

unsigned char dspReadByte(DWORD A)
{
	BYTE tmp=0;
	A>>=1;
	if((A>=0x2000)&&(A<=0x2fff))
	{
		A^=0x2000;
		if(dspBank==1024)
		{
			A+=0x800;
		}
		else
		{
			A+=(dspBank<<12);
			tmp=*(BYTE *)&m->dspRMem[A];
		}
	}
	else
	{
		switch(A)
		{
		case 0x3400:
			{
				adsp2105_set_irq_line(ADSP2105_IRQ2, 0);
				dspIRQClear=1;
				tmp=dspPeek;
				break;
			}
		default:
			{
				tmp=*(BYTE *)&m->dspDMem[A<<1];
				break;
			}
		}
	}
	return(tmp);
}

unsigned int dspReadWord(DWORD A)
{
	WORD tmp=0;
	A>>=1;
/*
	if ( adsp2100_get_mstat() & 2 )
	{
		A = reverse_table[ A & 0x3fff ];
	}
*/
	if((A>=0x2000)&&(A<=0x2fff))
	{
		A^=0x2000;
		if(dspBank==1024)
		{
			A+=0x800;
			tmp=*(WORD *)&m->dspDMem[A];
#ifdef LOG_STUFF
			if(A<A5RangeMin)
				A5RangeMin=A;
			if(A>A5RangeMax)
				A5RangeMax=A;
#endif
		}
		else
		{
			A+=(dspBank<<12);
			tmp=*(WORD *)&m->dspRMem[A];
//			tmp=(tmp>>8)|((tmp&0xff)<<8);
		}
	}
	else
	{
		switch(A)
		{
		case 0x3400:
			{
#ifdef LOG_STUFF
		theApp.LogMessage("DSP peek %X",dspPeek);
#endif
				adsp2105_set_irq_line(ADSP2105_IRQ2, 0);
				dspIRQClear=1;
				tmp=dspPeek;
//				NewTask=ERROR_BREAK;
				break;
			}
		default:
			{
				tmp=*(WORD *)&m->dspDMem[A<<1];
				break;
			}
		}
	}
	return(tmp);
}

DWORD dspReadDWord(DWORD A)
{
	DWORD tmp=*(DWORD *)&m->dspPMem[A];
//	tmp=(tmp&0xff0000)>>16|(tmp&0xff)<<16|(tmp&0xff00);
	return(tmp);
}

void dspWriteByte(DWORD A,BYTE V)
{
	switch(A>>1)
	{
	case 0x3000:
		dspBank = V & 0xfF;
#ifdef LOG_STUFF
		theApp.LogMessage("Setting DSP Bank to %d",dspBank);
#endif
		break;
	case 0x3400:
		dspPoke = V;
#ifdef LOG_STUFF
		theApp.LogMessage("DSP poke %X",dspPoke);
#endif
		break;
	case 0x3FEF:
		dspAuto = V;
		dspAutoCount=480;
		dspAutoBase=adsp2100_get_reg(ADSP2100_I7);
#ifdef LOG_STUFF
		theApp.LogMessage("DSP AutoTX %X base: %X",V,dspAutoBase);
#endif
		break;
	case 0x3FF1:
		dspClkDiv = V;
		break;
	case 0x3FFF:
		dspCtrl = V;
		if(V & 0x200)
		{
#ifdef LOG_STUFF
		theApp.LogMessage("DSP reset with bank=%d",dspBank);
#endif
		if(dspBank!=1)
		{
			DspTask=ERROR_BREAK;
		}
			dspAuto=0;
			iMainResetDSP();
		}
		break;
	default:
		{
		*(BYTE *)&m->dspDMem[A]=V;
		break;
		}
	}
}

void dspWriteWord(DWORD A,WORD V)
{
	A>>=1;
	if((A>=0x2000)&&(A<=0x2fff))
	{
		A^=0x2000;
		if(dspBank==1024)
		{
			A+=0x800;
			*(WORD *)&m->dspDMem[A]=V;
#ifdef LOG_STUFF
			theApp.LogMessage("I dunno %X",A);
			if(A<A5RangeMin)
				A5RangeMin=A;
			if(A>A5RangeMax)
				A5RangeMax=A;
#endif
		}
		else
		{
//			V=(V>>8)|((V&0xff)<<8);
			A+=(dspBank<<12);
			*(WORD *)&m->dspRMem[A]=V;
		}
	}
	else
	{
		A<<=1;
		switch(A>>1)
		{
		case 0x3000:
			{
			dspBank = ((V & 0xf00)>>1)|V&0x7f;
	#ifdef LOG_STUFF
			DWORD pc=adsp2100_get_pc();
			theApp.LogMessage("Setting DSP Bank to %d at %x",dspBank,pc);
	#endif
			break;
			}
		case 0x3400:
			{
			dspPeek = V;
//			adsp2105_set_irq_line(ADSP2105_IRQ2, 1);
	#ifdef LOG_STUFF
			theApp.LogMessage("DSP poke %X",dspPeek);
	#endif
			break;
			}
		case 0x3FEF:
			{
			dspAuto = V;
			dspAutoCount=480;
			dspAutoBase=adsp2100_get_reg(ADSP2100_I7)&0xffffe;
//			dspAutoBase=0x3800;
	#ifdef LOG_STUFF
			theApp.LogMessage("DSP AutoTX %X base: %X",V,dspAutoBase);
	#endif

			break;
			}
		case 0x3FF1:
			{
			dspClkDiv = V;
			break;
			}
		case 0x3FFF:
			{
			dspCtrl = V;
			if(V & 0x200)
			{
	#ifdef LOG_STUFF
			theApp.LogMessage("DSP reset with bank=%d",dspBank);
	#endif
			if(dspBank!=1)
			{
				DspTask=ERROR_BREAK;
			}
				iMainResetDSP();
			}
			break;
			}
		default:
			{
			*(WORD *)&m->dspDMem[A]=V;
			break;
			}
		}
	}
}

void dspWriteDWord(DWORD A,DWORD V)
{
//	V=(V&0xff0000)>>16|(V&0xff)<<16|(V&0xff00);
	*(DWORD *)&m->dspPMem[A]=V;
}


