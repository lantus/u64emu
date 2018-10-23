
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "DynaCompiler.h"
#include "dynaNative.h"
#include "dynaGeneral.h"

#define WIDTH_MULT 2
#define LOGIC64

WORD dynaOpSra(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_GPR(cp+l,op1*8,op0*8);
	l+=SRA_REG_IMM(cp+l,NATIVE_0,op2);
	l+=STORE_SE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpSrl(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_GPR(cp+l,op1*8,op0*8);
	l+=SRL_REG_IMM(cp+l,NATIVE_0,op2);
	l+=STORE_SE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpSll(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_GPR(cp+l,op1*8,op0*8);
	l+=SLL_REG_IMM(cp+l,NATIVE_0,op2);
	l+=STORE_SE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpSraEq(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=SAR_RBANK_IMM(cp+l,op0*8,op2);
	return(l);
}

WORD dynaOpSrlEq(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=SHR_RBANK_IMM(cp+l,op0*8,op2);
	return(l);
}

WORD dynaOpSllEq(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=SHL_RBANK_IMM(cp+l,op0*8,op2);
	return(l);
}

WORD dynaOpSraV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_GPR(cp+l,op1*8,op0*8);
	l+=dynaLoadRegForce(cp+l,op2,NATIVE_2);		// must be ecx
	l+=AND_REG_IMM(cp+l,NATIVE_2,0x1f);
	l+=SRA_REG_REG(cp+l,NATIVE_0,NATIVE_2);
	l+=STORE_SE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpSrlV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_GPR(cp+l,op1*8,op0*8);
	l+=dynaLoadRegForce(cp+l,op2,NATIVE_2);		// must be ecx
	l+=AND_REG_IMM(cp+l,NATIVE_2,0x1f);
	l+=SRL_REG_REG(cp+l,NATIVE_0,NATIVE_2);
	l+=STORE_SE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpSllV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_GPR(cp+l,op1*8,op0*8);
	l+=dynaLoadRegForce(cp+l,op2,NATIVE_2);		// must be ecx
	l+=AND_REG_IMM(cp+l,NATIVE_2,0x1f);
	l+=SLL_REG_REG(cp+l,NATIVE_0,NATIVE_2);
	l+=STORE_SE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpMfHi(BYTE *cp,BYTE op0)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	DWORD DstOff,SrcOff;
	DstOff=op0*8;
	SrcOff=HI;
	l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);
	return(l);
}

WORD dynaOpRegMove(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	DWORD DstOff,SrcOff;
	DstOff=op0*8;
	SrcOff=op1*8;
//	l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
	l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);
	return(l);
}

WORD dynaOpRegDMove(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	DWORD DstOff,SrcOff;
	DstOff=op0*8;
	SrcOff=op1*8;
	l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);
	return(l);
}

WORD dynaOpAddEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op1);
	l+=CONVERT_TO_QWORD(cp+l);
	l+=ADD_REG_TO_DWORD(cp+l,dynaMapToNative[op1].Reg,op0*8);
	l+=ADC_REG_TO_DWORD(cp+l,NATIVE_3,op0*8+4);
	dynaKillNative(dynaMapToNative[op1].Reg);
	//fixme, killnative should not be needed later
	return(l);
}


WORD dynaOpAdd(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_GPR(cp+l,op1*8,op0*8);
	l+=ADD_DWORD_TO_REG(cp+l,NATIVE_0,op2*8);
	l+=STORE_SE_D_GPR(cp+l);
	return(l);
}


WORD dynaOpSubEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=dynaLoadReg(cp+l,op1);
	l+=CONVERT_TO_QWORD(cp+l);
	l+=SUB_REG_FROM_DWORD(cp+l,dynaMapToNative[op1].Reg,op0*8);
	l+=SBB_REG_FROM_DWORD(cp+l,NATIVE_3,op0*8+4);
	dynaKillNative(dynaMapToNative[op1].Reg);
	//fixme, killnative should not be needed later
	return(l);
}


WORD dynaOpSub(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_GPR(cp+l,op1*8,op0*8);
	l+=SUB_DWORD_FROM_REG(cp+l,NATIVE_0,op2*8);
	l+=STORE_SE_D_GPR(cp+l);
	return(l);
}


WORD dynaOpMtHi(BYTE *cp,BYTE op0)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	DWORD DstOff,SrcOff;
	SrcOff=op0*8;
	DstOff=HI;
	l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);
	return(l);
}


WORD dynaOpMfLo(BYTE *cp,BYTE op0)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	DWORD DstOff,SrcOff;
	DstOff=op0*8;
	SrcOff=LO;
	l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);
	return(l);
}


WORD dynaOpMtLo(BYTE *cp,BYTE op0)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	DWORD DstOff,SrcOff;
	SrcOff=op0*8;
	DstOff=LO;
	l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);


	return(l);
}

WORD dynaOpMult(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx

	l+=dynaLoadRegForce(cp+l,op0,NATIVE_0);			// mov eax,op0
//	l+=dynaLoadReg(cp+l,op1);						// mov ???,op1
	l+=IMUL(cp+l,op0,op1);	// imul op1
	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx	 - save HI DWORD
	l+=CONVERT_TO_QWORD(cp+l);						// convert low DWORD to signed qword
	DWORD tmp=LO;
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);			//
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in lo
	l+=POP_REGISTER(cp+l,NATIVE_0);					// get edx back as eax
	l+=CONVERT_TO_QWORD(cp+l);						// convert high DWORD to signed qword
	tmp=HI;	
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in hi

	l+=POP_REGISTER(cp+l,NATIVE_3);					// recover edx

	dynaKillNative(NATIVE_0);

	return(l);
}

WORD dynaOpMultU(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx

	l+=dynaLoadRegForce(cp+l,op0,NATIVE_0);			// mov eax,op0
//	l+=dynaLoadReg(cp+l,op1);						// mov ???,op1
	l+=MUL(cp+l,op0,op1);	// imul op1
	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx	 - save HI DWORD
	l+=CONVERT_TO_QWORD(cp+l);						// convert low DWORD to signed qword
	DWORD tmp=LO;
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);			//
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in lo
	l+=POP_REGISTER(cp+l,NATIVE_0);					// get edx back as eax
	l+=CONVERT_TO_QWORD(cp+l);						// convert high DWORD to signed qword
	tmp=HI;	
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in hi

	l+=POP_REGISTER(cp+l,NATIVE_3);					// recover edx

	dynaKillNative(NATIVE_0);

	return(l);
}


WORD dynaOpDiv(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	WORD fixup1Len,fixup2Len;
	BYTE *fixup1Ptr,*fixup2Ptr;

	l+=INC_PC_COUNTER(cp+l);

	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_3,op1*8);	// check for div by zero... no Mips expection is thrown
	l+=CMP_REG_IMM(cp+l,NATIVE_3,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JZ_SHORT(cp+l,0);

	l+=dynaLoadRegForce(cp+l,op0,NATIVE_0);			// mov eax,op0

//	l+=ZERO_REG(cp+l,NATIVE_3);						// mov edx,0
	l+=CONVERT_TO_QWORD(cp+l);
	l+=IDIV(cp+l,op0,op1);	// imul op1
	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx	 - save HI DWORD
	l+=CONVERT_TO_QWORD(cp+l);						// convert low DWORD to signed qword
	DWORD tmp=LO;
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);			//
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in lo
	l+=POP_REGISTER(cp+l,NATIVE_0);					// get edx back as eax
	l+=CONVERT_TO_QWORD(cp+l);						// convert high DWORD to signed qword
	tmp=HI;	
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in hi

	JZ_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=POP_REGISTER(cp+l,NATIVE_3);					// recover edx

	dynaKillNative(NATIVE_0);

	return(l);
}

WORD dynaOpDivU(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	WORD fixupLen;
	BYTE *fixupPtr;

	l+=INC_PC_COUNTER(cp+l);

	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_3,op1*8);	// check for div by zero... no Mips expection is thrown
	l+=CMP_REG_IMM(cp+l,NATIVE_3,0);
	fixupPtr=cp+l;
	fixupLen=l;
	l+=JZ_SHORT(cp+l,0);

	l+=dynaLoadRegForce(cp+l,op0,NATIVE_0);			// mov eax,op0
	l+=ZERO_REG(cp+l,NATIVE_3);						// mov edx,0
//	l+=CONVERT_TO_QWORD(cp+l);
	l+=DIV(cp+l,op0,op1);	// imul op1
	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx	 - save HI DWORD
	l+=CONVERT_TO_QWORD(cp+l);						// convert low DWORD to signed qword
	DWORD tmp=LO;
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);			//
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in lo
	l+=POP_REGISTER(cp+l,NATIVE_0);					// get edx back as eax
	l+=CONVERT_TO_QWORD(cp+l);						// convert high DWORD to signed qword
	tmp=HI;	
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in hi

	JZ_SHORT(fixupPtr,(l-fixupLen)-2);
	l+=POP_REGISTER(cp+l,NATIVE_3);					// recover edx

	dynaKillNative(NATIVE_0);

	return(l);
}

WORD dynaOpSlt(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
/*
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);
	l+=dynaLoadReg(cp+l,op1);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op1].Reg,op2*8);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JGE_SHORT(cp+l,0);
	DWORD tmp=1;
	l+=STORE_DWORD_TO_RBANK(cp+l,tmp,op0*8);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	tmp=0;
	JGE_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=STORE_DWORD_TO_RBANK(cp+l,tmp,op0*8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);
	dynaKillNative(dynaMapToNative[op1].Reg);
	return(l);
*/
/*
	_asm
	{
		mov eax,dword ptr [ebp+0x04]
		mov edx,dword ptr [ebp+0x08]
		xor ebx,ebx
		cmp eax,edx
		setl bl
		mov dword ptr [ebp+04],ebx
	}
*/
	*(cp++)=0x8b;
	*(cp++)=0x45;
	*(cp++)=op1*8-128;

	*(cp++)=0x8b;
	*(cp++)=0x55;
	*(cp++)=op2*8-128;

	*(cp++)=0x33;
	*(cp++)=0xdb;

	*(cp++)=0x3b;
	*(cp++)=0xc2;

	*(cp++)=0x0f;
	*(cp++)=0x9c;
	*(cp++)=0xc3;

	*(cp++)=0x89;
	*(cp++)=0x5d;
	*(cp++)=op0*8-128;
	return(16);
}

WORD dynaOpSltU(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
/*
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);
	l+=dynaLoadReg(cp+l,op1);
	l+=CMP_DWORD_WITH_REG(cp+l,dynaMapToNative[op1].Reg,op2*8);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JAE_SHORT(cp+l,0);
	DWORD tmp=1;
	l+=STORE_DWORD_TO_RBANK(cp+l,tmp,op0*8);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	tmp=0;
	JAE_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=STORE_DWORD_TO_RBANK(cp+l,tmp,op0*8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);
	dynaKillNative(dynaMapToNative[op1].Reg);
	return(l);
*/
/*
	_asm
	{
		mov eax,dword ptr [ebp+0x04]
		mov edx,dword ptr [ebp+0x08]
		xor ebx,ebx
		cmp eax,edx
		setb bl
		mov dword ptr [ebp+04],ebx
	}
*/
	*(cp++)=0x8b;
	*(cp++)=0x45;
	*(cp++)=op1*8-128;

	*(cp++)=0x8b;
	*(cp++)=0x55;
	*(cp++)=op2*8-128;

	*(cp++)=0x33;
	*(cp++)=0xdb;

	*(cp++)=0x3b;
	*(cp++)=0xc2;

	*(cp++)=0x0f;
	*(cp++)=0x92;
	*(cp++)=0xc3;

	*(cp++)=0x89;
	*(cp++)=0x5d;
	*(cp++)=op0*8-128;
	return(16);
}

WORD dynaOpSltI(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate)
{
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

//	l+=PUSH_REGISTER(cp+l,NATIVE_0);
//	l+=LOAD_REG_DWORD(cp+l,NATIVE_0,(BYTE *)&SneekyAddress);
//	l+=CALL_REGISTER(cp+l,NATIVE_0);
//	l+=POP_REGISTER(cp+l,NATIVE_0);

	l+=INC_PC_COUNTER(cp+l);
/*
	l+=dynaLoadReg(cp+l,op1);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op1].Reg,Immediate);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JGE_SHORT(cp+l,0);
	DWORD tmp=1;
	l+=STORE_DWORD_TO_RBANK(cp+l,tmp,op0*8);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	tmp=0;
	JGE_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=STORE_DWORD_TO_RBANK(cp+l,tmp,op0*8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);
	dynaKillNative(dynaMapToNative[op1].Reg);
*/
/*
	_asm
	{
		xor ebx,ebx
		mov eax,dword ptr [ebp+0x04]
		cmp eax,0x1234
		setl bl
		mov dword ptr [ebp+04],ebx
	}
*/
	*(cp++)=0x33;
	*(cp++)=0xdb;
	*(cp++)=0x8b;
	*(cp++)=0x45;
	*(cp++)=op1*8-128;
	*(cp++)=0x3d;
	memcpy(cp,&Immediate,4);
	cp+=4;
	*(cp++)=0x0f;
	*(cp++)=0x9c;
	*(cp++)=0xc3;
	*(cp++)=0x89;
	*(cp++)=0x5d;
	*(cp++)=op0*8-128;
	return(16);
}

WORD dynaOpSltIU(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate)
{
/*
	WORD l=0;
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;

	l+=INC_PC_COUNTER(cp+l);
	l+=dynaLoadReg(cp+l,op1);
	l+=CMP_REG_IMM(cp+l,dynaMapToNative[op1].Reg,Immediate);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JAE_SHORT(cp+l,0);
	DWORD tmp=1;
	l+=STORE_DWORD_TO_RBANK(cp+l,tmp,op0*8);
	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JMP_SHORT(cp+l,0);
	tmp=0;
	JAE_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=STORE_DWORD_TO_RBANK(cp+l,tmp,op0*8);
	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);
	dynaKillNative(dynaMapToNative[op1].Reg);
	return(l);
*/
/*
	_asm
	{
		xor ebx,ebx
		mov eax,dword ptr [ebp+0x04]
		cmp eax,0x1234
		setb bl
		mov dword ptr [ebp+04],ebx
	}
*/
	*(cp++)=0x33;
	*(cp++)=0xdb;
	*(cp++)=0x8b;
	*(cp++)=0x45;
	*(cp++)=op1*8-128;
	*(cp++)=0x3d;
	memcpy(cp,&Immediate,4);
	cp+=4;
	*(cp++)=0x0f;
	*(cp++)=0x92;
	*(cp++)=0xc3;
	*(cp++)=0x89;
	*(cp++)=0x5d;
	*(cp++)=op0*8-128;
	return(16);
}


WORD dynaOpDAddEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=ADD_REG_TO_DWORD(cp+l,NATIVE_0,op0*8);
	l+=ADC_REG_TO_DWORD(cp+l,NATIVE_3,op0*8+4);
	return(l);
}


WORD dynaOpDAdd(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=ADD_DWORD_TO_REG(cp+l,NATIVE_0,op2*8);
	l+=ADC_DWORD_TO_REG(cp+l,NATIVE_3,op2*8+4);
	l+=STORE_D_GPR(cp+l);
	return(l);
}


WORD dynaOpDSubEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=SUB_REG_FROM_DWORD(cp+l,NATIVE_0,op0*8);
	l+=SBB_REG_FROM_DWORD(cp+l,NATIVE_3,op0*8+4);
	return(l);
}


WORD dynaOpDSub(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=SUB_DWORD_FROM_REG(cp+l,NATIVE_0,op2*8);
	l+=SBB_DWORD_FROM_REG(cp+l,NATIVE_3,op2*8+4);
	l+=STORE_D_GPR(cp+l);
	return(l);
}


void dynaHelpDMult(DWORD op0,DWORD op1)
{
	/*
	sQWORD tmp=*(sQWORD *)&m_Reg[op0*2]**(sQWORD *)&m_Reg[op1*2];
	*(sQWORD *)&r->Lo=(sQWORD)(sDWORD)tmp;
	*(sQWORD *)&r->Hi=(sQWORD)(sDWORD)(tmp>>32);
*/
	DWORD x0=r->GPR[op0*2];
	DWORD x1=r->GPR[op0*2+1];
	DWORD y0=r->GPR[op1*2];
	DWORD y1=r->GPR[op1*2+1];
	DWORD p0,p1,p2=0,p3=0;
	DWORD sign1=x1&0x80000000;
	DWORD sign2=y1&0x80000000;
	if(sign1)
	{
		QWORD tmp=((QWORD)x1<<32)+x0;
		tmp=~tmp;
		tmp++;
		x0=(DWORD)tmp;
		x1=(tmp>>32);
	}
	if(sign2)
	{
		QWORD tmp=((QWORD)y1<<32)+y0;
		tmp=~tmp;
		tmp++;
		y0=(DWORD)tmp;
		y1=(tmp>>32);
	}
	sign1^=sign2;
	_asm
	{
		mov eax,dword ptr x0
		mul dword ptr y0
		mov dword ptr p1,edx
		mov dword ptr p0,eax
		mov eax,dword ptr x0
		mul dword ptr y1
		add dword ptr p1,eax
		adc dword ptr p2,edx
		adc dword ptr p3,0
		mov eax,dword ptr x1
		mul dword ptr y0
		add dword ptr p1,eax
		adc dword ptr p2,edx
		adc dword ptr p3,0
		mov eax,dword ptr x1
		mul dword ptr y1
		add dword ptr p2,eax
		adc dword ptr p3,edx
		mov eax,dword ptr sign1
		cmp eax,0
		jz sign_ok
		not dword ptr p0
		not dword ptr p1
		not dword ptr p2
		not dword ptr p3
		add dword ptr p0,1
		adc dword ptr p1,0
		adc dword ptr p2,0
		adc dword ptr p3,0
sign_ok:
	}
	r->Lo=((QWORD)p1<<32)+(p0);
	r->Hi=((QWORD)p3<<32)+(p2);
}

void dynaHelpDMultU(DWORD op0,DWORD op1)
{
	DWORD x0=r->GPR[op0*2];
	DWORD x1=r->GPR[op0*2+1];
	DWORD y0=r->GPR[op1*2];
	DWORD y1=r->GPR[op1*2+1];
	DWORD p0,p1,p2=0,p3=0;
	_asm
	{
		mov eax,dword ptr x0
		mul dword ptr y0
		mov dword ptr p1,edx
		mov dword ptr p0,eax
		mov eax,dword ptr x0
		mul dword ptr y1
		add dword ptr p1,eax
		adc dword ptr p2,edx
		adc dword ptr p3,0
		mov eax,dword ptr x1
		mul dword ptr y0
		add dword ptr p1,eax
		adc dword ptr p2,edx
		adc dword ptr p3,0
		mov eax,dword ptr x1
		mul dword ptr y1
		add dword ptr p2,eax
		adc dword ptr p3,edx
	}
	r->Lo=((QWORD)p1<<32)+(p0);
	r->Hi=((QWORD)p3<<32)+(p2);
}

WORD dynaOpDMult(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp+l);

	l+=PUSH_ALL(cp+l);
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaHelpDMult);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+0x50, which will be the
	l+=POP_REGISTER(cp+l,NATIVE_0);
	l+=POP_REGISTER(cp+l,NATIVE_0);
	l+=POP_ALL(cp+l);
/*
	l+=INC_PC_COUNTER(cp+l);

	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx

	l+=dynaLoadRegForce(cp+l,op0,NATIVE_0);			// mov eax,op0
	l+=IMUL(cp+l,op0,op1);	// imul op1
	DWORD tmp=LO;
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);			//
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in lo
	tmp=HI;	
	l+=ZERO_REG(cp+l,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_0);			// save in hi

	l+=POP_REGISTER(cp+l,NATIVE_3);					// recover edx

	dynaKillNative(NATIVE_0);
*/
	return(l);
}

WORD dynaOpDMultU(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp+l);

	l+=PUSH_ALL(cp+l);
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaHelpDMultU);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+0x50, which will be the
	l+=POP_REGISTER(cp+l,NATIVE_0);
	l+=POP_REGISTER(cp+l,NATIVE_0);
	l+=POP_ALL(cp+l);
/*
	l+=INC_PC_COUNTER(cp+l);

	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx

	l+=dynaLoadRegForce(cp+l,op0,NATIVE_0);			// mov eax,op0
	l+=MUL(cp+l,op0,op1);	// imul op1
	DWORD tmp=LO;
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);			//
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in lo
	tmp=HI;	
	l+=ZERO_REG(cp+l,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_0);			// save in hi

	l+=POP_REGISTER(cp+l,NATIVE_3);					// recover edx

	dynaKillNative(NATIVE_0);
*/
	return(l);
}


void dynaHelpDDiv(DWORD op0,DWORD op1)
{
	if(*(__int64*)&r->GPR[op1*WIDTH_MULT] != 0)
	{
		r->Lo = *(__int64*)&r->GPR[op0*WIDTH_MULT] / *(__int64*)&r->GPR[op1*WIDTH_MULT];
		r->Hi = *(__int64*)&r->GPR[op0*WIDTH_MULT] % *(__int64*)&r->GPR[op1*WIDTH_MULT];
	}
	else
	{
		int shit=1;
	}
}


void dynaHelpDDivU(DWORD op0,DWORD op1)
{
	if(*(__int64*)&r->GPR[op1*WIDTH_MULT] != 0)
	{
		r->Lo = *(QWORD*)&r->GPR[op0*WIDTH_MULT] / *(QWORD*)&r->GPR[op1*WIDTH_MULT];
		r->Hi = *(QWORD*)&r->GPR[op0*WIDTH_MULT] % *(QWORD*)&r->GPR[op1*WIDTH_MULT];
	}
	else
	{
		int shit=1;
	}
}


WORD dynaOpDDiv(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	WORD fixup1Len,fixup2Len;
	BYTE *fixup1Ptr,*fixup2Ptr;

	l+=INC_PC_COUNTER(cp+l);

	l+=PUSH_ALL(cp+l);
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaHelpDDiv);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+0x50, which will be the
	l+=POP_REGISTER(cp+l,NATIVE_0);
	l+=POP_REGISTER(cp+l,NATIVE_0);
	l+=POP_ALL(cp+l);

/*
	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_3,op1*8);	// check for div by zero... no Mips expection is thrown
	l+=CMP_REG_IMM(cp+l,NATIVE_3,0);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JZ_SHORT(cp+l,0);

	l+=dynaLoadRegPair(cp+l,op0);

	l+=IDIV(cp+l,op0,op1);	// imul op1
	DWORD tmp=LO;
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);			//
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in lo
	tmp=HI;	
	l+=ZERO_REG(cp+l,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_0);			// save in hi

	JZ_SHORT(fixup1Ptr,(l-fixup1Len)-2);
	l+=POP_REGISTER(cp+l,NATIVE_3);					// recover edx

	dynaKillNative(NATIVE_0);
*/

	return(l);
}

WORD dynaOpDDivU(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	WORD fixupLen;
	BYTE *fixupPtr;

	l+=INC_PC_COUNTER(cp+l);

	l+=PUSH_ALL(cp+l);
	l+=PUSH_DWORD(cp+l,op1);			//base
	l+=PUSH_DWORD(cp+l,op0);			//what
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,(DWORD)dynaHelpDDivU);
	l+=CALL_REGISTER(cp+l,NATIVE_0);	// the called address will return back to 'start'+0x50, which will be the
	l+=POP_REGISTER(cp+l,NATIVE_0);
	l+=POP_REGISTER(cp+l,NATIVE_0);
	l+=POP_ALL(cp+l);


/*
	l+=PUSH_REGISTER(cp+l,NATIVE_3);				// push edx

	l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_3,op1*8);	// check for div by zero... no Mips expection is thrown
	l+=CMP_REG_IMM(cp+l,NATIVE_3,0);
	fixupPtr=cp+l;
	fixupLen=l;
	l+=JZ_SHORT(cp+l,0);

	l+=dynaLoadRegPair(cp+l,op0);
	l+=DIV(cp+l,op0,op1);	// imul op1
	DWORD tmp=LO;
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);			//
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_3);			// save in lo
	tmp=HI;	
	l+=ZERO_REG(cp+l,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp,NATIVE_0);
	l+=REG_TO_MEM_DWORD(cp+l,tmp+4,NATIVE_0);			// save in hi

	JZ_SHORT(fixupPtr,(l-fixupLen)-2);
	l+=POP_REGISTER(cp+l,NATIVE_3);					// recover edx

	dynaKillNative(NATIVE_0);
*/

	return(l);
}


WORD dynaOpAddI(BYTE *cp,BYTE rt,BYTE rs,sDWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_GPR(cp+l,rs*8,rt*8);
	l+=ADD_REG_IMM(cp+l,NATIVE_0,Immediate);
	l+=CONVERT_TO_QWORD(cp+l);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,rt*8);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_3,rt*8+4);
	return(l);
}


WORD dynaOpAddIEqual(BYTE *cp,BYTE op0,DWORD Immediate)
{
	WORD l=0;
	DWORD Upper=0;
	if(Immediate&0x80000000)
	{
		Upper=0xffffffff;
	}
	l+=LOAD_REG_IMM(cp+l,NATIVE_3,Upper);

	l+=INC_PC_COUNTER(cp+l);
	l+=ADD_IMM_TO_RBANK(cp+l,Immediate,op0*8);
	l+=ADC_REG_TO_DWORD(cp+l,NATIVE_3,op0*8+4);
	return(l);
}


WORD dynaOpLoadI(BYTE *cp,BYTE op0,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,Immediate);
	l+=CONVERT_TO_QWORD(cp+l);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,op0*8);
	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_3,op0*8+4);
	return(l);
}


WORD dynaOpDAddIEqual(BYTE *cp,BYTE rt,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,Immediate);
	l+=CONVERT_TO_QWORD(cp+l);
	l+=ADD_REG_TO_DWORD(cp+l,NATIVE_0,rt*8);
	l+=ADC_REG_TO_DWORD(cp+l,NATIVE_3,rt*8+4);
	return(l);
}


WORD dynaOpDAddI(BYTE *cp,BYTE rt,BYTE rs,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);
	DWORD DstOff,SrcOff;
	DstOff=rt*8;
	SrcOff=rs*8;
	l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);		//extra push/pop here - need to inline MEM_TO_MEM_QWORD
	l+=LOAD_REG_IMM(cp+l,NATIVE_0,Immediate);
	l+=CONVERT_TO_QWORD(cp+l);
	l+=ADD_REG_TO_DWORD(cp+l,NATIVE_0,rt*8);
	l+=ADC_REG_TO_DWORD(cp+l,NATIVE_3,rt*8+4);
	return(l);
}


WORD dynaOpDSllV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;
	WORD l=0;

//	l+=PUSH_REGISTER(cp+l,NATIVE_0);
//	l+=LOAD_REG_DWORD(cp+l,NATIVE_0,(BYTE *)&SneekyAddress);
//	l+=CALL_REGISTER(cp+l,NATIVE_0);
//	l+=POP_REGISTER(cp+l,NATIVE_0);

	l+=INC_PC_COUNTER(cp);

	l+=dynaLoadRegForce(cp+l,op2,NATIVE_2);		// must be ecx
	l+=AND_REG_IMM(cp+l,NATIVE_2,0x3f);

	l+=CMP_REG_IMM(cp+l,NATIVE_2,0x1f);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JG_SHORT(cp+l,0);

		l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
		l+=SHLD_REG_REG(cp+l);
		l+=SLL_REG_REG(cp+l,NATIVE_0,NATIVE_2);
		l+=STORE_D_GPR(cp+l);

	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JNE_SHORT(cp+l,0);

	JG_SHORT(fixup1Ptr,(l-fixup1Len)-2);
		l+=ZERO_REG(cp+l,NATIVE_3);
		l+=LOAD_GPR(cp+l,op1*8,op0*8);
		l+=SUB_REG_IMM(cp+l,NATIVE_2,32);
		l+=SLL_REG_REG(cp+l,NATIVE_0,NATIVE_2);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,op0*8+4);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_3,op0*8);


	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);
	return(l);
}

WORD dynaOpDSll(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=SHLD_REG_IMM(cp+l,(BYTE)Immediate);
	l+=SLL_REG_IMM(cp+l,NATIVE_0,Immediate);
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpDSll32(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=NATIVE_REG_TO_REG(cp+l,NATIVE_3,NATIVE_0);	//edx=eax ie; shift left 32
	l+=ZERO_REG(cp+l,NATIVE_0);
	l+=SLL_REG_IMM(cp+l,NATIVE_3,Immediate&0x1f);
	l+=STORE_D_GPR(cp+l);
	return(l);
}


WORD dynaOpDSrlV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;
	WORD l=0;

//	l+=PUSH_REGISTER(cp+l,NATIVE_0);
//	l+=LOAD_REG_DWORD(cp+l,NATIVE_0,(BYTE *)&SneekyAddress);
//	l+=CALL_REGISTER(cp+l,NATIVE_0);
//	l+=POP_REGISTER(cp+l,NATIVE_0);

	l+=INC_PC_COUNTER(cp);

	l+=dynaLoadRegForce(cp+l,op2,NATIVE_2);		// must be ecx
	l+=AND_REG_IMM(cp+l,NATIVE_2,0x3f);

	l+=CMP_REG_IMM(cp+l,NATIVE_2,0x1f);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JG_SHORT(cp+l,0);

		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,op1*8);
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_3,op1*8+4);
		l+=SHRD_REG_REG(cp+l);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,op0*8);
		l+=NATIVE_REG_TO_REG(cp+l,NATIVE_0,NATIVE_3);
		l+=SRL_REG_REG(cp+l,NATIVE_0,NATIVE_2);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,op0*8+4);

	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JNE_SHORT(cp+l,0);

	JG_SHORT(fixup1Ptr,(l-fixup1Len)-2);
		l+=ZERO_REG(cp+l,NATIVE_3);
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,op1*8+4);
		l+=SUB_REG_IMM(cp+l,NATIVE_2,32);
		l+=SRL_REG_REG(cp+l,NATIVE_0,NATIVE_2);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,op0*8);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_3,op0*8+4);


	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);
	return(l);
}

WORD dynaOpDSrl(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

//	l+=PUSH_REGISTER(cp+l,NATIVE_0);
//	l+=LOAD_REG_DWORD(cp+l,NATIVE_0,(BYTE *)&SneekyAddress);
//	l+=CALL_REGISTER(cp+l,NATIVE_0);
//	l+=POP_REGISTER(cp+l,NATIVE_0);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=SHRD_REG_IMM(cp+l,(BYTE)Immediate&0x3f);
	if(Immediate>0x1f)
	{
		l+=SRL_REG_IMM(cp+l,NATIVE_3,0x1f);
		l+=SRL_REG_IMM(cp+l,NATIVE_3,0x1f);
	}
	else
	{
		l+=SRL_REG_IMM(cp+l,NATIVE_3,Immediate&0x1f);
	}
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpDSrl32(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=NATIVE_REG_TO_REG(cp+l,NATIVE_0,NATIVE_3);	//edx=eax ie; shift left 32
	l+=ZERO_REG(cp+l,NATIVE_3);
	l+=SRL_REG_IMM(cp+l,NATIVE_0,Immediate&0x1f);
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpDSraV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	BYTE *fixup1Ptr,*fixup2Ptr;
	WORD fixup1Len,fixup2Len;
	WORD l=0;

//	l+=PUSH_REGISTER(cp+l,NATIVE_0);
//	l+=LOAD_REG_DWORD(cp+l,NATIVE_0,(BYTE *)&SneekyAddress);
//	l+=CALL_REGISTER(cp+l,NATIVE_0);
//	l+=POP_REGISTER(cp+l,NATIVE_0);

	l+=INC_PC_COUNTER(cp);

	l+=dynaLoadRegForce(cp+l,op2,NATIVE_2);		// must be ecx
	l+=AND_REG_IMM(cp+l,NATIVE_2,0x3f);

	l+=CMP_REG_IMM(cp+l,NATIVE_2,0x1f);
	fixup1Ptr=cp+l;
	fixup1Len=l;
	l+=JG_SHORT(cp+l,0);

		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,op1*8);
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_3,op1*8+4);
		l+=SHRD_REG_REG(cp+l);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,op0*8);
		l+=NATIVE_REG_TO_REG(cp+l,NATIVE_0,NATIVE_3);
		l+=SRA_REG_REG(cp+l,NATIVE_0,NATIVE_2);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,op0*8+4);

	fixup2Ptr=cp+l;
	fixup2Len=l;
	l+=JNE_SHORT(cp+l,0);

	JG_SHORT(fixup1Ptr,(l-fixup1Len)-2);
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,op1*8+4);
		l+=SUB_REG_IMM(cp+l,NATIVE_2,32);
		l+=SRA_REG_REG(cp+l,NATIVE_0,NATIVE_2);
		l+=CONVERT_TO_QWORD(cp+l);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,op0*8);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_3,op0*8+4);


	JMP_SHORT(fixup2Ptr,(l-fixup2Len)-2);
	return(l);
}

WORD dynaOpDSra(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=SHRD_REG_IMM(cp+l,(BYTE)Immediate&0x3f);
	l+=SRA_REG_IMM(cp+l,NATIVE_3,Immediate&0x3f);
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpDSra32(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=NATIVE_REG_TO_REG(cp+l,NATIVE_0,NATIVE_3);	//edx=eax ie; shift left 32
	l+=SRA_REG_IMM(cp+l,NATIVE_0,(BYTE)(Immediate&0x1f));
	l+=CONVERT_TO_QWORD(cp+l);
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpRs0Mf(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	if(op1==9)
	{
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,ICOUNT);
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_3,ICOUNT+4);
//		l+=SHLD_REG_IMM(cp+l,(BYTE)1);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,op0*8);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_3,op0*8+4);
	}
	else
	{
		DWORD DstOff,SrcOff;
		DstOff=op0*8;
		SrcOff=CPR0_+op1*8;
		l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
	}
	return(l);
}

WORD dynaOpRs0DMf(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	if(op1==9)
	{
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,ICOUNT);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,op0*8);
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,ICOUNT+4);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,op0*8+4);
	}
	else
	{
		DWORD DstOff,SrcOff;
		DstOff=op0*8;
		SrcOff=CPR0_+op1*8;
		l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);
	}
	return(l);
}


WORD dynaOpRs0Mt(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	if(op0==11) //compare
	{
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,op1*8);
	//	DWORD tmp=0x1234;
	//	l+=LOAD_REG_IMM(cp+l,NATIVE_0,tmp);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,CCOUNT);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,NCOUNT);
		l+=AND_DWORD_WITH_IMM(cp+l,~0x00008000,CPR0_+13*8);
	}
	else if(op0==13) //cause
	{
		DWORD DstOff,SrcOff;
		SrcOff=op1*8;
		DstOff=CPR0_+op0*8;
		l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
		l+=AND_DWORD_WITH_IMM(cp+l,0x00000300,CPR0_+13*8);
	}
	else 
	{
		DWORD DstOff,SrcOff;
		SrcOff=op1*8;
		DstOff=CPR0_+op0*8;
		l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
	}
	return(l);
}

WORD dynaOpRs0DMt(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	if(op0==11) //compare
	{
		l+=LOAD_REG_FROM_RBANK(cp+l,NATIVE_0,op1*8);
		l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,CCOUNT);
		l+=AND_DWORD_WITH_IMM(cp+l,~0x00008000,CPR0_+13*8);
	}
	else if(op0==13) //cause
	{
		DWORD DstOff,SrcOff;
		SrcOff=op1*8;
		DstOff=CPR0_+op0*8;
		l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
		l+=AND_DWORD_WITH_IMM(cp+l,0x00000300,CPR0_+13*8);
	}
	else 
	{
		DWORD DstOff,SrcOff;
		SrcOff=op1*8;
		DstOff=CPR0_+op0*8;
		l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);
	}
	return(l);
}

WORD dynaOpRs1Mf(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	DWORD DstOff,SrcOff;
	DstOff=op0*8;
	SrcOff=_FPR+op1*4;
	l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
	return(l);
}

WORD dynaOpRs1DMf(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	DWORD DstOff,SrcOff;
	DstOff=op0*8;
	SrcOff=_FPR+op1*4;
	l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);
	return(l);
}

WORD dynaOpRs1Mt(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	DWORD DstOff,SrcOff;
	SrcOff=op1*8;
	DstOff=_FPR+op0*4;
	l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
	return(l);
}

WORD dynaOpRs1DMt(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	DWORD DstOff,SrcOff;
	SrcOff=op1*8;
	DstOff=_FPR+op0*4;
	l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);
	return(l);
}

WORD dynaOpRs1Ct(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	DWORD DstOff,SrcOff;
	SrcOff=op1*8;
	DstOff=CPR1_+op0*8;
	l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
	return(l);
}


WORD dynaOpRs0Ct(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	DWORD DstOff,SrcOff;
	SrcOff=op1*8;
	DstOff=CPR0_+op0*8;
	l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
	return(l);
}


WORD dynaOpRs1Cf(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	DWORD DstOff,SrcOff;
	DstOff=op0*8;
	SrcOff=CPR1_+op1*8;
	l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
	return(l);
}


WORD dynaOpRs0Cf(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	DWORD DstOff,SrcOff;
	DstOff=op0*8;
	SrcOff=CPR0_+op1*8;
	l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
	return(l);
}


// Logic AND,OR,XOR,NOR

WORD dynaOpAndEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=AND_REG_WITH_DWORD(cp+l,NATIVE_0,op0*8);
#ifdef LOGIC64
	l+=AND_REG_WITH_DWORD(cp+l,NATIVE_3,op0*8+4);
#endif
	return(l);
}


WORD dynaOpAnd(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=AND_DWORD_WITH_REG(cp+l,NATIVE_0,op2*8);
#ifdef LOGIC64
	l+=AND_DWORD_WITH_REG(cp+l,NATIVE_3,op2*8+4);
#endif
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpOrEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;


	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_0,op0*8);
#ifdef LOGIC64
	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_3,op0*8+4);
#endif
	return(l);
}


WORD dynaOpOr(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=OR_DWORD_WITH_REG(cp+l,NATIVE_0,op2*8);
#ifdef LOGIC64
	l+=OR_DWORD_WITH_REG(cp+l,NATIVE_3,op2*8+4);
#endif
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpNorEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_0,op0*8);
#ifdef LOGIC64
	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_3,op0*8+4);
#endif
	l+=NOT_DWORD(cp+l,op0*8);
#ifdef LOGIC64
	l+=NOT_DWORD(cp+l,op0*8+4);
#endif
	return(l);
}


WORD dynaOpNor(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=OR_DWORD_WITH_REG(cp+l,NATIVE_0,op2*8);
#ifdef LOGIC64
	l+=OR_DWORD_WITH_REG(cp+l,NATIVE_3,op2*8+4);
#endif
	l+=NOT_REG(cp+l,NATIVE_0);
#ifdef LOGIC64
	l+=NOT_REG(cp+l,NATIVE_3);
#endif
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpXorEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=XOR_REG_WITH_DWORD(cp+l,NATIVE_0,op0*8);
#ifdef LOGIC64
	l+=XOR_REG_WITH_DWORD(cp+l,NATIVE_3,op0*8+4);
#endif
	return(l);
}


WORD dynaOpXor(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_D_GPR(cp+l,op1*8,op0*8);
	l+=XOR_DWORD_WITH_REG(cp+l,NATIVE_0,op2*8);
#ifdef LOGIC64
	l+=XOR_DWORD_WITH_REG(cp+l,NATIVE_3,op2*8+4);
#endif
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpAndI(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_GPR(cp+l,op1*8,op0*8);
	l+=AND_REG_IMM(cp+l,NATIVE_0,Immediate&0x0ffff);
#ifdef LOGIC64
	l+=AND_REG_IMM(cp+l,NATIVE_3,0);
#endif
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpOrI(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_GPR(cp+l,op1*8,op0*8);
	l+=OR_REG_IMM(cp+l,NATIVE_0,Immediate&0x0ffff);
	l+=OR_REG_IMM(cp+l,NATIVE_3,0);
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpXorI(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);

	l+=LOAD_GPR(cp+l,op1*8,op0*8);
	l+=XOR_REG_IMM(cp+l,NATIVE_0,Immediate&0x0ffff);
#ifdef LOGIC64
	l+=XOR_REG_IMM(cp+l,NATIVE_3,0);
#endif
	l+=STORE_D_GPR(cp+l);
	return(l);
}

WORD dynaOpAndIEqual(BYTE *cp,BYTE op0,DWORD Immediate)
{
	WORD l=0;
	QWORD  im=(QWORD)(DWORD)Immediate;

	l+=INC_PC_COUNTER(cp+l);

	l+=AND_DWORD_WITH_IMM(cp+l,Immediate&0x0ffff,op0*8);
#ifdef LOGIC64
	l+=AND_DWORD_WITH_IMM(cp+l,0,op0*8+4);
#endif
	//fixme, killnative should not be needed later
	return(l);
}

WORD dynaOpOrIEqual(BYTE *cp,BYTE op0,DWORD Immediate)
{
	WORD l=0;
	QWORD  im=(QWORD)(DWORD)Immediate;

	l+=INC_PC_COUNTER(cp+l);
	l+=OR_IMM_WITH_DWORD(cp+l,Immediate&0x0ffff,op0*8);
#ifdef LOGIC64
	l+=OR_IMM_WITH_DWORD(cp+l,0,op0*8+4);
#endif
	//fixme, killnative should not be needed later
	return(l);
}

WORD dynaOpXorIEqual(BYTE *cp,BYTE op0,DWORD Immediate)
{
	WORD l=0;
	QWORD  im=(QWORD)(DWORD)Immediate;

	l+=INC_PC_COUNTER(cp+l);

	l+=XOR_IMM_WITH_DWORD(cp+l,Immediate&0x0ffff,op0*8);
#ifdef LOGIC64
	l+=XOR_IMM_WITH_DWORD(cp+l,0,op0*8+4);
#endif
	//fixme, killnative should not be needed later
	return(l);
}

