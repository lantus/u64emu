			
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "DynaCompiler.h"
#include "dynaNative.h"
#include "dynaFP.h"
#include "iMain.h"

//#define FPR_OFFSET 1792
#define FPR_OFFSET 1792-128

WORD FP_COMP_S(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xd8;		
	*(cp++)=0x9d;		
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_COMP_D(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xdc;		
	*(cp++)=0x9d;		
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_COMPU_D(BYTE *cp,BYTE r1,BYTE r2)
{
	*(cp++)=0xdc;		
	*(cp++)=0x9d;		
	DWORD tmp=r1*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	cp+=4;

	*(cp++)=0xdc;		
	*(cp++)=0x9d;		
	tmp=r2*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	cp+=4;

	*(cp++)=0xda;		
	*(cp++)=0xe9;		
	return(14);
}

WORD FP_COMPU_S(BYTE *cp,BYTE r1,BYTE r2)
{
	*(cp++)=0xd8;		
	*(cp++)=0x9d;		
	DWORD tmp=r1*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	cp+=4;

	*(cp++)=0xd8;		
	*(cp++)=0x9d;		
	tmp=r2*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	cp+=4;

	*(cp++)=0xda;		
	*(cp++)=0xe9;		
	return(14);
}

WORD FP_STORE_STATUS(BYTE *cp)
{
	*(cp++)=0x9b;		
	*(cp++)=0xdf;		
	*(cp++)=0xe0;		
	return(3);
}

WORD FP_SET_ROUNDCONTROL(BYTE *cp,DWORD Mode)
{
	*(cp++)=0x9b;		// wait
	*(cp++)=0xd9;		// fldcw
    *(cp++)=0xad;

	Mode-=128;

	memcpy(cp,&Mode,4);
	cp+=4;
	*(cp++)=0x9b;		// wait

	return(8);
}

WORD FP_ROUND(BYTE *cp)
{
//	return(0);
	*(cp++)=0xd9;
    *(cp++)=0xfc;
	return(2);
}

WORD FP_LOAD_S_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xd9;
    *(cp++)=0x85;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_LOAD_W_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xdb;
    *(cp++)=0x85;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_LOAD_L_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xdf;
    *(cp++)=0xad;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}


WORD FP_LOAD_D_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xdd;
    *(cp++)=0x85;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_STORE_S_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xd9;
    *(cp++)=0x9d;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_STORE_D_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xdd;
    *(cp++)=0x9d;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_STORE_L_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xdf;
    *(cp++)=0xbd;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_STORE_W_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xdb;
    *(cp++)=0x9d;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_ADD_S_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xd8;
    *(cp++)=0x85;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_ADD_D_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xdc;
    *(cp++)=0x85;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_SUB_S_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xd8;
    *(cp++)=0xa5;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_SUB_D_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xdc;
    *(cp++)=0xa5;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}


WORD FP_MUL_S_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xd8;
    *(cp++)=0x8d;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_MUL_D_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xdc;
    *(cp++)=0x8d;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}


WORD FP_DIV_S_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xd8;
    *(cp++)=0xb5;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_DIV_D_REG(BYTE *cp,BYTE Reg)
{
	*(cp++)=0xdc;
    *(cp++)=0xb5;
	DWORD tmp=Reg*4+FPR_OFFSET;
	memcpy(cp,&tmp,4);
	return(6);
}

WORD FP_ADD(BYTE *cp)
{
	*(cp++)=0xde;
	*(cp++)=0xc1;
	return(2);
}

WORD FP_COS(BYTE *cp)
{
	*(cp++)=0xd9;
	*(cp++)=0xff;
	return(2);
}

WORD FP_SIN(BYTE *cp)
{
	*(cp++)=0xd9;
	*(cp++)=0xfe;
	return(2);
}

WORD FP_SUB(BYTE *cp)
{
	*(cp++)=0xde;
	*(cp++)=0xe9;
	return(2);
}


WORD FP_MUL(BYTE *cp)
{
	*(cp++)=0xde;
	*(cp++)=0xc9;
	return(2);
}


WORD FP_DIV(BYTE *cp)
{
	*(cp++)=0xde;
	*(cp++)=0xf9;
	return(2);
}


WORD FP_SQRT(BYTE *cp)
{
	*(cp++)=0xd9;
	*(cp++)=0xfa;
	return(2);
}


WORD FP_ABS(BYTE *cp)
{
	*(cp++)=0xd9;
	*(cp++)=0xe1;
	return(2);
}


WORD FP_NEG(BYTE *cp)
{
	*(cp++)=0xd9;
	*(cp++)=0xe0;
	return(2);
}


WORD dynaOpFPAddS(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_ADD_S_REG(cp+l,op2);
	l+=FP_STORE_S_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPAddD(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_ADD_D_REG(cp+l,op2);
	l+=FP_STORE_D_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPSubS(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_SUB_S_REG(cp+l,op2);
	l+=FP_STORE_S_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPSubD(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_SUB_D_REG(cp+l,op2);
	l+=FP_STORE_D_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPMulS(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_MUL_S_REG(cp+l,op2);
	l+=FP_STORE_S_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPMulD(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_MUL_D_REG(cp+l,op2);
	l+=FP_STORE_D_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPDivS(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_DIV_S_REG(cp+l,op2);
	l+=FP_STORE_S_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPDivD(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_DIV_D_REG(cp+l,op2);
	l+=FP_STORE_D_REG(cp+l,op0);
	return(l);
}


WORD dynaOpFPSqrtS(BYTE *cp,BYTE op0,BYTE op1)
{
	// DstReg=(float)op1+(float)op2;
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_SQRT(cp+l);
	l+=FP_STORE_S_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPSqrtD(BYTE *cp,BYTE op0,BYTE op1)
{
	// DstReg=(float)op1+(float)op2;
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_SQRT(cp+l);
	l+=FP_STORE_D_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPAbsS(BYTE *cp,BYTE op0,BYTE op1)
{
	// DstReg=(float)op1+(float)op2;
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_ABS(cp+l);
	l+=FP_STORE_S_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPAbsD(BYTE *cp,BYTE op0,BYTE op1)
{
	// DstReg=(float)op1+(float)op2;
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_ABS(cp+l);
	l+=FP_STORE_D_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPNegS(BYTE *cp,BYTE op0,BYTE op1)
{
	// DstReg=(float)op1+(float)op2;
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_NEG(cp+l);
	l+=FP_STORE_S_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPNegD(BYTE *cp,BYTE op0,BYTE op1)
{
	// DstReg=(float)op1+(float)op2;
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_NEG(cp+l);
	l+=FP_STORE_D_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPCvtSFromS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_STORE_S_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPCvtSFromD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_STORE_S_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPCvtSFromW(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_W_REG(cp+l,op1);
	l+=FP_STORE_S_REG(cp+l,op0);
	return(l);
}


WORD dynaOpFPCvtSFromL(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_L_REG(cp+l,op1);
	l+=FP_STORE_S_REG(cp+l,op0);
	return(l);
}


WORD dynaOpFPCvtDFromS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_STORE_D_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPCvtDFromW(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_W_REG(cp+l,op1);
	l+=FP_STORE_D_REG(cp+l,op0);
	return(l);
}


WORD dynaOpFPCvtDFromL(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_L_REG(cp+l,op1);
	l+=FP_STORE_D_REG(cp+l,op0);
	return(l);
}



WORD dynaOpFPCvtWFromS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_STORE_W_REG(cp+l,op0);	//change 2
	return(l);
}

WORD dynaOpFPCvtWFromD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_STORE_W_REG(cp+l,op0);	//change 2
	return(l);
}


WORD dynaOpFPCvtWFromL(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_L_REG(cp+l,op1);
	l+=FP_STORE_W_REG(cp+l,op0);
	return(l);
}


WORD dynaOpFPCvtLFromS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_STORE_L_REG(cp+l,op0);
	return(l);
}

WORD dynaOpFPCvtLFromD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_STORE_L_REG(cp+l,op0);
	return(l);
}


WORD dynaOpFPCvtLFromW(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_LOAD_W_REG(cp+l,op1);
	l+=FP_STORE_L_REG(cp+l,op0);
	return(l);
}


WORD dynaOpFPRoundLS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_SET_ROUNDCONTROL(cp+l,NEAREST);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_ROUND(cp+l);
	l+=FP_STORE_L_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}


WORD dynaOpFPRoundLD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_SET_ROUNDCONTROL(cp+l,NEAREST);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_ROUND(cp+l);
	l+=FP_STORE_L_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}


WORD dynaOpFPRoundWS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_SET_ROUNDCONTROL(cp+l,NEAREST);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_ROUND(cp+l);
	l+=FP_STORE_W_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}


WORD dynaOpFPRoundWD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	l+=FP_SET_ROUNDCONTROL(cp+l,NEAREST);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_ROUND(cp+l);
	l+=FP_STORE_W_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}


WORD dynaOpFPTruncWS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,TRUNC);
	l+=FP_STORE_W_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPTruncWD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,TRUNC);
	l+=FP_STORE_W_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPTruncLS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,TRUNC);
	l+=FP_STORE_L_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPTruncLD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,TRUNC);
	l+=FP_STORE_L_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPCeilLS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,CEIL);
	l+=FP_STORE_L_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPCeilLD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,CEIL);
	l+=FP_STORE_L_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPCeilWS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,CEIL);
	l+=FP_STORE_W_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPCeilWD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,CEIL);
	l+=FP_STORE_W_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPFloorWD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,FLOOR);
	l+=FP_STORE_W_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPFloorWS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,FLOOR);
	l+=FP_STORE_W_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPFloorLD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,FLOOR);
	l+=FP_STORE_L_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPFloorLS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);
	//l+=STORE_REG(cp+l,242,PC_PTR);
	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_SET_ROUNDCONTROL(cp+l,FLOOR);
	l+=FP_STORE_L_REG(cp+l,op0);
	l+=FP_SET_ROUNDCONTROL(cp+l,CUR_ROUND_MODE);
	return(l);
}

WORD dynaOpFPMovS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);

	DWORD DstOff,SrcOff;
	DstOff=op0*4+FPR_OFFSET+128;
	SrcOff=op1*4+FPR_OFFSET+128;
	l+=MEM_TO_MEM_DWORD(cp+l,DstOff,SrcOff);
	return(l);
}

WORD dynaOpFPMovD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);

	DWORD DstOff,SrcOff;
	DstOff=op0*4+FPR_OFFSET+128;
	SrcOff=op1*4+FPR_OFFSET+128;
	l+=MEM_TO_MEM_QWORD(cp+l,DstOff,SrcOff);
	return(l);
}

WORD dynaOpFPEqualS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);


//	l+=PUSH_REGISTER(cp+l,NATIVE_0);
	l+=FP_LOAD_S_REG(cp+l,op0);
	l+=FP_COMP_S(cp+l,op1);
	l+=FP_STORE_STATUS(cp+l);
//	l+=ZERO_EXTEND_REG(cp+l,NATIVE_0);
	l+=SLL_REG_IMM(cp+l,NATIVE_0,9);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	DWORD offset=CCR1_+8*31;
	l+=AND_DWORD_WITH_IMM(cp+l,~0x00800000,offset);
	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_0,offset);
//	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,offset);
//	l+=POP_REGISTER(cp+l,NATIVE_0);

	return(l);
}

WORD dynaOpFPLessThanS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);

	l+=FP_LOAD_S_REG(cp+l,op0);
	l+=FP_COMP_S(cp+l,op1);
	l+=FP_STORE_STATUS(cp+l);
//	l+=ZERO_EXTEND_REG(cp+l,NATIVE_0);
	l+=SLL_REG_IMM(cp+l,NATIVE_0,15);
	DWORD offset=CCR1_+8*31;
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	l+=AND_DWORD_WITH_IMM(cp+l,~0x00800000,offset);
	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_0,offset);
//	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,offset);

	return(l);
}

WORD dynaOpFPLessThanEqualS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);

	l+=FP_LOAD_S_REG(cp+l,op1);
	l+=FP_COMP_S(cp+l,op0);
	l+=FP_STORE_STATUS(cp+l);
	l+=SLL_REG_IMM(cp+l,NATIVE_0,15);
	DWORD offset=CCR1_+8*31;
	l+=NOT_REG(cp+l,NATIVE_0);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	l+=AND_DWORD_WITH_IMM(cp+l,~0x00800000,offset);
	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_0,offset);
//	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,offset);

	return(l);
}

WORD dynaOpFPEqualD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);

	l+=FP_LOAD_D_REG(cp+l,op0);
	l+=FP_COMP_D(cp+l,op1);
	l+=FP_STORE_STATUS(cp+l);
//	l+=ZERO_EXTEND_REG(cp+l,NATIVE_0);
	l+=SLL_REG_IMM(cp+l,NATIVE_0,9);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	DWORD offset=CCR1_+8*31;
	l+=AND_DWORD_WITH_IMM(cp+l,~0x00800000,offset);
	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_0,offset);
//	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,offset);

	return(l);
}

WORD dynaOpFPLessThanD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);

	l+=FP_LOAD_D_REG(cp+l,op0);
	l+=FP_COMP_D(cp+l,op1);
	l+=FP_STORE_STATUS(cp+l);
//	l+=ZERO_EXTEND_REG(cp+l,NATIVE_0);
	l+=SLL_REG_IMM(cp+l,NATIVE_0,15);
	DWORD offset=CCR1_+8*31;
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	l+=AND_DWORD_WITH_IMM(cp+l,~0x00800000,offset);
	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_0,offset);
//	l+=STORE_REG_TO_RBANK(cp+l,NATIVE_0,offset);

	return(l);
}

WORD dynaOpFPLessThanEqualD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);

	l+=FP_LOAD_D_REG(cp+l,op1);
	l+=FP_COMP_D(cp+l,op0);
	l+=FP_STORE_STATUS(cp+l);
	l+=SLL_REG_IMM(cp+l,NATIVE_0,15);
	DWORD offset=CCR1_+8*31;
	l+=NOT_REG(cp+l,NATIVE_0);
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	l+=AND_DWORD_WITH_IMM(cp+l,~0x00800000,offset);
	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_0,offset);

	return(l);
}

WORD dynaOpFPUnorderedD(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);

	l+=FP_COMPU_D(cp+l,op0,op1);
	l+=FP_STORE_STATUS(cp+l);
	l+=SLL_REG_IMM(cp+l,NATIVE_0,13);
	DWORD offset=CCR1_+8*31;
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	l+=AND_DWORD_WITH_IMM(cp+l,~0x00800000,offset);
//	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_0,offset);

	return(l);
}

WORD dynaOpFPUnorderedS(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);

	l+=FP_COMPU_S(cp+l,op0,op1);
	l+=FP_STORE_STATUS(cp+l);
	l+=SLL_REG_IMM(cp+l,NATIVE_0,13);
	DWORD offset=CCR1_+8*31;
	l+=AND_REG_IMM(cp+l,NATIVE_0,0x00800000);
	l+=AND_DWORD_WITH_IMM(cp+l,~0x00800000,offset);
//	l+=OR_REG_WITH_DWORD(cp+l,NATIVE_0,offset);

	return(l);
}


WORD dynaOpFPSetFalse(BYTE *cp)
{
	WORD l=0;
	l+=INC_PC_COUNTER(cp);

	DWORD offset=CCR1_+8*31;
	l+=AND_RBANK_WITH_IMM(cp+l,~0x00800000,offset);

	return(l);
}