
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "DynaCompiler.h"
#include "dynaNative.h"
#include "dynaMemory.h"
extern DWORD smart;

#define OFFSET 0
#define EBP_OFFSET 128
#define EXTEND_64

DWORD dynaReadMap[256];
DWORD dynaWriteMap[256];
extern DWORD smart2;

void dynaBuildReadMap()
{
	for(DWORD i=0;i<256;i++)
	{
		dynaReadMap[i]=(DWORD)m->rdRam-(i<<24);
	}
	dynaReadMap[0x80]=((DWORD)m->rdRam+0x800000)-(0x80000000);
	dynaReadMap[0x0]=((DWORD)m->rdRam+0x90000);
	dynaReadMap[0xb0]=((DWORD)m->aiReg)-(0xb0000000);
	dynaReadMap[0xa8]=((DWORD)m->aiReg)-(0xa8000000);
}

void dynaBuildWriteMap()
{
	for(DWORD i=0;i<256;i++)
	{
		dynaWriteMap[i]=(DWORD)m->rdRam-(i<<24);
	}
	dynaWriteMap[0x80]=((DWORD)m->rdRam+0x800000)-(0x80000000);
	dynaWriteMap[0x0]=((DWORD)m->rdRam+0x90000);
	dynaWriteMap[0xb0]=((DWORD)m->atReg)-(0xb0000000);
	dynaWriteMap[0xa8]=((DWORD)m->atReg)-(0xa8000000);
}

WORD dynaOpSmartLb(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaReadMap);
	l+=MOVSX_NATIVE_BPTR_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=SAR_NATIVE_IMM(cp+l,NATIVE_0,31);
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_0);
#endif
	return(l);
}

WORD dynaOpSmartLbU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaReadMap);
	l+=MOVZX_NATIVE_BPTR_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=XOR_NATIVE_NATIVE(cp+l,NATIVE_3,NATIVE_3);
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_3);
#endif
	return(l);
}

WORD dynaOpSmartLh(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaReadMap);
	l+=MOVSX_NATIVE_WPTR_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=SAR_NATIVE_IMM(cp+l,NATIVE_0,31);
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_0);
#endif
	return(l);
}

WORD dynaOpSmartLhU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaReadMap);
	l+=MOVZX_NATIVE_WPTR_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=XOR_NATIVE_NATIVE(cp+l,NATIVE_3,NATIVE_3);
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_3);
#endif
	return(l);
}

WORD dynaOpSmartLw(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaReadMap);
	l+=MOV_NATIVE_DPTR_NATIVE_OFFSET(cp+l,NATIVE_0,NATIVE_1,Imm);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=SAR_NATIVE_IMM(cp+l,NATIVE_0,31);
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_0);
#endif

	return(l);
}


WORD dynaOpSmartLd(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaReadMap);
	l+=MOV_NATIVE_DPTR_NATIVE_OFFSET(cp+l,NATIVE_0,NATIVE_1,Imm);
	l+=MOV_NATIVE_DPTR_NATIVE_OFFSET(cp+l,NATIVE_3,NATIVE_1,Imm+4);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_3);

	return(l);
}


WORD dynaOpSmartLwU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaReadMap);
	l+=MOV_NATIVE_DPTR_NATIVE_OFFSET(cp+l,NATIVE_0,NATIVE_1,Imm);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=XOR_NATIVE_NATIVE(cp+l,NATIVE_3,NATIVE_3);
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_3);
#endif
	return(l);
}

WORD dynaOpSmartLb2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	if(Page==0xb0)
		return(dynaOpLb(cp,op0,op1,Imm));

	WORD l=0;
	DWORD Offset=dynaReadMap[Page]+Imm;
	if((Page==0)&&(op1==29))
		Offset=(DWORD)m->rdRam+Imm;

smart2++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOVSX_NATIVE_BPTR_NATIVE_OFFSET(cp+l,NATIVE_0,NATIVE_1,Offset);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=SAR_NATIVE_IMM(cp+l,NATIVE_0,31);
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_0);
#endif

	return(l);
}

WORD dynaOpSmartLbU2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	if(Page==0xb0)
		return(dynaOpLbU(cp,op0,op1,Imm));

	WORD l=0;
	DWORD Offset=dynaReadMap[Page]+Imm;
	if((Page==0)&&(op1==29))
		Offset=(DWORD)m->rdRam+Imm;

smart2++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOVZX_NATIVE_BPTR_NATIVE_OFFSET(cp+l,NATIVE_0,NATIVE_1,Offset);
	l+=XOR_NATIVE_NATIVE(cp+l,NATIVE_3,NATIVE_3);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_3);
#endif

	return(l);
}

WORD dynaOpSmartLh2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	if(Page==0xb0)
		return(dynaOpLh(cp,op0,op1,Imm));

	WORD l=0;
	DWORD Offset=dynaReadMap[Page]+Imm;
	if((Page==0)&&(op1==29))
		Offset=(DWORD)m->rdRam+Imm;

smart2++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOVSX_NATIVE_WPTR_NATIVE_OFFSET(cp+l,NATIVE_0,NATIVE_1,Offset);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=SAR_NATIVE_IMM(cp+l,NATIVE_0,31);
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_0);
#endif

	return(l);
}

WORD dynaOpSmartLhU2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	if(Page==0xb0)
		return(dynaOpLhU(cp,op0,op1,Imm));

	WORD l=0;
	DWORD Offset=dynaReadMap[Page]+Imm;
	if((Page==0)&&(op1==29))
		Offset=(DWORD)m->rdRam+Imm;

smart2++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOVZX_NATIVE_WPTR_NATIVE_OFFSET(cp+l,NATIVE_0,NATIVE_1,Offset);
	l+=XOR_NATIVE_NATIVE(cp+l,NATIVE_3,NATIVE_3);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_3);
#endif

	return(l);
}

WORD dynaOpSmartLwU2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	if(Page==0xb0)
		return(dynaOpLwU(cp,op0,op1,Imm));

	WORD l=0;
	DWORD Offset=dynaReadMap[Page]+Imm;
	if((Page==0)&&(op1==29))
		Offset=(DWORD)m->rdRam+Imm;

smart2++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_DPTR_NATIVE_OFFSET(cp+l,NATIVE_0,NATIVE_1,Offset);
	l+=XOR_NATIVE_NATIVE(cp+l,NATIVE_3,NATIVE_3);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_3);
#endif

	return(l);
}

WORD dynaOpSmartLw2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	if(Page==0xb0)
		return(dynaOpLw(cp,op0,op1,Imm));

	WORD l=0;
	DWORD Offset=dynaReadMap[Page]+Imm;
	if((Page==0)&&(op1==29))
		Offset=(DWORD)m->rdRam+Imm;

smart2++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_DPTR_NATIVE_OFFSET(cp+l,NATIVE_0,NATIVE_1,Offset);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);

#ifdef EXTEND_64
	l+=SAR_NATIVE_IMM(cp+l,NATIVE_0,31);
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_0);
#endif

	return(l);
}

WORD dynaOpSmartLd2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	if(Page==0xb0)
		return(dynaOpLd(cp,op0,op1,Imm));

	WORD l=0;
	DWORD Offset=dynaReadMap[Page]+Imm;
	if((Page==0)&&(op1==29))
		Offset=(DWORD)m->rdRam+Imm;

smart2++;
	if(!op0) return(0);

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_DPTR_NATIVE_OFFSET(cp+l,NATIVE_0,NATIVE_1,Offset);
	l+=MOV_NATIVE_DPTR_NATIVE_OFFSET(cp+l,NATIVE_3,NATIVE_1,Offset+4);
	l+=MOV_GPR_NATIVE(cp+l,op0*8,NATIVE_0);
	l+=MOV_GPR_NATIVE(cp+l,op0*8+4,NATIVE_3);
	return(l);
}


WORD dynaOpSmartLwc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaReadMap);
	l+=MOV_NATIVE_DPTR_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=MOV_FPR_NATIVE(cp+l,op0*4,NATIVE_0);
	return(l);
}

WORD dynaOpSmartLdc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaReadMap);
	l+=MOV_NATIVE_DPTR_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=MOV_FPR_NATIVE(cp+l,op0*4,NATIVE_0);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,4);
	l+=MOV_NATIVE_DPTR_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=MOV_FPR_NATIVE(cp+l,op0*4+4,NATIVE_0);
	return(l);
}

WORD dynaOpSmartSb(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=MOV_NATIVE_GPR(cp+l,NATIVE_2,op0*8);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaWriteMap);
	l+=MOV_BPTR_NATIVE_NATIVE(cp+l,NATIVE_1,NATIVE_2);
	return(l);
}

WORD dynaOpSmartSh(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	 l+=MOV_NATIVE_GPR(cp+l,NATIVE_2,op0*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	 l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaWriteMap);
	l+=MOV_WPTR_NATIVE_NATIVE(cp+l,NATIVE_1,NATIVE_2);
	return(l);
}

WORD dynaOpSmartSw(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	 l+=MOV_NATIVE_GPR(cp+l,NATIVE_2,op0*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	 l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaWriteMap);
	l+=MOV_DPTR_NATIVE_NATIVE(cp+l,NATIVE_1,NATIVE_2);

	return(l);
}

WORD dynaOpSmartSb2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	if(Page==0xb0)
		return(dynaOpSb(cp,op0,op1,Imm));

	WORD l=0;
	DWORD Offset=dynaReadMap[Page]+Imm;
	if((Page==0)&&(op1==29))
		Offset=(DWORD)m->rdRam+Imm;

smart2++;

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	 l+=MOV_NATIVE_GPR(cp+l,NATIVE_2,op0*8);
	l+=MOV_BPTR_NATIVE_OFFSET_NATIVE(cp+l,NATIVE_1,NATIVE_2,Offset);

	return(l);
}

WORD dynaOpSmartSh2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	if(Page==0xb0)
		return(dynaOpSh(cp,op0,op1,Imm));

	WORD l=0;
	DWORD Offset=dynaReadMap[Page]+Imm;
	if((Page==0)&&(op1==29))
		Offset=(DWORD)m->rdRam+Imm;

smart2++;

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	 l+=MOV_NATIVE_GPR(cp+l,NATIVE_2,op0*8);
	l+=MOV_WPTR_NATIVE_OFFSET_NATIVE(cp+l,NATIVE_1,NATIVE_2,Offset);

	return(l);
}

WORD dynaOpSmartSw2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	if(Page==0xb0)
		return(dynaOpSw(cp,op0,op1,Imm));

	WORD l=0;
	DWORD Offset=dynaReadMap[Page]+Imm;
	if((Page==0)&&(op1==29))
		Offset=(DWORD)m->rdRam+Imm;

smart2++;

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	 l+=MOV_NATIVE_GPR(cp+l,NATIVE_2,op0*8);
	l+=MOV_DPTR_NATIVE_OFFSET_NATIVE(cp+l,NATIVE_1,NATIVE_2,Offset);

	return(l);
}

WORD dynaOpSmartSd2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	if(Page==0xb0)
		return(dynaOpSd(cp,op0,op1,Imm));

	WORD l=0;
	DWORD Offset=dynaReadMap[Page]+Imm;
	if((Page==0)&&(op1==29))
		Offset=(DWORD)m->rdRam+Imm;

smart2++;

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	 l+=MOV_NATIVE_GPR(cp+l,NATIVE_2,op0*8);
	l+=MOV_NATIVE_GPR(cp+l,NATIVE_3,op0*8+4);
	 l+=MOV_DPTR_NATIVE_OFFSET_NATIVE(cp+l,NATIVE_1,NATIVE_2,Offset);
	l+=MOV_DPTR_NATIVE_OFFSET_NATIVE(cp+l,NATIVE_1,NATIVE_3,Offset+4);

	return(l);
}

WORD dynaOpSmartSd(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;
	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	 l+=MOV_NATIVE_GPR(cp+l,NATIVE_2,op0*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	 l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=MOV_NATIVE_GPR(cp+l,NATIVE_3,op0*8+4);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaWriteMap);
	l+=MOV_DPTR_NATIVE_NATIVE(cp+l,NATIVE_1,NATIVE_2);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,4);
	l+=MOV_DPTR_NATIVE_NATIVE(cp+l,NATIVE_1,NATIVE_3);
	return(l);
}

WORD dynaOpSmartSwc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;

	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=MOV_NATIVE_FPR(cp+l,NATIVE_2,op0*4);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaWriteMap);
	l+=MOV_DPTR_NATIVE_NATIVE(cp+l,NATIVE_1,NATIVE_2);
	return(l);
}

WORD dynaOpSmartSdc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm,BYTE Page)
{
	WORD l=0;

smart++;
	l+=MOV_NATIVE_GPR(cp+l,NATIVE_1,op1*8);
	l+=MOV_NATIVE_NATIVE(cp+l,NATIVE_0,NATIVE_1);
	l+=MOV_NATIVE_FPR(cp+l,NATIVE_2,op0*4);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,Imm);
	l+=SHR_NATIVE_IMM(cp+l,NATIVE_0,24);
	l+=MOV_NATIVE_FPR(cp+l,NATIVE_3,op0*4+4);
	l+=ADD_NATIVE_DPTR_EAX4(cp+l,NATIVE_1,(DWORD)dynaWriteMap);
	l+=MOV_DPTR_NATIVE_NATIVE(cp+l,NATIVE_1,NATIVE_2);
	l+=ADD_NATIVE_IMM(cp+l,NATIVE_1,4);
	l+=MOV_DPTR_NATIVE_NATIVE(cp+l,NATIVE_1,NATIVE_3);
	return(l);
}


