
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#define GPR0_OPTIMIZATION	// basic optimization, assume GPR 0 = 0
//#define RD_EQUAL_RS			// basic optimization, if RD=RS do something easier; ie; +=

#include "iMain.h"
#include "iCPU.h"			// Core 4600 emulation routines
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines
#include "dynaCompiler.h"
#include "adsp2100.h"
#include "hleDSP.h"


volatile WORD DspTask;
volatile WORD NewTask;
extern bool dump;

RS4300iReg *r;		// the 'registers', ie; a area of memory that contains all registers sets for the
					// 4300 along with some helpful states and variables.

N64Mem *m;			// the 'memory'.  The N64 uses memory mapped i/o.  This structure contains arrays for
					// each significant memory mapped range.
N64RomStruct *rom;

DWORD iOpCode;		// current OpCode
DWORD iNextOpCode;	// next OpCode
DWORD iPC;			// copy of current PC
WORD iFPUMode=0x027f;		// rounds to nearest. special for Intel FP;

char iRegName[32][3]= {
	"R0","At","V0","V1","A0","A1","A2","A3",
	"T0","T1","T2","T3","T4","T5","T6","T7",
	"S0","S1","S2","S3","S4","S5","S6","S7",
	"T8","T9","K0","K1","GP","SP","S8","RA" };



// 'Reset' the main CPU and setup for processing as a N64

void iMainConstruct(char *filename)
{
	iRomConstruct();
	iMemConstruct();
	iCpuConstruct();
	iDspThreadId=NULL;
	hleDSPConstruct();
}

void iMainDestruct()
{
	hleDSPDestruct();
	iMemDestruct();
	iRomDestruct();
	iCpuDestruct();
	adsp2100_exit();
}

void iMainReset()
{
	iCpuReset();
	dspBank=1;
	iMainResetDSP();
}

void iMainResetDSP()
{
	DWORD *dst,*src,tmp;
	src=(DWORD *)&m->dspRMem[dspBank<<12];
	dst=(DWORD *)m->dspPMem;
	DWORD size=*src;
	size>>=24;
	size++;
	size<<=3;
	while(size)
	{
		tmp=*(src++);
		tmp=(tmp&0xff0000)>>16|(tmp&0xff)<<16|(tmp&0xff00);
		*(dst++)=tmp;
		size--;
	}
	adsp2100_reset(NULL);
}

void iMainStartCPU()
{
	int dwThrdParam = 0;
	unsigned long dwThreadId = 0;
	NewTask=BOOT_STAGE0;
	CFile *tmpf=(CFile *)new CFile();
	bool results;

	if(gRomSet==KI2)
		results=tmpf->Open("ki2.dat",CFile::modeReadWrite);
	else if(gRomSet==KI1)
		results=tmpf->Open("ki.dat",CFile::modeReadWrite);

	if(results)
	{
//		iMemLoad(tmpf);
		tmpf->Read(m->rdRam,0x100000);
		tmpf->Read(r,sizeof(RS4300iReg));
		tmpf->Close();
//		r->PC=0x88000000;
//		dynaInit();
		NewTask=NORMAL_GAME;
		dump=true;
	}
	delete tmpf;
	adsp2100_set_pc(0);
	DspTask=NORMAL_GAME;
//	if(theApp.m_EmuObj->m_Debug)
//		DspTask=DEBUG_STEPMODE;
	DspTask=NORMAL_GAME;
/*
*/
/*
						tmpf=(CFile *)new CFile();
						tmpf->Open("ki.dat",CFile::modeReadWrite|CFile::modeCreate);
						tmpf->Write(m->rdRam,0x80000);
						tmpf->Write(r,sizeof(RS4300iReg));
						tmpf->Close();
						delete tmpf;
*/
	iCpuThreadId = CreateThread( NULL, 0, 
		(LPTHREAD_START_ROUTINE) iCpuThreadProc,
		&dwThrdParam, 0, &dwThreadId);	
	SetThreadPriority(iCpuThreadId,THREAD_PRIORITY_NORMAL);

	iDspThreadId = CreateThread( NULL, 0, 
		(LPTHREAD_START_ROUTINE) iDspThreadProc,
		&dwThrdParam, 0, &dwThreadId);	
	SetThreadPriority(iDspThreadId,THREAD_PRIORITY_NORMAL);
/*
*/
} 

void iMainStopCPU()
{
	NewTask=EXIT_EMU;
	DspTask=EXIT_EMU;
	DWORD WaitLimit=timeGetTime()+2000;
	while(NewTask&&(timeGetTime()<WaitLimit))
	{
		Sleep(200);
	}

	if((iCpuThreadId!=NULL)&&(NewTask!=0))
		TerminateThread(iCpuThreadId,0);

	if((iDspThreadId!=NULL)&&(NewTask!=0))
		TerminateThread(iDspThreadId,0);
}