
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "DynaCompiler.h"
#include "dynaNative.h"
#include "dynaMemory.h"
#include "dynaSPCheat.h"
#include "dynaMemHelpers.h"

#define RAW_ACCESSn

extern DWORD dumb;

WORD LOAD_CALL1(BYTE *cp,DWORD dst,DWORD base, DWORD Imm)
{
	WORD l=0;

	l+=LOAD_REG((BYTE *)(cp+l),base,MEM_PTR);
	l+=PUSH_DWORD((BYTE *)(cp+l),dst);				// reg number
	l+=ADD_REG_IMM((BYTE *)(cp+l),MEM_PTR,Imm);
	l+=PUSH_REGISTER((BYTE *)(cp+l),MEM_PTR);		//where
	return(l);
}

WORD CALL_FUNC(BYTE *cp,DWORD address)
{
	WORD l=0;
	DWORD Current=(DWORD)cp+5;
	DWORD Offset=address-Current;
	*(cp++)=0xe8;
	memcpy(cp,&Offset,4);
	return(5);
}

WORD CLEAN_STACK(BYTE *cp,WORD count)
{
	WORD l=0;
	for(WORD i=0;i<count/4;i++)
	{
		l+=POP_REGISTER((BYTE *)(cp+l),NATIVE_2);
	}
	return(l);
}


WORD dynaOpLui(BYTE *cp,BYTE op0,DWORD Imm)
{
	WORD l=0;
	DWORD High;

	if(Imm&0x80000000)
		High=0xffffffff;
	else
		High=0;
	l+=INC_PC_COUNTER(cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,(DWORD)Imm,op0*8);
	l+=STORE_DWORD_TO_RBANK(cp+l,(DWORD)High,op0*8+4);

	return(l);
}

WORD dynaOpLb(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLb);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpLbU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLbU);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpLh(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLh);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}


WORD dynaOpLhU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLhU);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpLw(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLw);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}


WORD dynaOpLwU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLwU);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpLd(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLd);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}


WORD dynaOpSb(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperSb);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpSh(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperSh);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpSw(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperSw);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpSd(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperSd);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}


WORD dynaOpLwl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLwl);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpLwr(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLwr);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpLdl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLdl);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpLdr(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLdr);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpSwr(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperSwr);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpSwl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperSwl);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpSdl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperSdl);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpSdr(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperSdr);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpLl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLl);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpLld(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperLld);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

WORD dynaOpLwc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG(cp+l,op1,MEM_PTR);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,Imm);
	l+=AND_REG_IMM(cp+l,MEM_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,(DWORD) dynaRamPtr);
	l+=LOAD_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,_FPR+op0*4);
	return(l);
}

WORD dynaOpLwc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG(cp+l,op1,MEM_PTR);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,Imm);
	l+=AND_REG_IMM(cp+l,MEM_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,(DWORD) dynaRamPtr);
	l+=LOAD_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,CPR1_+op0*8);
	return(l);
}

WORD dynaOpLdc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG(cp+l,op1,MEM_PTR);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,Imm);
	l+=AND_REG_IMM(cp+l,MEM_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,(DWORD) dynaRamPtr);
	l+=LOAD_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,_FPR+op0*4);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,4);
	l+=LOAD_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,_FPR+op0*4+4);
	return(l);
}

WORD dynaOpLdc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG(cp+l,op1,MEM_PTR);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,Imm);
	l+=AND_REG_IMM(cp+l,MEM_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,(DWORD) dynaRamPtr);
	l+=LOAD_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,4);
	l+=LOAD_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,CPR1_+op0*8+4);
	return(l);
}


WORD dynaOpSwc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG(cp+l,op1,MEM_PTR);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,Imm);
	l+=AND_REG_IMM(cp+l,MEM_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,(DWORD) dynaRamPtr);
	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,_FPR+op0*4);
	l+=STORE_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	return(l);
}

WORD dynaOpSwc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG(cp+l,op1,MEM_PTR);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,Imm);
	l+=AND_REG_IMM(cp+l,MEM_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,(DWORD) dynaRamPtr);
	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,CPR1_+op0*8);
	l+=STORE_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	return(l);
}

WORD dynaOpSdc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG(cp+l,op1,MEM_PTR);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,Imm);
	l+=AND_REG_IMM(cp+l,MEM_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,(DWORD) dynaRamPtr);
	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,_FPR+op0*4);
	l+=STORE_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,4);
	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,_FPR+op0*4+4);
	l+=STORE_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	return(l);
}

WORD dynaOpSdc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1l,fixup2l;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG(cp+l,op1,MEM_PTR);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,Imm);
	l+=AND_REG_IMM(cp+l,MEM_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,(DWORD) dynaRamPtr);
	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,CPR1_+op0*8);
	l+=STORE_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,4);
	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,CPR1_+op0*8+4);
	l+=STORE_MEM_REG(cp+l,MEM_PTR,NATIVE_0);
	return(l);
}

WORD dynaOpSc(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperSc);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}


WORD dynaOpScd(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	WORD l=0;

	l+=LOAD_CALL1(cp+l,op0,op1,Imm);
	l+=CALL_FUNC(cp+l,(DWORD)helperScd);
	l+=CLEAN_STACK(cp+l,8);

	return(l);
}

