
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "DynaCompiler.h"
#include "dynaNative.h"
extern DWORD dynaTest;

#define EBP_TO_MID

WORD PUSH_STACK_POINTER(BYTE *cp)
{
	*(cp++)=0x54;
	return(1);
}

WORD LOAD_REG_DWORD(BYTE *cp,BYTE Reg,BYTE *Address)
{
	WORD len=0;

	*(cp++)=0xb8+Reg;		// x86 mov DWORD ptr
	len++;
	memcpy(cp,Address,4);	// DWORD to load
	len+=4;
	return(len);
}

WORD CALL_REGISTER(BYTE *cp,BYTE Reg)
{
	WORD len=0;

	*(cp++)=0xff;			// x86 call register
	len++;
	*(cp++)=0xd0+Reg;		// register to call
	len++;
	return(len);
}

WORD PUSH_DWORD(BYTE *cp,DWORD Value)
{
	WORD len=0;

	*(cp++)=0x68;			// x86 push memory
	len++;
	memcpy(cp,&Value,4);	// DWORD to push
	len+=4;

	return(len);
}

WORD POP_REGISTER(BYTE *cp,BYTE Reg)
{
	WORD len=0;

	*(cp++)=0x58+(Reg&0xf);		// register to pop
	len++;
	return(len);
}


WORD PUSH_REGISTER(BYTE *cp,BYTE Reg)
{
	WORD len=0;

	*(cp++)=0x50+Reg;		// register to push
	len++;
	return(len);
}


WORD STORE_REG(BYTE *cp,WORD MipsRegs,BYTE NativeReg)
{
	*(cp++)=0x89;
	*(cp++)=0x80+(NativeReg<<3)+REG_PTR;
	DWORD tmp=MipsRegs*8;
#ifdef EBP_TO_MID
	tmp-=128;
#endif
	memcpy(cp,&tmp,4);
	return(6);
}

WORD LOAD_REG(BYTE *cp,BYTE MipsRegs,WORD NativeReg)
{

	*(cp++)=0x8b;
	*(cp++)=0x80+(NativeReg<<3)+REG_PTR;
	DWORD tmp=MipsRegs*8;
#ifdef EBP_TO_MID
	tmp-=128;
#endif
	memcpy(cp,&tmp,4);
	return(6);
}

WORD SUB_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate)
{
	*(cp++)=0x81;
	*(cp++)=0xe8+NativeReg;
	memcpy(cp,&Immediate,4);
	return(6);
}

WORD ADD_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate)
{
	*(cp++)=0x81;
	*(cp++)=0xc0+NativeReg;
	memcpy(cp,&Immediate,4);
	return(6);
}

WORD ADD_REG_IMM_BYTE(BYTE *cp,BYTE NativeReg,BYTE Immediate)
{
	*(cp++)=0x83;
	*(cp++)=0xc0+NativeReg;
	*(cp++)=Immediate;
	return(3);
}

WORD NOP(BYTE *cp)
{
	*(cp++)=0x90;
	return(1);
}

WORD AND_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate)
{
	*(cp++)=0x81;
	*(cp++)=0xe0+NativeReg;
	memcpy(cp,&Immediate,4);
	return(6);
}

WORD AND_REG_IMM_BYTE(BYTE *cp,BYTE NativeReg,BYTE Immediate)
{
	*(cp++)=0x83;
	*(cp++)=0xe0+NativeReg;
	*(cp++)=Immediate;
	return(3);
}

WORD OR_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate)
{
	*(cp++)=0x81;
	*(cp++)=0xc8+NativeReg;
	memcpy(cp,&Immediate,4);
	return(6);
}

WORD NATIVE_OR_REG_WITH_EAX(BYTE *cp,BYTE r1)
{
	// destination reg is implied to be eax
	//	or eax,r1

	*(cp++)=0x0b;
	*(cp++)=0xc0+r1;
	return(2);
}

WORD XOR_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate)
{
	*(cp++)=0x81;
	*(cp++)=0xf0+NativeReg;
	memcpy(cp,&Immediate,4);
	return(6);
}

WORD NOT_REG(BYTE *cp,BYTE NativeReg)
{
	*(cp++)=0xf7;
	*(cp++)=0xd0+NativeReg;
	return(2);
}


WORD CMP_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate)
{
	*(cp++)=0x81;
	*(cp++)=0xf8+NativeReg;
	memcpy(cp,&Immediate,4);
	return(6);
}

WORD CMP_RBANK_WITH_IMM(BYTE *cp,DWORD Offset,DWORD Immediate)
{
	*(cp++)=0x81;
	*(cp++)=0xbd;
#ifdef EBP_TO_MID
	Offset-=128;
#endif
	memcpy(cp,&Offset,4);
	cp+=4;
	memcpy(cp,&Immediate,4);
	return(10);
}

WORD CMP_RBANK_WITH_IMM_BYTE(BYTE *cp,DWORD Offset,BYTE Immediate)
{
	*(cp++)=0x83;
	*(cp++)=0xbd;
#ifdef EBP_TO_MID
	Offset-=128;
#endif
	memcpy(cp,&Offset,4);
	cp+=4;
	*(cp++)=Immediate;
	return(7);
}

WORD STORE_MEM_REG(BYTE *cp,BYTE DstRegPtr,BYTE Reg)
{
	*(cp++)=0x89;
	*(cp++)=(Reg<<3)|(DstRegPtr);
	return(2);
}

WORD STORE_MEM_WORD_REG(BYTE *cp,BYTE DstRegPtr,BYTE Reg)
{
	*(cp++)=0x66;
	*(cp++)=0x89;
	*(cp++)=(Reg<<3)|(DstRegPtr);
	return(3);
}

WORD STORE_MEM_BYTE_REG(BYTE *cp,BYTE DstRegPtr,BYTE Reg)
{
	*(cp++)=0x88;
	*(cp++)=(Reg<<3)|(DstRegPtr);
	return(2);
}

WORD LOAD_MEM_REG(BYTE *cp,BYTE DstRegPtr,BYTE Reg)
{
	*(cp++)=0x8b;
	*(cp++)=(Reg<<3)|(DstRegPtr);
	return(2);
}

WORD JMP_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0xeb;
	*(cp++)=off;
	return(2);
}

WORD JMP_LONG(BYTE *cp,DWORD Address)
{
	DWORD Current=(DWORD)cp+5;
	DWORD Offset=Address-Current;
	*(cp++)=0xe9;
	memcpy(cp,&Offset,4);
	return(5);
}

WORD NATIVE_REG_TO_REG(BYTE *cp,BYTE r1,BYTE r2)
{
	*(cp++)=0x8b;
	*(cp++)=0xc0+(r1<<3)+(r2);
	return(2);
}


WORD ADD_REG_TO_DWORD(BYTE *cp,BYTE r1,DWORD Address)
{
//	_asm add DWORD ptr ICount,eax
	*(cp++)=0x01;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}


WORD ADD_DWORD_TO_REG(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x03;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}


WORD INC_RBANK(BYTE *cp,DWORD Address)
{
	*(cp++)=0xff;
	*(cp++)=0x85;
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	cp+=4;
	return(6);
}

WORD ADD_IMM_TO_RBANK_BYTE(BYTE *cp,BYTE Immediate,DWORD Address)
{
	*(cp++)=0x83;
	*(cp++)=0x85;
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	cp+=4;
	*(cp++)=Immediate;
	return(7);
}

WORD ADD_IMM_TO_RBANK(BYTE *cp,DWORD Immediate,DWORD Address)
{
	*(cp++)=0x81;
	*(cp++)=0x85;
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	cp+=4;
	memcpy(cp,&Immediate,4);
	return(10);
}


WORD AND_RBANK_WITH_IMM(BYTE *cp,DWORD Immediate,DWORD Address)
{
	*(cp++)=0x81;
	*(cp++)=0xa5;
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	cp+=4;
	memcpy(cp,&Immediate,4);
	return(10);
}


WORD OR_RBANK_WITH_REG(BYTE *cp,BYTE Reg,DWORD Address)
{
	*(cp++)=0x09;
	*(cp++)=0x85+(Reg<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}


WORD ZERO_EXTEND_REG(BYTE *cp,BYTE reg)
{
	*(cp++)=0x0f;
	*(cp++)=0xb7;
	*(cp++)=0xc0+reg;
	return(3);
}


WORD SUB_REG_FROM_DWORD(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x29;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}


WORD SUB_DWORD_FROM_REG(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x2b;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}

WORD AND_REG_WITH_DWORD(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x21;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}

WORD AND_DWORD_WITH_IMM(BYTE *cp,DWORD Immediate,DWORD Address)
{
	*(cp++)=0x81;
	*(cp++)=0xa5;
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	cp+=4;
	memcpy(cp,&Immediate,4);
	return(10);
}

WORD OR_IMM_WITH_DWORD(BYTE *cp,DWORD Immediate,DWORD Address)
{
	*(cp++)=0x81;
	*(cp++)=0x8d;
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	cp+=4;
	memcpy(cp,&Immediate,4);
	return(10);
}

WORD NOT_DWORD(BYTE *cp,DWORD Address)
{
	*(cp++)=0xf7;
	*(cp++)=0x95;
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}


WORD XOR_IMM_WITH_DWORD(BYTE *cp,DWORD Immediate,DWORD Address)
{
	*(cp++)=0x81;
	*(cp++)=0xb5;
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	cp+=4;
	memcpy(cp,&Immediate,4);
	return(10);
}

WORD CMP_DWORD_WITH_REG(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x3b;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}

WORD AND_DWORD_WITH_REG(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x23;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}

WORD XOR_REG_WITH_DWORD(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x31;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}

WORD XOR_DWORD_WITH_REG(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x33;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}


WORD OR_REG_WITH_DWORD(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x09;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}

WORD OR_DWORD_WITH_REG(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x0b;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}


WORD ADC_REG_TO_DWORD(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x11;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}

WORD ADC_DWORD_TO_REG(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x13;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}

WORD SBB_REG_FROM_DWORD(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x19;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}

WORD SBB_DWORD_FROM_REG(BYTE *cp,BYTE r1,DWORD Address)
{
	*(cp++)=0x1b;
	*(cp++)=0x85+(r1<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}

WORD JZ_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0x74;
	*(cp++)=off;
	return(2);
}

WORD JNZ_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0x75;
	*(cp++)=off;
	return(2);
}

WORD JG_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0x7f;
	*(cp++)=off;
	return(2);
}

WORD JL_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0x7c;
	*(cp++)=off;
	return(2);
}

WORD JB_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0x72;
	*(cp++)=off;
	return(2);
}


WORD JLE_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0x7e;
	*(cp++)=off;
	return(2);
}

WORD JGE_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0x7d;
	*(cp++)=off;
	return(2);
}


WORD JE_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0x74;
	*(cp++)=off;
	return(2);
}


WORD JNE_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0x75;
	*(cp++)=off;
	return(2);
}

WORD JAE_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0x73;
	*(cp++)=off;
	return(2);
}


WORD JBE_SHORT(BYTE *cp,BYTE off)
{
	*(cp++)=0x76;
	*(cp++)=off;
	return(2);
}

WORD PUSH_ALL(BYTE *cp)
{
	*(cp++)=0x60;
	return(1);
}

WORD POP_ALL(BYTE *cp)
{
	*(cp++)=0x61;
	return(1);
}

WORD ZERO_REG(BYTE *cp,BYTE NativeReg)
{
	// xor NativeReg,NativeReg
	*(cp++)=0x33;
	*(cp++)=0xc0+(NativeReg<<3)+NativeReg;
	return(2);
}

WORD REG_COMPARE_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate)
{
	// cmp NativeReg,Immediate
	*(cp++)=0x3d;
	memcpy(cp,&Immediate,4);
	return(5);
}

WORD PREDICT_REG_INC(BYTE *cp,BYTE NativeReg)
{
	return(2);
}

WORD JGE_RELATIVE(BYTE *cp,DWORD Offset)
{
	// jge offset
	return(6);
}

WORD REG_INC(BYTE *cp,BYTE NativeReg)
{
	// inc NativeReg
	return(2);
}

WORD SRA_REG_IMM(BYTE *cp,BYTE Reg,DWORD Immediate)
{
	*(cp++)=0xc1;
	*(cp++)=0xf8+Reg;
	*(cp++)=(BYTE)Immediate;
	return(3);
}

WORD SRL_REG_IMM(BYTE *cp,BYTE Reg,DWORD Immediate)
{
	*(cp++)=0xc1;
	*(cp++)=0xe8+Reg;
	*(cp++)=(BYTE)Immediate;
	return(3);
}

WORD SLL_REG_IMM(BYTE *cp,BYTE Reg,DWORD Immediate)
{
	*(cp++)=0xc1;
	*(cp++)=0xe0+Reg;
	*(cp++)=(BYTE)Immediate;
	return(3);
}

WORD SAR_RBANK_IMM(BYTE *cp,DWORD addy,BYTE amount)
{
	*(cp++)=0xc1;
	*(cp++)=0xbd;
#ifdef EBP_TO_MID
	addy-=128;
#endif
	memcpy(cp,&addy,4);
	cp+=4;
	*(cp++)=(BYTE)amount;
	return(7);
}

WORD SHR_RBANK_IMM(BYTE *cp,DWORD addy,BYTE amount)
{
	*(cp++)=0xc1;
	*(cp++)=0xad;
#ifdef EBP_TO_MID
	addy-=128;
#endif
	memcpy(cp,&addy,4);
	cp+=4;
	*(cp++)=(BYTE)amount;
	return(7);
}

WORD SHL_RBANK_IMM(BYTE *cp,DWORD addy,BYTE amount)
{
	*(cp++)=0xc1;
	*(cp++)=0xa5;
#ifdef EBP_TO_MID
	addy-=128;
#endif
	memcpy(cp,&addy,4);
	cp+=4;
	*(cp++)=(BYTE)amount;
	return(7);
}

WORD SHLD_REG_IMM(BYTE *cp,BYTE Immediate)
{
	*(cp++)=0x0f;
	*(cp++)=0xa4;
	*(cp++)=0xc2;
	*(cp++)=Immediate;
	return(4);
}

WORD SHLD_REG_REG(BYTE *cp)
{
	*(cp++)=0x0f;
	*(cp++)=0xa5;
	*(cp++)=0xc2;
	return(3);
}

WORD SHLD_REG_REG2(BYTE *cp)
{
	*(cp++)=0x0f;
	*(cp++)=0xa5;
	*(cp++)=0xd0;
	return(3);
}


WORD SHRD_REG_IMM(BYTE *cp,BYTE Immediate)
{
	*(cp++)=0x0f;
	*(cp++)=0xac;
	*(cp++)=0xd0;
	*(cp++)=Immediate;
	return(4);
}

WORD SHRD_REG_REG(BYTE *cp)
{
	*(cp++)=0x0f;
	*(cp++)=0xad;
	*(cp++)=0xd0;
	return(3);
}

WORD SHRD_REG_REG2(BYTE *cp)
{
	*(cp++)=0x0f;
	*(cp++)=0xad;
	*(cp++)=0xc2;
	return(3);
}


WORD SRA_REG_REG(BYTE *cp,BYTE Reg,BYTE ShiftReg)
{
	*(cp++)=0xd3;
	*(cp++)=0xf8;
	return(2);
}


WORD SUB_NATIVE_REG_REG(BYTE *cp,BYTE r1,BYTE r2)
{
	*(cp++)=0x2b;
	*(cp++)=0xc0+(r1<<3)+r2;
	return(2);
}

WORD ADD_NATIVE_REG_REG(BYTE *cp,BYTE r1,BYTE r2)
{
	//limited to add esi,eax or edi,eax
	*(cp++)=0x3;
	if(r1==PC_PTR)
		*(cp++)=0xf8;
	else
		*(cp++)=0xf0;
	return(2);
}

WORD SRL_REG_REG(BYTE *cp,BYTE Reg,DWORD Immediate)
{
	*(cp++)=0xd3;
	*(cp++)=0xe8;
	return(2);
}

WORD SLL_REG_REG(BYTE *cp,BYTE Reg,DWORD Immediate)
{
	*(cp++)=0xd3;
	*(cp++)=0xe0;
	return(2);
}

WORD REG_TO_REG(BYTE *cp,BYTE MipsDst,BYTE MipsSrc)
{
	return(0);
}

static BYTE dynaDstOff;

WORD LOAD_D_GPR(BYTE *cp,DWORD Offset1,DWORD Offset2)
{
#ifdef EBP_TO_MID
	Offset1-=128;
	Offset2-=128;
#endif
/*
	_asm
	{
		mov ebx,0x1234
		mov eax,dword ptr [ebp+ebx]
		mov edx,dword ptr [ebp+ebx+4]
		add ebx,0xff
		sub ebx,0x1234
		
		mov [ebp+ebx],eax
		mov [ebp+ebx+4],edx
	}
	*(cp++)=0xbb;
	memcpy(cp,&Offset1,4);
	cp+=4;
	*(cp++)=0x8b;
	*(cp++)=0x44;
	*(cp++)=0x1d;
	*(cp++)=0x00;

	*(cp++)=0x8b;
	*(cp++)=0x54;
	*(cp++)=0x1d;
	*(cp++)=0x04;
	if(Offset2>Offset1)
	{
		DWORD diff=Offset2-Offset1;
		*(cp++)=0x81;
		*(cp++)=0xc3;
		memcpy(cp,&diff,4);
		cp+=4;
		return(19);
	}
	else if(Offset2!=Offset1)
	{
		DWORD diff=Offset1-Offset2;
		*(cp++)=0x81;
		*(cp++)=0xeb;
		memcpy(cp,&diff,4);
		cp+=4;
		return(19);
	}
	return(13);
*/
//	_asm mov eax,dword ptr [ebp+0x00]
//	_asm mov edx,dword ptr [ebp+0x04]
	dynaDstOff=Offset2;
	*(cp++)=0x8b;
	*(cp++)=0x45;
	*(cp++)=Offset1;
	*(cp++)=0x8b;
	*(cp++)=0x55;
	*(cp++)=Offset1+4;
	return(6);
}

WORD STORE_D_GPR(BYTE *cp)
{
/*
	*(cp++)=0x89;
	*(cp++)=0x44;
	*(cp++)=0x1d;
	*(cp++)=0x00;
	*(cp++)=0x89;
	*(cp++)=0x54;
	*(cp++)=0x1d;
	*(cp++)=0x04;
	return(8);
*/
//	_asm mov dword ptr [ebp+0x40],eax
//	_asm mov dword ptr [ebp+0x44],edx

	*(cp++)=0x89;
	*(cp++)=0x45;
	*(cp++)=dynaDstOff;
	*(cp++)=0x89;
	*(cp++)=0x55;
	*(cp++)=dynaDstOff+4;
	return(6);
}


WORD LOAD_GPR(BYTE *cp,DWORD Offset1,DWORD Offset2)
{
#ifdef EBP_TO_MID
	Offset1-=128;
	Offset2-=128;
#endif
/*
	_asm
	{
		mov ebx,0x1234
		mov eax,dword ptr [ebp+ebx]
		mov edx,dword ptr [ebp+ebx+4]
		add ebx,0xff
		sub ebx,0x1234
		
		mov [ebp+ebx],eax
		mov [ebp+ebx+4],edx
	}
	*(cp++)=0xbb;
	memcpy(cp,&Offset1,4);
	cp+=4;
	*(cp++)=0x8b;
	*(cp++)=0x44;
	*(cp++)=0x1d;
	*(cp++)=0x00;
	if(Offset2>Offset1)
	{
		DWORD diff=Offset2-Offset1;
		*(cp++)=0x81;
		*(cp++)=0xc3;
		memcpy(cp,&diff,4);
		cp+=4;
		return(15);
	}
	else if(Offset2!=Offset1)
	{
		DWORD diff=Offset1-Offset2;
		*(cp++)=0x81;
		*(cp++)=0xeb;
		memcpy(cp,&diff,4);
		cp+=4;
		return(15);
	}
	return(9);
*/
//		mov eax,dword ptr [ebp+0xfe]
	*(cp++)=0x8b;
	*(cp++)=0x45|(0<<3);
	*(cp++)=Offset1;
	dynaDstOff=Offset2;
	return(3);
}

WORD STORE_GPR(BYTE *cp)
{
	*(cp++)=0x89;
	*(cp++)=0x45;
	*(cp++)=dynaDstOff;
	return(3);
}

WORD STORE_SE_D_GPR(BYTE *cp)
{
/*
	_asm
	{
		mov [ebp+ebx],eax
		cdq
		mov [ebp+ebx+4],edx

	}
	*(cp++)=0x89;
	*(cp++)=0x44;
	*(cp++)=0x1d;
	*(cp++)=0x00;
	*(cp++)=0x99;
	*(cp++)=0x89;
	*(cp++)=0x54;
	*(cp++)=0x1d;
	*(cp++)=0x04;
	return(9);
*/

/*
//		mov [ebp+ebx],eax
//		sar eax,31
//		mov [ebp+ebx+4],eax
	
	*(cp++)=0x89;
	*(cp++)=0x44;
	*(cp++)=0x1d;
	*(cp++)=0x00;
	*(cp++)=0xc1;
	*(cp++)=0xf8;
	*(cp++)=0x1f;
	*(cp++)=0x89;
	*(cp++)=0x54;
	*(cp++)=0x1d;
	*(cp++)=0x04;
	return(11);
*/

//	mov [ebp+0],eax
//	sar eax,31
//	mov [ebp+4],eax

	*(cp++)=0x89;
	*(cp++)=0x45;
	*(cp++)=dynaDstOff;
	*(cp++)=0xc1;
	*(cp++)=0xf8;
	*(cp++)=0x1f;
	*(cp++)=0x89;
	*(cp++)=0x45;
	*(cp++)=dynaDstOff+4;
	return(9);
}

WORD LOAD_REG_FROM_RBANK(BYTE *cp,BYTE NativeReg,DWORD Offset)
{
	if(Offset==0) 
	{
		return(ZERO_REG(cp,NativeReg));
	}
#ifdef EBP_TO_MID
	Offset-=128;
#endif
	*(cp++)=0x8b;
	*(cp++)=0x80+(NativeReg<<3)+REG_PTR;
	memcpy(cp,&Offset,4);	// mov eax,[ebp+SrcOff]
	return(6);
}

WORD STORE_REG_TO_RBANK(BYTE *cp,BYTE NativeReg,DWORD Offset)
{
	if(Offset==0) return(0);
	if(Offset==4) return(0);

#ifdef EBP_TO_MID
	Offset-=128;
#endif
	*(cp++)=0x89;
	*(cp++)=0x80+(NativeReg<<3)+REG_PTR;
	memcpy(cp,&Offset,4);	// mov eax,[ebp+SrcOff]
	return(6);
}

WORD STORE_DWORD_TO_RBANK(BYTE *cp,DWORD Value,DWORD Offset)
{
	if(Offset==0) return(0);
	if(Offset==4) return(0);

#ifdef EBP_TO_MID
	Offset-=128;
#endif
	*(cp++)=0xc7;
	*(cp++)=0x80+REG_PTR;
	memcpy(cp,&Offset,4);	// mov eax,[ebp+SrcOff]
	cp+=4;
	memcpy(cp,&Value,4);	// mov eax,[ebp+SrcOff]
	return(10);
}

WORD IMUL(BYTE *cp,BYTE op0,BYTE op1)		// op0 is ignored
{
	DWORD tmp=op1*8;
	*(cp++)=0xf7;
	*(cp++)=0xad;
#ifdef EBP_TO_MID
	tmp-=128;
#endif
	memcpy(cp,&tmp,4);		// imul DWORD ptr [ebp+Op1*8]
	return(6);
}

WORD MUL(BYTE *cp,BYTE op0,BYTE op1)		// op0 is ignored
{
	DWORD tmp=op1*8;
	*(cp++)=0xf7;
	*(cp++)=0xa5;
#ifdef EBP_TO_MID
	tmp-=128;
#endif
	memcpy(cp,&tmp,4);		// mul DWORD ptr [ebp+Op1*8]
	return(6);
}

WORD IDIV(BYTE *cp,BYTE op0,BYTE op1)		// op0 is ignored
{
	DWORD tmp=op1*8;
	*(cp++)=0xf7;
	*(cp++)=0xbd;
#ifdef EBP_TO_MID
	tmp-=128;
#endif
	memcpy(cp,&tmp,4);		// imul DWORD ptr [ebp+Op1*8]
	return(6);
}

WORD DIV(BYTE *cp,BYTE op0,BYTE op1)		// op0 is ignored
{
	DWORD tmp=op1*8;
	*(cp++)=0xf7;
	*(cp++)=0xb5;
#ifdef EBP_TO_MID
	tmp-=128;
#endif
	memcpy(cp,&tmp,4);		// mul DWORD ptr [ebp+Op1*8]
	return(6);
}

WORD CONVERT_TO_QWORD(BYTE *cp)
{
	*(cp++)=0x99;
	return(1);
}

WORD CONVERT_TO_DWORD(BYTE *cp)
{
	*(cp++)=0x98;
	return(1);
}

WORD CONVERT_TO_WORD(BYTE *cp)
{
	*(cp++)=0x66;
	*(cp++)=0x98;
	return(2);
}

WORD REG_TO_MEM_DWORD(BYTE *cp,DWORD DstOff,BYTE Reg)
{
	*(cp++)=0x89;
	*(cp++)=0x80+(Reg<<3)+REG_PTR;
#ifdef EBP_TO_MID
	DstOff-=128;
#endif
	memcpy(cp,&DstOff,4);
	return(6);
}

WORD CLD(BYTE *cp)
{
	*(cp++)=0xfc;
	return(1);
}

WORD STD(BYTE *cp)
{
	*(cp++)=0xfd;
	return(1);
}

WORD REP_MOVE_STORE_BYTE(BYTE *cp)
{
	*(cp++)=0xf3;
	*(cp++)=0xa4;
	return(2);
}

WORD MOVSD(BYTE *cp)
{
	*(cp++)=0xa5;
	return(1);
}

WORD LODSD(BYTE *cp)
{
	*(cp++)=0xad;
	return(1);
}

WORD STOSD(BYTE *cp)
{
	*(cp++)=0xab;
	return(1);
}

WORD LOAD_REG_IMM(BYTE *cp,BYTE Reg,DWORD Immediate)
{
	*(cp++)=0xb8+Reg;
	memcpy(cp,&Immediate,4);
	return(5);
}


WORD LOAD_REG_FROM_MEMORY(BYTE *cp,BYTE Reg,DWORD Address)
{
	*(cp++)=0x8b;
	*(cp++)=0x05+(Reg<<3);
#ifdef EBP_TO_MID
	Address-=128;
#endif
	memcpy(cp,&Address,4);
	return(6);
}

WORD CALL_CHECKINTS(BYTE *cp)
{
	int Current=(int)cp+5;
	int Offset=dynaCheckIntsAddress-Current;
	*(cp++)=0xe8;
	memcpy(cp,&Offset,4);
	return(5);
}

WORD JMP_REGISTER(BYTE *cp,BYTE reg)
{
	*(cp++)=0xff;
	*(cp++)=0xe0;
	return(2);
}

WORD JNE_LONG(BYTE *cp,int Offset)
{
	*(cp++)=0x0f;
	*(cp++)=0x85;
	memcpy(cp,&Offset,4);
	return(6);
}

WORD JE_LONG(BYTE *cp,int Offset)
{
	*(cp++)=0x0f;
	*(cp++)=0x84;
	memcpy(cp,&Offset,4);
	return(6);
}


WORD JG_LONG(BYTE *cp,int Offset)
{
	*(cp++)=0x0f;
	*(cp++)=0x8f;
	memcpy(cp,&Offset,4);
	return(6);
}

WORD JGE_LONG(BYTE *cp,int Offset)
{
	*(cp++)=0x0f;
	*(cp++)=0x8D;
	memcpy(cp,&Offset,4);
	return(6);
}


WORD JAE_LONG(BYTE *cp,int Offset)
{
	*(cp++)=0x0f;
	*(cp++)=0x83;
	memcpy(cp,&Offset,4);
	return(6);
}

WORD JL_LONG(BYTE *cp,int Offset)
{
	*(cp++)=0x0f;
	*(cp++)=0x8c;
	memcpy(cp,&Offset,4);
	return(6);
}

WORD JB_LONG(BYTE *cp,int Offset)
{
	*(cp++)=0x0f;
	*(cp++)=0x82;
	memcpy(cp,&Offset,4);
	return(6);
}

WORD JLE_LONG(BYTE *cp,int Offset)
{
	*(cp++)=0x0f;
	*(cp++)=0x8e;
	memcpy(cp,&Offset,4);
	return(6);
}

WORD JZ_LONG(BYTE *cp,int Offset)		// same as JE
{
	*(cp++)=0x0f;
	*(cp++)=0x85;
	memcpy(cp,&Offset,4);
	return(6);
}

WORD JNZ_LONG(BYTE *cp,int Offset)		// same as JNE
{
	*(cp++)=0x0f;
	*(cp++)=0x84;
	memcpy(cp,&Offset,4);
	return(6);
}

WORD J_LONG(BYTE *cp,int Offset)
{
	*(cp++)=0x0f;
	*(cp++)=0x84;
	memcpy(cp,&Offset,4);
	return(6);
}

WORD RETURN(BYTE *cp)
{
	WORD l=0;
//	l=STORE_DWORD_TO_RBANK(cp,dynaPC+4,PC_);
//	cp+=l;
	*(cp++)=0xc3;
	l++;
	return(l);
}

WORD INC_PC_COUNTER(BYTE *cp)
{
	WORD l=0;

//	l+=INC_RBANK(cp+l,ICOUNT);

//	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+4,242*8);
//	l+=ADD_IMM_TO_RBANK(cp+l,8,ICOUNT);
/*
	if((dynaInBranch)||(iOpCode==dynaNextOpCode))
		dynaTest+=2;
	else
	{
		dynaTest+=4;
		if(dynaTest!=dynaPC+4)
			int stop=1;
	}
//	dynaICount++;
*/

	return(l);
}

WORD INC_PC_COUNTER_S(BYTE *cp)
{
	WORD l=0;

//	l+=INC_RBANK(cp+l,ICOUNT);

//	l+=STORE_DWORD_TO_RBANK(cp+l,dynaPC+4,242*8);
//	l+=ADD_IMM_TO_RBANK(cp+l,4,PC_);
//	dynaICount++;

	return(l);
}

WORD CALL_FUNCTION(BYTE *cp,DWORD address)
{
	int Current=(int)cp+5;
	int Offset=address-Current;
	*(cp++)=0xe8;
	memcpy(cp,&Offset,4);
	return(5);
}

WORD MEM_TO_MEM_QWORD(BYTE *cp,DWORD DstOff,DWORD SrcOff)
{
#ifdef EBP_TO_MID
	DstOff-=128;
	SrcOff-=128;
#endif
	WORD l=0;
	l+=NATIVE_REG_TO_REG((BYTE *)(cp+l),PC_PTR,REG_PTR);
	l+=NATIVE_REG_TO_REG((BYTE *)(cp+l),MEM_PTR,REG_PTR);
	l+=ADD_REG_IMM((BYTE *)(cp+l),MEM_PTR,(DWORD) SrcOff);
	l+=ADD_REG_IMM((BYTE *)(cp+l),PC_PTR,(DWORD) DstOff);
	l+=MOVSD(cp+l);
	l+=MOVSD(cp+l);
	return(l);


/*
	*(cp++)=0x8b;
	*(cp++)=0x80+REG_PTR;
	memcpy(cp,&SrcOff,4);	// mov eax,[ebp+SrcOff]
	cp+=4;

	*(cp++)=0x89;
	*(cp++)=0x80+REG_PTR;
	memcpy(cp,&DstOff,4);	// move [ebp+DstOff],eax
	cp+=4;
	SrcOff+=4;
	DstOff+=4;

	*(cp++)=0x8b;
	*(cp++)=0x80+REG_PTR;
	memcpy(cp,&SrcOff,4);	// mov eax,[ebp+SrcOff+4]
	cp+=4;

	*(cp++)=0x89;
	*(cp++)=0x80+REG_PTR;
	memcpy(cp,&DstOff,4);	// move [ebp+DstOff+4],eax
	cp+=4;
*/
	

	return(24);
}

WORD MEM_TO_MEM_DWORD(BYTE *cp,DWORD DstOff,DWORD SrcOff)
{
	WORD l=0;
#ifdef EBP_TO_MID
	DstOff-=128;
	SrcOff-=128;
#endif
	l+=NATIVE_REG_TO_REG((BYTE *)(cp+l),PC_PTR,REG_PTR);
	l+=NATIVE_REG_TO_REG((BYTE *)(cp+l),MEM_PTR,REG_PTR);
	l+=ADD_REG_IMM((BYTE *)(cp+l),MEM_PTR,(DWORD) SrcOff);
	l+=ADD_REG_IMM((BYTE *)(cp+l),PC_PTR,(DWORD) DstOff);
	l+=MOVSD(cp+l);
	return(l);


/*
	*(cp++)=0x8b;
	*(cp++)=0x80+REG_PTR;
	memcpy(cp,&SrcOff,4);	// mov eax,[ebp+SrcOff]
	cp+=4;

	*(cp++)=0x89;
	*(cp++)=0x80+REG_PTR;
	memcpy(cp,&DstOff,4);	// move [ebp+DstOff],eax
	cp+=4;
	
*/

	return(12);
}


/************************* new section ***********************/

WORD MOVSX_NATIVE_BPTR_NATIVE(BYTE *cp,BYTE n1,BYTE n2)
{
	//movsx n1,byte ptr [n2]
	*(cp++)=0x0f;
	*(cp++)=0xbe;
	*(cp++)=n2|(n1<<3);
	return(3);
}

WORD MOVZX_NATIVE_BPTR_NATIVE(BYTE *cp,BYTE n1,BYTE n2)
{
	//movzx n1,byte ptr [n2]
	*(cp++)=0x0f;
	*(cp++)=0xb6;
	*(cp++)=n2|(n1<<3);
	return(3);
}

WORD MOVSX_NATIVE_WPTR_NATIVE(BYTE *cp,BYTE n1,BYTE n2)
{
	//movsx n1,word ptr [n2]
	*(cp++)=0x0f;
	*(cp++)=0xbf;
	*(cp++)=n2|(n1<<3);
	return(3);
}

WORD MOVZX_NATIVE_WPTR_NATIVE(BYTE *cp,BYTE n1,BYTE n2)
{
	//movzx n1,word ptr [n2]
	*(cp++)=0x0f;
	*(cp++)=0xb7;
	*(cp++)=n2|(n1<<3);
	return(3);
}

WORD MOVSX_NATIVE_BPTR_NATIVE_OFFSET(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset)
{
	//movsx n1,byte ptr [n2+0x1234]
	*(cp++)=0x0f;
	*(cp++)=0xbe;
	*(cp++)=n2|(n1<<3)|0x80;
	memcpy(cp,&Offset,4);
	return(7);
}

WORD MOVZX_NATIVE_BPTR_NATIVE_OFFSET(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset)
{
	//movzx n1,byte ptr [n2]
	*(cp++)=0x0f;
	*(cp++)=0xb6;
	*(cp++)=n2|(n1<<3)|0x80;
	memcpy(cp,&Offset,4);
	return(7);
}

WORD MOVSX_NATIVE_WPTR_NATIVE_OFFSET(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset)
{
	//movsx n1,word ptr [n2]
	*(cp++)=0x0f;
	*(cp++)=0xbf;
	*(cp++)=n2|(n1<<3)|0x80;
	memcpy(cp,&Offset,4);
	return(7);
}

WORD MOVZX_NATIVE_WPTR_NATIVE_OFFSET(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset)
{
	//movzx n1,word ptr [n2]
	*(cp++)=0x0f;
	*(cp++)=0xb7;
	*(cp++)=n2|(n1<<3)|0x80;
	memcpy(cp,&Offset,4);
	return(7);
}

WORD MOV_NATIVE_DPTR_NATIVE(BYTE *cp,BYTE n1,BYTE n2)
{
	//mov n1,dword ptr [n2]
	*(cp++)=0x8b;
	*(cp++)=n2|(n1<<3);
	return(2);
}

WORD MOV_NATIVE_GPR(BYTE *cp,BYTE n1,char Offset)
{
	//mov n1,[ebp+Offset]
	Offset-=(char)128; //ebp point to GPR[15], ie; the middle of the GP registers, this allows use to use signed 8 bit offset

	*(cp++)=0x8b;
	*(cp++)=0x45|(n1<<3);
	*(cp++)=Offset;
	return(3);
}

WORD MOV_GPR_NATIVE(BYTE *cp,char Offset,BYTE n1)
{
	//mov [ebp+Offset],n1
	Offset-=(char)128; //ebp point to GPR[15], ie; the middle of the GP registers, this allows use to use signed 8 bit offset

	*(cp++)=0x89;
	*(cp++)=0x45|(n1<<3);
	*(cp++)=Offset;
	return(3);
}

WORD MOV_NATIVE_NATIVE(BYTE *cp,BYTE n1,BYTE n2)
{
	//mov n1,n2
	*(cp++)=0x8b;
	*(cp++)=0xc0|(n1<<3)|n2;
	return(2);
}

WORD ADD_NATIVE_IMM(BYTE *cp,BYTE n1,DWORD Immediate)
{
	if(Immediate<128)
	{
		*(cp++)=0x83;
		*(cp++)=0xc0|n1;
		*(cp++)=(BYTE)Immediate;
		return(3);
	}
	if(n1==NATIVE_0)
	{
		*(cp++)=0x05;
		memcpy(cp,&Immediate,4);
		return(5);
	}
	*(cp++)=0x81;
	*(cp++)=0xc0+n1;
	memcpy(cp,&Immediate,4);
	return(6);
}

WORD ADD_NATIVE_DPTR_EAX4(BYTE *cp,BYTE n1,DWORD Offset)
{
	//add n1,dword ptr [offset+eax*4]					// yes, eax*4 is the hardcoded index

	*(cp++)=0x03;
	*(cp++)=0x04|(n1<<3);
	*(cp++)=0x85;
	memcpy(cp,&Offset,4);
	return(7);
}

WORD SHR_NATIVE_IMM(BYTE *cp,BYTE n1,BYTE Imm)
{
	//shr n1,Imm

	*(cp++)=0xc1;
	*(cp++)=0xe8|n1;
	*(cp++)=Imm;
	return(3);
}

WORD XOR_NATIVE_NATIVE(BYTE *cp,BYTE n1,BYTE n2)
{
	// xor n1,n2

	*(cp++)=0x33;
	*(cp++)=0xc0+(n1<<3)+n2;
	return(2);
}

WORD MOV_BPTR_NATIVE_NATIVE(BYTE *cp,BYTE n1,BYTE n2)
{
	//mov byte ptr [n1],n2

	*(cp++)=0x88;
	*(cp++)=(n2<<3)|n1;
	return(2);
}

WORD MOV_WPTR_NATIVE_NATIVE(BYTE *cp,BYTE n1,BYTE n2)
{
	//mov word ptr [n1],n2

	*(cp++)=0x66;
	*(cp++)=0x89;
	*(cp++)=(n2<<3)|n1;
	return(3);
}

WORD MOV_DPTR_NATIVE_NATIVE(BYTE *cp,BYTE n1,BYTE n2)
{
	//mov dword ptr [n1],n2

	*(cp++)=0x89;
	*(cp++)=(n2<<3)|n1;
	return(2);
}

WORD MOV_DPTR_NATIVE_OFFSET_NATIVE(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset)
{
	//mov dword ptr [n1+offset],n2

	*(cp++)=0x89;
	*(cp++)=(n2<<3)|n1|0x80;
	memcpy(cp,&Offset,4);
	return(6);
}


WORD MOV_WPTR_NATIVE_OFFSET_NATIVE(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset)
{
	//mov dword ptr [n1+offset],n2

	*(cp++)=0x66;
	*(cp++)=0x89;
	*(cp++)=(n2<<3)|n1|0x80;
	memcpy(cp,&Offset,4);
	return(7);
}


WORD MOV_BPTR_NATIVE_OFFSET_NATIVE(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset)
{
	//mov dword ptr [n1+offset],n2

	*(cp++)=0x88;
	*(cp++)=(n2<<3)|n1|0x80;
	memcpy(cp,&Offset,4);
	return(6);
}


WORD MOV_NATIVE_FPR(BYTE *cp,BYTE n1,DWORD Offset)
{
	//mov n1,[ebp+Offset]
	Offset+=(1792-128); //ebp points to the middle of GPRs, thus index from there to the start of FPR

	*(cp++)=0x8b;
	*(cp++)=0x80+(n1<<3)+REG_PTR;
	memcpy(cp,&Offset,4);
	return(6);
}

WORD MOV_FPR_NATIVE(BYTE *cp,DWORD Offset,BYTE n1)
{
	//mov [ebp+Offset],n1
	Offset+=(1792-128); //ebp points to the middle of GPRs, thus index from there to the start of FPR

	*(cp++)=0x89;
	*(cp++)=0x80+(n1<<3)+REG_PTR;
	memcpy(cp,&Offset,4);	
	return(6);
}

WORD SAR_NATIVE_IMM(BYTE *cp,BYTE n1,BYTE Imm)
{
//	sar eax,Imm

	*(cp++)=0xc1;
	*(cp++)=0xf8;
	*(cp++)=Imm;
	return(3);
}

WORD MOV_NATIVE_DPTR_NATIVE_OFFSET(BYTE *cp,BYTE n0,BYTE n1,DWORD Imm)
{
//	mov eax,dword ptr [edx+0x1234]

	*(cp++)=0x8b;
	*(cp++)=n1|(n0<<3)|0x80;
	memcpy(cp,&Imm,4);
	return(6);
}
