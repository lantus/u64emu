
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "iMain.h"
#include "iCPU.h"
#include "iMemory.h"
#include "iMemoryOps.h"

#include "dynaCompiler.h"
#include "dynains.h"
#include "dasmmain.h"
#include "dynaNative.h"		// functions that map to native processor
#include "dynaFP.h"		// floating point routines
#include "dynaMemory.h" // memory routines
#include "dynaGeneral.h" // general routines
#include "dynaBranch.h" // branch instructions
#include "dynaBranchSP.h" // 'Same Page' branch instructions
#include "dynaFastMem.h" // fast memory routines; include TLB support
#include "dynaSmartMem.h"

#include "hleMain.h"


#define TRWIN_32
#define TRWIN


#define PATCH0n
#define PATCH1n
#define PATCH2n
#define PATCH3n

#define BASIC_MEMn
#define SMART_MEMn
#define NEW_MEMn
#define ALLOW_SHIFT_EQ
#define STERN_CHECKn
#define LEFT_RIGHT

extern bool dynaIsInfinite(DWORD target,DWORD pc);

//#define BYPASS_FP				// bypass floating point ops
//#define BYPASS_LOGIC			// bypass logic ops; or, ori, and, xor etc.
//#define BYPASS_MEMn				// bypass memory ops
//#define BYPASS_MEM				// bypass memory ops
//#define BYPASS_MULT_DIV		// bypass multiply/divide ops; mult, div etc.
//#define BYPASS_MULT_DIV_D		// bypass double multiply/divide ops; ddiv, dmult etc.
//#define BYPASS_SETLESS			// bypass set on less ops; stli, stlu etc.
//#define BYPASS_SHIFTS			// bypass shift ops; sra, srav etc.
//#define BYPASS_DSHIFTS		// bypass double shift ops; dsra, dsrav etc.
//#define BYPASS_ARITH			// bypass arithmetic ops; add, addu etc.
//#define BYPASS_DARITH			// bypass double arithmetic ops; dadd, daddu etc.
//#define BYPASS_TRANSFER		// bypass transfer ops; mthi, mflo, mfc, mtc etc.
#define BYPASS_ERET			// bypass eret op
//#define UNSURE
/*
	DWORD len=LoadReg(BYTE *cp,BYTE MReg)
	Checks to see if MReg is already mapped, if so, loads from RegisterMemory into the NReg, map is marked !Dirty
	Calls GetFreeReg(), load from RegisterMemory in NReg,
	If no free registers are available, uses IsUsed to try and find a discardable NReg and uses that
	If no register can be discarded, calls FlushReg to free up a register slot.
	80201680
*/

DWORD smart=0;
DWORD smart2=0;
DWORD dumb=0;
DWORD dumb2=0;
extern DWORD mmio;
// #define PASS_MEM		// not needed now
#define SP_CATCHn
#define LUI_DELAY 1
#define JALn
#define JALRn
#define JRn
#define Jn
#define BEQn
#define BNEn

#define BEQLn
#define BNELn

#define BLTZn
#define BLEZn
#define BGTZn
#define BGEZn

#define BLTZLn
#define BLEZLn
#define BGTZLn
#define BGEZLn

#define BLTZALn
#define BLEZALn
#define BGTZALn
#define BGEZALn

#define BLTZALLn
#define BLEZALLn
#define BGEZALLn

#define BCFn
#define BCTn
#define BCFLn
#define BCTLn

extern DWORD iOpCode;
BYTE dynaCmp0,dynaCmp1;
DWORD dynaCatchPC=0;
DWORD dynaCmpI,dynaCheck,dynaCheckU;
short dynaFPointer[32];
BYTE dynaFPPage[32];

DWORD dynaTest;
DWORD dynaNextOpCode;
DWORD dynaPrvOpCode;
DWORD dynaPreCompileLength;
BYTE *dynaCompileTargetPtr;
DWORD *dynaRam;					//source for what we are compiling, must be global so that branch ops can get next instruction
WORD dynaSrc;
DWORD dynaRegPtr;				// this points to a data structure that contains the GPR,CPR,CCR,PC etc. for the 4300
DWORD dynaPCPtr;
DWORD dynaCurPage;
BYTE dynaForceFallOut;
DWORD dynaLengthTable[PAGE_SIZE];
BYTE *dynaRamPtr;
DWORD dynaLowRange=0xffffffff;
DWORD dynaHighRange=0;

dynaPageTableStruct dynaPageTable[NUM_PAGES];		// 256 pages, 4096 DWORD ptrs per page, ie; 16K per page * 256 pages = 4MB


int dynaNumBPoints=0;
DWORD dynaBPoint[1024];
bool dynaBreakOnKnown=false;
extern DWORD dasmNumLookups;
extern DWORD *dasmLookupAddy;
DWORD dynaPrimer=60;
bool dynaSkip=false;

DWORD dynaImmediates[16];
DWORD *dynaImm;
DWORD InterpHelperAddress;
DWORD helperLoadDWordAddress;
DWORD helperLoadWordAddress;
DWORD helperLoadHalfAddress;
DWORD helperLoadByteAddress;
DWORD helperStoreDWordAddress;
DWORD helperStoreWordAddress;
DWORD helperStoreHalfAddress;
DWORD helperStoreByteAddress;
DWORD helperCheckIntsAddress;
DWORD helperLDLAddress;
DWORD helperLWLAddress;
DWORD helperLDRAddress;
DWORD helperLWRAddress;
DWORD bugFinderAddress;

//DWORD ICountPtr;
//DWORD CompareCountPtr;
DWORD SneekyAddress;
DWORD dynaPC;
DWORD dynaVCount=0;
DWORD dynaICount=0;
BYTE dynaInBranch;
BYTE *dynaHold;
BOOL dynaHLEAudio=0;
bool dynaStepMode=false;
bool dynaBreakSet=false;

extern volatile WORD NewTask;

volatile WORD dynaVTRACE;

#define CHECK_RD_ZERO() if(MAKE_RD==0) return(0);
#define CHECK_RT_ZERO() if(MAKE_RT==0) return(0);


DWORD dynaCheckIntsAddress;


dynaRegMap dynaMapToNative[32];	// index is MIPS GPR, value is the Native register currently mapped to
								// Dirty flag marks the register if it has been modified since load
								// 0xff if register is not mapped

dynaRegMap dynaMapToMips[NATIVE_REGS];	// index is Native register, value is the MIPS GPR currently mapped
										// Dirty flag marks the register if it has been modified since load
										// 0xff if register is not mapped
dynaRegMap buffer[2048];

BYTE dynaNumFreeRegs;			// number of 'free' Native registers

DWORD dynaBirthYear;			// the 'year' that any newly mapped registers will be 'born' on. used to determine
								// which registers should flush out when we need space in the Native register set
								// incremented everytime a register is loaded or modified.

void dynaMarkRegDirty(BYTE MipsReg,BYTE IsDirty)
{
	dynaMapToNative[MipsReg].Dirty=IsDirty;
	dynaMapToMips[dynaMapToNative[MipsReg].Reg].Dirty=IsDirty;
}

BYTE dynaIsUsed(BYTE MipsReg)
{
	return(true);
}

BYTE dynaGetFreeReg()
{
	for(BYTE i=0;i<NATIVE_REGS;i++)
	{
		if(dynaMapToMips[i].Reg==0xff)	// 0xff marks an unmapped register
			return(i);
	}
	return(0xff);			// error case, should not be reached
}



BYTE NMAP[32]={NATIVE_0,NATIVE_1,NATIVE_2,NATIVE_3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

WORD dynaFlushReg(BYTE *cp,BYTE NReg)
{
	return(STORE_REG(cp,dynaMapToMips[NReg].Reg,NReg));		// store the Native register in Mips register memory
}

WORD dynaKillNative(BYTE NReg)
{
	return(0); //broken for now	
	dynaMapToNative[dynaMapToMips[NReg].Reg].Reg=0xff;
	dynaMapToMips[NReg].Reg=0xff;
	return(0);
}

void dynaUpdateMMIO()
{
//	return;

	for(int i=0;i<32;i++)
	{
		if(dynaFPointer[i]<=0)
			dynaFPointer[i]++;
		dynaFPointer[i]=1;
	}
}

// When dynaCompiler is 100% complete, dynaTmpFlushReg will no longer be needed.

WORD dynaTmpFlushReg(BYTE *cp,BYTE NReg)
{
	if((NReg==0xff)||(dynaMapToMips[NReg].Reg==0xff))
	{
		DWORD stop=1;
		return(0);
	}
	WORD l=0;
	WORD MipsReg=dynaMapToMips[NReg].Reg;
	if(MipsReg==0)
		return(0);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,MipsReg*8);
	l+=CONVERT_TO_QWORD(cp+l);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_3,MipsReg*8+4);

	dynaMapToNative[dynaMapToMips[NReg].Reg].Reg=0xff;
	dynaMapToMips[NReg].Reg=0xff;
	return(l);
}

WORD dynaTmpFlushRegPair(BYTE *cp,BYTE MipsReg)
{
	WORD l=0;
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,MipsReg*8);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_3,MipsReg*8+4);
	return(l);
}

void dynaSwitchMap(BYTE a,BYTE b)
{
	if((a==0xff)||(b==0xff))
	{
		DWORD stop=1;
		return;
	}
	dynaMapToNative[a].Reg=dynaMapToNative[b].Reg;
	dynaMapToMips[dynaMapToNative[a].Reg].Reg=a;
	dynaMarkRegDirty(a,true);					// mark the registers dirty

	if(a!=b)
		dynaMapToNative[b].Reg=0xff;
}

WORD dynaMakeFreeReg(BYTE *cp,BYTE *NReg)
{
	return(0);

	// first try and see if a mapped register can be freed
	DWORD BirthDate=0xffffffff;
	BYTE Oldest=0;
	for(BYTE i=0;i<NATIVE_REGS;i++)
	{
		if(dynaMapToMips[i].Dirty==false)		// must have been temporary, ok to use it
		{
			*NReg=i;
			return(0);
		}
		if(!dynaIsUsed(dynaMapToMips[i].Reg))
		{
			*NReg=i;
			return(0);		// no code needed, we found a mapped register that will not be used again
		}
		if(dynaMapToMips[i].BirthDate<BirthDate)
		{
			Oldest=i;
			BirthDate=dynaMapToMips[i].BirthDate;
		}
	}
	// kick the oldest register in the map, ie; the one with the earliest birthdate
	*NReg=Oldest;
	return(dynaFlushReg(cp,*NReg));
}

void dynaResetCompiler()
{
	dynaInBranch=false;
	for(BYTE i=0;i<NATIVE_REGS;i++)
	{
		dynaMapToMips[i].Reg=0xff;
		dynaMapToMips[i].Dirty=false;
	}

	for(i=0;i<32;i++)
	{
		dynaMapToNative[i].Reg=0xff;
		dynaMapToNative[i].Dirty=false;
		dynaFPointer[i]=0;
	}
	dynaNumFreeRegs=NATIVE_REGS;
}


WORD dynaLoadReg(BYTE *cp,BYTE MipsReg)
{
	BYTE NReg;
	WORD l=0;
/*
	if(dynaMapToNative[MipsReg].Reg!=0xff)						// register is already mapped, so we are free to use it.
	{
		NReg=dynaMapToNative[MipsReg].Reg;
		dynaMapToNative[MipsReg].BirthDate=dynaBirthYear;	// set birthdate of register
		dynaMapToMips[NReg].BirthDate=dynaBirthYear;		// set birthdate of register
		dynaMarkRegDirty(MipsReg,false);					// mark the registers not dirty
		dynaBirthYear++;									// another year goes by.... ;)
		return(0);											// no code needed.
	}
	if(dynaNumFreeRegs)										// are any Native regs available?
	{
		NReg=dynaGetFreeReg();								// yes, so get which one.
		dynaNumFreeRegs--;
	}
	else
	{
		l+=dynaMakeFreeReg(cp+l,&NReg);						// free up an Native register
	}
*/
	NReg=0;
	dynaMapToNative[MipsReg].Reg=NReg;						// update register map
	dynaMapToNative[MipsReg].BirthDate=dynaBirthYear;		// set birthdate of register
	dynaMapToMips[NReg].Reg=MipsReg;						// update register map
	dynaMapToMips[NReg].BirthDate=dynaBirthYear;			// set birthdate of register
	dynaMarkRegDirty(MipsReg,false);						// mark the registers not dirty
	dynaBirthYear++;										// another year goes by.... ;)
	if(MipsReg==0)
	{
		l+=ZERO_REG(cp+l,NATIVE_0);
	}
	else
	{
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,MipsReg*8);
	}
	return(l);
}

// for the Intel, this will force EAX and EDX to be flushed (if needed) and loaded with a unsigned __int64

WORD dynaLoadRegPair(BYTE *cp,BYTE MipsReg)
{
	BYTE NReg;
	WORD l=0;
/*
	if(dynaMapToNative[MipsReg].Reg!=0xff)						// register is already mapped, so we are free to use it.
	{
		NReg=dynaMapToNative[MipsReg].Reg;
		dynaMapToNative[MipsReg].BirthDate=dynaBirthYear;	// set birthdate of register
		dynaMapToMips[NReg].BirthDate=dynaBirthYear;		// set birthdate of register
		dynaMarkRegDirty(MipsReg,false);					// mark the registers not dirty
		dynaBirthYear++;									// another year goes by.... ;)
		return(0);											// no code needed.
	}
	if(dynaNumFreeRegs)										// are any Native regs available?
	{
		NReg=dynaGetFreeReg();								// yes, so get which one.
		dynaNumFreeRegs--;
	}
	else
	{
		l+=dynaMakeFreeReg(cp+l,&NReg);						// free up an Native register
	}
*/
	NReg=0;
	dynaMapToNative[MipsReg].Reg=NReg;						// update register map
	dynaMapToNative[MipsReg].BirthDate=dynaBirthYear;		// set birthdate of register
	dynaMapToMips[NReg].Reg=MipsReg;						// update register map
	dynaMapToMips[NReg].BirthDate=dynaBirthYear;			// set birthdate of register
	dynaMarkRegDirty(MipsReg,false);						// mark the registers not dirty
	dynaBirthYear++;										// another year goes by.... ;)
	if(MipsReg==0)
	{
		l+=ZERO_REG(cp+l,NATIVE_0);
		l+=ZERO_REG(cp+l,NATIVE_3);
	}
	else
	{
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,MipsReg*8);
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_3,MipsReg*8+4);
	}
	return(l);
}


WORD dynaLoadRegForceNot(BYTE *cp,BYTE MipsReg,BYTE ExcludedNativeReg)
{
	BYTE NReg;
	WORD l=0;
/*
	if(dynaMapToNative[MipsReg].Reg!=0xff)						// register is already mapped, so we are free to use it.
	{
		if(dynaMapToNative[MipsReg].Reg!=ExcludedNativeReg)
		{
			NReg=dynaMapToNative[MipsReg].Reg;
			dynaMapToNative[MipsReg].BirthDate=dynaBirthYear;	// set birthdate of register
			dynaMapToMips[NReg].BirthDate=dynaBirthYear;		// set birthdate of register
			dynaMarkRegDirty(MipsReg,false);					// mark the registers not dirty
			dynaBirthYear++;									// another year goes by.... ;)
			return(0);											// no code needed.
		}
	}
	if(dynaMapToMips[ExcludedNativeReg].Reg!=0xff)
	{
		l+=dynaFlushReg(cp+l,ExcludedNativeReg);
	}
*/
	NReg=ExcludedNativeReg;
	if(NReg!=0) NReg--;
	else NReg++;			// pick a reg on either side
/*
	if(dynaMapToMips[NReg].Reg!=0xff)
	{
		l+=dynaFlushReg(cp+l,NReg);
	}
*/
	dynaMapToNative[MipsReg].Reg=NReg;						// update register map
	dynaMapToNative[MipsReg].BirthDate=dynaBirthYear;		// set birthdate of register
	dynaMapToMips[NReg].Reg=MipsReg;						// update register map
	dynaMapToMips[NReg].BirthDate=dynaBirthYear;			// set birthdate of register
	dynaMarkRegDirty(MipsReg,false);						// mark the registers not dirty
	dynaBirthYear++;										// another year goes by.... ;)
	l+=LOAD_REG(cp+l,MipsReg,NReg);						// create code to load this register
	return(l);
}


WORD dynaLoadRegForce(BYTE *cp,BYTE MipsReg,BYTE ForcedNativeReg)
{
	BYTE NReg;
	WORD l=0;
/*
	if(dynaMapToNative[MipsReg].Reg!=0xff)						// register is already mapped, so we are free to use it.
	{
		if(dynaMapToNative[MipsReg].Reg==ForcedNativeReg)		// amazingly, this registers is already mapped correctly
		{
			NReg=dynaMapToNative[MipsReg].Reg;
			dynaMapToNative[MipsReg].BirthDate=dynaBirthYear;	// set birthdate of register
			dynaMapToMips[NReg].BirthDate=dynaBirthYear;		// set birthdate of register
			dynaMarkRegDirty(MipsReg,false);					// mark the registers not dirty
			dynaBirthYear++;									// another year goes by.... ;)
			return(0);											// no code needed.
		}
	}
	if(dynaMapToMips[ForcedNativeReg].Reg!=0xff)
	{
		l+=dynaFlushReg(cp+l,ForcedNativeReg);
	}
*/
	NReg=ForcedNativeReg;
	dynaMapToNative[MipsReg].Reg=NReg;						// update register map
	dynaMapToNative[MipsReg].BirthDate=dynaBirthYear;		// set birthdate of register
	dynaMapToMips[NReg].Reg=MipsReg;						// update register map
	dynaMapToMips[NReg].BirthDate=dynaBirthYear;			// set birthdate of register
	dynaMarkRegDirty(MipsReg,false);						// mark the registers not dirty
	dynaBirthYear++;										// another year goes by.... ;)
/*
*/
	l+=LOAD_REG(cp+l,MipsReg,NReg);						// create code to load this register
	return(l);
}

WORD dynaOpAddEqualI(BYTE *cp,BYTE MipsReg,DWORD Immediate)
{
	WORD l=0;
	l+=ADD_REG_IMM(cp+l,dynaMapToNative[MipsReg].Reg,Immediate);
	dynaMarkRegDirty(MipsReg,true);					// mark the registers dirty
	return(l);
}

void Sneeky()
{
	DWORD hehe;
	hehe=1;
}

WORD dynaNOP(BYTE *cp)
{
	WORD l=0;
//	l+=INC_PC_COUNTER(cp+l);
	l+=NOP(cp+l);
	return(l);
}

void dynaInvalidate(DWORD Start,DWORD Length)
{
//	return;
	DWORD Page;
	DWORD Address=Start;
	while(Address<(Start+Length))			// plus a little extra... ;)
	{
		Page=(Address&PAGE_MASK)>>PAGE_SHIFT;
		SafeFree(dynaPageTable[Page].Offset[0]);
		for(int o=0;o<PAGE_SIZE;o++)
		{
			dynaPageTable[Page].Offset[o]=NULL;				// not mapped, allocated or compiled
			dynaPageTable[Page].Value[o]=0xffffffff;
		}
		Address+=(PAGE_SIZE*4);
	}
}

void dynaDestroy()
{
	for(int p=0;p<NUM_PAGES;p++)
	{
		SafeFree(dynaPageTable[p].Offset[0]);			// the zero offset of each page is the allocated memory
	}
	theApp.LogMessage("Smart Compiles %d vs. Dumb Compiles %d MMIO %d",smart2,dumb2,mmio);
	theApp.LogMessage("Exe range %X to %X",dynaLowRange,dynaHighRange);
}



bool dynaCheckBPoint(DWORD address)
{
	for(int i=0;i<dynaNumBPoints;i++)
	{
		if((dynaBPoint[i]&MEM_MASK)==(address&MEM_MASK))
			return(true);
	}
	if(dynaBreakOnKnown)
	{
		for(DWORD i=0;i<dasmNumLookups;i++)
		{
			if((dasmLookupAddy[i]&MEM_MASK)==(address&MEM_MASK))
			{
				return(true);
			}
		}
	}
	return(false);
}

BYTE dynaPreCompilePage(DWORD Address)
{
	DWORD Page;

	dynaResetCompiler();

	Page=(Address&PAGE_MASK)>>PAGE_SHIFT;

	WORD len;
	BYTE TmpWorkSpace[4096];

	dynaRam=(DWORD *)&dynaRamPtr[Address&PAGE_MASK];

	dynaCompileTargetPtr=TmpWorkSpace;	// this is where the compiled code goes
										// it is up to us to advance this pointer
										// note, that during the PreCompile stage, 
										// we don't advance the pointer as we are 
										// primarily building the BranchTable

	dynaPreCompileLength=0;
	DWORD *btable=dynaLengthTable;
	dynaForceFallOut=false;

	dynaCurPage=Page;
	dynaSrc=0;
	dynaPC=Address&PAGE_MASK;
//	dynaPC|=0x80000000;
	dynaTest=dynaPC;
	dynaImm=dynaImmediates;
	while(dynaSrc<PAGE_SIZE)
	{
		iOpCode=*(dynaRam++);
		dynaNextOpCode=*dynaRam;
		r->PC=dynaPC+4;
		dynaFPointer[29]=1;
		dynaFPPage[29]=0x88;
		if(dynaForceFallOut)
		{
			len=MainInstruction[iOpCode>>26](dynaCompileTargetPtr);
			len+=dynaFallOut(dynaCompileTargetPtr+len);
			dynaForceFallOut=false;
		}
		else
		{
			len=MainInstruction[iOpCode>>26](dynaCompileTargetPtr);
		}
		dynaUpdateMMIO();
		dynaSrc++;
		*(btable++)=dynaPreCompileLength;
		dynaPreCompileLength+=len;
		dynaPC+=4;
		if(dynaSrc==(PAGE_SIZE-1))
			dynaForceFallOut=TRUE;
		dynaPrvOpCode=iOpCode;
	}
	return true;
}


BYTE dynaInPage(DWORD Address)
{
	DWORD Page;
//	return false;
//	if((dynaPrvOpCode>>26)==35) return false;
//	if((dynaNextOpCode>>26)==35) return false;
//	if(Address<dynaPC) return false;
	Page=(Address&PAGE_MASK)>>PAGE_SHIFT;
	if(Page==dynaCurPage) return true;
	else return false;
}

BYTE dynaInPage2(DWORD Address)
{
	return false;
}

DWORD dynaGetCompiledAddress(DWORD NewPC)
{
	DWORD CompiledPC;
	DWORD Page;
	DWORD Offset;

	NewPC+=0x80000000;
	Page=  (NewPC&PAGE_MASK)>>PAGE_SHIFT;
	Offset=(NewPC&OFFSET_MASK)>>OFFSET_SHIFT;
	CompiledPC=(DWORD) dynaPageTable[Page].Offset[Offset];
	return(CompiledPC);
}


BYTE dynaCompilePage(DWORD Address)
{
	DWORD Page,Offset;

	dynaResetCompiler();

	Page=(Address&PAGE_MASK)>>PAGE_SHIFT;

	WORD len;

	dynaRam=(DWORD *)&dynaRamPtr[Address&PAGE_MASK];

	SafeFree(dynaPageTable[Page].Offset[0]);
	dynaPageTable[Page].Offset[0]=(BYTE *)malloc(dynaPreCompileLength);
	float ratio=(float)dynaPreCompileLength/(float)PAGE_SIZE;
	theApp.LogMessage("CompiledPage @%x used %d bytes for %3.2f bytes/MIPS instruction",Address,dynaPreCompileLength,ratio);

	dynaCompileTargetPtr=dynaPageTable[Page].Offset[0];

	DWORD *btable=dynaLengthTable;
	dynaForceFallOut=false;

	dynaSrc=0;
	while(dynaSrc<PAGE_SIZE)
	{
		dynaPageTable[Page].Offset[dynaSrc]=dynaCompileTargetPtr+*(btable++);
		dynaSrc++;
	}

	dynaCompileTargetPtr=dynaPageTable[Page].Offset[0];
	btable=dynaLengthTable;
	dynaSrc=0;
	DWORD Used=0;
	dynaCurPage=Page;
	dynaPC=Address&PAGE_MASK;
	dynaTest=dynaPC;

//	dynaPC|=0x80000000;
	dynaImm=dynaImmediates;
	while(dynaSrc<PAGE_SIZE)
	{
			iOpCode=*(dynaRam++);
			dynaNextOpCode=*dynaRam;
			r->PC=dynaPC+4;
			dynaFPointer[29]=1;
			dynaFPPage[29]=0x88;
	//		if((DWORD)dynaCompileTargetPtr==0x169985e)
	//			DWORD S=1;
			if(dynaPC==0x30018)
				int s=1;
			Page=  (dynaPC&PAGE_MASK)>>PAGE_SHIFT;
			Offset=(dynaPC&OFFSET_MASK)>>OFFSET_SHIFT;
			BYTE *test= dynaPageTable[Page].Offset[Offset];
			if((dynaPC&0xfffff)==0xa6c)
			{
				dynaHold=dynaPageTable[Page].Offset[Offset];
			}

			dynaPageTable[Page].Value[Offset]=iOpCode;
			if((DWORD)dynaCompileTargetPtr==0x35a14f7)
				int stop=1;
			if(dynaForceFallOut)
			{
				len=MainInstruction[iOpCode>>26](dynaCompileTargetPtr);
				len+=dynaFallOut(dynaCompileTargetPtr+len);
				dynaForceFallOut=false;
			}
			else
			{
				len=MainInstruction[iOpCode>>26](dynaCompileTargetPtr);
			}
			dynaUpdateMMIO();
			if(dynaPageTable[Page].Offset[dynaSrc]!=dynaCompileTargetPtr)
				DWORD crap=1;
			Page=  (dynaPC&PAGE_MASK)>>PAGE_SHIFT;
			Offset=(dynaPC&OFFSET_MASK)>>OFFSET_SHIFT;
			test= dynaPageTable[Page].Offset[Offset];
			if(test!=dynaCompileTargetPtr)
				DWORD asdf=1;
			dynaSrc++;
			dynaCompileTargetPtr+=len;
			if(Used!=*btable)
			{
				DWORD Stop=1;
			}
			dynaPrvOpCode=iOpCode;
			btable++;
			Used+=len;
			dynaPC+=4;
			if(dynaSrc==(PAGE_SIZE-1))
				dynaForceFallOut=TRUE;
	}
	dynaPC=0xa6c;
	Page=  (dynaPC&PAGE_MASK)>>PAGE_SHIFT;
	Offset=(dynaPC&OFFSET_MASK)>>OFFSET_SHIFT;
	BYTE *test= dynaPageTable[Page].Offset[Offset];
	if(test!=dynaHold)
		DWORD stop=1;
	return true;
}
typedef void (*funcptr)(void);
funcptr dynaStart;

WORD dynaCallInterpHelper(BYTE *cp,DWORD code)
{
	WORD len=0;

//	len+=ADD_REG_IMM(cp+len,PC_PTR,4);
//	len+=STORE_REG(cp+len,242,PC_PTR);
	len+=STORE_DWORD_TO_RBANK(cp+len,dynaPC+4,242*8);

	len+=PUSH_ALL(cp+len);
	len+=PUSH_DWORD(cp+len,code);
	len+=LOAD_REG_DWORD(cp+len,NATIVE_0,(BYTE *)&InterpHelperAddress);
	len+=CALL_REGISTER(cp+len,NATIVE_0);
	len+=POP_REGISTER(cp+len,NATIVE_0);
	len+=POP_ALL(cp+len);
	len+=LOAD_REG(cp+len,242,PC_PTR);

	return(len);
}

WORD dynaFallOut(BYTE *cp)
{
	WORD len=0;
//	len+=STORE_REG(cp+len,242,PC_PTR);

	len=STORE_DWORD_TO_RBANK(cp,dynaPC+4,242*8);

	cp+=len;
	*(cp++)=0xc3;	//ret
	len++;
	return(len);
}


void dynaInit()
{
//	rcConstruct();
	for(int p=0;p<NUM_PAGES;p++)
	{
		for(int o=0;o<PAGE_SIZE;o++)
		{
			dynaPageTable[p].Offset[o]=NULL;				// not mapped, allocated or compiled
		}
	}
	dynaForceFallOut=false;
	SneekyAddress=(DWORD)Sneeky;
	dynaCheckIntsAddress=(DWORD) iCpuCheckInts;
	helperLWLAddress=(DWORD)dynaHelperLWL;
	helperLDLAddress=(DWORD)dynaHelperLDL;
	helperLWRAddress=(DWORD)dynaHelperLWR;
	helperLDRAddress=(DWORD)dynaHelperLDR;

	dynaRamPtr=m->rdRam;
	dynaBuildReadMap();
	dynaBuildWriteMap();

}


BYTE dynaExecute(DWORD Address)
{
//	return false;
	DWORD Page;
	DWORD Offset;
	BYTE Done=false;
	static WORD first=0;

	if((Address&0x7fffff)<dynaLowRange)
		dynaLowRange=Address;
	if((Address&0x7fffff)>dynaHighRange)
		dynaHighRange=Address;

	while(!Done)
	{

//		r->ICount+=2;
/*
		if(theApp.m_UpdateAudio)
			theApp.m_EmuObj->UpdateAudio();
		if((((DWORD *)m->spReg)[4]&1)==0)
		{
			if((m->spDmem[0xfc0]<3)) //&&(m->spDmem[0xfc0]==4))
			{
				if(m->spDmem[0xfc0]==1)
				{
					if(NewTask)
					{
						hleRdpBypass();
						iCpuVSYNC();
					}
				}
				else
				{
						hleRdpBypass();
				}
			}
		}
*/
		if(NewTask!=NORMAL_GAME)
			return false;
		Address=r->PC;

		iCpuCheckInts();

		if(r->PC==0x88000180) 
		{
			return false;
		}
/*
*/
		Page=  (Address&PAGE_MASK)>>PAGE_SHIFT;
		Offset=(Address&OFFSET_MASK)>>OFFSET_SHIFT;
		dynaPC=Address;
		iOpCode=*(DWORD *)&dynaRamPtr[Address&MEM_MASK];
		if((dynaPageTable[Page].Offset[Offset]!=NULL)&&			// page is allocated and compiled
			(dynaPageTable[Page].Value[Offset]==iOpCode))

		{
/*
			if(rcFunctionList[(Address&MEM_MASK)/4]==NULL)
			{
				rcFinderLocateFunctions((DWORD *)m->rdRam,Address&MEM_MASK,512);
			}
*/
			dynaStart=(funcptr)dynaPageTable[Page].Offset[Offset];
			DWORD *stack=(DWORD *)&dynaRamPtr+r->GPR[29*2];
			_asm
			{
				pushad
				mov	ebp,dword ptr dynaRegPtr
				wait
				fldcw word ptr [ebp+(CUR_ROUND_MODE-128)]
				wait
				mov ebx,dword ptr stack
				call dword ptr [dynaStart]
				popad
			}
			return true;
		}
		else
		{
			static first=1;
/*
			if(rcFunctionList==NULL)
			{
				rcFinderConstruct();
				rcFinderLocateFunctions((DWORD *)m->rdRam,Address&MEM_MASK,512);
				rcFinderDestruct();
			}
			if(rcFunctionList[(Address&MEM_MASK)/4]==NULL)
			{
				rcFinderLocateFunctions((DWORD *)m->rdRam,Address&MEM_MASK,512);
			}
*/
			first++;
			dynaPreCompilePage(Address);					// page is not allocated or compiled
			dynaCompilePage(Address);						// compile it
			Page=  (Address&PAGE_MASK)>>PAGE_SHIFT;
			Offset=(Address&OFFSET_MASK)>>OFFSET_SHIFT;
			if(dynaPageTable[Page].Offset[Offset]!=NULL)		// try again...
			{
				dynaStart=(funcptr)dynaPageTable[Page].Offset[Offset];
				DWORD *stack=(DWORD *)&dynaRamPtr+r->GPR[29*2];
				_asm
				{
					pushad
					mov	ebp,dword ptr dynaRegPtr
					mov ebx,dword ptr stack
					wait
					fldcw word ptr [ebp+(CUR_ROUND_MODE-128)]
					wait
					call dword ptr [dynaStart]
					popad
				}
				return true;
			}
			else	//screw it
				return false;								// something didn't work, let interp code handle it
		}

	}
	return true;
}



WORD dynaCompile_sra(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_SHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef ALLOW_SHIFT_EQ
	if(MAKE_RD==MAKE_RT)
	{
		return(dynaOpSraEq(cp,MAKE_RD,MAKE_RT,MAKE_SA));
	}
#endif
	dynaFPointer[MAKE_RD]=1;
	return(dynaOpSra(cp,MAKE_RD,MAKE_RT,MAKE_SA));
}

WORD dynaCompile_sll(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_SHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if((MAKE_RD==0)&&(MAKE_RT==0)&&(MAKE_SA==0))
	{
		return(dynaNOP(cp));
	}
#ifdef ALLOW_SHIFT_EQ
	if(MAKE_RD==MAKE_RT)
	{
		return(dynaOpSllEq(cp,MAKE_RD,MAKE_RT,MAKE_SA));
	}
#endif
	dynaFPointer[MAKE_RD]=1;
	return(dynaOpSll(cp,MAKE_RD,MAKE_RT,MAKE_SA));
}

WORD dynaCompile_reserved(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_srl(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_SHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef ALLOW_SHIFT_EQ
	if(MAKE_RD==MAKE_RT)
	{
		return(dynaOpSrlEq(cp,MAKE_RD,MAKE_RT,MAKE_SA));
	}
#endif
	dynaFPointer[MAKE_RD]=1;
	return(dynaOpSrl(cp,MAKE_RD,MAKE_RT,MAKE_SA));
}

WORD dynaCompile_sllv(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_SHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	WORD len=0;
	len+=dynaOpSllV(cp+len,MAKE_RD,MAKE_RT,MAKE_RS);
	dynaFPointer[MAKE_RD]=1;
	return(len);
}

WORD dynaCompile_srlv(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_SHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	WORD len=0;
	len+=dynaOpSrlV(cp+len,MAKE_RD,MAKE_RT,MAKE_RS);
	dynaFPointer[MAKE_RD]=1;
	return(len);
}

WORD dynaCompile_srav(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_SHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	WORD len=0;
	len+=dynaOpSraV(cp+len,MAKE_RD,MAKE_RT,MAKE_RS);
	dynaFPointer[MAKE_RD]=1;
	return(len);
}

WORD dynaCompile_syscall(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_Break(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_sync(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_mfhi(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	dynaFPointer[MAKE_RD]=1;
	return(dynaOpMfHi(cp,MAKE_RD));
}

WORD dynaCompile_mthi(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpMtHi(cp,MAKE_RS));
}

WORD dynaCompile_mflo(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpMfLo(cp,MAKE_RD));
}

WORD dynaCompile_mtlo(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpMtLo(cp,MAKE_RS));
}

WORD dynaCompile_dsllv(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DSHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDSllV(cp,MAKE_RD,MAKE_RT,MAKE_RS));
}

WORD dynaCompile_dsrlv(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DSHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDSrlV(cp,MAKE_RD,MAKE_RT,MAKE_RS));
}

WORD dynaCompile_dsrav(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DSHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDSraV(cp,MAKE_RD,MAKE_RT,MAKE_RS));
}

WORD dynaCompile_mult(BYTE *cp)
{
#ifdef BYPASS_MULT_DIV
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpMult(cp,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_multu(BYTE *cp)
{
#ifdef BYPASS_MULT_DIV
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpMultU(cp,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_div(BYTE *cp)
{
#ifdef BYPASS_MULT_DIV
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDiv(cp,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_divu(BYTE *cp)
{
#ifdef BYPASS_MULT_DIV
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDivU(cp,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_dmult(BYTE *cp)
{
#ifdef BYPASS_MULT_DIV_D
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDMult(cp,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_dmultu(BYTE *cp)
{
#ifdef BYPASS_MULT_DIV_D
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDMultU(cp,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_ddiv(BYTE *cp)
{
#ifdef BYPASS_MULT_DIV_D
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDDiv(cp,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_ddivu(BYTE *cp)
{
#ifdef BYPASS_MULT_DIV_D
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDDivU(cp,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_add(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_ARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef GPR0_OPTIMIZATION
	if(MAKE_RS==0)
	{
		return(dynaOpRegMove(cp,MAKE_RD,MAKE_RT));
	}
	if(MAKE_RT==0)
	{
		dynaFPointer[MAKE_RD]=dynaFPointer[MAKE_RS];
		dynaFPPage[MAKE_RD]=dynaFPPage[MAKE_RS];
		return(dynaOpRegMove(cp,MAKE_RD,MAKE_RS));
	}
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		dynaFPointer[MAKE_RD]=dynaFPointer[MAKE_RT];
		dynaFPPage[MAKE_RD]=dynaFPPage[MAKE_RT];
		return(dynaOpAddEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	dynaFPointer[MAKE_RD]=1;
	return(dynaOpAdd(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_addu(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_ARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef GPR0_OPTIMIZATION
	if(MAKE_RS==0)
	{
		dynaFPointer[MAKE_RD]=dynaFPointer[MAKE_RS];
		dynaFPPage[MAKE_RD]=dynaFPPage[MAKE_RS];
		return(dynaOpRegMove(cp,MAKE_RD,MAKE_RT));
	}
	if(MAKE_RT==0)
	{
		dynaFPointer[MAKE_RD]=dynaFPointer[MAKE_RT];
		dynaFPPage[MAKE_RD]=dynaFPPage[MAKE_RT];
		return(dynaOpRegMove(cp,MAKE_RD,MAKE_RS));
	}
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		return(dynaOpAddEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	dynaFPointer[MAKE_RD]=1;
	return(dynaOpAdd(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_sub(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_ARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef GPR0_OPTIMIZATION
	if(MAKE_RT==0)
	{
		dynaFPointer[MAKE_RD]=dynaFPointer[MAKE_RS];
		dynaFPPage[MAKE_RD]=dynaFPPage[MAKE_RS];
		return(dynaOpRegMove(cp,MAKE_RD,MAKE_RS));
	}
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		return(dynaOpSubEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	dynaFPointer[MAKE_RD]=1;
	return(dynaOpSub(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_subu(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_ARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
/*
#ifdef GPR0_OPTIMIZATION
	if(MAKE_RT==0)
	{
		dynaFPointer[MAKE_RD]=dynaFPointer[MAKE_RS];
		dynaFPPage[MAKE_RD]=dynaFPPage[MAKE_RS];
		return(dynaOpRegMove(cp,MAKE_RD,MAKE_RS));
	}
#endif
*/
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		dynaFPointer[MAKE_RD]=1;
		return(dynaOpSubEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	dynaFPointer[MAKE_RD]=1;
	return(dynaOpSub(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_and(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_LOGIC
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		return(dynaOpAndEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	dynaFPointer[MAKE_RD]=1;
	return(dynaOpAnd(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_or(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_LOGIC
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		return(dynaOpOrEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	if(MAKE_RT==0)
	{
		return(dynaOpRegDMove(cp,MAKE_RD,MAKE_RS));
	}
/*
*/
	dynaFPointer[MAKE_RD]=1;
	return(dynaOpOr(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_xor(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_LOGIC
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		return(dynaOpXorEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	return(dynaOpXor(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_nor(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_LOGIC
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		return(dynaOpNorEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	return(dynaOpNor(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_slt(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_SETLESS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	DWORD tmp=iOpCode;
	BYTE dst=MAKE_RD;
	BYTE src=MAKE_RS;
	BYTE src2=MAKE_RT;
	DWORD imm=MAKE_I;
/*
	if((!dynaInBranch)&&(0))
	{
		switch(dynaNextOpCode>>26)
		{
		case 4:
		case 28:
			{
				iOpCode=dynaNextOpCode;
				BYTE op0=MAKE_RT;
				BYTE op1=MAKE_RS;
				DWORD target=MAKE_O+4;
				if((op0==0)&&(op1==dst)&&dynaInPage(target))
				{
					dynaCheck=0;
					dynaCheckU=dynaPC+4;
					dynaCmp0=src;
					dynaCmp1=src2;
					dynaNextOpCode&=0x03ffffff;
					dynaNextOpCode|=(28<<26);
					*dynaRam=dynaNextOpCode;
					iOpCode=tmp;
//					*(cp++)=0x90;
					return(0);
				}
				iOpCode=tmp;
				break;
			}
		case 5:
		case 30:
			{
				iOpCode=dynaNextOpCode;
				BYTE op0=MAKE_RT;
				BYTE op1=MAKE_RS;
				DWORD target=MAKE_O+4;
				if((op0==0)&&(op1==dst)&&dynaInPage(target))
				{
					dynaCheck=0;
					dynaCheckU=dynaPC+4;
					dynaCmp0=src;
					dynaCmp1=src2;
					dynaNextOpCode&=0x03ffffff;
					dynaNextOpCode|=(30<<26);
					*dynaRam=dynaNextOpCode;
					iOpCode=tmp;
//					*(cp++)=0x90;
					return(0);
				}
				iOpCode=tmp;
				break;
			}
		}
	}
*/
	return(dynaOpSlt(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_sltu(BYTE *cp)
{
	CHECK_RD_ZERO();

#ifdef BYPASS_SETLESS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	DWORD tmp=iOpCode;
	BYTE dst=MAKE_RD;
	BYTE src=MAKE_RS;
	BYTE src2=MAKE_RT;
/*
	if((!dynaInBranch)&&(0))
	{
		switch(dynaNextOpCode>>26)
		{
		case 4:
		case 29:
			{
				iOpCode=dynaNextOpCode;
				BYTE op0=MAKE_RT;
				BYTE op1=MAKE_RS;
				DWORD target=MAKE_O+4;
				if((op0==0)&&(op1==dst)&&dynaInPage(target))
				{
					dynaCheck=0;
					dynaCheckU=dynaPC+4;
					dynaCmp0=src;
					dynaCmp1=src2;
					dynaNextOpCode&=0x03ffffff;
					dynaNextOpCode|=(29<<26);
					*dynaRam=dynaNextOpCode;
					iOpCode=tmp;
//					*(cp++)=0x90;
					return(0);
				}
				iOpCode=tmp;
				break;
			}
		case 500:
		case 31:
			{
				iOpCode=dynaNextOpCode;
				BYTE op0=MAKE_RT;
				BYTE op1=MAKE_RS;
				DWORD target=MAKE_O+4;
				if((op0==0)&&(op1==dst)&&dynaInPage(target)&&(target!=dynaPC+8))
				{
					dynaCheck=0;
					dynaCheckU=dynaPC+4;
					dynaCmp0=src;
					dynaCmp1=src2;
					dynaNextOpCode&=0x03ffffff;
					dynaNextOpCode|=(31<<26);
					*dynaRam=dynaNextOpCode;
					iOpCode=tmp;
//					*(cp++)=0x90;
					return(0);
				}
				iOpCode=tmp;
				break;
			}
		}
	}
*/
	return(dynaOpSltU(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_dadd(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef GPR0_OPTIMIZATION
	if(MAKE_RS==0)
	{
		return(dynaOpRegDMove(cp,MAKE_RD,MAKE_RT));
	}
	if(MAKE_RT==0)
	{
		return(dynaOpRegDMove(cp,MAKE_RD,MAKE_RS));
	}
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		return(dynaOpDAddEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	return(dynaOpDAdd(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_daddu(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif

#ifdef GPR0_OPTIMIZATION
	if(MAKE_RS==0)
	{
		return(dynaOpRegDMove(cp,MAKE_RD,MAKE_RT));
	}
	if(MAKE_RT==0)
	{
		return(dynaOpRegDMove(cp,MAKE_RD,MAKE_RS));
	}
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		return(dynaOpDAddEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	return(dynaOpDAdd(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_dsub(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		return(dynaOpDSubEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	return(dynaOpDSub(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_dsubu(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RD==MAKE_RS)
	{
		return(dynaOpDSubEqual(cp,MAKE_RD,MAKE_RT));
	}
#endif
	return(dynaOpDSub(cp,MAKE_RD,MAKE_RS,MAKE_RT));
}

WORD dynaCompile_tge(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_tgeu(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_tlt(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_tltu(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_teq(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_tne(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_dsll(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DSHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDSll(cp,MAKE_RD,MAKE_RT,MAKE_SA));
}

WORD dynaCompile_dsrl(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DSHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDSrl(cp,MAKE_RD,MAKE_RT,MAKE_SA));
}

WORD dynaCompile_dsra(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DSHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDSra(cp,MAKE_RD,MAKE_RT,MAKE_SA));
}

WORD dynaCompile_dsll32(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DSHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDSll32(cp,MAKE_RD,MAKE_RT,MAKE_SA));
}

WORD dynaCompile_dsrl32(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DSHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDSrl32(cp,MAKE_RD,MAKE_RT,MAKE_SA));
}

WORD dynaCompile_dsra32(BYTE *cp)
{
	CHECK_RD_ZERO();

	dynaFPointer[MAKE_RD]=1;
#ifdef BYPASS_DSHIFTS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDSra32(cp,MAKE_RD,MAKE_RT,MAKE_SA));
}

WORD dynaCompile_special(BYTE *cp)
{
	return(SpecialInstruction[iOpCode&0x3f](cp));
}

WORD dynaCompile_regimm(BYTE *cp)
{
	return(RegimmInstruction[(iOpCode>>16)&0x1f](cp));
}

WORD dynaCompile_addi(BYTE *cp)
{
	CHECK_RT_ZERO();

#ifdef BYPASS_ARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif

#ifdef SP_CATCHno
	if(MAKE_RT==29)
	{
		return(dynaOpAddSP(cp,MAKE_RS,MAKE_I));
	}
#endif

#ifdef GPR0_OPTIMIZATION
	if(MAKE_RS==0)
	{
		dynaFPointer[MAKE_RT]=1;
		return(dynaOpLoadI(cp,MAKE_RT,MAKE_I));
	}
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RT==MAKE_RS)
	{
		return(dynaOpAddIEqual(cp,MAKE_RT,MAKE_I));
	}
#endif
	if(MAKE_I==0)
	{
		dynaFPointer[MAKE_RT]=dynaFPointer[MAKE_RS];
		dynaFPPage[MAKE_RT]=dynaFPPage[MAKE_RS];
		return(dynaOpRegMove(cp,MAKE_RT,MAKE_RS));
	}
	dynaFPointer[MAKE_RT]=1;
	return(dynaOpAddI(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_addiu(BYTE *cp)
{
	CHECK_RT_ZERO();

#ifdef BYPASS_ARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef GPR0_OPTIMIZATION
	if(MAKE_RS==0)
	{
		dynaFPointer[MAKE_RT]=1;
		return(dynaOpLoadI(cp,MAKE_RT,MAKE_I));
	}
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RT==MAKE_RS)
	{
		return(dynaOpAddIEqual(cp,MAKE_RT,MAKE_I));
	}
#endif
	if(MAKE_I==0)
	{
		dynaFPointer[MAKE_RT]=dynaFPointer[MAKE_RS];
		dynaFPPage[MAKE_RT]=dynaFPPage[MAKE_RS];
//		dynaFPointer[MAKE_RT]=1;
		return(dynaOpRegMove(cp,MAKE_RT,MAKE_RS));
	}
	dynaFPointer[MAKE_RT]=1;
	return(dynaOpAddI(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_slti(BYTE *cp)
{
	CHECK_RT_ZERO();

	dynaFPointer[MAKE_RT]=1;
#ifdef BYPASS_SETLESS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	DWORD tmp=iOpCode;
	BYTE dst=MAKE_RT;
	BYTE src=MAKE_RS;
	DWORD imm=MAKE_I;
/*
	if((!dynaInBranch)&&(0))
	{
		switch(dynaNextOpCode>>26)
		{
		case 4:
		case 28:
			{
				iOpCode=dynaNextOpCode;
				BYTE op0=MAKE_RT;
				BYTE op1=MAKE_RS;
				DWORD target=MAKE_O;
				if((op0==0)&&(op1==dst)&&dynaInPage(target))
				{
					dynaCheck=dynaPC+4;
					dynaCheckU=0;
					dynaCmp0=src;
					dynaCmpI=imm;
					dynaNextOpCode&=0x03ffffff;
					dynaNextOpCode|=(28<<26);
					*dynaRam=dynaNextOpCode;
					iOpCode=tmp;
//					*(cp++)=0x90;
					return(0);
				}
				iOpCode=tmp;
				break;
			}
		case 5:
		case 30:
			{
				iOpCode=dynaNextOpCode;
				BYTE op0=MAKE_RT;
				BYTE op1=MAKE_RS;
				DWORD target=MAKE_O;
				if((op0==0)&&(op1==dst)&&dynaInPage(target))
				{
					dynaCheck=dynaPC+4;
					dynaCheckU=0;
					dynaCmp0=src;
					dynaCmpI=imm;
					dynaNextOpCode&=0x03ffffff;
					dynaNextOpCode|=(30<<26);
					*dynaRam=dynaNextOpCode;
					iOpCode=tmp;
//					*(cp++)=0x90;
					return(0);
				}
				iOpCode=tmp;
				break;
			}
		}
	}
*/
	return(dynaOpSltI(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_bgei(BYTE  *cp)
{
#ifdef VARI_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		if(MAKE_RS==MAKE_RT)
		{
			return(dynaOpILoop(cp,MAKE_O));
		}
	}
#endif
	if(dynaPC==dynaCheck)
		return(dynaOpBgei(cp,dynaCmp0,MAKE_RS,dynaCmpI,MAKE_O));
	else if(dynaPC==dynaCheckU)
		return(dynaOpBge(cp,dynaCmp0,MAKE_RS,dynaCmp1,MAKE_O));
	else
		return(0);
}

WORD dynaCompile_bgeiu(BYTE  *cp)
{
#ifdef VARI_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		if(MAKE_RS==MAKE_RT)
		{
			return(dynaOpILoop(cp,MAKE_O));
		}
	}
#endif
	if(dynaPC==dynaCheck)
		return(dynaOpBgeiu(cp,dynaCmp0,MAKE_RS,dynaCmpI,MAKE_O));
	else if(dynaPC==dynaCheckU)
		return(dynaOpBgeu(cp,dynaCmp0,MAKE_RS,dynaCmp1,MAKE_O));
	else
		return(0);
}

WORD dynaCompile_bli(BYTE  *cp)
{
	if(dynaPC==dynaCheck) 
		return(dynaOpBli(cp,dynaCmp0,MAKE_RS,dynaCmpI,MAKE_O));
	else if(dynaPC==dynaCheckU)
		return(dynaOpBl(cp,dynaCmp0,MAKE_RS,dynaCmp1,MAKE_O));
	else
		return(0);
}

WORD dynaCompile_bliu(BYTE  *cp)
{
	WORD len=0;
	if(dynaInBranch) return(0);
	dynaInBranch=true;
	if(dynaPC==dynaCheck) 
		len=dynaOpBliu(cp,dynaCmp0,MAKE_RS,dynaCmpI,MAKE_O);
	else if(dynaPC==dynaCheckU)
		len=dynaOpBlu(cp,dynaCmp0,MAKE_RS,dynaCmp1,MAKE_O);
	else
		len=0;
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_sltiu(BYTE *cp)
{
	CHECK_RT_ZERO();

	dynaFPointer[MAKE_RT]=1;
#ifdef BYPASS_SETLESS
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	DWORD tmp=iOpCode;
	BYTE dst=MAKE_RT;
	BYTE src=MAKE_RS;
	DWORD imm=MAKE_I;
/*
	if((!dynaInBranch)&&(0))
	{
		switch(dynaNextOpCode>>26)
		{
		case 4:
		case 29:
			{
				iOpCode=dynaNextOpCode;
				BYTE op0=MAKE_RT;
				BYTE op1=MAKE_RS;
				DWORD target=MAKE_O;
				if((op0==0)&&(op1==dst)&&dynaInPage(target))
				{
					dynaCheck=dynaPC+4;
					dynaCheckU=0;
					dynaCmp0=src;
					dynaCmpI=imm;
					dynaNextOpCode&=0x03ffffff;
					dynaNextOpCode|=(29<<26);
					*dynaRam=dynaNextOpCode;
					iOpCode=tmp;
//					*(cp++)=0x90;
					return(0);
				}
				iOpCode=tmp;
				break;
			}
		case 5:
		case 31:
			{
				iOpCode=dynaNextOpCode;
				BYTE op0=MAKE_RT;
				BYTE op1=MAKE_RS;
				DWORD target=MAKE_O;
				if((op0==0)&&(op1==dst)&&dynaInPage(target))
				{
					dynaCheck=dynaPC+4;
					dynaCheckU=0;
					dynaCmp0=src;
					dynaCmpI=imm;
					dynaNextOpCode&=0x03ffffff;
					dynaNextOpCode|=(31<<26);
					*dynaRam=dynaNextOpCode;
					iOpCode=tmp;
//					*(cp++)=0x90;
					return(0);
				}
				iOpCode=tmp;
				break;
			}
		}
	}
*/
	return(dynaOpSltIU(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_andi(BYTE *cp)
{
	CHECK_RT_ZERO();

#ifdef BYPASS_LOGIC
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RT==MAKE_RS)
	{
		return(dynaOpAndIEqual(cp,MAKE_RT,MAKE_IU));
	}
#endif
	dynaFPointer[MAKE_RT]=1;
	return(dynaOpAndI(cp,MAKE_RT,MAKE_RS,MAKE_IU));
}

WORD dynaCompile_ori(BYTE *cp)
{
	CHECK_RT_ZERO();

#ifdef BYPASS_LOGIC
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RT==MAKE_RS)
	{
		return(dynaOpOrIEqual(cp,MAKE_RT,MAKE_IU));
	}
#endif
		dynaFPointer[MAKE_RT]=dynaFPointer[MAKE_RS];
		dynaFPPage[MAKE_RT]=dynaFPPage[MAKE_RS];
//	dynaFPointer[MAKE_RT]=1;
	return(dynaOpOrI(cp,MAKE_RT,MAKE_RS,MAKE_IU));
}

WORD dynaCompile_xori(BYTE *cp)
{
	CHECK_RT_ZERO();

	dynaFPointer[MAKE_RT]=1;
#ifdef BYPASS_LOGIC
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef RD_EQUAL_RS
	if(MAKE_RT==MAKE_RS)
	{
		return(dynaOpXorIEqual(cp,MAKE_RT,MAKE_IU));
	}
#endif
		dynaFPointer[MAKE_RT]=dynaFPointer[MAKE_RS];
		dynaFPPage[MAKE_RT]=dynaFPPage[MAKE_RS];
//	dynaFPointer[MAKE_RT]=1;
	return(dynaOpXorI(cp,MAKE_RT,MAKE_RS,MAKE_IU));
}

WORD dynaCompile_lui(BYTE *cp)
{
	CHECK_RT_ZERO();

	if(gAllowHLE)
	{
		if((dynaPC&0x7fffff)==0x28DAC)
		{
			return(hlePatchScreenCopy(cp));
		}
	}

#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	WORD Page=(MAKE_I>>8)&0xff;
	switch(Page)
	{
	case 0xb0:
		{
			dynaFPointer[MAKE_RT]=LUI_DELAY;
			dynaFPPage[MAKE_RT]=Page;
			break;
		}
	case 0x80:
		{
			dynaFPointer[MAKE_RT]=LUI_DELAY;
			dynaFPPage[MAKE_RT]=Page;
			break;
		}
	case 0x88:
		{
			dynaFPointer[MAKE_RT]=LUI_DELAY;
			dynaFPPage[MAKE_RT]=Page;
			break;
		}
	default:
		{
			dynaFPointer[MAKE_RT]=1;
			break;
		}
	}
	return(dynaOpLui(cp,MAKE_RT,MAKE_I<<16));
}


WORD dynaCompile_cop0(BYTE *cp)
{
	return(Cop0RSInstruction[(iOpCode>>21)&0x1f](cp));
}

WORD dynaCompile_cop1(BYTE *cp)
{
	return(Cop1RSInstruction[(iOpCode>>21)&0x1f](cp));
}

WORD dynaCompile_cop2(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_daddi(BYTE *cp)
{
	CHECK_RT_ZERO();

	dynaFPointer[MAKE_RT]=1;
#ifdef BYPASS_DARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDAddI(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_daddiu(BYTE *cp)
{
	CHECK_RT_ZERO();

	dynaFPointer[MAKE_RT]=1;
#ifdef BYPASS_DARITH
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpDAddI(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_lb(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartLb(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		dynaFPointer[MAKE_RT]=1;
		return(dynaOpSmartLb2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
	dynaFPointer[MAKE_RT]=1;
	dumb++;
#ifdef BASIC_MEM
	return(dynaOpLb(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLB(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_lh(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartLh(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		dynaFPointer[MAKE_RT]=1;
		return(dynaOpSmartLh2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
	dynaFPointer[MAKE_RT]=1;
	dumb++;
#ifdef BASIC_MEM
	return(dynaOpLh(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLH(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_lw(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef SP_CATCH
	if(MAKE_RS==29)
	{
		return(dynaOpLwSP(cp,MAKE_RT,MAKE_I));
	}
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		dynaFPointer[MAKE_RT]=1;
		return(dynaOpSmartLw2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
#ifdef NEW_MEM
	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartLw(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	dynaFPointer[MAKE_RT]=1;
	dumb++;
#ifdef BASIC_MEM
	return(dynaOpLw(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLW(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_lbu(BYTE *cp)
{
/*
	if((dynaPC&0x7fffff)==0x10E4C)
	{
		return(hlePatchPal(cp));
	}
*/

#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartLbU(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		dynaFPointer[MAKE_RT]=1;
		return(dynaOpSmartLbU2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
	dynaFPointer[MAKE_RT]=1;
	dumb++;
#ifdef BASIC_MEM
	return(dynaOpLbU(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLBU(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_lhu(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	if(MAKE_RS==0)
		return(dynaOpSmartLhU(cp,MAKE_RT,MAKE_RS,MAKE_I,0));

	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartLhU(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		dynaFPointer[MAKE_RT]=1;
		return(dynaOpSmartLhU2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
	dynaFPointer[MAKE_RT]=1;
	dumb++;
#ifdef BASIC_MEM
	return(dynaOpLhU(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLHU(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_lwr(BYTE *cp)
{
#ifdef LEFT_RIGHT
	return(dynaOpLwr(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
#ifdef BYPASS_MEMn
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartLw(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		dynaFPointer[MAKE_RT]=1;
		return(dynaOpSmartLw2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
	return(dynaOpLw(cp,MAKE_RT,MAKE_RS,MAKE_I));
#ifdef BASIC_MEM
	return(dynaOpLwr(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLWR(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_lwl(BYTE *cp)
{
#ifdef LEFT_RIGHT
	return(dynaOpLwl(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
#ifdef BYPASS_MEMn
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	dynaFPointer[MAKE_RT]=1;
	return(0);
#ifdef BASIC_MEM
	return(dynaOpLwl(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLWL(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_ldl(BYTE *cp)
{
#ifdef LEFT_RIGHT
	return(dynaOpLdl(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
#ifdef BYPASS_MEMn
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	dynaFPointer[MAKE_RT]=1;
	return(0);
#ifdef BASIC_MEM
	return(dynaOpLdl(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLDL(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_ldr(BYTE *cp)
{
#ifdef LEFT_RIGHT
	return(dynaOpLdr(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
#ifdef BYPASS_MEMn
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartLd(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	dumb++;
	dynaFPointer[MAKE_RT]=1;
	return(dynaOpLd(cp,MAKE_RT,MAKE_RS,MAKE_I));
#ifdef BASIC_MEM
	return(dynaOpLdr(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLDR(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_lwu(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef SP_CATCH
	if(MAKE_RS==29)
	{
		return(dynaOpLwSP(cp,MAKE_RT,MAKE_I));
	}
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		dynaFPointer[MAKE_RT]=1;
		return(dynaOpSmartLwU2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
#ifdef NEW_MEM
	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartLwU(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	dynaFPointer[MAKE_RT]=1;
	dumb++;
#ifdef BASIC_MEM
	return(dynaOpLwU(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLWU(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_sb(BYTE *cp)
{
	return(dynaCallInterpHelper(cp,iOpCode));
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		return(dynaOpSmartSb2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
#ifdef NEW_MEM
	if((0>=dynaFPointer[MAKE_RS])&&(dynaPC!=0x814)&&(MAKE_I!=0x80)&&(MAKE_I!=0x98))
		return(dynaOpSmartSb(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	dumb++;
#ifdef BASIC_MEM
	return(dynaOpSb(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSB(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_sh(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		return(dynaOpSmartSh2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
#ifdef NEW_MEM
	if((0>=dynaFPointer[MAKE_RS])&&(MAKE_I!=0x138)&&(MAKE_I!=0x98))
		return(dynaOpSmartSh(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	dumb++;
#ifdef BASIC_MEM
	return(dynaOpSh(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSH(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_sw(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef SP_CATCH
	if(MAKE_RS==29)
	{
		return(dynaOpSwSP(cp,MAKE_RT,MAKE_I));
	}
#endif
/*
*/
	if(0>=dynaFPointer[MAKE_RS])
	{
		return(dynaOpSmartSw2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
#ifdef NEW_MEM
	if((0>=dynaFPointer[MAKE_RS])&&(MAKE_I!=0x138)&&(MAKE_I!=0x98))
		return(dynaOpSmartSw(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	dumb++;
#ifdef BASIC_MEM
	return(dynaOpSw(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSW(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_sd(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef SP_CATCHno
	if(MAKE_RS==29)
	{
		return(dynaOpSdSP(cp,MAKE_RT,MAKE_I));
	}
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		return(dynaOpSmartSd2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
#ifdef NEW_MEM
	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartSd(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	dumb++;
#ifdef BASIC_MEM
	return(dynaOpSd(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSD(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_swl(BYTE *cp)
{
#ifdef LEFT_RIGHT
	return(dynaOpSwl(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
#ifdef BYPASS_MEMn
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(0);
#ifdef BASIC_MEM
	return(dynaOpSwl(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSWL(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_sdl(BYTE *cp)
{
#ifdef LEFT_RIGHT
	return(dynaOpSdl(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
#ifdef BYPASS_MEMn
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(0);
#ifdef BASIC_MEMn
	return(dynaOpSdl(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSDL(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_sdr(BYTE *cp)
{
#ifdef LEFT_RIGHT
	return(dynaOpSdr(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
#ifdef BYPASS_MEMn
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartSd(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		return(dynaOpSmartSd2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
	dumb++;
	return(dynaOpSd(cp,MAKE_RT,MAKE_RS,MAKE_I));
#ifdef BASIC_MEM
	return(dynaOpSdr(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSDR(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_swr(BYTE *cp)
{
#ifdef LEFT_RIGHT
	return(dynaOpSwr(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
#ifdef BYPASS_MEMn
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartSw(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		return(dynaOpSmartSw2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	dumb2++;
	dumb++;
	return(dynaOpSw(cp,MAKE_RT,MAKE_RS,MAKE_I));
#ifdef BASIC_MEM
	return(dynaOpSwr(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSWR(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_cache(BYTE *cp)
{
	WORD l;
	l=dynaCallInterpHelper(cp,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
/*
*/

	return(dynaNOP(cp));
}

WORD dynaCompile_ll(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef STERN_CHECK
	return(dynaOpLl(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLL(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_lwc1(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	return(dynaOpSmartLwc1(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	dumb++;
#ifdef STERN_CHECK
	return(dynaOpLwc1(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLWC1(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_lwc2(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	dumb++;
#ifdef STERN_CHECK
	return(dynaOpLwc2(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLWC2(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_lld(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	dumb++;
#ifdef STERN_CHECK
	return(dynaOpLld(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLLD(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_ldc1(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	return(dynaOpSmartLdc1(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	dumb++;
#ifdef STERN_CHECK
	return(dynaOpLdc1(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLDC1(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_ldc2(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	dumb++;
	dynaFPointer[MAKE_RT]=1;
#ifdef STERN_CHECK
	return(dynaOpLdc2(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLDC2(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_ld(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef SP_CATCHno
	if(MAKE_RS==29)
	{
		return(dynaOpLdSP(cp,MAKE_RT,MAKE_I));
	}
/*
	if(MAKE_RT==29)
	{
		return(dynaOpSpLoadD(cp,MAKE_RS,MAKE_I));
	}
*/
#endif
	if(0>=dynaFPointer[MAKE_RS])
	{
		dynaFPointer[MAKE_RT]=1;
		return(dynaOpSmartLd2(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	}
	smart2++;
	return(dynaOpSmartLd(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#ifdef NEW_MEM
	if(0>=dynaFPointer[MAKE_RS])
		return(dynaOpSmartLd(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
	dumb++;
#endif
	dynaFPointer[MAKE_RT]=1;
#ifdef BASIC_MEM
	return(dynaOpLd(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderLD(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_swc1(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	return(dynaOpSmartSwc1(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	dumb++;
#ifdef STERN_CHECK
	return(dynaOpSwc1(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSWC1(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_swc2(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	dumb++;
#ifdef STERN_CHECK
	return(dynaOpSwc2(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSWC2(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_sc(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	dumb++;
#ifdef STERN_CHECK
	return(dynaOpSc(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaOpSc(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_scd(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	dumb++;
#ifdef STERN_CHECK
	return(dynaOpScd(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaOpScd(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_sdc1(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
#ifdef NEW_MEM
	return(dynaOpSmartSdc1(cp,MAKE_RT,MAKE_RS,MAKE_I,dynaFPPage[MAKE_RS]));
#endif
	dumb++;
#ifdef STERN_CHECK
	return(dynaOpSdc1(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSDC1(cp,MAKE_RT,MAKE_RS,MAKE_I));
}

WORD dynaCompile_sdc2(BYTE *cp)
{
#ifdef BYPASS_MEM
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	dumb++;
#ifdef STERN_CHECK
	return(dynaOpSdc2(cp,MAKE_RT,MAKE_RS,MAKE_I));
#endif
	return(dynaCompileBuilderSDC2(cp,MAKE_RT,MAKE_RS,MAKE_I));
	return(dynaOpSdc2(cp,MAKE_RT,MAKE_RS,MAKE_I));
}


WORD dynaCompile_tgei(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_tgeiu(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_tlti(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_tltiu(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_teqi(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_tnei(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_cop0_rs_mf(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	WORD len=0;
	if(len=hleCheckFunction(cp,dynaPC))
	{
		return(len);
	}
	return(dynaCallInterpHelper(cp,iOpCode));
	return(dynaOpRs0Mf(cp,MAKE_RT,MAKE_RD));
}

WORD dynaCompile_cop0_rs_dmf(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaCallInterpHelper(cp,iOpCode));
	return(dynaOpRs0DMf(cp,MAKE_RT,MAKE_RD));
}

WORD dynaCompile_cop0_rs_cf(BYTE *cp)
{
	return(dynaOpRs0Cf(cp,MAKE_RT,MAKE_RD));
}

WORD dynaCompile_cop0_rs_mt(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaCallInterpHelper(cp,iOpCode));
	return(dynaOpRs0Mt(cp,MAKE_RD,MAKE_RT));
}

WORD dynaCompile_cop0_rs_dmt(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaCallInterpHelper(cp,iOpCode));
	return(dynaOpRs0DMt(cp,MAKE_RD,MAKE_RT));
}

WORD dynaCompile_cop0_rs_ct(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpRs0Ct(cp,MAKE_RD,MAKE_RT));
}

WORD dynaCompile_cop0_rs_bc(BYTE *cp)
{
	return(Cop0RTInstruction[(iOpCode>>16)&0x1f](cp));
}

WORD dynaCompile_cop0_rs_co(BYTE *cp)
{
	return(Cop0Instruction[iOpCode&0x3f](cp));
}

WORD dynaCompile_cop0_rt_bcf(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_cop0_rt_bct(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_cop0_rt_bcfl(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_cop0_rt_bctl(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_cop0_tlbr(BYTE *cp)
{
	return(dynaCallInterpHelper(cp,iOpCode));
	return(dynaNOP(cp));
}

WORD dynaCompile_cop0_tlbwi(BYTE *cp)
{
	return(dynaCallInterpHelper(cp,iOpCode));
	return(dynaNOP(cp));
}

WORD dynaCompile_cop0_tlbwr(BYTE *cp)
{
	return(dynaCallInterpHelper(cp,iOpCode));
	return(dynaNOP(cp));
}

WORD dynaCompile_cop0_tlbp(BYTE *cp)
{
	return(dynaCallInterpHelper(cp,iOpCode));
	return(dynaNOP(cp));
}

WORD dynaCompile_cop0_eret(BYTE *cp)
{
#ifdef BYPASS_ERET
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	return(dynaOpEret(cp));
}

WORD dynaCompile_cop1_rs_mf(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpRs1Mf(cp,MAKE_RT,MAKE_RD));
}

WORD dynaCompile_cop1_rs_dmf(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpRs1DMf(cp,MAKE_RT,MAKE_RD));
}

WORD dynaCompile_cop1_rs_cf(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpRs1Cf(cp,MAKE_RT,MAKE_RD));
}

WORD dynaCompile_cop1_rs_mt(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpRs1Mt(cp,MAKE_RD,MAKE_RT));
}

WORD dynaCompile_cop1_rs_dmt(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaOpRs1DMt(cp,MAKE_RD,MAKE_RT));
}

WORD dynaCompile_cop1_rs_ct(BYTE *cp)
{
#ifdef BYPASS_TRANSFER
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	return(dynaCallInterpHelper(cp,iOpCode));
	return(dynaOpRs1Ct(cp,MAKE_RD,MAKE_RT));
}

WORD dynaCompile_cop1_rs_bc(BYTE *cp)
{
	return(Cop1RTInstruction[(iOpCode>>16)&0x1f](cp));
}

WORD dynaCompile_cop1_rs_co(BYTE *cp)
{
	return(Cop1Instruction[iOpCode&0x3f](cp));
}

WORD dynaCompile_cop1_add(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPAddS(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	}
	return(dynaOpFPAddD(cp,MAKE_FD,MAKE_FS,MAKE_FT));
}

WORD dynaCompile_cop1_sub(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPSubS(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	}
	return(dynaOpFPSubD(cp,MAKE_FD,MAKE_FS,MAKE_FT));
}

WORD dynaCompile_cop1_mul(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPMulS(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	}
	return(dynaOpFPMulD(cp,MAKE_FD,MAKE_FS,MAKE_FT));
}

WORD dynaCompile_cop1_div(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPDivS(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	}
	return(dynaOpFPDivD(cp,MAKE_FD,MAKE_FS,MAKE_FT));
}

WORD dynaCompile_cop1_sqrt(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPSqrtS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPSqrtD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_abs(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPAbsS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPAbsD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_mov(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPMovS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPMovD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_neg(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPNegS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPNegD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_roundw(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPRoundWS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPRoundWD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_roundl(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPRoundLS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPRoundLD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_floorl(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPFloorLS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPFloorLD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_floorw(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPFloorWS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPFloorWD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_truncl(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPTruncLS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPTruncLD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_truncw(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPTruncWS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPTruncWD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_ceill(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPCeilLS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPCeilLD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_ceilw(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	if(MAKE_FMT==FMT_S)
	{
		return(dynaOpFPCeilWS(cp,MAKE_FD,MAKE_FS));
	}
	return(dynaOpFPCeilWD(cp,MAKE_FD,MAKE_FS));
}

WORD dynaCompile_cop1_cvts(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	switch(MAKE_FMT)
	{
	case FMT_S:
		return(dynaOpFPMovS(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_D:
		return(dynaOpFPCvtSFromD(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_W:
		return(dynaOpFPCvtSFromW(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_L:
		return(dynaOpFPCvtSFromL(cp,MAKE_FD,MAKE_FS));
		break;
	}
	return(dynaNOP(cp));
}

WORD dynaCompile_cop1_cvtd(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	switch(MAKE_FMT)
	{
	case FMT_S:
		return(dynaOpFPCvtDFromS(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_D:
		return(dynaOpFPMovD(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_W:
		return(dynaOpFPCvtDFromW(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_L:
		return(dynaOpFPCvtDFromL(cp,MAKE_FD,MAKE_FS));
		break;
	}
	return(dynaNOP(cp));
}

WORD dynaCompile_cop1_cvtw(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	switch(MAKE_FMT)
	{
	case FMT_S:
		return(dynaOpFPCvtWFromS(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_D:
		return(dynaOpFPCvtWFromD(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_W:
		return(dynaOpFPMovS(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_L:
		return(dynaOpFPCvtWFromL(cp,MAKE_FD,MAKE_FS));
		break;
	}
	return(dynaNOP(cp));
}

WORD dynaCompile_cop1_cvtl(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	switch(MAKE_FMT)
	{
	case FMT_S:
		return(dynaOpFPCvtLFromS(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_D:
		return(dynaOpFPCvtLFromD(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_W:
		return(dynaOpFPCvtLFromW(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_L:
		return(dynaOpFPMovD(cp,MAKE_FD,MAKE_FS));
		break;
	}
	return(dynaNOP(cp));
}

WORD dynaCompile_cop1_c(BYTE *cp)
{
#ifdef BYPASS_FP
	return(dynaCallInterpHelper(cp,iOpCode));
#endif
	switch(MAKE_FMT)
	{
		case FMT_S:   /* c.*.s */
		{
			switch(MAKE_F)
			{
				case 0x30:   /* c.f */
				case 0x38:   /* c.sf */
				case 0x39:   /* c.ngle */
					return(dynaOpFPSetFalse(cp));
					break;
					
				case 0x31:   /* c.un */
					return(dynaOpFPUnorderedS(cp,MAKE_FS,MAKE_FT));
					break;

				case 0x33:   /* c.ueq */
#ifdef UNORDER_COMP
					return(dynaOpFPEqualS(cp,MAKE_FS,MAKE_FT));
#endif
					break;
					
				case 0x32:   /* c.eq */
				case 0x3a:   /* c.seq */
				case 0x3b:   /* c.ngl */
					return(dynaOpFPEqualS(cp,MAKE_FS,MAKE_FT));
					break;

				case 0x35:   /* c.ult */
#ifdef UNORDER_COMP
					return(dynaOpFPLessThanS(cp,MAKE_FS,MAKE_FT));
#endif
					break;
					
				case 0x34:   /* c.olt */		//change1
				case 0x3c:   /* c.lt */
				case 0x3d:   /* c.nge */
					return(dynaOpFPLessThanS(cp,MAKE_FS,MAKE_FT));
					break;
					
				case 0x37:   /* c.ule */
				case 0x36:   /* c.ole */
#ifdef UNORDER_COMP
					return(dynaOpFPLessThanEqualS(cp,MAKE_FS,MAKE_FT));
#endif
					break;

				case 0x3f:   /* c.ngt */
				case 0x3e:   /* c.le */
					return(dynaOpFPLessThanEqualS(cp,MAKE_FS,MAKE_FT));
					break;
					
			} 
			break;
		}
		
		case FMT_D:   /* c.*.d */
		{
			switch(MAKE_F)
			{
				case 0x30:   /* c.f */
				case 0x38:   /* c.sf */
				case 0x39:   /* c.ngle */
					return(dynaOpFPSetFalse(cp));
					break;
					
				case 0x31:   /* c.un */
					return(dynaOpFPUnorderedD(cp,MAKE_FS,MAKE_FT));
					break;

				case 0x33:   /* c.ueq */
#ifdef UNORDER_COMP
					return(dynaOpFPEqualD(cp,MAKE_FS,MAKE_FT));
#endif
					break;
					
				case 0x32:   /* c.eq */
				case 0x3a:   /* c.seq */
				case 0x3b:   /* c.ngl */
					return(dynaOpFPEqualD(cp,MAKE_FS,MAKE_FT));
					break;

				case 0x35:   /* c.ult */
#ifdef UNORDER_COMP
					return(dynaOpFPLessThanD(cp,MAKE_FS,MAKE_FT));
#endif
					break;
					
				case 0x34:   /* c.olt */		//change1
				case 0x3c:   /* c.lt */
				case 0x3d:   /* c.nge */
					return(dynaOpFPLessThanD(cp,MAKE_FS,MAKE_FT));
					break;
					
				case 0x37:   /* c.ule */
				case 0x36:   /* c.ole */
#ifdef UNORDER_COMP
					return(dynaOpFPLessThanEqualD(cp,MAKE_FS,MAKE_FT));
#endif
					break;

				case 0x3f:   /* c.ngt */
				case 0x3e:   /* c.le */
					return(dynaOpFPLessThanEqualD(cp,MAKE_FS,MAKE_FT));
					break;
					
			}
		}
    } 
	return(dynaNOP(cp));
}

WORD dynaCompile_cop2_rs_not_implemented(BYTE *cp)
{
	return(dynaNOP(cp));
}

WORD dynaCompile_jr(BYTE *cp)
{
#ifdef JR
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;
	WORD len=dynaOpJr(cp,MAKE_RS);
	dynaInBranch=false;
	return(len);

}

WORD dynaCompile_jalr(BYTE *cp)
{
#ifdef JALR
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

	WORD len;
	len=dynaOpJalr(cp,MAKE_RD,MAKE_RS);
	dynaInBranch=false;
	return(len);
}


WORD dynaCompile_j(BYTE *cp)
{
#ifdef J
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;
	DWORD t=MAKE_T;

#ifdef BASIC_ILOOP
	if(((MAKE_T&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_T));
	}
#endif

	WORD len=0;
	len+=dynaOpJ(cp,MAKE_T);
	dynaInBranch=false;
	return(len);

}

WORD dynaCompile_jal(BYTE *cp)
{
#ifdef JAL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif

	WORD len;
	if(dynaInBranch)
	{
		return(NOP(cp));
	}

	if(len=hleCheckFunction(cp,MAKE_T))
	{
		return(len);
	}
/*
*/
	DWORD Target=MAKE_T;
	if(Target==0x2562f0)
		int stop=1;

	dynaInBranch=true;

	len=dynaOpJal(cp,MAKE_T);
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_beq(BYTE *cp)
{
	DWORD Target=MAKE_O;
#ifdef BEQ
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif

	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;


	WORD len=0;
	if(MAKE_RT==MAKE_RS)
	{
		len+=dynaOpJ(cp,MAKE_O);
		dynaInBranch=false;
		return(len);
	}

	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBeqSamePage(cp,MAKE_RT,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBeq(cp,MAKE_RT,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bne(BYTE *cp)
{
	DWORD Target=MAKE_O;
#ifdef BNE
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
/*
	if((MAKE_O&0x7fffff)==0x1408)
	{
		return(NOP(cp));
	}
*/
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

	WORD len;

	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBneSamePage(cp,MAKE_RT,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBne(cp,MAKE_RT,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_blez(BYTE *cp)
{
#ifdef BLEZ
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;


#ifdef BASIC_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0)&&(MAKE_RS==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBlezSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBlez(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bgtz(BYTE *cp)
{
#ifdef BGTZ
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBgtzSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBgtz(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_beql(BYTE *cp)
{
#ifdef BEQL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBeqlSamePage(cp,MAKE_RT,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBeql(cp,MAKE_RT,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bnel(BYTE *cp)
{
#ifdef BNEL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(MAKE_RT==MAKE_RS)
	{
		dynaInBranch=false;
		return(dynaOpBnel(cp,MAKE_RT,MAKE_RS,MAKE_O));
	}


	if(dynaInPage(MAKE_O)&&(dynaPC!=0x317937))
	{
		len=dynaOpBnelSamePage(cp,MAKE_RT,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBnel(cp,MAKE_RT,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_blezl(BYTE *cp)
{
#ifdef BLEZL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef BASIC_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0)&&(MAKE_RS==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBlezlSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBlezl(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bgtzl(BYTE *cp)
{
#ifdef BGTZL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBgtzlSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBgtzl(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bltz(BYTE *cp)
{
#ifdef BLTZ
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBltzSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBltz(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bgez(BYTE *cp)
{
#ifdef BGEZ
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBgezSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBgez(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bltzl(BYTE *cp)
{
#ifdef BLTZL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;


#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif
	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBltzlSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBltzl(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bgezl(BYTE *cp)
{
#ifdef BGEZL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBgezlSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBgezl(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bltzal(BYTE *cp)
{
#ifdef BLTZAL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
/*
	len=hleCheckFunction(cp,MAKE_O);
	if(len) return(len);
*/
	len=hleCheckFunctionBLTZAL(cp,MAKE_O,MAKE_RS);
	if(len) 
	{
		dynaInBranch=false;
		return(len);
	}

	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBltzalSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBltzal(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bgezal(BYTE *cp)
{
#ifdef BGEZAL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	len=hleCheckFunctionBGEZAL(cp,MAKE_O,MAKE_RS);
	dynaInBranch=false;
	if(len) 
	{
		dynaInBranch=false;
		return(len);
	}
/*
*/

	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBgezalSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBgezal(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bltzall(BYTE *cp)
{
#ifdef BLTZALL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBltzallSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBltzall(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_bgezall(BYTE *cp)
{
#ifdef BGEZALL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBgezallSamePage(cp,MAKE_RS,MAKE_O);
	}
	else
	{
		len=dynaOpBgezall(cp,MAKE_RS,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_cop1_rt_bcf(BYTE *cp)
{
#ifdef BCF
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;


#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif
	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBcfSamePage(cp,MAKE_O);
	}
	else
	{
		len=dynaOpBcf(cp,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_cop1_rt_bct(BYTE *cp)
{
#ifdef BCT
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBctSamePage(cp,MAKE_O);
	}
	else
	{
		len=dynaOpBct(cp,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_cop1_rt_bcfl(BYTE *cp)
{
#ifdef BCFL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif
	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;

#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif

	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBcflSamePage(cp,MAKE_O);
	}
	else
	{
		len=dynaOpBcfl(cp,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_cop1_rt_bctl(BYTE *cp)
{
#ifdef BCTL
	WORD l=0;
	l+=dynaCallInterpHelper(cp+l,iOpCode);
	l+=dynaFallOut(cp+l);
	return(l);
#endif

	if(dynaInBranch)
	{
		return(NOP(cp));
	}
	dynaInBranch=true;


#ifdef ALL_ILOOP
	if(((MAKE_O&MEM_MASK)==(dynaPC&MEM_MASK))&&(dynaNextOpCode==0))
	{
		dynaInBranch=false;
		return(dynaOpILoop(cp,MAKE_O));
	}
#endif
	WORD len;
	if(dynaInPage(MAKE_O))
	{
		len=dynaOpBctlSamePage(cp,MAKE_O);
	}
	else
	{
		len=dynaOpBctl(cp,MAKE_O);
	}
	dynaInBranch=false;
	return(len);
}

WORD dynaCompile_invalid(BYTE *cp)
{
	return(NOP(cp));
}
void dynaStep(DWORD Address)
{
}


#ifndef TRWIN
}
#endif
