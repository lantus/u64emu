
#include "stdafx.h"
#include "math.h"
#include "ki.h"
#include "iMain.h"
#include "iCPU.h"
#include "iMemory.h"

#include "DynaCompiler.h"
#include "dynaNative.h"

#include "dynaFastMem.h"
#include "dynaMemHelpers.h"
#include "dynaSmartMem.h"
#include "dynaMemory.h"
/*

dynaFastMem

  During the intial compile the BuilderStoreXXX and BuildLoadXXX routines are compiled into the generated code. This is
  done in such a manner that the compiled code only calls the Build routines and then JMPs over an area of reserved 
  memory.  As follows

  call _BuilderStoreWord
  jmp here+BLOCK_SIZE
  ..
  ..
  reserved BLOCK_SIZE bytes
  ..
  ..
  next MIPS instruction

  The builder routine evaluates the Load/Store instruction and compiles custom code into the reserved space, overlaying
  the call to itself.  Thus the next execution of this compiled instruction will directly access the proper translated
  memory area.  The Builder routine accounts for TLB, normal 'virtual' memory and register accesses.  Thus there is a
  slight overhead during the first execution (ie; the time required to compile the custom code), but zero overhead for
  future executions.  Thus sections of the target ROM that are 'run once' initialization type routines will actually
  run somewhat slower, however, these sections are, by nature, rare and do not occur during active gameplay.  While this
  approach is somewhat radical, and self-modifying code is generally a bad idea, we are already creating the code on the 
  fly, so it really makes no difference if we modifiy that code later.  This actually fits the term 'Dynamic Recompilation' 
  better than what is currently the 'industry' standard.

  Assumptions:
	The calculated address for the load/store is based on the designated register and an immediate value.  The assumption
	is that the designated register value will not change by such amount that it would be accessing a different area of
	memory.  This would be possible if the designated base register values were read from a table and the same immediate
	offset were used for all... however, it would still have to 'cross' mapped areas to create a problem.  This is highly 
	unlikely, as it would make little sense for the programmer to code in such a fashion.  It is more likely that the e
	designated base register does not change at all, however, it is probably not safe to make this assumption.  Thus the
	translation routine returns a value that added to the base register and offset will equal the proper address.  Thus
	the base register _is_ allowed to change.  This will require and additional add instruction for each use. Thus follows
	the algorithm for the TranslateAddress function:

		TranslatedAddress=PhysicalAddress(VirtualBaseAddress+Immediate)-VirtualAddress; 

	where the routine PhysicalAddress() determines the actual memory mapping and VirtualBaseAddress is the value in the
	designated register.  During the second execution, the address is resolved as follows:  Note that the Immediate value
	has been accounted for in the value of TranslatedAddress.

		ActualAddress=TranslatedAddress+VirtualBaseAddress;

*/


#define ACCESS_LIMIT 1
#define INIT_VALUE 0
#define EXTEND_64
#define DO_UNALIGN 99

bool special;
DWORD mmio=0;

WORD MAKE_PHYS_ADDR(BYTE *codeptr,DWORD Offset,DWORD Imm,DWORD Mask)
{
	WORD l=0;
//	l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);
	l+=AND_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Mask);
	l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Offset);
	return(l);
}

#define BLOCK_SIZE 0x2a

void Prelude(BYTE *cp)
{
	memset(cp,0x90,BLOCK_SIZE);
}

WORD Postlude(BYTE *cp,WORD offset)
{
	WORD l=0;
	if(offset>BLOCK_SIZE)
		int stop1=0;
	l+=JMP_SHORT(cp,BLOCK_SIZE-offset);
	return(l);
}

void dynaTranslateReadAddress(DWORD VAddr,DWORD Imm,DWORD *Addr,DWORD *mask,BOOL *IsDirect,BOOL IsRead)
{
	*IsDirect=false;
	special=false;
//	return;
//	Imm=0;
	VAddr+=Imm;
	*mask=0x7fffff;
	{
		DWORD PhysAddr;
		*mask=0x7fffff;
		if((VAddr&0xff000000)==0xa8000000)
		{
			*Addr=(DWORD)(SRAM+Imm);
			*IsDirect=true;
			special=true;
			return;
		}
		if((VAddr&0xff000000)==0x88000000)
		{
			*Addr=(DWORD)(m->rdRam+Imm);
			*IsDirect=true;
			special=true;
			return;
		}
		if((VAddr&0xff000000)==0xa0000000)
		{
			*Addr=(DWORD)(m->rdRam+Imm);
			*IsDirect=true;
			special=true;
			return;
		}
		if((VAddr&0xff000000)==0x00000000)
		{
			if((VAddr<0x400)&&IsRead)
				*Addr=(DWORD)(m->rdRam+0x90000+Imm);
			else
				*Addr=(DWORD)(m->rdRam+Imm);
			*IsDirect=true;
			return;
		}
		if((VAddr&0xff000000)==0x80000000)
		{
			switch(VAddr&0x000f0000)
			{
				case 0:
				default:
				{
					*Addr=(DWORD)(m->rdRam+Imm);
					*IsDirect=true;
					special=true;
					return;
					break;
				}
				case 0x20000:
				case 0x30000:
				case 0x40000:
				case 0x50000:
				case 0x60000:
				case 0x70000:
				case 0x80000:
				case 0x90000:
				case 0xa0000:
				{
					*Addr=(DWORD)(SRAM+Imm-0x0000);
					*IsDirect=true;
					special=true;
					return;
					break;
				}
			}
		}
		*IsDirect=false;
	}
}

int dynaCompileBuilderSH(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSH);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	return(l);

}


int dynaCompileBuilderSW(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSW);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	return(l);

}


int dynaCompileBuilderSB(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSB);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	return(l);

}


int dynaCompileBuilderSD(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSD);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	return(l);

}

int dynaCompileBuilderLW(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
/*
	if((op0==15)&&(op1==15))
		int stop=1;
	if((DWORD)start==0x44B4B34)
		int stop=1;
*/
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLW);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}


int dynaCompileBuilderLWU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLWU);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLD(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLD);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLDL(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLDL);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}


int dynaCompileBuilderLDR(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLDR);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLWL(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLWL);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLWR(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLWR);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLB(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLB);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLH(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLH);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLWC1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLWC1);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLDC1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLDC1);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLWC2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLWC2);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLDC2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLDC2);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLL(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLL);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}


int dynaCompileBuilderLLD(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLLD);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLHU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLHU);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderLBU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderLBU);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

void dynaRuntimeBuilderSW(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	DWORD address=r->GPR[op1*2]; //+(int)Imm);
	DWORD Mask;
	DWORD value=r->GPR[op0*2];
	DWORD Offset;
	BOOL IsDirect;
	iMemWriteDWord(value,address+Imm);

	BOOL FlipFlopAddress=false;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,false);
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartSw((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSw((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}

void dynaRuntimeBuilderSB(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	DWORD address=r->GPR[op1*2]; //+(int)Imm);
	DWORD value=r->GPR[op0*2];
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;

	iMemWriteByte(value,address+Imm);

	BOOL FlipFlopAddress=false;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,false);
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartSb((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSb((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}

void dynaRuntimeBuilderSH(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	DWORD address=r->GPR[op1*2]; //+(int)Imm);
	DWORD value=r->GPR[op0*2];
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	iMemWriteWord(value,address+Imm);

	BOOL FlipFlopAddress=false;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,false);
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartSh((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSh((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderSD(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	DWORD address=r->GPR[op1*2]; //+(int)Imm);
	QWORD value=*(QWORD *)&r->GPR[op0*2];
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	iMemWriteQWord(value,address+Imm);

	BOOL FlipFlopAddress=false;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,false);
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{

			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartSd((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSd((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}

void dynaRuntimeBuilderLW(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	DWORD address=r->GPR[op1*2]; //+(int)Imm);
	DWORD help=(DWORD) helperLw;
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	value=iMemReadDWord(address+Imm);
	if(op0)
		*(sQWORD *)&r->GPR[op0*2]=value;

	BOOL FlipFlopAddress=false;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect&&(op0))
		{

			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLw((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpLw((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}

void dynaRuntimeBuilderLWU(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	DWORD address=r->GPR[op1*2]; //+(int)Imm);
	DWORD help=(DWORD)helperLwU;
	DWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	value=iMemReadDWord(address+Imm);
	if(op0)
		*(QWORD *)&r->GPR[op0*2]=value;

	BOOL FlipFlopAddress=false;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect&&(op0))
		{

			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLwU((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpLwU((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLD(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	DWORD address=r->GPR[op1*2]; //+(int)Imm);
	QWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	value=iMemReadQWord(address+Imm);
	if(op0)
		*(QWORD *)&r->GPR[op0*2]=value;

	BOOL FlipFlopAddress=false;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect&&(op0))
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLd((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLd((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLB(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sQWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	value=(sQWORD)(sDWORD)(char)iMemReadByte(address);
	if(op0)
		*(sQWORD *)&r->GPR[op0*2]=value;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect&&op0)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLb((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpLb((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLBU(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	DWORD address=r->GPR[op1*2]; //+(int)Imm);
	DWORD Mask;
	QWORD value;
	DWORD Offset;
	BOOL IsDirect;
	value=(QWORD)(BYTE)iMemReadByte(address+Imm);
	if(op0)
		*(QWORD *)&r->GPR[op0*2]=value;

	BOOL FlipFlopAddress=false;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect&&op0)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLbU((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{

			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpLbU((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLHU(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	QWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	value=(DWORD)(WORD)iMemReadWord(address);
	if(op0)
		*(QWORD *)&r->GPR[op0*2]=value;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect&&(op0))//&&(Offset!=(DWORD)dynaRamPtr))
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLhU((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpLhU((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLH(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sQWORD value;
	DWORD Mask;
	DWORD help=(DWORD) helperLh;
	DWORD Offset;
	BOOL IsDirect;
	value=(sQWORD)(sDWORD)(short)iMemReadWord(address);
	if(op0)
		*(sQWORD *)&r->GPR[op0*2]=value;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect&&(op0))//&&(Offset!=(DWORD)dynaRamPtr))
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLh((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpLh((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLWC1(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	value=(sDWORD)iMemReadDWord(address);
	r->FPR[op0]=value;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)//&&(Offset!=(DWORD)dynaRamPtr))
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLwc1((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpLwc1((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLL(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	value=(sDWORD)iMemReadDWord(address);

	r->GPR[op0*2]=value;
	r->CPR0[2*LLADDR] = (sQWORD)address;
	r->Llbit = 1;

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)//&&(Offset!=(DWORD)dynaRamPtr))
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);

			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),MEM_PTR,CPR0_+17*8);
			l+=LOAD_REG_IMM((BYTE *)(codeptr+l),NATIVE_0,1);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,1972);
//sadf
			l+=AND_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,MEM_MASK);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,(DWORD) dynaRamPtr);
			l+=LOAD_MEM_REG((BYTE *)(codeptr+l),MEM_PTR,NATIVE_0);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,op0*8);
#ifdef EXTEND_64
			l+=CONVERT_TO_QWORD((BYTE *)(codeptr+l));
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_3,op0*8+4);
#endif
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);

			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),MEM_PTR,CPR0_+17*8);
			l+=LOAD_REG_IMM((BYTE *)(codeptr+l),NATIVE_0,1);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,1972);
			
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),MEM_PTR);		//where
			l+=LOAD_REG_DWORD((BYTE *)(codeptr+l),NATIVE_0,(BYTE *)&iMemReadDWordAddress);
			l+=CALL_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,op0*8);
#ifdef EXTEND_64
			l+=CONVERT_TO_QWORD((BYTE *)(codeptr+l));
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_3,op0*8+4);
#endif
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLLD(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sQWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	value=(sDWORD)iMemReadQWord(address);

	*(sQWORD *)&r->GPR[op0*2]=value;
	r->CPR0[2*LLADDR] = (sQWORD)address;
	r->Llbit = 1;

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)//&&(Offset!=(DWORD)dynaRamPtr))
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);

			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),MEM_PTR,CPR0_+17*8);
			l+=LOAD_REG_IMM((BYTE *)(codeptr+l),NATIVE_0,1);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,1972);
//asdf
			l+=AND_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,MEM_MASK);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,(DWORD) dynaRamPtr);
			l+=LOAD_MEM_REG((BYTE *)(codeptr+l),MEM_PTR,NATIVE_3);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,4);
			l+=LOAD_MEM_REG((BYTE *)(codeptr+l),MEM_PTR,NATIVE_0);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,op0*8);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_3,op0*8+4);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);

			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),MEM_PTR,CPR0_+17*8);
			l+=LOAD_REG_IMM((BYTE *)(codeptr+l),NATIVE_0,1);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,1972);

			l+=PUSH_REGISTER((BYTE *)(codeptr+l),MEM_PTR);		//where
			l+=LOAD_REG_DWORD((BYTE *)(codeptr+l),NATIVE_0,(BYTE *)&iMemReadQWordAddress);
			l+=CALL_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,op0*8);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_3,op0*8+4);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLDC1(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sQWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	value=(sQWORD)iMemReadQWord(address);
	if(r->CPR0[2*STATUS] & 0x04000000)
	{
		r->FPR[op0]=(sDWORD)value;
	}
	else
	{
		r->FPR[op0]=(sDWORD)value;
		r->FPR[op0+1]=(sDWORD)(value>>32);
	}

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)//&&(Offset!=(DWORD)dynaRamPtr))
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLdc1((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpLdc1((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}

void dynaHelperLDL(DWORD address,DWORD op0)
{
	DWORD    offset;
	QWORD   data;
	
	offset = (DWORD)address;
	
	data=iMemReadQWord(offset & 0xfffffff8);

	switch(7-(offset % 8))
	{
	case 0:
		*(QWORD *)&r->GPR[op0*2] = data;
		break;
		
	case 1:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x00000000000000ff) | (data << 8);
		break;
		
	case 2:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x000000000000ffff) | (data << 16);
		break;
		
	case 3:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x0000000000ffffff) | (data << 24);
		break;
		
	case 4:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x00000000ffffffff) | (data << 32);
		break;
		
	case 5:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x000000ffffffffff) | (data << 40);
		break;
		
	case 6:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x0000ffffffffffff) | (data << 48);
		break;
		
	case 7:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x00ffffffffffffff) | (data << 56);
		break;
		
	} 
}

void dynaHelperLWL(DWORD address,DWORD op0)
{
	DWORD    offset;
	DWORD   data;
	
	offset = (DWORD)address;
	
	data=iMemReadDWord(offset & 0xfffffffc);

	switch(3-(offset % 4))
	{
	case 0:
		*(sQWORD *)&r->GPR[op0*2] = (sQWORD)(sDWORD)data;
		break;
		
	case 1:
		*(sQWORD *)&r->GPR[op0*2]  = (sQWORD)(sDWORD)((r->GPR[op0*2] & 0x000000ff) | (data << 8));
		break;
		
	case 2:
		*(sQWORD *)&r->GPR[op0*2]  = (sQWORD)(sDWORD)((r->GPR[op0*2] & 0x0000ffff) | (data << 16));
		break;
		
	case 3:
		*(sQWORD *)&r->GPR[op0*2]  = (sQWORD)(sDWORD)((r->GPR[op0*2] & 0x00ffffff) | (data << 24));
		break;
	}
}

void dynaHelperLDR(DWORD address,DWORD op0)
{
	DWORD    offset;
	QWORD   data;
	
	offset = (DWORD)address;
	
	data=iMemReadQWord(offset & 0xfffffff8);

	switch(7-(offset % 8))
	{
	case 7:
		*(QWORD *)&r->GPR[op0*2] = data;
		break;
		
	case 6:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xff00000000000000) | (data >> 8);
		break;
		
	case 5:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffff000000000000) | (data >> 16);
		break;
		
	case 4:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffffff0000000000) | (data >> 24);
		break;
		
	case 3:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffffffff00000000) | (data >> 32);
		break;
		
	case 2:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffffffffff000000) | (data >> 40);
		break;
		
	case 1:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffffffffffff0000) | (data >> 48);
		break;
		
	case 0:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffffffffffffff00) | (data >> 56);
		break;
		
	}
}

void dynaHelperLWR(DWORD address,DWORD op0)
{
	DWORD    offset;
	DWORD   data;
	
	offset = (DWORD)address;
	
	data=iMemReadDWord(offset & 0xfffffffc);

	switch(3-(offset % 4))
	{
	case 0:
		*(sQWORD *)&r->GPR[op0*2]  = (*(sQWORD *)&r->GPR[op0*2] & 0xffffff00) | (data >> 24);
		break;
		
	case 1:
		*(sQWORD *)&r->GPR[op0*2]  = (*(sQWORD *)&r->GPR[op0*2] & 0xffff0000) | (data >> 16);
		break;
		
	case 2:
		*(sQWORD *)&r->GPR[op0*2]  = (*(sQWORD *)&r->GPR[op0*2] & 0xff000000) | (data >> 8);
		break;
		
	case 3:
		*(sQWORD *)&r->GPR[op0*2]  = (sQWORD)(sDWORD)data;
		break;
	}
}

void dynaRuntimeBuilderLDL(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;

	dynaHelperLDL(address,op0);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm&0xffffff8,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect==98)//&&((Imm&7)==7))//&&(Offset!=(DWORD)dynaRamPtr))
		{
			int Part1,Part2,Count;
			DWORD Offset2;
			Offset2=7-(Imm&7);
			Count=(int)(Imm&7)+1;
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			WORD l=0;
			Prelude((BYTE *)(codeptr+l));
			l+=PUSH_ALL((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=CLD((BYTE *)(codeptr+l));
			Imm&=0xfffffff8;
			l+=MAKE_PHYS_ADDR((BYTE *)(codeptr+l),Offset,Imm,0x7ffff8);
			DWORD tmp=op0*8;
			l+=NATIVE_REG_TO_REG((BYTE *)(codeptr+l),PC_PTR,REG_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),PC_PTR,tmp+Offset2);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Offset2);		// src+4 (upper dword)
			l+=LOAD_REG_IMM((BYTE *)(codeptr+l),NATIVE_2,Count);
			l+=REP_MOVE_STORE_BYTE((BYTE *)(codeptr+l));
			l+=POP_ALL((BYTE *)(codeptr+l));
			l+=Postlude((BYTE *)(codeptr+l),l+2);
/*
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);
			l+=PUSH_DWORD((BYTE *)(codeptr+l),op0);				// reg number
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),MEM_PTR);		//where
			l+=LOAD_REG_DWORD((BYTE *)(codeptr+l),NATIVE_0,(BYTE *)&helperLDLAddress);
			l+=CALL_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
*/
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);
			l+=PUSH_DWORD((BYTE *)(codeptr+l),op0);				// reg number
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),MEM_PTR);		//where
			l+=LOAD_REG_DWORD((BYTE *)(codeptr+l),NATIVE_0,(BYTE *)&helperLDLAddress);
			l+=CALL_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLWL(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;

	dynaHelperLWL(address,op0);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
		NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect==DO_UNALIGN)//&&(Offset!=(DWORD)dynaRamPtr))
		{
			int Count;
			DWORD Offset2;
			Offset2=Imm&7;
			Count=8-(int)Offset2;
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=PUSH_ALL((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=CLD((BYTE *)(codeptr+l));
			l+=MAKE_PHYS_ADDR((BYTE *)(codeptr+l),Offset,Imm,Mask);
			DWORD tmp=op0*8;
			l+=NATIVE_REG_TO_REG((BYTE *)(codeptr+l),PC_PTR,REG_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),PC_PTR,tmp);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),PC_PTR,Offset2);		// dst+(offset%8)
			l+=LOAD_REG_IMM((BYTE *)(codeptr+l),NATIVE_2,Count);
			l+=REP_MOVE_STORE_BYTE((BYTE *)(codeptr+l));
			l+=POP_ALL((BYTE *)(codeptr+l));
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);
			l+=PUSH_DWORD((BYTE *)(codeptr+l),op0);				// reg number
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),MEM_PTR);		//where
			l+=LOAD_REG_DWORD((BYTE *)(codeptr+l),NATIVE_0,(BYTE *)&helperLWLAddress);
			l+=CALL_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLDR(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;

	dynaHelperLDR(address,op0);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLd((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpLd((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}

void dynaRuntimeBuilderLWR(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;

	dynaHelperLWR(address,op0);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartLw((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpLw((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderLDC2(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sQWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	value=(sQWORD)iMemReadQWord(address);
	r->CPR2[op0*2]=value;

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)//&&(Offset!=(DWORD)dynaRamPtr))
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=MAKE_PHYS_ADDR((BYTE *)(codeptr+l),Offset,Imm,Mask);
			l+=LOAD_MEM_REG((BYTE *)(codeptr+l),MEM_PTR,NATIVE_0);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,CPR2_+op0*8+4);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,4);
			l+=LOAD_MEM_REG((BYTE *)(codeptr+l),MEM_PTR,NATIVE_0);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,CPR2_+op0*8);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),MEM_PTR);		//where
			l+=LOAD_REG_DWORD((BYTE *)(codeptr+l),NATIVE_0,(BYTE *)&iMemReadQWordAddress);
			l+=CALL_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,CPR2_+op0*8);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_3,CPR2_+op0*8+4);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}

void dynaRuntimeBuilderLWC2(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	value=(sDWORD)iMemReadDWord(address);
	r->CPR2[op0*2]=(sQWORD)value;

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)//&&(Offset!=(DWORD)dynaRamPtr))
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=MAKE_PHYS_ADDR((BYTE *)(codeptr+l),Offset,Imm,Mask);
			l+=LOAD_MEM_REG((BYTE *)(codeptr+l),MEM_PTR,NATIVE_0);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,CPR2_+op0*8);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),MEM_PTR);		//where
			l+=LOAD_REG_DWORD((BYTE *)(codeptr+l),NATIVE_0,(BYTE *)&iMemReadDWordAddress);
			l+=CALL_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=STORE_REG_TO_RBANK((BYTE *)(codeptr+l),NATIVE_0,CPR2_+op0*8);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}

void bugFinder(DWORD what,DWORD where)
{
}


void dynaHelperSDL(DWORD offset,DWORD op0)
{
	QWORD   data;

	data=iMemReadQWord((DWORD)(offset & 0xfffffff8));
	
	switch(7-(offset % 8))
	{
	case 0:
		iMemWriteQWord(*(QWORD *)&r->GPR[op0*2], offset & 0xfffffff8);
		break;
		
	case 1:
		iMemWriteQWord((data & 0xff00000000000000) | (*(QWORD *)&r->GPR[op0*2] >> 8), offset & 0xfffffff8);
		break;
		
	case 2:
		iMemWriteQWord((data & 0xffff000000000000) | (*(QWORD *)&r->GPR[op0*2] >> 16), offset & 0xfffffff8);
		break;
		
	case 3:
		iMemWriteQWord((data & 0xffffff0000000000) | (*(QWORD *)&r->GPR[op0*2] >> 24), offset & 0xfffffff8);
		break;
		
	case 4:
		iMemWriteQWord((data & 0xffffffff00000000) | (*(QWORD *)&r->GPR[op0*2] >> 32), offset & 0xfffffff8);
		break;
		
	case 5:
		iMemWriteQWord((data & 0xffffffffff000000) | (*(QWORD *)&r->GPR[op0*2] >> 40), offset & 0xfffffff8);
		break;
		
	case 6:
		iMemWriteQWord((data & 0xffffffffffff0000) | (*(QWORD *)&r->GPR[op0*2] >> 48), offset & 0xfffffff8);
		break;
		
	case 7:
		iMemWriteQWord((data & 0xffffffffffffff00) | (*(QWORD *)&r->GPR[op0*2] >> 56), offset & 0xfffffff8);
		break;
		
	} /* switch(offset % 8) */
}

void dynaHelperSDR(DWORD offset,DWORD op0)
{
	QWORD   data;

	data=iMemReadQWord((DWORD)(offset & 0xfffffff8));
	
	switch(7-(offset % 8))
	{
	case 7:
		iMemWriteQWord(*(QWORD *)&r->GPR[op0*2], offset & 0xfffffff8);
		break;
		
	case 6:
		iMemWriteQWord((data & 0x00000000000000ff) | (*(QWORD *)&r->GPR[op0*2] << 8), offset & 0xfffffff8);
		break;
		
	case 5:
		iMemWriteQWord((data & 0x000000000000ffff) | (*(QWORD *)&r->GPR[op0*2] << 16), offset & 0xfffffff8);
		break;
		
	case 4:
		iMemWriteQWord((data & 0x0000000000ffffff) | (*(QWORD *)&r->GPR[op0*2] << 24), offset & 0xfffffff8);
		break;
		
	case 3:
		iMemWriteQWord((data & 0x00000000ffffffff) | (*(QWORD *)&r->GPR[op0*2] << 32), offset & 0xfffffff8);
		break;
		
	case 2:
		iMemWriteQWord((data & 0x000000ffffffffff) | (*(QWORD *)&r->GPR[op0*2] << 40), offset & 0xfffffff8);
		break;
		
	case 1:
		iMemWriteQWord((data & 0x0000ffffffffffff) | (*(QWORD *)&r->GPR[op0*2] << 48), offset & 0xfffffff8);
		break;
		
	case 0:
		iMemWriteQWord((data & 0x00ffffffffffffff) | (*(QWORD *)&r->GPR[op0*2] << 56), offset & 0xfffffff8);
		break;
		
	} /* switch(offset % 8) */
}

void dynaHelperSWR(DWORD offset,DWORD op0)
{
	DWORD  data;
	DWORD  old_data;

	old_data=iMemReadDWord((DWORD)(offset & 0xfffffffc));
	
	switch(3-(offset % 4))
	{
	case 0:
		data = (old_data & 0x00ffffff) | ((DWORD)r->GPR[op0*2] << 24);
		break;
		
	case 1:
		data = (old_data & 0x0000ffff) | ((DWORD)r->GPR[op0*2] << 16);
		break;
		
	case 2:
		data = (old_data & 0x000000ff) | ((DWORD)r->GPR[op0*2] << 8);
		break;
		
	case 3:
		data = (DWORD)r->GPR[op0*2];
		break;
		
	default:
		data = 0;   /* to make compiler happy */
	}
	
	iMemWriteDWord(data, (DWORD)(offset & 0xfffffffc));
}


void dynaHelperSWL(DWORD offset,DWORD op0)
{
	DWORD  data;
	DWORD  old_data;
	
	old_data = iMemReadDWord((DWORD)(offset & 0xfffffffc));
	
	switch(3-(offset % 4))
	{
	case 0:
		data = (DWORD)r->GPR[op0*2];
		break;
		
	case 1:
		data = (old_data & 0xff000000) | ((DWORD)r->GPR[op0*2] >> 8);
		break;
		
	case 2:
		data = (old_data & 0xffff0000) | ((DWORD)r->GPR[op0*2] >> 16);
		break;
		
	case 3:
		data = (old_data & 0xffffff00) | ((DWORD)r->GPR[op0*2] >> 24);
		break;
		
	default:
		data = 0;   /* to make compiler happy */
	}
	
	iMemWriteDWord(data, (DWORD)(offset & 0xfffffffc));
}


int dynaCompileBuilderSDL(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSDL);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}


int dynaCompileBuilderSDR(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSDR);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderSWL(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSWL);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}


int dynaCompileBuilderSWR(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSWR);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderSWC1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSWC1);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderSDC1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSDC1);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderSWC2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSWC2);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

int dynaCompileBuilderSDC2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *start=cp;

	memset(cp+l,0x90,BLOCK_SIZE);
	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,(DWORD)start);
	l+=PUSH_ALL(cp+l);
	l+=PUSH_STACK_POINTER(cp+l);
	l+=PUSH_DWORD(cp+l,Imm);			//imm
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=PUSH_DWORD(cp+l,(DWORD)start);	//overwrite code area
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaRuntimeBuilderSDC2);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+BLOCK_SIZE, which will be the
									// next compiled instruction.  This way we don't risk returning to code that
									// we have invalidated with new instructions!
									// note the called function will handle cleaning up the stack
	*(DWORD *)(cp+(BLOCK_SIZE-8))=ACCESS_LIMIT;
	*(DWORD *)(cp+(BLOCK_SIZE-4))=INIT_VALUE;
	l+=BLOCK_SIZE-l;	// reserve an area of memory
	return(l);

}

void dynaRuntimeBuilderSWC1(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;

	iMemWriteDWord(r->FPR[op0],address);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartSwc1((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSwc1((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderSWC2(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;

	iMemWriteQWord(*(sQWORD *)&r->CPR2[op0*2],address);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=LOAD_REG_FROM_RBANK((BYTE *)(codeptr+l),NATIVE_0,CPR2_+op0*8);
			l+=MAKE_PHYS_ADDR((BYTE *)(codeptr+l),Offset,Imm,Mask);
			l+=STORE_MEM_REG((BYTE *)(codeptr+l),MEM_PTR,NATIVE_0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=LOAD_REG_FROM_RBANK((BYTE *)(codeptr+l),NATIVE_0,CPR2_+op0*8);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),MEM_PTR);		//where
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),NATIVE_0);	//what
			l+=LOAD_REG_DWORD((BYTE *)(codeptr+l),NATIVE_0,(BYTE *)&iMemWriteDWordAddress);
			l+=CALL_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderSDC1(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;

	iMemWriteDWord(r->FPR[op0],address);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartSdc1((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSdc1((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderSDC2(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;

	iMemWriteQWord(*(sQWORD *)&r->CPR2[op0*2],address);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
	NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=LOAD_REG_FROM_RBANK((BYTE *)(codeptr+l),NATIVE_0,CPR2_+op0*8);
			l+=MAKE_PHYS_ADDR((BYTE *)(codeptr+l),Offset,Imm,Mask);
			l+=STORE_MEM_REG((BYTE *)(codeptr+l),MEM_PTR,NATIVE_0);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,4);
			l+=LOAD_REG_FROM_RBANK((BYTE *)(codeptr+l),NATIVE_0,CPR2_+op0*8+4);
			l+=STORE_MEM_REG((BYTE *)(codeptr+l),MEM_PTR,NATIVE_0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			//not used, will not work
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=LOAD_REG_FROM_RBANK((BYTE *)(codeptr+l),NATIVE_0,CPR2_+op0*8);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),MEM_PTR);		//where
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),NATIVE_0);	//what
			l+=LOAD_REG_DWORD((BYTE *)(codeptr+l),NATIVE_0,(BYTE *)&iMemWriteDWordAddress);
			l+=CALL_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderSWL(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	DWORD help=(DWORD) dynaHelperSWL;

	dynaHelperSWL(address,op0);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
		NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect==DO_UNALIGN)//&&(Offset!=(DWORD)dynaRamPtr))
		{
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);
			l+=PUSH_DWORD((BYTE *)(codeptr+l),op0);				// reg number
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),MEM_PTR);		//where
			l+=LOAD_REG_DWORD((BYTE *)(codeptr+l),NATIVE_0,(BYTE *)&help);
			l+=CALL_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderSWR(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	DWORD help=(DWORD) dynaHelperSWR;

	dynaHelperSWR(address,op0);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
		NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartSw((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSw((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderSDL(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	DWORD help=(DWORD) dynaHelperSDL;

	dynaHelperSDL(address,op0);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
		NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect==DO_UNALIGN)//&&(Offset!=(DWORD)dynaRamPtr))
		{
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=INC_PC_COUNTER((BYTE *)(codeptr+l));
			l+=LOAD_REG((BYTE *)(codeptr+l),op1,MEM_PTR);
			l+=ADD_REG_IMM((BYTE *)(codeptr+l),MEM_PTR,Imm);
			l+=PUSH_DWORD((BYTE *)(codeptr+l),op0);				// reg number
			l+=PUSH_REGISTER((BYTE *)(codeptr+l),MEM_PTR);		//where
			l+=LOAD_REG_DWORD((BYTE *)(codeptr+l),NATIVE_0,(BYTE *)&help);
			l+=CALL_REGISTER((BYTE *)(codeptr+l),NATIVE_0);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=POP_REGISTER((BYTE *)(codeptr+l),NATIVE_2);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}


void dynaRuntimeBuilderSDR(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer)
{
	WORD l=0;
	BOOL FlipFlopAddress=false;	
	DWORD address=r->GPR[op1*2]+Imm;
	DWORD LastAddress=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-4));
	DWORD NumAccess=*(DWORD  *)((BYTE *)codeptr+(BLOCK_SIZE-8));
	sDWORD value;
	DWORD Mask;
	DWORD Offset;
	BOOL IsDirect;
	DWORD help=(DWORD) dynaHelperSDR;

	dynaHelperSDR(address,op0);

	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-4))=address;
		NumAccess--;
	*(DWORD *)((BYTE *)codeptr+(BLOCK_SIZE-8))=NumAccess;
	dynaTranslateReadAddress(address,Imm,&Offset,&Mask,&IsDirect,true);
	if(((LastAddress&0xff000000)!=(address&0xff000000))&&(LastAddress!=0))
	{
		IsDirect=false;
		FlipFlopAddress=true;
	}
	if((NumAccess==0)||FlipFlopAddress)
	{
		if(IsDirect)
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSmartSd((BYTE *)(codeptr+l),op0,op1,Imm,0);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
		else
		{
			Prelude((BYTE *)(codeptr+l));
			l+=dynaOpSd((BYTE *)(codeptr+l),op0,op1,Imm);
			l+=Postlude((BYTE *)(codeptr+l),l+2);
		}
	}
	_asm
	{
		mov esp,StackPointer	// nasty - get the original stack pointer; ie; before we called the BuilderFunction
								// this 'cleans' the stack.  We put five 32 bit values on the stack and the C++ compiler
								// put some stuff on it as well... we have no way of knowing how much, which is why we 
								// sent the original stackpointer in as a variable.
		popad					// all the original register, cause who knows what C++ did to them.... ;)
		add edx,BLOCK_SIZE			// eax contains the starting address... return back to BLOCK_SIZE past that
		push edx				// new return address
		ret						// hehe... bye!
	}
}

/*

  vaddr=r->GPR[op1*8];
  vaddy+=Imm;
  paddy=mapping[vaddy>>24];
  paddy+=vaddy&0x7fffff;
  r->GPR[op0*8]=*paddy;

   _asm
   {
	mov edi,ebp
	mov ebx,op1*8
	mov ecx,[ebp+ebx]
	add edi,(op1*8)-(op0*8)
	add ecx,Imm





*/
