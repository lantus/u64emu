
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "iMain.h"
#include "dynaCompiler.h"


#include "iCPU.h"			// Core 4600 emulation routines
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines

#include "iFPOps.h"			// floating point opcodes
#include "iMemory.h"
#include "iMemoryOps.h"		// memory opcodes
#include "iGeneralOps.h"	// general routines
#include "iBranchOps.h"		// branch opcodes
#include "iIns.h"			// instruction pointer arrays


#include "hleMain.h"
#include "hleDSP.h"
 
#include "adsp2100.h"

//#pragma optimize("",off)

//#define CCORE1
#define FRAME_SKIPn
#define FAKE_COP1_UNUSABLE
#define SEND_COMPARE


HANDLE iCpuThreadId;
HANDLE iDspThreadId;
DWORD iCpuNextVSYNC=0;
DWORD stats=0;
DWORD aiRate;
extern int hleDSPPingTime;
DWORD aiBufLen;
DWORD aiUsed;
DWORD aiUsedF;
DWORD aiUsedW;
DWORD aiUsedA;
DWORD aiDataUsed;
extern int hleRdpTimeLag;
int skipCount;
DWORD iCpuVSYNCAccum=0;
bool iCpuResetVSYNC=false;
bool dump=false;
DWORD Watch[64];
DWORD dspCount=0;

extern bool hleFMVDelay;
DWORD lagging=0;


void iCpuConstruct()
{
	r=(RS4300iReg *)malloc(sizeof(RS4300iReg));

#ifdef CCORE1
	dynaInit();
	dynaRegPtr=(DWORD)r;
	dynaRegPtr+=128;
	InterpHelperAddress=(DWORD)iCpuHelper;
	dynaHLEAudio=false;
#endif

	
/*
	LIBFile *test=hleReadLib("\\n64em\\roms\\libultra.lib");
	hleDumpLib(test);
	hleFreeLib(test);
	*/
}

void iCpuDestruct()
{
#ifdef CCORE1
	dynaDestroy();
#endif

	SafeFree(r);
	theApp.LogMessage("calls to 0x853 %d",stats);
}

void iCpuReset()
{
	memset(r,0,sizeof(RS4300iReg));

    r->DoOrCheckSthg = 0;

	r->PC = 0xa4000040;

    for(int i=0; i<64; i++)
	{
            r->GPR[i] = 0;
	}
  
	if((rom->Info.CountryCode=='E')||(rom->Info.CountryCode=='J'))
		r->GPR[20*2] = 0x00000001;
	else
		r->GPR[20*2] = 0x00000000;
    r->GPR[22*2] = theApp.m_BootCode;
	r->PC=0x388;

    r->GPR[29*2] = 0xa4001ff0;
//    r->GPR[29*2] = 0x00400000;
    r->Hi = 0;
    r->Lo = 0;
    r->Llbit = 0;
    r->Delay = NO_DELAY;
	r->CurRoundMode=0x027f;		// rounds to nearest. special for Intel FP
	r->RoundMode=0x027f;		// rounds to nearest. special for Intel FP
	r->TruncMode=0x0e7f;		// truncates. special for Intel FP
	r->CeilMode=0x0a7f;			// rounds up. special for Intel FP
	r->FloorMode=0x067f;		// rounds down. special for Intel FP
    r->CPR0[2*STATUS] = 0x50400004;
//    r->CPR0[2*STATUS] = 0x70400004;
    r->CPR0[2*RANDOM] = 0x0000002f;
    r->CPR0[2*CONFIG] = 0x00066463;
//    r->CPR0[2*PRID]   = 0x00000b00;
    r->CCR1[0]      = 0x00000511;
	r->ICount=1;
	r->NextIntCount=6250000;
	r->CompareCount=0;
	r->VTraceCount=6250000;


/*
*/
	r->PC=0x03c00000;

//	rcFinderConstruct();
//	rcFinderLocateFunctions((DWORD *)rom->Image,0xdec,512);
//	rcFinderDestruct();
}

UINT8 iDspThreadProc()
{
	//Sleep(1000);
	svcOutputDebugString("iDspThreadProc()", 20);
	 
	DWORD tmp,tmp2;
	tmp=tmp2=0;
#define FULL_HLE
#ifdef FULL_HLE
	while(1)
	{
		switch(DspTask)
		{
		case DEBUG_SINGLESTEP:
		case NORMAL_GAME:
			{
				if(tmp==0)
				{
					*(WORD *)&m->dspDMem[0x39fc<<1]=1;
					hleDSPInit();
					tmp=1;
				}
				hleDSPMain();
				hleDSPMain2();

				if(hleDSPPingTime==0) hleDSPPingTime=armGetSystemTick ()+7;
				WORD tmp=*(WORD *)&m->dspDMem[0x60c*2];
				if(tmp==0x400)
				{
					int diff;

					tmp=0x4f0;
					dspUpdateCount++;
					theApp.m_EmuObj->UpdateAudio(0x400);
					diff=hleDSPPingTime-armGetSystemTick ();
					if(diff>0)
					{
						//Sleep(diff);
						hleDSPPingTime+=7;
					}
					else
					{
						hleDSPPingTime+=7;
						if(diff<-500) hleDSPPingTime=armGetSystemTick()+7;
					}
				}
				else
				{
					int diff;
					tmp=0x400;
					theApp.m_EmuObj->UpdateAudio(0x4f0);
					diff=hleDSPPingTime-armGetSystemTick();
					if(diff>0)
					{
						//Sleep(diff);
						hleDSPPingTime+=8;
					}
					else
					{
						hleDSPPingTime+=8;
						if(diff<-500) hleDSPPingTime=armGetSystemTick()+8;
					}
				}
				*(WORD *)&m->dspDMem[0x60c*2]=tmp;
				hleDSPMain3();
				break;
			}
		case 0xff:
			{
				//Sleep(1000);
				break;
			}
		}
	}
#else
	while(1)
	{
		switch(DspTask)
		{
		case ERROR_BREAK:
			{
				svcOutputDebugString("iDspThreadProc() : ERROR_BREAK", 40); 
				DspTask=EXIT_EMU;
				break;
			}
		case DEBUG_SINGLESTEP:
		case NORMAL_GAME:
			{
				*(WORD *)&m->dspDMem[0x39fc<<1]=1;
				DWORD pc=adsp2100_get_pc();
/*
				if(pc==0x0177)
				{
					pc++;
					adsp2100_set_pc(pc);
					hleDSPMama();
				}
				if(pc==0x1e7)
				{
					pc=0x1ac;
					adsp2100_set_pc(pc);
					hleDSPFuncB(0,0);
				}
*/
/*
				if(pc==0x97a)
				{
					pc++;
					adsp2100_set_pc(pc);
					DWORD page=*(WORD *)&m->dspDMem[0x390f*2];
					page=dspBank<<12;
					WORD m3=adsp2100_get_reg(ADSP2100_M3);
					hleDSPSkipSample(m3,page);
				}
				if(pc==0x166)
				{
					if(*(DWORD *)&m->dspPMem[0x166*4]==0x1C802F)
					{
						pc++;
						adsp2100_set_pc(pc);
						hleDSPDecode();
					}
				}
				if(pc==0xa4)
				{
					if(*(DWORD *)&m->dspPMem[0xa4*4]==0x1C168F)
					{
						pc++;
						adsp2100_set_pc(pc);
						WORD ar=adsp2100_get_reg(ADSP2100_AR);
						hleDSPProcess(ar);
					}
				}
				if(pc==0x23a)
				{
					if(*(DWORD *)&m->dspPMem[0x23a*4]==0x1C34BF)
					{
						hleDSPFuncC();
						pc=0x1ac;
						adsp2100_set_pc(pc);
					}

				}
				if(pc==0x265)
				{
					if(*(DWORD *)&m->dspPMem[0x265*4]==0x1C35EF)
					{
						hleDSPFuncD();
						pc=0x1ac;
						adsp2100_set_pc(pc);
					}

				}
				if(pc==0x268)
				{
					if(*(DWORD *)&m->dspPMem[0x268*4]==0x1C34BF)
					{
						hleDSPFuncE();
						pc=0x1ac;
						adsp2100_set_pc(pc);
					}

				}
				if(pc==0x26d)
				{
					if(*(DWORD *)&m->dspPMem[0x26d*4]==0x1C34BF)
					{
						hleDSPFuncF();
						pc=0x1ac;
						adsp2100_set_pc(pc);
					}

				}
				if(pc==0x2af)
				{
					if(*(DWORD *)&m->dspPMem[0x2af*4]==0x40007A)
					{
						WORD ay1;
						ay1=adsp2100_get_reg(ADSP2100_AY1);
						hleDSPFuncH(ay1);
						pc=0x1ac;
						adsp2100_set_pc(pc);
					}

				}
				if(pc==0x2b5)
				{
					if(*(DWORD *)&m->dspPMem[0x2b5*4]==0x4000AA)
					{
						WORD ay1;
						ay1=adsp2100_get_reg(ADSP2100_AY1);
						hleDSPFuncI(ay1);
						pc=0x1ac;
						adsp2100_set_pc(pc);
					}

				}
				if(pc==0x2e9)
				{
					if(*(DWORD *)&m->dspPMem[0x2e9*4]==0x1C34BF)
					{
						hleDSPFuncK();
						pc=0x1ac;
						adsp2100_set_pc(pc);
					}

				}
				if(pc==0x2ee)
				{
					if(*(DWORD *)&m->dspPMem[0x2ee*4]==0x4FFF40)
					{
						hleDSPFuncL();
						pc=0x1ac;
						adsp2100_set_pc(pc);
					}

				}
				if(pc==0x803)
				{
					if(*(DWORD *)&m->dspPMem[0x803*4]==0x805FB8)
					{
						pc=0x852;
						adsp2100_set_pc(pc);
						hleDSPNextSample();
					}
				}
				if(pc==0x332)
				{
					pc=0x33e;
					WORD mr0;
					mr0=adsp2100_get_reg(ADSP2100_MR0);
					hleDSPResetBlocks(mr0);
					adsp2100_set_pc(pc);
				}

				if(pc==0x1d6)
				{
					pc=0x1e6;
					WORD m2,m3;
					m2=adsp2100_get_reg(ADSP2100_M2);
					m3=adsp2100_get_reg(ADSP2100_M3);
					hleDSPFuncA(m3,m2);
					adsp2100_set_pc(pc);
				}
				if(pc==0x50)
				{
					pc++;
					adsp2100_set_pc(pc);
					DWORD value;
					WORD offset;
					value=adsp2100_get_reg(ADSP2100_AR);
					value|=(adsp2100_get_reg(ADSP2100_PX)<<16);
					offset=adsp2100_get_reg(ADSP2100_MR0);
					hleDSPResetPtrs(value,offset);
				}
*/
/*
				if(pc==0x7d)
				{
					if(*(DWORD *)&m->dspPMem[0x7d*4]==0x343880)
					{
						hleDSPMain2();
						pc=0x12c;
						adsp2100_set_pc(pc);
					}
				}
*/
/*
				if(pc==0x1ac)
				{
					if(*(DWORD *)&m->dspPMem[0x1ac*4]==0x1C35EF)
					{
						hleDSPDispatch();
						pc=0x1c4;
						adsp2100_set_pc(pc);
					}

				}
*/
#define USE_MAMEn
#ifndef USE_MAME
				if((pc==0)||(pc==8))
				{
					hleDSPInit();
					pc=0x21;
					adsp2100_set_pc(pc);
				}
				if(pc==0x21)
				{
					hleDSPMain();
					hleDSPMain2();

					if(hleDSPPingTime==0) hleDSPPingTime=armGetSystemTick()+7;
					WORD tmp=*(WORD *)&m->dspDMem[0x60c*2];
					if(tmp==0x400)
					{
						int diff;

						tmp=0x4f0;
						adsp2100_set_reg(ADSP2100_I7,0x400);
						dspUpdateCount++;
						theApp.m_EmuObj->UpdateAudio(0x400);
						diff=hleDSPPingTime-armGetSystemTick();
						if(diff>0)
						{
							//Sleep(diff);
							hleDSPPingTime+=7;
						}
						else
						{
							hleDSPPingTime+=7;
							if(diff<-500) hleDSPPingTime=armGetSystemTick()+7;
						}
					}
					else
					{
						int diff;
						tmp=0x400;
						adsp2100_set_reg(ADSP2100_I7,0x4f0);
						theApp.m_EmuObj->UpdateAudio(0x4f0);
						diff=hleDSPPingTime-armGetSystemTick();
						if(diff>0)
						{
							//Sleep(diff);
							hleDSPPingTime+=7;
						}
						else
						{
							hleDSPPingTime+=7;
							if(diff<-500) hleDSPPingTime=armGetSystemTick()+7;
						}
					}
					*(WORD *)&m->dspDMem[0x60c*2]=tmp;
					hleDSPMain3();
					pc=0x21;
					adsp2100_set_pc(pc);
				}
#else

				if((pc>=0x12c)&&(pc<0x13b))
				{
					if(hleDSPPingTime==0) hleDSPPingTime=armGetSystemTick()+7;
					WORD tmp=*(WORD *)&m->dspDMem[0x60c*2];
					if(tmp==0x400)
					{
						int diff;

						tmp=0x4f0;
						adsp2100_set_reg(ADSP2100_I7,0x400);
						dspUpdateCount++;
						theApp.m_EmuObj->UpdateAudio(0x400);
						diff=hleDSPPingTime-armGetSystemTick();
						if(diff>0)
						{
							//Sleep(diff);
							hleDSPPingTime+=7;
						}
						else
						{
							hleDSPPingTime+=7;
							if(diff<-500) hleDSPPingTime=armGetSystemTick()+7;
						}
					}
					else
					{
						int diff;
						tmp=0x400;
						adsp2100_set_reg(ADSP2100_I7,0x4f0);
						theApp.m_EmuObj->UpdateAudio(0x4f0);
						diff=hleDSPPingTime-armGetSystemTick();
						if(diff>0)
						{
							//Sleep(diff);
							hleDSPPingTime+=7;
						}
						else
						{
							hleDSPPingTime+=7;
							if(diff<-500) hleDSPPingTime=armGetSystemTick()+7;
						}
					}
					*(WORD *)&m->dspDMem[0x60c*2]=tmp;
					pc=0x13b;
					adsp2100_set_pc(pc);
				}
				adsp2100_execute(1);
#endif
 
				 
/*
*/
				break;
			}
		}
	}
#endif
}

UINT8 iCpuThreadProc()
{
	 
	svcOutputDebugString("iCpuThreadProc()", 20);
	
	while(1)
	{
		switch(NewTask)
		{
		case ERROR_BREAK:
			{
				svcOutputDebugString("iCpuThreadProc() : ERROR_BREAK", 40);
				NewTask=EXIT_EMU;
				break;
			}
		case DEBUG_BREAK:
			{
				NewTask=DEBUG_STEPMODE;
				break;
			}
		case SAVE_GAME:
			{
				iCpuSaveGame();
				NewTask=NORMAL_GAME;
				break;
			}
		case LOAD_GAME:
			{
				iCpuLoadGame();
				NewTask=NORMAL_GAME;
				break;
			}
		case EXIT_EMU:
			{
				NewTask=0;
				//ExitThread(0); //TODO:FIX
				break;
			}
		case BOOT_STAGE0:
			{
								 			
				if((r->PC&0xff000000)==0x88000000)
					iOpCode=*(DWORD *)&m->rdRam[r->PC&MEM_MASK];
				else
					iOpCode=*(DWORD *)&rom->Image[r->PC&0x7ffff];
				r->PC+=4;
//				r->ICount++;
				 
				iMain[iOpCode>>26]();
				r->GPR[0]=0;
				r->GPR[1]=0;
				switch(r->Delay)
				{
				case DO_DELAY:
					{
						r->Delay=EXEC_DELAY;
						break;
					}
				case EXEC_DELAY:
					{
						r->Delay=NO_DELAY;
						r->PC=r->PCDelay;
						if(r->PC==0x88000000)
						{
							NewTask=NORMAL_GAME;
						}
						break;
					}
				}
				
/*
*/
				break;
			}
		case DEBUG_SINGLESTEP:
		case DEBUG_FASTSTEP:
		case NORMAL_GAME:
			{
 	 
/*
				DWORD test=iOpCode>>26;
				BYTE rd,rs,rt,sa;
				rd=MAKE_RD;
				rs=MAKE_RS;
				rt=MAKE_RT;
				sa=MAKE_SA;
				WORD im=MAKE_I;
*/
/*
				adsp2100_execute(1);
				*(WORD *)&m->dspDMem[0x39fc<<1]=1;
				if(DspTask==DEBUG_SINGLESTEP)
					DspTask=DEBUG_STEPMODE;
				DWORD pc=adsp2100_get_pc();
				if(dspAuto)
				{
					dspAutoCount+=1;
					DWORD i7=adsp2100_get_reg(ADSP2100_I7);
					if((dspAutoCount%320)==0)
					{
						i7++;
//						dspAutoCount=0;
						adsp2100_set_reg(ADSP2100_I7,i7);
					}
					if(i7>=(0x1e0+dspAutoBase))
					{
						adsp2100_set_reg(ADSP2100_I7,dspAutoBase);
						dspAutoCount=0;
						theApp.LogMessage("Update Audio");
						theApp.m_EmuObj->UpdateAudio();
						adsp2105_set_irq_line(ADSP2105_IRQ1, 1);
					}
				}
*/
#ifdef CCORE1
				if(NewTask==NORMAL_GAME)
				{
					if((r->PC&0xff000000)==0x88000000)
					{
						if((r->PC&MEM_MASK)>=0x400)
						{
							if(r->Delay==NO_DELAY)
							{
								if(dynaExecute(r->PC&MEM_MASK))		// attempts to execute, compile if not already and execute returns
								{
									r->PC|=0x88000000;
	//								if(r->Delay==DO_DELAY)
	//									r->Delay=EXEC_DELAY;
									goto SkipInterpreter;
								}
								r->PC|=0x88000000;
							}
						}
					}
				}
#endif
/*
				if((r->PC&0x7fffff)==0x107A8)
				{
					NewTask=ERROR_BREAK;
				}
*/
				if((r->PC&0xff000000)==0x88000000)
				{
					iOpCode=*(DWORD *)&m->rdRam[r->PC&MEM_MASK];
					if(!dump)
					{
						dump=true;
						fstream *tmpf=(fstream *)new fstream();
						if(gRomSet==KI2)
						{
							tmpf->open("ki2.dat", ios::in | ios::out | ios::binary);
						}
						else if(gRomSet==KI1)
						{
							tmpf->open("ki.dat",ios::in | ios::out | ios::binary);
						}
						tmpf->write((char *)m->rdRam,0x100000);
						tmpf->write((char *)r,sizeof(RS4300iReg));
						tmpf->close();
						delete tmpf;
					}
				}
				else
					iOpCode=*(DWORD *)&rom->Image[r->PC&0x7ffff];


				r->PC+=4;
//				r->ICount++;

				iMain[iOpCode>>26]();
//				r->PCDelay=(r->PCDelay&MEM_MASK)|0x80000000;
				r->GPR[0]=0;
				r->GPR[1]=0;
				switch(r->Delay)
				{
				case DO_DELAY:
					{
						r->Delay=EXEC_DELAY;
						break;
					}
				case EXEC_DELAY:
					{
						r->Delay=NO_DELAY;
						if((r->PC&0xff000000)==0x88000000)
							r->PCDelay|=0x88000000;
						r->PC=r->PCDelay;
						break;
					}
				}
				iCpuCheckInts();
//				if((dspCount%6)==0)
//					iCpuStepDSP();
				dspCount++;

SkipInterpreter:

//				if((r->PC&0x7fffff)==0x29f90) NewTask=ERROR_BREAK;
				 
/*
*/

				break;
			}
		}
	}
}

void iCpuStepDSP()
{
				adsp2100_execute(1);
				*(WORD *)&m->dspDMem[0x39fc<<1]=1;
				if(DspTask==DEBUG_SINGLESTEP)
					DspTask=DEBUG_STEPMODE;
				DWORD pc=adsp2100_get_pc();
				if(dspAuto)
				{
					dspAutoCount+=1;
					DWORD i7=adsp2100_get_reg(ADSP2100_I7);
					if((dspAutoCount%160)==0)
					{
						i7++;
//						dspAutoCount=0;
						adsp2100_set_reg(ADSP2100_I7,i7);
					}
					if(i7>=(0x1e0+dspAutoBase))
					{
						adsp2100_set_reg(ADSP2100_I7,dspAutoBase);
						dspAutoCount=0;
						theApp.LogMessage("Update Audio");
						theApp.m_EmuObj->UpdateAudio(0x400);
						adsp2105_set_irq_line(ADSP2105_IRQ1, 1);
/*
						DspTask=DEBUG_STEPMODE;
						DWORD tmp=0;
						if(pc>0x100)
							tmp=pc-0x100;
						m_Debugger->BreakDSP(tmp);
*/
					}
				}
}

void iCpuDoNextOp()
{
	DWORD tmp=iOpCode;
	iOpCode=*(DWORD *)&m->rdRam[r->PC&MEM_MASK];
	iMain[iOpCode>>26]();
	r->GPR[0]=0;
	r->GPR[1]=0;
	iOpCode=tmp;
}

void iCpuHelper(DWORD OpCode)
{

	r->Code=OpCode;
	iOpCode=OpCode;
//	r->ICount++;
	iMain[iOpCode>>26]();
	r->GPR[0]=0;
	r->GPR[1]=0;
}

void iOpSpecial()
{
	iSpecial[iOpCode&0x3f]();
}

void iOpRegimm()
{
	iRegimm[(iOpCode>>16)&0x1f]();
}

void iOpCop0()
{
	iCop0RS[(iOpCode>>21)&0x1f]();
}

void iOpCop1()
{
	iCop1RS[(iOpCode>>21)&0x1f]();
}

void iOpCop2()
{
	iCop2RS[(iOpCode>>21)&0x1f]();
}

void iOpBc0()
{
	iCop0RT[(iOpCode>>16)&0x1f]();
}

void iOpBc1()
{
	iCop1RT[(iOpCode>>16)&0x1f]();
}

void iOpBc2()
{
//	iCop2RT[(iOpCode>>16)&0x1f]();
}

void iOpCo0()
{
	iCop0[iOpCode&0x3f]();
}

void iOpCo1()
{
	iCop1[iOpCode&0x3f]();
}

void iOpVector()
{
//	Cop2Vector[iOpCode&0x3f]();
}

void iCpuVSYNC()
{
	//Sleep(100);
//	if(dynaVCount<5)
//		return;
	if(iCpuNextVSYNC==0)
		iCpuNextVSYNC=armGetSystemTick()+theApp.m_FrameDelay;
	if(theApp.m_LockOn)
	{
		int diff=(iCpuNextVSYNC-armGetSystemTick());


		if(lagging)
		{
			lagging=0;
			if(diff>0)
			{
				//Sleep(diff);
			}
		}
		else
		{
			if(diff>0)
			{
				//Sleep(diff);
			}
			else if(diff<-theApp.m_FrameDelay)
				lagging=1;
		}
		iCpuNextVSYNC+=theApp.m_FrameDelay;
/*
		if(hleFMVDelay)
			iCpuNextVSYNC+=theApp.m_FrameDelay;
		hleFMVDelay=false;
*/		iCpuVSYNCAccum+=40;
		if(iCpuVSYNCAccum>60)
		{
			iCpuNextVSYNC+=1;
			iCpuVSYNCAccum-=60;
		}
	}
	{
		r->ICount=r->NextIntCount;
		r->VTraceCount+=(theApp.m_VTraceInc);
		r->NextIntCount=r->VTraceCount;
		
		/* TODO:FIX
		dynaVCount++;
		if(gRomSet==KI2)
			hleISR2();
		else if(gRomSet==KI1)
			hleISR();
		*/
/*
			r->CPR0[2*CAUSE] |= 0x8000;

			if((r->CPR0[2*STATUS] & 0x8001) != 0x8001)
			{
		//		r->DoOrCheckSthg &= ~CHECK_MI_INTERRUPTS;
				return;
			}

			if(r->CPR0[2*12] & 0x0006)
					return;

			r->CPR0[2*CAUSE] &= ~EXC_CODE__MASK;

			r->CPR0[2*STATUS] |= 0x00000002;              
			r->CPR0[2*EPC]  = r->PC;                  

			if(r->Delay == EXEC_DELAY)
			{
					r->CPR0[2*CAUSE] |= 0x80000000;       
					r->CPR0[2*EPC]   -= 4;                

			} 
			else
			{
					r->CPR0[2*CAUSE] &= ~0x80000000;      
			}

			r->PC = 0x88000180;                           
*/
	}
}

void iCpuCheckVSYNC()
{
}

void iCpuSetFPUFlag()
{
	return;
	DWORD op3=*(DWORD *)&m->rdRam[0x180];
	DWORD Address;
	if((op3&0xff000000)==0x3c000000)
	{
		short op4=*(WORD *)&m->rdRam[0x184];
		Address=(((DWORD)op3<<16)+op4);
		DWORD *patch=(DWORD *)&m->rdRam[Address&MEM_MASK];
		DWORD a0=patch[0x38/4];
		DWORD a1=patch[0x38/4+1];
		a0=((a0&0xffff)<<16)+(short)(a1&0xffff);
		DWORD *thread=(DWORD *)&m->rdRam[a0&MEM_MASK];
		thread[6]=0xffffffff;
	}
}

void iCpuCheckInts()
{
	return;

	if(iCpuNextVSYNC==0)
		iCpuNextVSYNC=armGetSystemTick()+16;
	if(theApp.m_LockOn)
	{
		int diff=(iCpuNextVSYNC-armGetSystemTick());
		if(diff>0)
		{
//			Sleep(diff);
			return;
		}
/*
		if(dspUpdateCount)
			while(dspUpdateCount%32)
			{
				Sleep(1);
			}
*/
		iCpuNextVSYNC+=16;
		iCpuVSYNCAccum+=40;
		if(iCpuVSYNCAccum>60)
		{
			iCpuNextVSYNC+=1;
			iCpuVSYNCAccum-=60;
		}
	}
//	if((r->NextIntCount <= r->ICount)) //&&!r->DoOrCheckSthg) 
	{
		r->ICount=r->NextIntCount;
		r->VTraceCount+=(833333);
		r->NextIntCount=r->VTraceCount;
		dynaVCount++;
		hleISR();
	}
	return;

	if(r->Delay!=NO_DELAY)
		return;

	if((r->NextIntCount <= r->ICount)) //&&!r->DoOrCheckSthg) 
	{
//		if(((r->NextIntCount & 0xffffffff) == r->CompareCount)&&(r->CompareCount))
//		if((r->CompareCount)&&(r->CompareCount<r->ICount))
		{
//			r->NextIntCount=r->CompareCount+625000;
			r->ICount=r->NextIntCount;
			r->CompareCount=0;
			r->VTraceCount+=(833333);
			r->NextIntCount=r->VTraceCount;

			/* set IP7 in CAUSE reg */
			r->CPR0[2*CAUSE] |= 0x8000;

			/* if IP7 or IE bits in STATUS reg are clr: return */
			if((r->CPR0[2*STATUS] & 0x8001) != 0x8001)
			{
		//		r->DoOrCheckSthg &= ~CHECK_MI_INTERRUPTS;
				return;
			}

			/* if EXL or ERL bits in status reg are set: return */
			if(r->CPR0[2*12] & 0x0006)
					return;

			/* set ExcCode to Int (0) */
			r->CPR0[2*CAUSE] &= ~EXC_CODE__MASK;

			r->CPR0[2*STATUS] |= 0x00000002;              
			r->CPR0[2*EPC]  = r->PC;                  

			if(r->Delay == EXEC_DELAY)
			{
					r->CPR0[2*CAUSE] |= 0x80000000;       
					r->CPR0[2*EPC]   -= 4;                

			} 
			else
			{
					r->CPR0[2*CAUSE] &= ~0x80000000;      
			}

//			NewTask=ERROR_BREAK;
			r->PC = 0x88000180;                           
			r->Delay = NO_DELAY;                          
			r->Break=r->Lo;
			r->LastPC=r->Hi;
/*
*/
//			NewTask=ERROR_BREAK;
			return;
		}
/*
//		if(((DWORD *)m->aiReg)[1])
		{
			aiUsed+=aiUsedW;
			aiUsedA+=aiUsedF;
//			((DWORD *)m->aiReg)[1]-=(aiUsedW/2);
			if(aiUsedA>60)
			{
				aiUsed+=aiUsedW;
//				((DWORD *)m->aiReg)[1]-=(aiUsedW/2);
				aiUsedA-=60;
			}
			if(aiUsed>(aiBufLen/4))
			{
				aiUsed-=(aiBufLen/4);
//				aiDataUsed+=aiBufLen;
				r->DoOrCheckSthg|=MI_INTR_AI;
				((DWORD *)m->aiReg)[3] = 0x0000000;		// we are now not busy
				((DWORD *)m->aiReg)[1] = 0x0000000;		// we are now not busy
//				return;
			}
		}
*/
/*
		r->NextIntCount=r->ICount+(833333);
		r->NextIntCount&=0xffffffff;
		if(r->NextIntCount<r->ICount)
			r->ICount=0;
		r->VTraceCount=r->NextIntCount;
		if(((r->NextIntCount&0xffffffff)>r->CompareCount)&&(r->CompareCount))
		{
			r->NextIntCount&=0xffffffff00000000;
			r->NextIntCount|=(DWORD)r->CompareCount;
		}
*/
	}
}


void iCpuCheckFPU()
{
	if(r->Delay!=NO_DELAY)
		return;

	if((!(r->CPR0[2*STATUS]&0x20000000)))
	{
		r->CPR0[2*COUNT] = (DWORD) r->ICount;

//		r->CPR0[2*CAUSE] |= 0x0400;	// set IP2 in CAUSE reg 
		// if IP2 or IE bits in STATUS reg are clr: return 
		if((r->CPR0[2*STATUS] & 0x0401) != 0x0401)
				return;

		// if EXL or ERL bits in STATUS reg are set: return 
		if(r->CPR0[2*STATUS] & 0x0006)
				return;

		// set ExcCode to Int (0) 
		r->CPR0[2*CAUSE] &= ~EXC_CODE__MASK;

		r->CPR0[2*CAUSE] |= (11<<2);

		r->CPR0[2*STATUS] |= 0x00000002;              
		r->CPR0[2*EPC]  = r->PC;                  

		if(r->Delay == EXEC_DELAY)
		{
				r->CPR0[2*CAUSE] |= 0x80000000;       
				r->CPR0[2*EPC]   -= 4;                

		} 
		else
		{
				r->CPR0[2*CAUSE] &= ~0x80000000;      
		}
		if(!(r->CPR0[2*STATUS]&0x20000000))
		{
			r->CPR0[2*CAUSE]&=~0x30000000;		// clear out usable
			r->CPR0[2*CAUSE]|=0x10000000;		// fake COP1 unusable
		}

		r->PC = 0x80000180;                           
		r->Delay = NO_DELAY;                          

		r->DoOrCheckSthg = 0;
	}
}


void iOpReserved()
{
	NewTask=ERROR_BREAK;
}

void iOpSyscall()
{
	NewTask=ERROR_BREAK;
}

void iOpBreak()
{
	NewTask=ERROR_BREAK;
}

void iOpInvalid()
{
	NewTask=ERROR_BREAK;
}

void iOpCop2Reserved()
{
}

void iOpSync()
{
	NewTask=ERROR_BREAK;
}

void iOpTge()
{
}

void iOpTgeu()
{
}

void iOpTlt()
{
}

void iOpTltu()
{
}

void iOpTeq()
{
}

void iOpTne()
{
}

void iOpCache()
{
//	return;

	DWORD op=MAKE_RT;
	DWORD Page,Offset;
	DWORD Address=(sDWORD)(r->GPR[MAKE_RS*2] + MAKE_I);

//	if(op==0)
//	theApp.BPMessage("Cache %X %X",op,Address);


	if((op&1)==0) //instruction cache
	{
		Page=  (Address&0x003fc000)>>14;
		Offset=(Address&0x00003ffc)>>2;
		
		
#ifdef DYNAREC		
		if((dynaPageTable[Page].Ofsfset[Offset]!=NULL))
		{
	//		dynaInvalidate(Address,16);
			dynaPageTable[Page].Value[Offset]=0;
			dynaPageTable[Page].Value[Offset+1]=0;
			dynaPageTable[Page].Value[Offset+2]=0;
			dynaPageTable[Page].Value[Offset+3]=0;
		}
#endif		
	}
}

void iOpTgei()
{
}

void iOpTgeiu()
{
}

void iOpTlti()
{
}

void iOpTltiu()
{
}

void iOpTeqi()
{
}

void iOpTnei()
{
}

void test()
{
	DWORD *p1=(DWORD *)0x3ff01234;
	DWORD *p2=(DWORD *)0x3ff01000;
	DWORD *p3=(DWORD *)0x3e001000;
	DWORD tmp;

	p3+=0x200;
	do
	{
		tmp=*(p1++)+*(p2++);
		*(p3++)=tmp;
	} while(tmp!=0);
}

void iCpuSaveGame()
{
	char szFile[50];
	
	if (gRomSet==KI1)
		sprintf(szFile,"KI1.dat");
	else
		sprintf(szFile,"KI2.dat");

	FILE *file=NULL;
	file=fopen(szFile,"wb");
//	hleRdpEvictN64Textures();
	iMemSave(file);
	r->PC|=0x88000000;
	fwrite(r,sizeof(RS4300iReg),1,file);
	fclose(file);
}

void iCpuLoadGame()
{
	char szFile[50];
	
	if (gRomSet==KI1)
		sprintf(szFile,"KI1.dat");
	else
		sprintf(szFile,"KI2.dat");

	FILE *file=NULL;
	file=fopen(szFile,"rb");
//	hleRdpEvictN64Textures();
	iMemLoad(file);
	fread(r,sizeof(RS4300iReg),1,file);
	fclose(file);
	r->PC|=0x88000000;
	//dynaInit();
}


void iCpuCreateThreadHeader()
{
	DWORD addr=r->GPR[4*2];
	DWORD *thread=(DWORD *)&m->rdRam[addr&MEM_MASK];
//	thread->prv=0x0001;
//	theApp.LogMessage("SaveFP %X",thread[6]);
	thread[6]=1;
}