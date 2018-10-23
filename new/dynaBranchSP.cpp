
#include "stdafx.h"
#include "math.h"
#include "ki.h"
#include "iCPU.h"

#include "DynaCompiler.h"
#include "dynaNative.h"

#include "dynaBranchSP.h"

extern void iCpuCatch();

#define DO_CHECKn (((NewPC&MEM_MASK)<=(dynaPC&MEM_MASK))) //&&(dynaNextOpCode))
#define DO_CHECK (0)

/*
	l+=dynaLoadRegPair(cp+l,op0);
	l+=XOR_DWORD_WITH_REG(cp+l,NATIVE_0,op1*8);
	l+=XOR_DWORD_WITH_REG(cp+l,NATIVE_3,op1*8+4);
	l+=NATIVE_OR_REG_WITH_EAX(cp+l,NATIVE_3);
	l+=CMP_REG_IMM(cp+l,NATIVE_0,0);

	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JE_LONG(cp+l,0); 
*/


WORD dynaOpBeqSamePage(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op0].Reg,op1*8);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JNE_LONG(cp+l,0);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JNE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}

WORD dynaOpBneSamePage(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	if(CompiledPC==0x3304dd9)
		int stop=1;
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op0].Reg,op1*8);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JE_LONG(cp+l,0);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
//	l+=STORE_DWORD_TO_RBANK(cp+l,NewPC,242*8);
	l+=JMP_LONG(cp+l,CompiledPC);

	JE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBlezSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	l+=INC_PC_COUNTER(cp+l);

	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JG_LONG(cp+l,0);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JG_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBgtzSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;
	if(CompiledPC==0x334e33d)
		int stop=1;

	l+=INC_PC_COUNTER(cp+l);

	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JLE_LONG(cp+l,0);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JLE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBltzSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	l+=INC_PC_COUNTER(cp+l);

	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JGE_LONG(cp+l,0);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JGE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBgezSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	l+=INC_PC_COUNTER(cp+l);

	l+=CMP_RBANK_WITH_IMM_BYTE(cp+l,op0*8,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JL_LONG(cp+l,0);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JL_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}

WORD dynaOpBeqlSamePage(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	DWORD TruePC=dynaGetCompiledAddress(NewPC);
	DWORD FalsePC=dynaGetCompiledAddress(dynaPC+8);
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}


	l+=INC_PC_COUNTER(cp+l);
	l+=INC_PC_COUNTER_S(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op0].Reg,op1*8);
	DWORD Offset=FalsePC-(DWORD)(cp+l+6);
	l+=JNE_LONG(cp+l,Offset);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,TruePC);
	return(l);
}


WORD dynaOpBnelSamePage(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	DWORD TruePC=dynaGetCompiledAddress(NewPC);
	DWORD FalsePC=dynaGetCompiledAddress(dynaPC+8);
	if(FalsePC==0x16bc699)
		int stop=1;

	l+=INC_PC_COUNTER(cp+l);
	l+=INC_PC_COUNTER_S(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op0].Reg,op1*8);
	DWORD Offset=FalsePC-(DWORD)(cp+l+6);
	l+=JE_LONG(cp+l,Offset);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,TruePC);
	return(l);
}

WORD dynaOpBlezlSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	DWORD TruePC=dynaGetCompiledAddress(NewPC);
	DWORD FalsePC=dynaGetCompiledAddress(dynaPC+8);
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}


	l+=INC_PC_COUNTER(cp+l);
	l+=INC_PC_COUNTER_S(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	DWORD Offset=FalsePC-(DWORD)(cp+l+6);
	l+=JG_LONG(cp+l,Offset);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,TruePC);
	return(l);
}


WORD dynaOpBgtzlSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	DWORD TruePC=dynaGetCompiledAddress(NewPC);
	DWORD FalsePC=dynaGetCompiledAddress(dynaPC+8);

	l+=INC_PC_COUNTER(cp+l);
	l+=INC_PC_COUNTER_S(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	DWORD Offset=FalsePC-(DWORD)(cp+l+6);
	l+=JLE_LONG(cp+l,Offset);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,TruePC);
	return(l);
}


WORD dynaOpBltzlSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	DWORD TruePC=dynaGetCompiledAddress(NewPC);
	DWORD FalsePC=dynaGetCompiledAddress(dynaPC+8);
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}


	l+=INC_PC_COUNTER(cp+l);
	l+=INC_PC_COUNTER_S(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	DWORD Offset=FalsePC-(DWORD)(cp+l+6);
	l+=JGE_LONG(cp+l,Offset);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,TruePC);
	return(l);
}


WORD dynaOpBgezlSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	DWORD TruePC=dynaGetCompiledAddress(NewPC);
	DWORD FalsePC=dynaGetCompiledAddress(dynaPC+8);

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	l+=INC_PC_COUNTER(cp+l);
	l+=INC_PC_COUNTER_S(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	DWORD Offset=FalsePC-(DWORD)(cp+l+6);
	l+=JL_LONG(cp+l,Offset);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,TruePC);
	return(l);
}

WORD dynaOpBltzalSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	l+=INC_PC_COUNTER(cp+l);

	l+=CMP_RBANK_WITH_IMM(cp+l,op0*8+4,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JGE_LONG(cp+l,0);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+8,31*8);		//link address
	l+=JMP_LONG(cp+l,CompiledPC);

	JGE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBgezalSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	l+=INC_PC_COUNTER(cp+l);

	l+=CMP_RBANK_WITH_IMM(cp+l,op0*8+4,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JL_LONG(cp+l,0);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+8,31*8);		//link address
	l+=JMP_LONG(cp+l,CompiledPC);

	JL_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBltzallSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	DWORD TruePC=dynaGetCompiledAddress(NewPC);
	DWORD FalsePC=dynaGetCompiledAddress(dynaPC+8);
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	l+=INC_PC_COUNTER(cp+l);
	l+=INC_PC_COUNTER_S(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	DWORD Offset=FalsePC-(DWORD)(cp+l+6);
	l+=JGE_LONG(cp+l,Offset);

	iOpCode=dynaNextOpCode;
	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+8,31*8);		//link address
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,TruePC);
	return(l);
}


WORD dynaOpBgezallSamePage(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	DWORD TruePC=dynaGetCompiledAddress(NewPC);
	DWORD FalsePC=dynaGetCompiledAddress(dynaPC+8);

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	l+=INC_PC_COUNTER(cp+l);
	l+=INC_PC_COUNTER_S(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op0].Reg,0);
	DWORD Offset=FalsePC-(DWORD)(cp+l+6);
	l+=JL_LONG(cp+l,Offset);

	iOpCode=dynaNextOpCode;
	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+8,31*8);		//link address
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,TruePC);
	return(l);
}


WORD dynaOpBctSamePage(BYTE *cp,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=ADD_IMM_TO_RBANK(cp+l,100,ICOUNT);
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;


	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,CCR1_+31*8);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JNZ_LONG(cp+l,0);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JNZ_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBctlSamePage(BYTE *cp,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD TruePC=dynaGetCompiledAddress(NewPC);
	DWORD FalsePC=dynaGetCompiledAddress(dynaPC+8);

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=ADD_IMM_TO_RBANK(cp+l,100,ICOUNT);
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}



	l+=INC_PC_COUNTER(cp+l);
	l+=INC_PC_COUNTER_S(cp+l);

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,CCR1_+31*8);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	DWORD Offset=FalsePC-(DWORD)(cp+l+6);
	l+=JNZ_LONG(cp+l,Offset);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,TruePC);
	return(l);
}


WORD dynaOpBcfSamePage(BYTE *cp,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;
	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=ADD_IMM_TO_RBANK(cp+l,100,ICOUNT);
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}


	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,CCR1_+31*8);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JZ_LONG(cp+l,0);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JZ_LONG(fixup1Ptr,(l-fixup1Len)-6);
	return(l);
}


WORD dynaOpBcflSamePage(BYTE *cp,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD TruePC=dynaGetCompiledAddress(NewPC);
	DWORD FalsePC=dynaGetCompiledAddress(dynaPC+8);

	l+=INC_PC_COUNTER(cp+l);
	l+=INC_PC_COUNTER_S(cp+l);

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,CCR1_+31*8);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	DWORD Offset=FalsePC-(DWORD)(cp+l+6);
	l+=JZ_LONG(cp+l,Offset);

	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,TruePC);
	return(l);
}

WORD dynaOpBgei(BYTE *cp,BYTE op0,BYTE op1,DWORD imm,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}
	l+=INC_PC_COUNTER(cp+l);

	l+=CMP_RBANK_WITH_IMM(cp+l,op0*8,imm);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JL_LONG(cp+l,0);

	l+=STORE_DWORD_TO_RBANK(cp+l,0,op1*8);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JL_LONG(fixup1Ptr,(l-fixup1Len)-6);
	l+=STORE_DWORD_TO_RBANK(cp+l,1,op1*8);
	return(l);
}


WORD dynaOpBgeiu(BYTE *cp,BYTE op0,BYTE op1,DWORD imm,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}
	l+=INC_PC_COUNTER(cp+l);

	l+=CMP_RBANK_WITH_IMM(cp+l,op0*8,imm);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JB_LONG(cp+l,0);

	l+=STORE_DWORD_TO_RBANK(cp+l,0,op1*8);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JB_LONG(fixup1Ptr,(l-fixup1Len)-6);
	l+=STORE_DWORD_TO_RBANK(cp+l,1,op1*8);
	return(l);
}

WORD dynaOpBli(BYTE *cp,BYTE op0,BYTE op1,DWORD imm,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}
	l+=INC_PC_COUNTER(cp+l);

	l+=CMP_RBANK_WITH_IMM(cp+l,op0*8,imm);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JGE_LONG(cp+l,0);

	l+=STORE_DWORD_TO_RBANK(cp+l,1,op1*8);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JGE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	l+=STORE_DWORD_TO_RBANK(cp+l,0,op1*8);
	return(l);
}

WORD dynaOpBliu(BYTE *cp,BYTE op0,BYTE op1,DWORD imm,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}
	l+=INC_PC_COUNTER(cp+l);

	l+=CMP_RBANK_WITH_IMM(cp+l,op0*8,imm);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JAE_LONG(cp+l,0);

	l+=STORE_DWORD_TO_RBANK(cp+l,1,op1*8);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JAE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	l+=STORE_DWORD_TO_RBANK(cp+l,0,op1*8);
	return(l);
}

WORD dynaOpBge(BYTE *cp,BYTE op0,BYTE op1,BYTE op2,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}
	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op0].Reg,op2*8);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JL_LONG(cp+l,0);

	l+=STORE_DWORD_TO_RBANK(cp+l,0,op1*8);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JL_LONG(fixup1Ptr,(l-fixup1Len)-6);
	l+=STORE_DWORD_TO_RBANK(cp+l,1,op1*8);
	return(l);
}

WORD dynaOpBgeu(BYTE *cp,BYTE op0,BYTE op1,BYTE op2,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}
	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op0].Reg,op2*8);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JB_LONG(cp+l,0);

	l+=STORE_DWORD_TO_RBANK(cp+l,0,op1*8);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JB_LONG(fixup1Ptr,(l-fixup1Len)-6);
	l+=STORE_DWORD_TO_RBANK(cp+l,1,op1*8);
	return(l);
}

WORD dynaOpBl(BYTE *cp,BYTE op0,BYTE op1,BYTE op2,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	BYTE *intbypass;
	WORD intbypasslen;
	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}
	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op0].Reg,op2*8);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JGE_LONG(cp+l,0);

	l+=STORE_DWORD_TO_RBANK(cp+l,1,op1*8);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JGE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	l+=STORE_DWORD_TO_RBANK(cp+l,0,op1*8);
	return(l);
}

WORD dynaOpBlu(BYTE *cp,BYTE op0,BYTE op1,BYTE op2,DWORD NewPC)
{
	WORD l=0;
	BYTE *fixup1Ptr;
	WORD fixup1Len;

	DWORD CompiledPC=dynaGetCompiledAddress(NewPC);
	NewPC |=0x80000000;

	BYTE *intbypass;
	WORD intbypasslen;

	if(DO_CHECK)
	{
		l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC,242*8);
		l+=CALL_CHECKINTS(cp+l);

		l+=CMP_RBANK_WITH_IMM(cp+l,242*8,0x80000180);
		intbypass=cp+l;
		intbypasslen=l;
		l+=JNE_SHORT(cp+l,0);
		l+=RETURN(cp+l);
		JNE_SHORT(intbypass,(l-intbypasslen)-2);
	}
	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op0);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op0].Reg,op2*8);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JAE_LONG(cp+l,0);

	l+=STORE_DWORD_TO_RBANK(cp+l,1,op1*8);
	iOpCode=dynaNextOpCode;
	l+=MainInstruction[dynaNextOpCode>>26](cp+l);
	l+=JMP_LONG(cp+l,CompiledPC);

	JAE_LONG(fixup1Ptr,(l-fixup1Len)-6);
	l+=STORE_DWORD_TO_RBANK(cp+l,0,op1*8);
	return(l);
}
