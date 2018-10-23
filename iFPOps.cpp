#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "iMain.h"
#include "iCPU.h"			// Core 4600 emulation routines
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines


#define COMPILE_AND_EXECno
#ifdef COMPILE_AND_EXEC


	typedef void (*funcptr)(void);
	funcptr dynaStart;
	BYTE cp[512];

	void CompileAndExec(BYTE *Addrs,WORD Len)
	{
		RETURN(Addrs+Len);
		dynaStart=(funcptr)Addrs;
		DWORD dynaRegPtr=(DWORD)r;
		_asm
		{
			pushad
			mov	ebp,dword ptr dynaRegPtr
			mov edi,dword ptr [ebp+1936]
			call dword ptr [dynaStart]
			popad
		}

	}
#endif


#define SINGLE_STORE *(float  *)&r->FPR[MAKE_FD]
#define DOUBLE_STORE *(double  *)&r->FPR[MAKE_FD]

void iOpFAdd()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPAddS(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	else
		CompileAndExec(cp,dynaOpFPAddD(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		SINGLE_STORE=(float) *(float *)&r->FPR[MAKE_FS]+*(float *)&r->FPR[MAKE_FT];
	else
		DOUBLE_STORE=(double) *(double *)&r->FPR[MAKE_FS]+*(double *)&r->FPR[MAKE_FT];
}

void iOpFSub()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPSubS(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	else
		CompileAndExec(cp,dynaOpFPSubD(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		SINGLE_STORE=(float) *(float *)&r->FPR[MAKE_FS]-*(float *)&r->FPR[MAKE_FT];
	else
		DOUBLE_STORE=(double) *(double *)&r->FPR[MAKE_FS]-*(double *)&r->FPR[MAKE_FT];
}

void iOpFMul()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPMulS(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	else
		CompileAndExec(cp,dynaOpFPMulD(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		SINGLE_STORE=(float) *(float *)&r->FPR[MAKE_FS]**(float *)&r->FPR[MAKE_FT];
	else
		DOUBLE_STORE=(double) *(double *)&r->FPR[MAKE_FS]**(double *)&r->FPR[MAKE_FT];
}

void iOpFDiv()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
	{
		if(*(float *)&r->FPR[MAKE_FT]!=0.f)
			CompileAndExec(cp,dynaOpFPDivS(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	}
	else
	{
		if(*(double *)&r->FPR[MAKE_FT]!=0.)
			CompileAndExec(cp,dynaOpFPDivD(cp,MAKE_FD,MAKE_FS,MAKE_FT));
	}
	return;
#endif

	if(MAKE_FMT==FMT_S)
	{
		if(*(float *)&r->FPR[MAKE_FT]!=0.f)
			SINGLE_STORE=(float) *(float *)&r->FPR[MAKE_FS]/ *(float *)&r->FPR[MAKE_FT];
	}
	else
	{
		if(*(double *)&r->FPR[MAKE_FT]!=0.)
			DOUBLE_STORE=(double) *(double *)&r->FPR[MAKE_FS]/ *(double *)&r->FPR[MAKE_FT];
	}
}

void iOpFSqrt()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPSqrtS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPSqrtD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		SINGLE_STORE=(float)sqrt(*(float *)&r->FPR[MAKE_FS]);
	else
		DOUBLE_STORE=(double) sqrt(*(double *)&r->FPR[MAKE_FS]);
}

void iOpFAbs()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPAbsS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPAbsD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		SINGLE_STORE=(float)fabs(*(float *)&r->FPR[MAKE_FS]);
	else
		DOUBLE_STORE=(double) fabs(*(double *)&r->FPR[MAKE_FS]);
}

void iOpFNeg()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPNegS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPNegD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		SINGLE_STORE=(float) -*(float *)&r->FPR[MAKE_FS];
	else
		DOUBLE_STORE=(double) -*(double *)&r->FPR[MAKE_FS];
}

void iOpFMov()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPMovS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPMovD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		r->FPR[MAKE_FD]=r->FPR[MAKE_FS];
	else
	{
		r->FPR[MAKE_FD]=r->FPR[MAKE_FS];
		r->FPR[MAKE_FD+1]=r->FPR[MAKE_FS+1];
	}
}

void iOpFCeilw()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPCeilWS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPCeilWD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		r->FPR[MAKE_FD]=(sDWORD) ceil(*(float *)&r->FPR[MAKE_FS]);
	else
		r->FPR[MAKE_FD]=(sDWORD) ceil(*(double *)&r->FPR[MAKE_FS]);
}

void iOpFFloorw()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPFloorWS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPFloorWD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		r->FPR[MAKE_FD]=(sDWORD) floor(*(float *)&r->FPR[MAKE_FS]);
	else
		r->FPR[MAKE_FD]=(sDWORD) floor(*(double *)&r->FPR[MAKE_FS]);
}

void iOpFRoundl()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPRoundLS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPRoundLD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		*(sQWORD *)&r->FPR[MAKE_FD]=(sQWORD) *(float *)&r->FPR[MAKE_FS];
	else
		*(sQWORD *)&r->FPR[MAKE_FD]=(sQWORD) *(double *)&r->FPR[MAKE_FS];
}

void iOpFCeill()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPCeilLS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPCeilLD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		r->FPR[MAKE_FD]=(sQWORD) ceil(*(float *)&r->FPR[MAKE_FS]);
	else
		r->FPR[MAKE_FD]=(sQWORD) ceil(*(double *)&r->FPR[MAKE_FS]);
}

void iOpFFloorl()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPFloorLS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPFloorLD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		*(sQWORD *)&r->FPR[MAKE_FD]=(sQWORD) floor(*(float *)&r->FPR[MAKE_FS]);
	else
		*(sQWORD *)&r->FPR[MAKE_FD]=(sQWORD) floor(*(double *)&r->FPR[MAKE_FS]);
}

void iOpFRoundw()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPRoundWS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPRoundWD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
		r->FPR[MAKE_FD]=(sDWORD) *(float *)&r->FPR[MAKE_FS];
	else
		r->FPR[MAKE_FD]=(sDWORD) *(double *)&r->FPR[MAKE_FS];
}

void iOpFTruncl()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPTruncLS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPTruncLD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
	{
		float src1;
		double dst;
		WORD trunc=0xe7f;
		WORD round=0x27f;
		src1=*(float *)&r->FPR[MAKE_FS];
		
/* TODO: FIX		
		_asm
		{
			fld dword ptr src1
			wait
			fldcw trunc
			wait
			fistp qword ptr dst
			wait
			fldcw round
			wait
		}
*/		
		*(double *)&r->FPR[MAKE_FD]=dst;
	}
	else
	{
		double dst;
		double src1;
		WORD trunc=0xe7f;
		WORD round=0x27f;
		src1=*(double *)&r->FPR[MAKE_FS];
/* TODO: FIX			
		_asm
		{
			fld qword ptr src1
			wait
			fldcw trunc
			wait
			fistp qword ptr dst
			wait
			fldcw round
			wait
		}
/* END		*/
		*(double *)&r->FPR[MAKE_FD]=dst;
	}
}

void iOpFTruncw()
{
#ifdef COMPILE_AND_EXECa
	if(MAKE_FMT==FMT_S)
		CompileAndExec(cp,dynaOpFPTruncWS(cp,MAKE_FD,MAKE_FS));
	else
		CompileAndExec(cp,dynaOpFPTruncWD(cp,MAKE_FD,MAKE_FS));
	return;
#endif

	if(MAKE_FMT==FMT_S)
	{
		float dst,src1;
		WORD trunc=0xe7f;
		WORD round=0x27f;
		src1=*(float *)&r->FPR[MAKE_FS];
		
/* TODO: FIX			
		_asm
		{
			fld dword ptr src1
			wait
			fldcw trunc
			wait
			fistp dword ptr dst
			wait
			fldcw round
			wait
		}
	*/
		*(float *)&r->FPR[MAKE_FD]=dst;
	}
	else
	{
		float dst;
		double src1;
		WORD trunc=0xe7f;
		WORD round=0x27f;
		src1=*(double *)&r->FPR[MAKE_FS];
/* TODO: FIX			
		_asm
		{
			fld qword ptr src1
			wait
			fldcw trunc
			wait
			fistp dword ptr dst
			wait
			fldcw round
			wait
		}
	*/
		*(float *)&r->FPR[MAKE_FD]=dst;
	}
}


void iOpFCvts()
{
#ifdef COMPILE_AND_EXECa
	switch(MAKE_FMT)
	{
	case FMT_S:
//		CompileAndExec(cp,dynaOpFPMovS(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_D:
		CompileAndExec(cp,dynaOpFPCvtSFromD(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_W:
		CompileAndExec(cp,dynaOpFPCvtSFromW(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_L:
		CompileAndExec(cp,dynaOpFPCvtSFromL(cp,MAKE_FD,MAKE_FS));
		break;
	}
	return;
#endif

	switch(MAKE_FMT)
	{
	case FMT_S:
		{
			SINGLE_STORE=(float) *(float *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_D:
		{
			SINGLE_STORE=(float)*(double *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_W:
		{
			SINGLE_STORE=(float)*(sDWORD *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_L:
		{
			SINGLE_STORE=(float)*(sQWORD *)&r->FPR[MAKE_FS];
			break;
		}
	}
}

void iOpFCvtd()
{
#ifdef COMPILE_AND_EXECa
	switch(MAKE_FMT)
	{
	case FMT_S:
		CompileAndExec(cp,dynaOpFPCvtDFromS(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_D:
//		CompileAndExec(cp,dynaOpFPMovD(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_W:
		CompileAndExec(cp,dynaOpFPCvtDFromW(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_L:
		CompileAndExec(cp,dynaOpFPCvtDFromL(cp,MAKE_FD,MAKE_FS));
		break;
	}
	return;
#endif

	switch(MAKE_FMT)
	{
	case FMT_S:
		{
			DOUBLE_STORE=(double)*(float *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_D:
		{
			DOUBLE_STORE=(double)*(double *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_W:
		{
			DOUBLE_STORE=(double)*(sDWORD *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_L:
		{
			DOUBLE_STORE=(double)*(sQWORD *)&r->FPR[MAKE_FS];
			break;
		}
	}
}

void iOpFCvtw()
{
#ifdef COMPILE_AND_EXECa
	switch(MAKE_FMT)
	{
	case FMT_S:
		CompileAndExec(cp,dynaOpFPCvtWFromS(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_D:
		CompileAndExec(cp,dynaOpFPCvtWFromD(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_W:
//		CompileAndExec(cp,dynaOpFPMovS(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_L:
		CompileAndExec(cp,dynaOpFPCvtWFromL(cp,MAKE_FD,MAKE_FS));
		break;
	}
	return;
#endif

	switch(MAKE_FMT)
	{
	case FMT_S:
		{
			r->FPR[MAKE_FD]=(sDWORD)*(float *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_D:
		{
			r->FPR[MAKE_FD]=(sDWORD)*(double *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_W:
		{
			r->FPR[MAKE_FD]=*(sDWORD *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_L:
		{
			r->FPR[MAKE_FD]=*(sQWORD *)&r->FPR[MAKE_FS];
			break;
		}
	}
}

void iOpFCvtl()
{
#ifdef COMPILE_AND_EXECa
	switch(MAKE_FMT)
	{
	case FMT_S:
		CompileAndExec(cp,dynaOpFPCvtLFromS(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_D:
		CompileAndExec(cp,dynaOpFPCvtLFromD(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_W:
		CompileAndExec(cp,dynaOpFPCvtLFromW(cp,MAKE_FD,MAKE_FS));
		break;
	case FMT_L:
//		CompileAndExec(cp,dynaOpFPMovD(cp,MAKE_FD,MAKE_FS));
		break;
	}
	return;
#endif

	switch(MAKE_FMT)
	{
	case FMT_S:
		{
			*(sQWORD *)&r->FPR[MAKE_FD]=(sQWORD)*(float *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_D:
		{
			*(sQWORD *)&r->FPR[MAKE_FD]=(sQWORD)*(double *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_W:
		{
			*(sQWORD *)&r->FPR[MAKE_FD]=(sQWORD)*(sDWORD *)&r->FPR[MAKE_FS];
			break;
		}
	case FMT_L:
		{
			*(sQWORD *)&r->FPR[MAKE_FD]=(sQWORD)*(sQWORD *)&r->FPR[MAKE_FS];
			break;
		}
	}
}

void iOpFc()
{
#ifdef COMPILE_AND_EXECa
	switch(MAKE_FMT)
	{
	case FMT_S:   /* c.*.s */
		{
		switch(MAKE_F)
		{
		case 0x30:   /* c.f */
			CompileAndExec(cp,dynaOpFPSetFalse(cp));
			break;
			
		case 0x31:   /* c.un */
			break;
			
		case 0x32:   /* c.eq */
			CompileAndExec(cp,dynaOpFPEqualS(cp,MAKE_FS,MAKE_FT));
			break;
			
		case 0x33:   /* c.ueq */
			CompileAndExec(cp,dynaOpFPEqualS(cp,MAKE_FS,MAKE_FT));
			break;
			
		case 0x34:   /* c.olt */
		case 0x35:   /* c.ult */
		case 0x36:   /* c.ole */
		case 0x37:   /* c.ule */
		case 0x38:   /* c.sf */
		case 0x39:   /* c.ngle */
		case 0x3a:   /* c.seq */
		case 0x3b:   /* c.ngl */
			break;
			
		case 0x3c:   /* c.lt */
			CompileAndExec(cp,dynaOpFPLessThanS(cp,MAKE_FS,MAKE_FT));
			break;
			
		case 0x3d:   /* c.nge */
			CompileAndExec(cp,dynaOpFPLessThanS(cp,MAKE_FS,MAKE_FT));
			break;
			
		case 0x3e:   /* c.le */
			CompileAndExec(cp,dynaOpFPLessThanEqualS(cp,MAKE_FS,MAKE_FT));
			break;
			
		case 0x3f:   /* c.ngt */
			CompileAndExec(cp,dynaOpFPLessThanEqualS(cp,MAKE_FS,MAKE_FT));
			break;
			
		} /* switch(__F) */
		
		break;
		}
		
		case FMT_D:   /* c.*.d */
			{
			switch(MAKE_F)
			{
			case 0x30:   /* c.f */
				CompileAndExec(cp,dynaOpFPSetFalse(cp));
				break;
				
			case 0x31:   /* c.un */
				break;
				
			case 0x32:   /* c.eq */
				CompileAndExec(cp,dynaOpFPEqualD(cp,MAKE_FS,MAKE_FT));
				break;
				
			case 0x33:   /* c.ueq */
				CompileAndExec(cp,dynaOpFPEqualD(cp,MAKE_FS,MAKE_FT));
				break;
				
			case 0x34:   /* c.olt */
			case 0x35:   /* c.ult */
			case 0x36:   /* c.ole */
			case 0x37:   /* c.ule */
			case 0x38:   /* c.sf */
			case 0x39:   /* c.ngle */
			case 0x3a:   /* c.seq */
			case 0x3b:   /* c.ngl */
				break;
				
			case 0x3c:   /* c.lt */
				CompileAndExec(cp,dynaOpFPLessThanD(cp,MAKE_FS,MAKE_FT));
				break;
				
			case 0x3d:   /* c.nge */
				CompileAndExec(cp,dynaOpFPLessThanD(cp,MAKE_FS,MAKE_FT));
				break;
				
			case 0x3e:   /* c.le */
				CompileAndExec(cp,dynaOpFPLessThanEqualD(cp,MAKE_FS,MAKE_FT));
				break;
				
			case 0x3f:   /* c.ngt */
				CompileAndExec(cp,dynaOpFPLessThanEqualD(cp,MAKE_FS,MAKE_FT));
				break;
				
			} /* switch(__F) */
			}
				
    } /* switch(__FMT) */
	return;
#endif
	
	switch(MAKE_FMT)
	{
	case FMT_S:   /* c.*.s */
		switch(MAKE_F)
		{
		case 0x30:   /* c.f */
			r->CCR1[31*2] &= ~0x00800000;
			break;
			
		case 0x31:   /* c.un */
			r->CCR1[31*2] &= ~0x00800000;//test
				break;
			
		case 0x32:   /* c.eq */
			if((float)  *(float *)&r->FPR[MAKE_FS] == (float) *(float *)&r->FPR[MAKE_FT])
				r->CCR1[31*2] |= 0x00800000;
			else
				r->CCR1[31*2] &= ~0x00800000;
			break;
			
		case 0x33:   /* c.ueq */
			/* BUG: check NaN */
			if( (float) *(float *)&r->FPR[MAKE_FS] == (float) *(float *)&r->FPR[MAKE_FT]) 
				r->CCR1[31*2] |= 0x00800000;
			else
				r->CCR1[31*2] &= ~0x00800000;
			break;
			
		case 0x34:   /* c.olt */
		case 0x35:   /* c.ult */
		case 0x36:   /* c.ole */
		case 0x37:   /* c.ule */
		case 0x38:   /* c.sf */
		case 0x39:   /* c.ngle */
		case 0x3a:   /* c.seq */
		case 0x3b:   /* c.ngl */
		case 0x3c:   /* c.lt */
			if( (float) *(float *)&r->FPR[MAKE_FS] < (float) *(float *)&r->FPR[MAKE_FT]) 
				r->CCR1[31*2] |= 0x00800000;
			else
				r->CCR1[31*2] &= ~0x00800000;
			break;
			
		case 0x3d:   /* c.nge */
			if((float)  *(float *)&r->FPR[MAKE_FS] < (float) *(float *)&r->FPR[MAKE_FT]) 
				r->CCR1[31*2] |= 0x00800000;
			else
				r->CCR1[31*2] &= ~0x00800000;
			break;
			
		case 0x3e:   /* c.le */
			if((float)  *(float *)&r->FPR[MAKE_FS] <= (float) *(float *)&r->FPR[MAKE_FT]) 
				r->CCR1[31*2] |= 0x00800000;
			else
				r->CCR1[31*2] &= ~0x00800000;
			break;
			
		case 0x3f:   /* c.ngt */
			/* BUG: check NaN */
			if((float)  *(float *)&r->FPR[MAKE_FS] <= (float) *(float *)&r->FPR[MAKE_FT]) 
				r->CCR1[31*2] |= 0x00800000;
			else
				r->CCR1[31*2] &= ~0x00800000;
			break;
			
		} /* switch(MAKE_F) */
		
		break;
		
		case FMT_D:   /* c.*.d */
			switch(MAKE_F)
			{
			case 0x30:   /* c.f */
				r->CCR1[31*2] &= ~0x00800000;
				break;
				
			case 0x31:   /* c.un */
			r->CCR1[31*2] &= ~0x00800000;//test
					break;
				
			case 0x32:   /* c.eq */
				if( *(double *)&r->FPR[MAKE_FS] == *(double *)&r->FPR[MAKE_FT]) 
					r->CCR1[31*2] |= 0x00800000;
				else
					r->CCR1[31*2] &= ~0x00800000;
				break;
				
			case 0x33:   /* c.ueq */
				/* BUG: check NaN */
				if( *(double *)&r->FPR[MAKE_FS] == *(double *)&r->FPR[MAKE_FT]) 
					r->CCR1[31*2] |= 0x00800000;
				else
					r->CCR1[31*2] &= ~0x00800000;
				break;
				
			case 0x34:   /* c.olt */
			case 0x35:   /* c.ult */
			case 0x36:   /* c.ole */
			case 0x37:   /* c.ule */
			case 0x38:   /* c.sf */
			case 0x39:   /* c.ngle */
			case 0x3a:   /* c.seq */
			case 0x3b:   /* c.ngl */
			if( (double) *(double *)&r->FPR[MAKE_FS] < (double) *(double *)&r->FPR[MAKE_FT]) 
				r->CCR1[31*2] |= 0x00800000;
			else
				r->CCR1[31*2] &= ~0x00800000;
					break;
				
			case 0x3c:   /* c.lt */
				if((double)  *(double *)&r->FPR[MAKE_FS] <(double)  *(double *)&r->FPR[MAKE_FT]) 
					r->CCR1[31*2] |= 0x00800000;
				else
					r->CCR1[31*2] &= ~0x00800000;
				break;
				
			case 0x3d:   /* c.nge */
				/* BUG: check NaN */
				if((double) *(double *)&r->FPR[MAKE_FS] < (double) *(double *)&r->FPR[MAKE_FT]) 
					r->CCR1[31*2] |= 0x00800000;
				else
					r->CCR1[31*2] &= ~0x00800000;
				break;
				
			case 0x3e:   /* c.le */
				if( (double) *(double *)&r->FPR[MAKE_FS] <= (double) *(double *)&r->FPR[MAKE_FT]) 
					r->CCR1[31*2] |= 0x00800000;
				else
					r->CCR1[31*2] &= ~0x00800000;
				break;
				
			case 0x3f:   /* c.ngt */
				/* BUG: check NaN */
				if((double)  *(double *)&r->FPR[MAKE_FS] <= (double) *(double *)&r->FPR[MAKE_FT]) 
					r->CCR1[31*2] |= 0x00800000;
				else
					r->CCR1[31*2] &= ~0x00800000;
				break;
				
			} /* switch(MAKE_F) */
			
			break;
			
					
        } /* switch(MAKE_FMT) */
		
} /* void cop1_c() */

