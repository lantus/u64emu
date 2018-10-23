#include "stdafx.h"
#include "math.h"
#include "ki.h"

//#define CHECK_NEWPC
#include "DynaCompiler.h"
#include "dynaNative.h"

#include "dynaBranch.h"
#include "imain.h"


extern DWORD ForceInt;

WORD globalAudioIncRateWhole=10000;
WORD globalAudioIncRatePartial=10000;
WORD globalAudioIncRateFrac=10000;
WORD dynaNextAIWhole=0;
WORD dynaNextAIPartial=0;

extern void iCpuVSYNC();
#define FULL_COMPARE
//#define CHECK_INTS_FIRST


void StopMe()
{
	DWORD Stop=1;
}

WORD dynaOpILoop(BYTE *cp,DWORD NewPC)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,NewPC,242*8);

	l+=CALL_FUNCTION(cp+l,(DWORD)iCpuVSYNC);
	l+=RETURN(cp+l);
	return(l);
}

WORD dynaOpEret(BYTE *cp)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=AND_DWORD_WITH_IMM(cp+l,0xfffffffd,CPR0_+12*8);
	l+=MEM_TO_MEM_DWORD(cp+l,242*8,CPR0_+14*8);
	l+=ZERO_REG(cp+l,NATIVE_0);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,1972);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,1944);
	l+=RETURN(cp+l);
	return(l);
}

bool dynaIsInfinite(DWORD target,DWORD pc)
{
	target&=MEM_MASK;
	pc&=MEM_MASK;
	DWORD *src=(DWORD *)&m->rdRam[target&MEM_MASK];
	DWORD count=pc-target;

	return false;
/*
	if(count<=4) return true;
	if(count>1024) return false;
	count-=4;

	OpCodeInfo *tmpOpCodes=rcDecodeRange((DWORD *)m->rdRam,target,count/4);
	OpCodeInfo *thisOpCode=&tmpOpCodes[0];


	WORD jalCount=0;
	bool LoadDetect=false;
	DWORD next=pc;
	pc=target;
	while(count)
	{
		switch(thisOpCode->Type)
		{
		case OP_Jal:
			{
				jalCount++;
				if(jalCount>1) return false;
				return false;
				break;
			}		
//		case OP_Jalr:
		case OP_Jr:
		case OP_J:
			{
				return false;
				break;
			}
		case OP_Bgezal:
		case OP_Bltzal:
		case OP_Bgezall:
		case OP_Bltzall:
		case OP_Break:
		case OP_Beq:
		case OP_Bne:
		case OP_Blez:
		case OP_Bgtz:
		case OP_Beql:
		case OP_Bnel:
		case OP_Blezl:
		case OP_Bgtzl:
		case OP_Bltz:
		case OP_Bgez:
		case OP_Bltzl:
		case OP_Bgezl:
		case OP_Bcf:
		case OP_Bct:
		case OP_Bcfl:
		case OP_Bctl:
			{
				DWORD btarg=(pc+((thisOpCode->Code&0xffff)<<2))&MEM_MASK;
				if(btarg>=next)
					return false;
				break;
			}
		case OP_Lw:
		case OP_Lb:
		case OP_Lbu:
			{
				LoadDetect=true;
				break;
			}
		case OP_Sw:
		case OP_Sb:
		case OP_Swl:
		case OP_Swr:
		case OP_Sd:
		case OP_Swc1:
			{
				return false;
				break;
			}
		}
		thisOpCode++;
		count-=4;
		pc+=4;
	}
	SafeFree(tmpOpCodes);

	return LoadDetect;
*/
}

WORD dynaOpJ(BYTE *cp,DWORD NewPC)
{
	WORD l=0;

	DWORD CompiledPC;
	DWORD Page;
	DWORD Offset;
	NewPC|=0x80000000;
	Page=  (NewPC&PAGE_MASK)>>PAGE_SHIFT;
	Offset=(NewPC&OFFSET_MASK)>>OFFSET_SHIFT;
	CompiledPC=(DWORD) dynaPageTable[Page].Offset[Offset];

	l+=INC_PC_COUNTER(cp+l);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	if((Page==dynaCurPage)&&((NewPC&MEM_MASK)>(dynaPC&MEM_MASK)))
	{
		CompiledPC=(DWORD) dynaPageTable[Page].Offset[Offset];
		l+=JMP_LONG(cp+l,CompiledPC);
	}
	else
	{
		iOpCode=dynaNextOpCode;
		l+=STORE_DWORD_TO_RBANK(cp+l,NewPC,242*8);
		if(dynaIsInfinite(NewPC,dynaPC))
		{
			theApp.LogMessage("Nuked iloop at %X",dynaPC);
			l+=CALL_FUNCTION(cp+l,(DWORD)iCpuVSYNC);
			l+=CALL_CHECKINTS(cp+l);
		}
		l+=RETURN(cp+l);
	}
	return(l);
}


WORD dynaOpJr(BYTE *cp,BYTE op0)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+4,242*8);

	iOpCode=dynaNextOpCode;
	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,op0*8);
//	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,PCDELAY);
//	l+=STORE_DWORD_TO_RBANK(cp+l,1,DELAY);

	l+=PUSH_REGISTER(cp+l,NATIVE_0);

	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}

WORD dynaOpJalr(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);


	iOpCode=dynaNextOpCode;
	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,op1*8);
	l+=PUSH_REGISTER(cp+l,NATIVE_0);
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);

	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+8,op0*8);
	
	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpJal(BYTE *cp,DWORD NewPC)
{
	DWORD CompiledPC;
	DWORD Page;
	DWORD Offset;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	NewPC+=0x80000000;
	Page=  (NewPC&PAGE_MASK)>>PAGE_SHIFT;
	Offset=(NewPC&OFFSET_MASK)>>OFFSET_SHIFT;


	WORD l=0;

	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
	l+=CALL_CHECKINTS(cp+l);

	l+=LOAD_REG_FROM_RBANK(cp+l,PC_PTR,242*8);
	l+=CMP_REG_IMM(cp+l,PC_PTR,0x80000180);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JNE_SHORT(cp+l,0);
	l+=RETURN(cp+l);
	JNE_SHORT(fixup1Ptr,(l-fixup1Len)-2);
/*
*/
	
	l+=INC_PC_COUNTER(cp+l);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	if(Page==dynaCurPage)
	{
		CompiledPC=(DWORD) dynaPageTable[Page].Offset[Offset];
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+8,31*8);
		l+=JMP_LONG(cp+l,CompiledPC);
	}
	else
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,NewPC,242*8);
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+8,31*8);

		l+=RETURN(cp+l);
	}

	return(l);
}

WORD dynaOpBeq(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;


	l+=dynaLoadRegPair(cp+l,op0);
	l+=XOR_DWORD_WITH_REG(cp+l,NATIVE_0,op1*8);
#ifdef FULL_COMPARE
//	l+=XOR_DWORD_WITH_REG(cp+l,NATIVE_3,op1*8+4);
//	l+=NATIVE_OR_REG_WITH_EAX(cp+l,NATIVE_3);
#endif

	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JNE_LONG(cp+l,0); 

	iOpCode=dynaNextOpCode;

	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,NewPC,242*8);
	l+=RETURN(cp+l);

	JNE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}



WORD dynaOpBne(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;


	l+=dynaLoadRegPair(cp+l,op0);
	l+=XOR_DWORD_WITH_REG(cp+l,NATIVE_0,op1*8);
#ifdef FULL_COMPARE
//	l+=XOR_DWORD_WITH_REG(cp+l,NATIVE_3,op1*8+4);
//	l+=NATIVE_OR_REG_WITH_EAX(cp+l,NATIVE_3);
#endif

	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JE_LONG(cp+l,0); 

	iOpCode=dynaNextOpCode;

	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,NewPC,242*8);
	l+=RETURN(cp+l);

	JE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBlez(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;


#ifdef FULL_COMPARE
	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8+4,0);
#else
	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8,0);
#endif
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JG_LONG(cp+l,0); 

	iOpCode=dynaNextOpCode;

	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,NewPC,242*8);
	l+=RETURN(cp+l);

	JG_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBgtz(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;


#ifdef FULL_COMPARE
	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8+4,0);
#else
	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8,0);
#endif
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JLE_LONG(cp+l,0); 

	iOpCode=dynaNextOpCode;

	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,NewPC,242*8);
	l+=RETURN(cp+l);

	JLE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBltz(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;


#ifdef FULL_COMPARE
	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8+4,0);
#else
	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8,0);
#endif
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JGE_LONG(cp+l,0); 

	iOpCode=dynaNextOpCode;

	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,NewPC,242*8);
	l+=RETURN(cp+l);

	JGE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBgez(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;


#ifdef FULL_COMPARE
	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8+4,0);
#else
	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8,0);
#endif
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JL_LONG(cp+l,0); 

	iOpCode=dynaNextOpCode;

	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,NewPC,242*8);
	l+=RETURN(cp+l);

	JL_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBeql(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op0].Reg,op1*8);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JNE_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JNE_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);

	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpBnel(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op0].Reg,op1*8);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JE_SHORT(cp+l,0);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=PUSH_DWORD(cp+l,NewPC);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JE_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);


	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}

WORD dynaOpBlezl(BYTE *cp,BYTE op0,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JG_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JG_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);

	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpBgtzl(BYTE *cp,BYTE op0,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JLE_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JLE_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);

	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpBltzl(BYTE *cp,BYTE op0,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JGE_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JGE_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);

	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpBgezl(BYTE *cp,BYTE op0,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JL_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JL_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);

	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}

WORD dynaOpBltzal(BYTE *cp,BYTE op0,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadRegPair(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,NATIVE_3,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JGE_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JGE_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);

	iOpCode=dynaNextOpCode;

	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+8,31*8);		//link address

	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpBgezal(BYTE *cp,BYTE op0,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadRegPair(cp+l,op0);
	l+=AND_REG_IMM(cp+l,NATIVE_3,0x80000000);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JMP_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JMP_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);

	iOpCode=dynaNextOpCode;

	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+8,31*8);		//link address

	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpBltzall(BYTE *cp,BYTE op0,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JGE_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JGE_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	l+=INC_PC_COUNTER(cp+l);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);

	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+8,31*8);		//link address

	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpBgezall(BYTE *cp,BYTE op0,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JL_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JL_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	l+=INC_PC_COUNTER(cp+l);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);

	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+8,31*8);		//link address

	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpBct(BYTE *cp,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,CCR1_+31*8);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JZ_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JZ_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);

	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpBctl(BYTE *cp,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,CCR1_+31*8);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JZ_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JZ_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);


	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpBcf(BYTE *cp,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;


	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,CCR1_+31*8);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JNZ_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JNZ_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);

	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}


WORD dynaOpBcfl(BYTE *cp,DWORD NewPC)
{
#ifdef CHECK_NEWPC
	if(NewPC>0x80400000)
		StopMe();
#endif
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,CCR1_+31*8);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JNZ_SHORT(cp+l,0);
	l+=PUSH_DWORD(cp+l,NewPC);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	JNZ_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=PUSH_DWORD(cp+l,dynaPC+8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);


	l+=POP_REGISTER(cp+l,PC_PTR);
	l+=STORE_REG(cp+l,242,PC_PTR);
	l+=RETURN(cp+l);
	return(l);
}

