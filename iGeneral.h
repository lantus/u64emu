void iOpSra()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=((sDWORD)r->GPR[2*MAKE_RT]>>MAKE_SA);
}

void iOpSrl()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=((DWORD)r->GPR[2*MAKE_RT]>>MAKE_SA);
}

void iOpSll()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=((sDWORD)r->GPR[2*MAKE_RT]<<MAKE_SA);
}

void iOpSrav()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=((sDWORD)r->GPR[2*MAKE_RT]>>(r->GPR[2*MAKE_RS]&0x1f));
}

void iOpSrlv()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=((DWORD)r->GPR[2*MAKE_RT]>>(r->GPR[2*MAKE_RS]&0x1f));
}

void iOpSllv()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=((sDWORD)r->GPR[2*MAKE_RT]<<(r->GPR[2*MAKE_RS]&0x1f));
}

void iOpMfHi(BYTE op0)
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=r->Hi;
}

void iOpMfLo(BYTE op0)
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=r->Lo;
}

void iOpMtHi(BYTE op0)
{
	r->Hi=*(sQWORD *)&r->GPR[2*MAKE_RD];
}

void iOpMtLo(BYTE op0)
{
	r->Lo=*(sQWORD *)&r->GPR[2*MAKE_RD];
}

void iOpAdd()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=((sDWORD)r->GPR[2*MAKE_RS]+(sDWORD)r->GPR[2*MAKE_RT]);
}

void iOpSub()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=((sDWORD)r->GPR[2*MAKE_RS]-(sDWORD)r->GPR[2*MAKE_RT]);
}

void iOpAnd()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(*(sQWORD *)&r->GPR[2*MAKE_RS]&*(sQWORD *)&r->GPR[2*MAKE_RT]);
}

void iOpOr()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(*(sQWORD *)&r->GPR[2*MAKE_RS]|*(sQWORD *)&r->GPR[2*MAKE_RT]);
}

void iOpXor()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(*(sQWORD *)&r->GPR[2*MAKE_RS]^*(sQWORD *)&r->GPR[2*MAKE_RT]);
}

void iOpNor()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=~((*(sQWORD *)&r->GPR[2*MAKE_RS]|*(sQWORD *)&r->GPR[2*MAKE_RT]));
}


void iOpMult()
{
	sQWORD tmp;
	tmp=((sDWORD)r->GPR[2*MAKE_RS]*(sDWORD)r->GPR[2*MAKE_RT]);

	r->Hi=(sDWORD)(tmp>>32);
	r->Lo=(sDWORD)tmp;
}

void iOpMultU()
{
	QWORD tmp;
	tmp=((DWORD)r->GPR[2*MAKE_RS]*(DWORD)r->GPR[2*MAKE_RT]);

	r->Hi=(sDWORD)(tmp>>32);
	r->Lo=(sDWORD)tmp;
}

void iOpDiv()
{
	r->Lo=(sDWORD)((sDWORD)r->GPR[2*MAKE_RS]/(sDWORD)r->GPR[2*MAKE_RT]);
	r->Hi=(sDWORD)((sDWORD)r->GPR[2*MAKE_RS]%(sDWORD)r->GPR[2*MAKE_RT]);
}

void iOpDivU()
{
	r->Lo=(sDWORD)((DWORD)r->GPR[2*MAKE_RS]/(DWORD)r->GPR[2*MAKE_RT]);
	r->Hi=(sDWORD)((DWORD)r->GPR[2*MAKE_RS]%(DWORD)r->GPR[2*MAKE_RT]);
}

void iOpSlt()
{
	if(*(sQWORD *)&r->GPR[2*MAKE_RS]<*(sQWORD *)&r->GPR[2*MAKE_RT])
		*(sQWORD *)&r->GPR[2*MAKE_RD]=1;
	else
		*(sQWORD *)&r->GPR[2*MAKE_RD]=0;
}

void iOpSltU()
{
	if(*(QWORD *)&r->GPR[2*MAKE_RS]<*(QWORD *)&r->GPR[2*MAKE_RT])
		*(sQWORD *)&r->GPR[2*MAKE_RD]=1;
	else
		*(sQWORD *)&r->GPR[2*MAKE_RD]=0;
}

void iOpSlti()
{
	if(*(sQWORD *)&r->GPR[2*MAKE_RS]<(sQWORD)MAKE_I)
		*(sQWORD *)&r->GPR[2*MAKE_RT]=1;
	else
		*(sQWORD *)&r->GPR[2*MAKE_RT]=0;
}

void iOpSltiU()
{
	if(*(QWORD *)&r->GPR[2*MAKE_RS]<(sQWORD)MAKE_I)
		*(sQWORD *)&r->GPR[2*MAKE_RT]=1;
	else
		*(sQWORD *)&r->GPR[2*MAKE_RT]=0;
}


void iOpDAddI()
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

void iOpDMult()
{
	r->Hi=(r->GPR[2*MAKE_RS]*r->GPR[2*MAKE_RT])>>32;
	r->Lo=(DWORD)(r->GPR[2*MAKE_RS]*r->GPR[2*MAKE_RT]);
}

void iOpDMultU()
{
	r->Hi=((QWORD)r->GPR[2*MAKE_RS]*(QWORD)r->GPR[2*MAKE_RT])>>32;
	r->Lo=(DWORD)((QWORD)r->GPR[2*MAKE_RS]*(QWORD)r->GPR[2*MAKE_RT]);
}

void iOpDDiv()
{
	if(r->GPR[2*op1] != 0)
	{
		m_Reg->Lo = r->GPR[2*MAKE_RS]/r->GPR[2*MAKE_RT];
		m_Reg->Hi = r->GPR[2*MAKE_RS]%r->GPR[2*MAKE_RT];
	}
}

void iOpDDivU()
{
	if(r->GPR[2*op1] != 0)
	{
		m_Reg->Lo = ((QWORD)r->GPR[2*MAKE_RS]/(QWORD)r->GPR[2*MAKE_RT]);
		m_Reg->Hi = ((QWORD)r->GPR[2*MAKE_RS]%(QWORD)r->GPR[2*MAKE_RT]);
	}
}

void iOpAddI(,DWORD op2)
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=((sDWORD)r->GPR[2*MAKE_RT]+(sDWORD)MAKE_I);
}

void iOpDSra()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=*(sQWORD *)&r->GPR[2*MAKE_RT]>>MAKE_SA;
}

void iOpDSraV()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=*(sQWORD *)&r->GPR[2*MAKE_RT]>>(r->GPR[2*MAKE_RS]&0x1f);
}

void iOpDSra32()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=*(sQWORD *)&r->GPR[2*MAKE_RT]>>(MAKE_RS+32);
}

void iOpDSrl()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(*(QWORD *)&r->GPR[2*MAKE_RT]>>MAKE_SA);
}

void iOpDSrlV()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(*(QWORD *)&r->GPR[2*MAKE_RT]>>(r->GPR[2*MAKE_RS]&0x1f));
}

void iOpDSrl32()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(*(QWORD *)&r->GPR[2*MAKE_RT]>>(MAKE_RS+32));
}

void iOpDSll()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(*(QWORD *)&r->GPR[2*MAKE_RT]<<MAKE_SA);
}

void iOpDSllV()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(*(QWORD *)&r->GPR[2*MAKE_RT]<<(r->GPR[2*MAKE_RS]&0x1f));
}

void iOpDSll32()
{
	*(sQWORD *)&r->GPR[2*MAKE_RD]=(*(QWORD *)&r->GPR[2*MAKE_RT]<<(MAKE_RS+32));
}

void iOpRs0Mf()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=(sDWORD)r->CPR0[2*MAKE_RS];
}

void iOpRs0DMf()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=*(sQWORD *)&r->CPR0[2*MAKE_RS];
}

void iOpRs0Mt()
{
	*(sQWORD *)&r->CPR0[2*MAKE_RT]=(sDWORD)r->GPR[2*MAKE_RD];
}

void iOpRs0DMt()
{
	*(sQWORD *)&r->CPR0[2*MAKE_RT]=*(sQWORD *)&r->GPR[2*MAKE_RD];
}

void iOpRs1Mf()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=(sDWORD)r->CPR1[2*MAKE_RS];
}

void iOpRs1DMf()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=*(sQWORD *)&r->CPR1[2*MAKE_RS];
}

void iOpRs1Mt()
{
	*(sQWORD *)&r->CPR1[2*MAKE_RT]=(sDWORD)r->GPR[2*MAKE_RD];
}

void iOpRs1DMt()
{
	*(sQWORD *)&r->CPR1[2*MAKE_RT]=*(sQWORD *)&r->GPR[2*MAKE_RD];
}

void iOpRs0Ct()
{
	*(sQWORD *)&r->CPC0[2*MAKE_RD]=(sDWORD)r->GPR[2*MAKE_RT];
}

void iOpRs1Ct()
{
	*(sQWORD *)&r->CPC1[2*MAKE_RD]=(sDWORD)r->GPR[2*MAKE_RT];
}

void iOpAndI()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=(*(sQWORD *)&r->GPR[2*MAKE_RS]&(DWORD)MAKE_IU);
}

void iOpOrI()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=(*(sQWORD *)&r->GPR[2*MAKE_RS]|(DWORD)MAKE_IU);
}

void iOpXorI()
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=(*(sQWORD *)&r->GPR[2*MAKE_RS]^(DWORD)MAKE_IU);
}

