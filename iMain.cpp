
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


#define STACKSIZE (8 * 1024)

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

static u64 s_startTicks;

char iRegName[32][3]= {
	"R0","At","V0","V1","A0","A1","A2","A3",
	"T0","T1","T2","T3","T4","T5","T6","T7",
	"S0","S1","S2","S3","S4","S5","S6","S7",
	"T8","T9","K0","K1","GP","SP","S8","RA" };



// 'Reset' the main CPU and setup for processing as a N64

void iMainConstruct(char *filename)
{
	s_startTicks = armGetSystemTick();
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
	Thread cpu_thread;
	Thread dsp_thread;
	
	int dwThrdParam = 0;
	unsigned long dwThreadId = 0;
	NewTask=BOOT_STAGE0;
	FILE *tmpf=NULL;
	 
	if(gRomSet==KI2)
		tmpf=fopen("ki2.dat","rb");
	else if(gRomSet==KI1)
		tmpf=fopen("ki.dat","rb");

	dump=false;
	if(tmpf)
	{
//		iMemLoad(tmpf);
		fread(m->rdRam,0x100000, 1, tmpf);
		fread(r,sizeof(RS4300iReg), 1, tmpf);
		fclose(tmpf);
//		r->PC=0x88000000;
//		dynaInit();
		NewTask=NORMAL_GAME;
		dump=true;
	}

	if (tmpf)
		fclose(tmpf);
	  
	adsp2100_set_pc(0);
	DspTask=NORMAL_GAME;	
	
	u32 prio;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);  
	threadCreate(&cpu_thread, (void (*)(void *))iCpuThreadProc, &dwThrdParam, STACKSIZE, 0x2F   , 1);
	threadStart(&cpu_thread);
	threadCreate(&dsp_thread, (void (*)(void *))iDspThreadProc, &dwThrdParam, STACKSIZE, 0x2F   , 2);
	threadStart(&dsp_thread);
 
} 

void iMainStopCPU()
{
	NewTask=EXIT_EMU;
	DspTask=EXIT_EMU;
	DWORD WaitLimit=getTime()+2000;
	while(NewTask&&(getTime()<WaitLimit))
	{
		svcSleepThread(200);
	}

	/*if((iCpuThreadId!=NULL)&&(NewTask!=0))
		TerminateThread(iCpuThreadId,0);

	if((iDspThreadId!=NULL)&&(NewTask!=0))
		TerminateThread(iDspThreadId,0); */
}


float getTime()
{
    u64 elapsed = armGetSystemTick() - s_startTicks;
    return (elapsed * 625 / 12) / 1000000000.0;
}