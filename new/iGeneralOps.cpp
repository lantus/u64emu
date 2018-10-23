
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "iMain.h"
#include "iCPU.h"			// Core 4600 emulation routines
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines


#define BRANCH_CAST *(sDWORD *)&r
#define BRANCH_CAST_U *(DWORD *)&r
extern DWORD dynaVCount;
//#pragma optimize("",off)

void iOpSra()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(sQWORD)((sDWORD)r->GPR[2*MAKE_RT]>>MAKE_SA);
}

void iOpSrl()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(sQWORD)(sDWORD)((DWORD)r->GPR[2*MAKE_RT]>>MAKE_SA);
}

void iOpSll()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(sQWORD)((sDWORD)r->GPR[2*MAKE_RT]<<MAKE_SA);
}

void iOpSrav()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(sQWORD)(sDWORD)((sDWORD)r->GPR[2*MAKE_RT]>>(r->GPR[2*MAKE_RS]&0x1f));
}

void iOpSrlv()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(sQWORD)(sDWORD)((DWORD)r->GPR[2*MAKE_RT]>>(r->GPR[2*MAKE_RS]&0x1f));
}

void iOpSllv()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(sQWORD)(sDWORD)((DWORD)r->GPR[2*MAKE_RT]<<(r->GPR[2*MAKE_RS]&0x1f));
}

void iOpMfHi()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=r->Hi;
}

void iOpMfLo()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=r->Lo;
}

void iOpMtHi()
{
	r->Hi=*(sQWORD *)&r->GPR[2*MAKE_RS];
}

void iOpMtLo()
{
	r->Lo=*(sQWORD *)&r->GPR[2*MAKE_RS];
}

void iOpAdd()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(sQWORD)((sDWORD)r->GPR[2*MAKE_RS]+(sDWORD)r->GPR[2*MAKE_RT]);
}

void iOpSub()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(sQWORD)((sDWORD)r->GPR[2*MAKE_RS]-(sDWORD)r->GPR[2*MAKE_RT]);
}

void iOpAddu()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(sQWORD)((sDWORD)r->GPR[2*MAKE_RS]+(sDWORD)r->GPR[2*MAKE_RT]);
}

void iOpSubu()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(sQWORD)((sDWORD)r->GPR[2*MAKE_RS]-(sDWORD)r->GPR[2*MAKE_RT]);
}

void iOpAnd()
{
	*(QWORD *)&r->GPR[2*MAKE_RD]=(QWORD)(*(QWORD *)&r->GPR[2*MAKE_RS] & *(QWORD *)&r->GPR[2*MAKE_RT]);
}

void iOpOr() 
{
	*(QWORD *)&r->GPR[2*MAKE_RD]=(QWORD)(*(QWORD *)&r->GPR[2*MAKE_RS] | *(QWORD *)&r->GPR[2*MAKE_RT]);
}

void iOpXor()
{
	*(QWORD *)&r->GPR[2*MAKE_RD]=(QWORD)(*(QWORD *)&r->GPR[2*MAKE_RS] ^ *(QWORD *)&r->GPR[2*MAKE_RT]);
}

void iOpNor()
{
	*(QWORD *)&r->GPR[2*MAKE_RD]=(QWORD)~((*(QWORD *)&r->GPR[2*MAKE_RS] | *(QWORD *)&r->GPR[2*MAKE_RT]));
}


void iOpMult()
{
/*
	sQWORD tmp;
	sDWORD hi,lo;
	tmp=(sDWORD)r->GPR[2*MAKE_RS]*(sDWORD)r->GPR[2*MAKE_RT];

	r->Hi=(sDWORD)(tmp>>32);
	r->Lo=(sDWORD)(tmp&0x0ffffffff);
*/
	DWORD *r1,*r2,*hi,*lo;
	r1=(DWORD *)&r->GPR[2*MAKE_RS];
	r2=(DWORD *)&r->GPR[2*MAKE_RT];
	hi=(DWORD *)&r->Hi;
	lo=(DWORD *)&r->Lo;

	_asm
	{
		mov		eax,dword ptr r2
		mov		esi,dword ptr lo
		mov		ecx,dword ptr r1
		mov		eax,dword ptr [eax]
		imul	DWORD PTR [ecx]			// *m_Param[1]* *m_Param[2]
		mov		ecx,edx
		cdq
		mov		DWORD PTR [esi],eax			// m_Reg->Lo
		mov		DWORD PTR [esi+4],edx		// m_Reg->Lo
		mov		eax,ecx
		cdq
		mov		DWORD PTR [esi+8],eax		// m_Reg->Hi
		mov		DWORD PTR [esi+12],edx		// m_Reg->Hi
	}
}

void iOpMultu()
{
/*
	QWORD tmp;
	tmp=(DWORD)r->GPR[2*MAKE_RS]*(DWORD)r->GPR[2*MAKE_RT];

	r->Hi=(sDWORD)(tmp>>32);
	r->Lo=(sDWORD)(tmp&0x0ffffffff);
*/
	DWORD *r1,*r2,*hi,*lo;
	r1=(DWORD *)&r->GPR[2*MAKE_RS];
	r2=(DWORD *)&r->GPR[2*MAKE_RT];
	hi=(DWORD *)&r->Hi;
	lo=(DWORD *)&r->Lo;

	_asm
	{
		mov		eax,dword ptr r2
		mov		esi,dword ptr lo
		mov		ecx,dword ptr r1
		mov		eax,dword ptr [eax]
		mul	DWORD PTR [ecx]			// *m_Param[1]* *m_Param[2]
		mov		ecx,edx
		cdq
		mov		DWORD PTR [esi],eax			// m_Reg->Lo
		mov		DWORD PTR [esi+4],edx		// m_Reg->Lo
		mov		eax,ecx
		cdq
		mov		DWORD PTR [esi+8],eax		// m_Reg->Hi
		mov		DWORD PTR [esi+12],edx		// m_Reg->Hi
	}
}

void iOpDiv()
{
	if((sDWORD)r->GPR[2*MAKE_RT]!=0)
	{
		DWORD t1,t2;
		t1=r->GPR[2*MAKE_RS];
		t2=r->GPR[2*MAKE_RT];
		r->Lo=(sDWORD)((sDWORD)r->GPR[2*MAKE_RS]/(sDWORD)r->GPR[2*MAKE_RT]);
		r->Hi=(sDWORD)((sDWORD)r->GPR[2*MAKE_RS]%(sDWORD)r->GPR[2*MAKE_RT]);
	}
}

void iOpDivu()
{
	if((sDWORD)r->GPR[2*MAKE_RT]!=0)
	{
		DWORD t1,t2;
		t1=r->GPR[2*MAKE_RS];
		t2=r->GPR[2*MAKE_RT];
		r->Lo=(sDWORD)((DWORD)r->GPR[2*MAKE_RS]/(DWORD)r->GPR[2*MAKE_RT]);
		r->Hi=(sDWORD)((DWORD)r->GPR[2*MAKE_RS]%(DWORD)r->GPR[2*MAKE_RT]);
	}
}

void iOpSlt()
{
	if(BRANCH_CAST->GPR[2*MAKE_RS]<BRANCH_CAST->GPR[2*MAKE_RT])
		*(sQWORD *)&r->GPR[2*MAKE_RD]=1;
	else
		*(sQWORD *)&r->GPR[2*MAKE_RD]=0;
}

void iOpSltu()
{
	if(BRANCH_CAST_U->GPR[2*MAKE_RS]<BRANCH_CAST_U->GPR[2*MAKE_RT])
		*(sQWORD *)&r->GPR[2*MAKE_RD]=1;
	else
		*(sQWORD *)&r->GPR[2*MAKE_RD]=0;
}

void iOpSlti()
{
	if(BRANCH_CAST->GPR[2*MAKE_RS]<(sQWORD)MAKE_I)
		*(sQWORD *)&r->GPR[2*MAKE_RT]=1;
	else
		*(sQWORD *)&r->GPR[2*MAKE_RT]=0;
}

void iOpSltiu()
{
	if(BRANCH_CAST_U->GPR[2*MAKE_RS]<(QWORD)MAKE_I)
		*(sQWORD *)&r->GPR[2*MAKE_RT]=1;
	else
		*(sQWORD *)&r->GPR[2*MAKE_RT]=0;
}


void iOpDAddi()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=*(sQWORD *)&r->GPR[2*MAKE_RS]+(sQWORD)MAKE_I;
}

void iOpDAddiu()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=*(sQWORD *)&r->GPR[2*MAKE_RS]+(sQWORD)MAKE_I;
}

void iOpDAdd()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=*(sQWORD *)&r->GPR[2*MAKE_RS]+*(sQWORD *)&r->GPR[2*MAKE_RT];
}

void iOpDSub()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=*(sQWORD *)&r->GPR[2*MAKE_RS]-*(sQWORD *)&r->GPR[2*MAKE_RT];
}

void iOpDAddu()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=*(sQWORD *)&r->GPR[2*MAKE_RS]+*(sQWORD *)&r->GPR[2*MAKE_RT];
}

void iOpDSubu()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=*(sQWORD *)&r->GPR[2*MAKE_RS]-*(sQWORD *)&r->GPR[2*MAKE_RT];
}

void iOpDMult()
{
//	r->Hi=(*(sQWORD *)&r->GPR[2*MAKE_RS]**(sQWORD *)&r->GPR[2*MAKE_RT])>>32;
//	r->Lo=(DWORD)(*(sQWORD *)&r->GPR[2*MAKE_RS]**(sQWORD *)&r->GPR[2*MAKE_RT]);
//	return;

	DWORD x0=r->GPR[2*MAKE_RS];
	DWORD x1=r->GPR[2*MAKE_RS+1];
	DWORD y0=r->GPR[2*MAKE_RT];
	DWORD y1=r->GPR[2*MAKE_RT+1];
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
	r->Lo=((QWORD)p1<<32)+((QWORD)p0);
	r->Hi=((QWORD)p3<<32)+((QWORD)p2);
}

void iOpDMultu()
{
//	r->Hi=(*(QWORD *)&r->GPR[2*MAKE_RS]**(QWORD *)&r->GPR[2*MAKE_RT])>>32;
//	r->Lo=(DWORD)(*(QWORD *)&r->GPR[2*MAKE_RS]**(QWORD *)&r->GPR[2*MAKE_RT]);
//	return;

	DWORD x0=r->GPR[2*MAKE_RS];
	DWORD x1=r->GPR[2*MAKE_RS+1];
	DWORD y0=r->GPR[2*MAKE_RT];
	DWORD y1=r->GPR[2*MAKE_RT+1];
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
	r->Lo=((QWORD)p1<<32)+((QWORD)p0);
	r->Hi=((QWORD)p3<<32)+((QWORD)p2);
}

void iOpDDiv()
{
	if(r->GPR[2*MAKE_RT] != 0)
	{
		r->Lo = *(sQWORD *)&r->GPR[2*MAKE_RS]/ *(sQWORD *)&r->GPR[2*MAKE_RT];
		r->Hi = *(sQWORD *)&r->GPR[2*MAKE_RS]%*(sQWORD *)&r->GPR[2*MAKE_RT];
	}
}

void iOpDDivu()
{
	if(r->GPR[2*MAKE_RT] != 0)
	{
		r->Lo = (*(QWORD *)&r->GPR[2*MAKE_RS]/ *(QWORD *)&r->GPR[2*MAKE_RT]);
		r->Hi = (*(QWORD *)&r->GPR[2*MAKE_RS]%*(QWORD *)&r->GPR[2*MAKE_RT]);
	}
}

void iOpAddi()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=(sQWORD)((sDWORD)r->GPR[2*MAKE_RS]+(sDWORD)MAKE_I);
}

void iOpAddiu()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=(sQWORD)((sDWORD)r->GPR[2*MAKE_RS]+(sDWORD)MAKE_I);
}

void iOpDSra()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=*(sQWORD *)&r->GPR[2*MAKE_RT]>>MAKE_SA;
}

void iOpDSrav()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=*(sQWORD *)&r->GPR[2*MAKE_RT]>>(r->GPR[2*MAKE_RS]&0x3f);
}

void iOpDSra32()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=*(sQWORD *)&r->GPR[2*MAKE_RT]>>(MAKE_SA+32);
}

void iOpDSrl()
{
	*(QWORD *)&r->GPR[2*MAKE_RD]=*(QWORD *)&r->GPR[2*MAKE_RT]>>MAKE_SA;
}

void iOpDSrlv()
{
	*(QWORD *)&r->GPR[2*MAKE_RD]=*(QWORD *)&r->GPR[2*MAKE_RT]>>(r->GPR[2*MAKE_RS]&0x3f);
}

void iOpDSrl32()
{
	r->GPR[2*MAKE_RD]=r->GPR[2*MAKE_RT+1]>>MAKE_SA;
	r->GPR[2*MAKE_RD+1]=0;
//	*(QWORD *)&r->GPR[2*MAKE_RD]=*(QWORD *)&r->GPR[2*MAKE_RT]>>(MAKE_SA+32);
}

void iOpDSll()
{
	*(QWORD *)&r->GPR[2*MAKE_RD]=*(QWORD *)&r->GPR[2*MAKE_RT]<<MAKE_SA;
}

void iOpDSllv()
{
	*(QWORD *)&r->GPR[2*MAKE_RD]=*(QWORD *)&r->GPR[2*MAKE_RT]<<(r->GPR[2*MAKE_RS]&0x3f);
}

void iOpDSll32()
{
	r->GPR[2*MAKE_RD+1]=r->GPR[2*MAKE_RT]<<MAKE_SA;
	r->GPR[2*MAKE_RD]=0;
//	*(QWORD *)&r->GPR[2*MAKE_RD]=(QWORD)(*(QWORD *)&r->GPR[2*MAKE_RT]<<(MAKE_SA+32));
}

void iOpMf0()
{
	if(MAKE_RD==9)
	{
		r->ICount+=1200;
		*(sQWORD *)&r->GPR[2*MAKE_RT]=r->ICount;
	}
	else 
		r->GPR[2*MAKE_RT]=r->CPR0[2*MAKE_RD];
	if(MAKE_RD==13)
	{
//		if(!(r->CPR0[2*MAKE_RD]&0x800))
		{
			if(r->CPR0[2*MAKE_RD]&0x400)
			{
//				iCpuVSYNC();
//				dynaVCount++;
			}
	//		r->CPR0[2*MAKE_RD]|=0x400;
			r->CPR0[2*MAKE_RD]^=0x400;
		}
//		r->CPR0[2*MAKE_RD]^=0x400;
	}
/*
*/
}


void iOpDMf0()
{
	if(MAKE_RD==9)
	{
		r->ICount+=12;
		*(sQWORD *)&r->GPR[2*MAKE_RT]=r->ICount;
	}
	else
	{
		r->GPR[2*MAKE_RT]=r->CPR0[2*MAKE_RD];
		r->GPR[2*MAKE_RT+1]=r->CPR0[2*MAKE_RD+1];
	}
	if(MAKE_RD==13)
	{
		r->CPR0[2*MAKE_RD]^=0x400;
	}
}

void iOpMf2()
{
	r->GPR[2*MAKE_RT]=r->CPR2[2*MAKE_RD];
}

void iOpDMf2()
{
	r->GPR[2*MAKE_RT]=r->CPR2[2*MAKE_RD];
	r->GPR[2*MAKE_RT+1]=r->CPR2[2*MAKE_RD+1];
}

void iOpMt0()
{
	if(MAKE_RD==STATUS)
	{
		r->CPR0[2*MAKE_RD]=r->GPR[2*MAKE_RT];
//		r->CPR0[2*MAKE_RD]&=~0x20000000;
		if(r->GPR[MAKE_RT*2]&0x20000000)
		{
//			theApp.LogMessage("COP1 SetUnusable at %X",r->PC);
		}
	}
	if(MAKE_RD == COMPARE)
	{
		r->CPR0[2*CAUSE] &= ~0x00008000;
		r->CompareCount=r->GPR[MAKE_RT*2];
//		r->NextIntCount=r->CompareCount;
		return;
		if((r->CompareCount<r->NextIntCount)&&r->CompareCount)
		{
			if(r->CompareCount>r->ICount)
				r->NextIntCount=r->CompareCount;
		}
	}
	if(MAKE_RD==COUNT)
	{
//		r->ICount=r->GPR[2*MAKE_RT];
//		r->NextIntCount=r->ICount+200000;
	}
	
	/* if write to CAUSE reg mask out all bits except IP0 & IP1 */
	if(MAKE_RD == CAUSE)
	{
		r->CPR0[2*CAUSE] = r->GPR[MAKE_RT*2] & 0x00000300;
		return;
	}
	r->CPR0[2*MAKE_RD]=r->GPR[2*MAKE_RT];
}

void iOpDMt0()
{
	if(MAKE_RD==STATUS)
	{
//		r->GPR[MAKE_RT*2]&=~0x20000000;
		if(r->GPR[MAKE_RT*2]&0x20000000)
		{
			theApp.LogMessage("COP1 SetUnusable at %X",r->PC);
		}
	}
	if(MAKE_RD == COMPARE)
	{
		r->CPR0[2*CAUSE] &= ~0x00008000;
		r->CompareCount=r->GPR[MAKE_RT*2];
//		r->NextIntCount=r->CompareCount;
		return;
		if((r->CompareCount<r->NextIntCount)&&r->CompareCount)
			if(r->CompareCount>r->ICount)
				r->NextIntCount=r->CompareCount;
			else if(r->CompareCount)
			{
				r->ICount=r->CompareCount-0x20000;
			}
	}
	
	/* if write to CAUSE reg mask out all bits except IP0 & IP1 */
	if(MAKE_RD == CAUSE)
	{
		r->CPR0[2*CAUSE] = r->GPR[MAKE_RT*2] & 0x00000300;
		return;
	}
	r->CPR0[2*MAKE_RD]=r->GPR[2*MAKE_RT];
	r->CPR0[2*MAKE_RD+1]=r->GPR[2*MAKE_RT+1];
}

void iOpMt2()
{
	*(sDWORD *)&r->CPR2[2*MAKE_RD]=r->GPR[2*MAKE_RT];
}

void iOpDMt2()
{
	*(sQWORD *)&r->CPR2[2*MAKE_RD]=*(sQWORD *)&r->GPR[2*MAKE_RT];
}

void iOpMf1()
{
	r->GPR[2*MAKE_RT]=r->FPR[MAKE_FS];
}

void iOpDMf1()
{
	r->GPR[2*MAKE_RT]=r->FPR[MAKE_FS];
	r->GPR[2*MAKE_RT+1]=r->FPR[MAKE_FS+1];
}

void iOpMt1()
{
	r->FPR[MAKE_RD]=r->GPR[2*MAKE_RT];
}

void iOpDMt1()
{
	r->FPR[MAKE_RD]=r->GPR[2*MAKE_RT];
	r->FPR[MAKE_RD+1]=r->GPR[2*MAKE_RT+1];
}

void iOpCt0()
{
	*(sQWORD *)&r->CCR0[2*MAKE_RD]=(sQWORD)*(sQWORD *)&r->GPR[2*MAKE_RT];
}

void iOpCt1()
{
	WORD NearMode=0x027f;		// rounds to nearest. special for Intel FP
	WORD ZeroMode=0x0e7f;		// truncates. special for Intel FP
	WORD UpMode=0x0a7f;			// rounds up. special for Intel FP
	WORD DownMode=0x067f;		// rounds down. special for Intel FP
	WORD CurMode;
	if(MAKE_RD==31)
	{
		DWORD val=*(sDWORD *)&r->GPR[2*MAKE_RT];
		switch(val&3)
		{
		case 0:
			{
				r->CurRoundMode=NearMode;
/*
				_asm
				{
					fldcw NearMode
					wait
				}
*/
				break;
			}
		case 1:
			{
				r->CurRoundMode=ZeroMode;
/*
				_asm
				{
					fldcw ZeroMode
					wait
				}
*/
				break;
			}
		case 2:
			{
				r->CurRoundMode=UpMode;
/*
				_asm
				{
					fldcw UpMode
					wait
				}
*/
				break;
			}
		case 3:
			{
				r->CurRoundMode=DownMode;
/*
				_asm
				{
					fldcw DownMode
					wait
				}
*/
				break;
			}
		}
		CurMode=r->CurRoundMode;
//		theApp.LogMessage("Setting Control - CurValue %X, New Value %X",*(sDWORD *)&r->CCR1[2*MAKE_RD],val);

		_asm
		{
			wait
			fldcw CurMode
			wait
		}
	}
	*(sQWORD *)&r->CCR1[2*MAKE_RD]=(sQWORD)*(sQWORD *)&r->GPR[2*MAKE_RT];
}

void iOpCt2()
{
	*(sQWORD *)&r->CCR2[2*MAKE_RD]=(sQWORD)*(sQWORD *)&r->GPR[2*MAKE_RT];
}

void iOpCf0()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=(sQWORD)*(sQWORD *)&r->CCR0[2*MAKE_RD];
}

void iOpCf1()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=(sQWORD)*(sQWORD *)&r->CCR1[2*MAKE_RD];
}

void iOpCf2()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=(sQWORD)*(sQWORD *)&r->CCR2[2*MAKE_RD];
}


void iOpAndi()
{
//	*(QWORD *)&r->GPR[2*MAKE_RT]=*(QWORD *)&r->GPR[2*MAKE_RS]&(QWORD)(DWORD)MAKE_IU;
	*(DWORD *)&r->GPR[2*MAKE_RT]=*(DWORD *)&r->GPR[2*MAKE_RS]&MAKE_IU;
	*(DWORD *)&r->GPR[2*MAKE_RT+1]=0;
}

void iOpOri()
{
//	*(QWORD *)&r->GPR[2*MAKE_RT]=*(QWORD *)&r->GPR[2*MAKE_RS]|(QWORD)(DWORD)MAKE_IU;
	*(DWORD *)&r->GPR[2*MAKE_RT]=*(DWORD *)&r->GPR[2*MAKE_RS]|MAKE_IU;
	*(DWORD *)&r->GPR[2*MAKE_RT+1]=*(DWORD *)&r->GPR[2*MAKE_RS+1];
}

void iOpXori()
{
//	*(QWORD *)&r->GPR[2*MAKE_RT]=*(QWORD *)&r->GPR[2*MAKE_RS]^(QWORD)(DWORD)MAKE_IU;
	*(DWORD *)&r->GPR[2*MAKE_RT]=*(DWORD *)&r->GPR[2*MAKE_RS]^MAKE_IU;
	*(DWORD *)&r->GPR[2*MAKE_RT+1]=*(DWORD *)&r->GPR[2*MAKE_RS+1];
}

#pragma optimize("",on)
