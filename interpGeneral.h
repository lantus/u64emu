void interpOpSra(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]>>op2);
}

void interpOpSrl(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((DWORD)dasmReg[op1]>>op2);
}

void interpOpSll(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]<<op2);
}

void interpOpSrav(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]>>(dasmReg[op2]&0x1f));
}

void interpOpSrlv(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((DWORD)dasmReg[op1]>>(dasmReg[op2]&0x1f));
}

void interpOpSllv(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]<<(dasmReg[op2]&0x1f));
}

void interpOpMfHi(BYTE op0)
{
	dasmReg[op0]=dasmRegHi;
}

void interpOpMfLo(BYTE op0)
{
	dasmReg[op0]=dasmRegLo;
}

void interpOpMtHi(BYTE op0)
{
	dasmRegHi=dasmReg[op0];
}

void interpOpMtLo(BYTE op0)
{
	dasmRegLo=dasmReg[op0];
}

void interpOpRegMove(BYTE op0,BYTE op1)
{
	dasmReg[op0]=(sDWORD) dasmReg[op1];
}

void interpOpRegMoveD(BYTE op0,BYTE op1)
{
	dasmReg[op0]= dasmReg[op1];
}

void interpOpAdd(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]+(sDWORD)dasmReg[op2]);
}

void interpOpSub(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]-(sDWORD)dasmReg[op2]);
}

void interpOpAnd(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]&(sDWORD)dasmReg[op2]);
}

void interpOpOr(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]|(sDWORD)dasmReg[op2]);
}

void interpOpXor(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]^(sDWORD)dasmReg[op2]);
}

void interpOpNor(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=~(((sDWORD)dasmReg[op1]|(sDWORD)dasmReg[op2]));
}

void interpOpAddEqual(BYTE op0,BYTE op1)
{
	dasmReg[op0]=((sDWORD)dasmReg[op0]+(sDWORD)dasmReg[op1]);
}

void interpOpSubEqual(BYTE op0,BYTE op1)
{
	dasmReg[op0]=((sDWORD)dasmReg[op0]+(sDWORD)dasmReg[op1]);
}

void interpOpAndEqual(BYTE op0,BYTE op1)
{
	dasmReg[op0]=((sDWORD)dasmReg[op0]&(sDWORD)dasmReg[op1]);
}

void interpOpOrEqual(BYTE op0,BYTE op1)
{
	dasmReg[op0]=((sDWORD)dasmReg[op0]|(sDWORD)dasmReg[op1]);
}

void interpOpXorEqual(BYTE op0,BYTE op1)
{
	dasmReg[op0]=((sDWORD)dasmReg[op0]^(sDWORD)dasmReg[op1]);
}

void interpOpNorEqual(BYTE op0,BYTE op1)
{
	dasmReg[op0]=~(((sDWORD)dasmReg[op0]|(sDWORD)dasmReg[op1]));
}

void interpOpMult(BYTE op0,BYTE op1)
{
	sQWORD tmp;
	tmp=((sDWORD)dasmReg[op0]*(sDWORD)dasmReg[op1]);

	dasmRegHi=(sDWORD)(tmp>>32);
	dasmRegLo=(sDWORD)tmp;
}

void interpOpMultU(BYTE op0,BYTE op1)
{
	QWORD tmp;
	tmp=((DWORD)dasmReg[op0]*(DWORD)dasmReg[op1]);

	dasmRegHi=(sDWORD)(tmp>>32);
	dasmRegLo=(sDWORD)tmp;
}

void interpOpDiv(BYTE op0,BYTE op1)
{
	dasmRegLo=(sDWORD)((sDWORD)dasmReg[op0]/(sDWORD)dasmReg[op1]);
	dasmRegHi=(sDWORD)((sDWORD)dasmReg[op0]%(sDWORD)dasmReg[op1]);
}

void interpOpDivU(BYTE op0,BYTE op1)
{
	dasmRegLo=(sDWORD)((DWORD)dasmReg[op0]/(DWORD)dasmReg[op1]);
	dasmRegHi=(sDWORD)((DWORD)dasmReg[op0]%(DWORD)dasmReg[op1]);
}

void interpOpSlt(BYTE op0,BYTE op1,BYTE op2)
{
	if(dasmReg[op1]<dasmReg[op2])
		dasmReg[op0]=1;
	else
		dasmReg[op0]=0;
}

void interpOpSltU(BYTE op0,BYTE op1,BYTE op2)
{
	if((QWORD)dasmReg[op1]<(QWORD)dasmReg[op2])
		dasmReg[op0]=1;
	else
		dasmReg[op0]=0;
}

void interpOpSlti(BYTE op0,BYTE op1,DWORD imm)
{
	if(dasmReg[op1]<(sDWORD)imm)
		dasmReg[op0]=1;
	else
		dasmReg[op0]=0;
}

void interpOpSltiU(BYTE op0,BYTE op1,DWORD imm)
{
	if((QWORD)dasmReg[op1]<imm)
		dasmReg[op0]=1;
	else
		dasmReg[op0]=0;
}


void interpOpDAddI(BYTE op0,BYTE op1,DWORD Imm)
{
	dasmReg[op0]=dasmReg[op1]+(sDWORD)Imm;
}

void interpOpDAddIEqual(BYTE op0,DWORD Imm)
{
	dasmReg[op0]+=(sDWORD)Imm;
}

void interpOpDAdd(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=dasmReg[op1]+dasmReg[op2];
}

void interpOpDAddEqual(BYTE op0,BYTE op1)
{
	dasmReg[op0]+=dasmReg[op1];
}

void interpOpDSub(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=dasmReg[op1]-dasmReg[op2];
}

void interpOpDSubEqual(BYTE op0,BYTE op1)
{
	dasmReg[op0]-=dasmReg[op1];
}

void interpOpDMult(BYTE op0,BYTE op1)
{
	dasmRegHi=(dasmReg[op0]*dasmReg[op1])>>32;
	dasmRegLo=(DWORD)(dasmReg[op0]*dasmReg[op1]);
}

void interpOpDMultU(BYTE op0,BYTE op1)
{
	dasmRegHi=((QWORD)dasmReg[op0]*(QWORD)dasmReg[op1])>>32;
	dasmRegLo=(DWORD)((QWORD)dasmReg[op0]*(QWORD)dasmReg[op1]);
}

void interpOpDDiv(BYTE op0,BYTE op1)
{
	if(dasmReg[op1] != 0)
	{
//		m_Reg->Lo = dasmReg[op0]/dasmReg[op1];
//		m_Reg->Hi = dasmReg[op0]%dasmReg[op1];
	}
}

void interpOpDDivU(BYTE op0,BYTE op1)
{
	if(dasmReg[op1] != 0)
	{
//		m_Reg->Lo = ((QWORD)dasmReg[op0]/(QWORD)dasmReg[op1]);
//		m_Reg->Hi = ((QWORD)dasmReg[op0]%(QWORD)dasmReg[op1]);
	}
}

void interpOpAddI(BYTE op0,BYTE op1,DWORD op2)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]+(sDWORD)op2);
}

void interpOpAddIEqual(BYTE op0,DWORD op2)
{
	dasmReg[op0]+=(sDWORD)op2;
}

void interpOpLoadI(BYTE op0,DWORD op2)
{
	dasmReg[op0]=(sDWORD)op2;
}

void interpOpDSra(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=dasmReg[op1]>>op2;
}

void interpOpDSraV(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=dasmReg[op1]>>(dasmReg[op2]&0x1f);
}

void interpOpDSra32(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=dasmReg[op1]>>(op2+32);
}

void interpOpDSrl(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((QWORD)dasmReg[op1]>>op2);
}

void interpOpDSrlV(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((QWORD)dasmReg[op1]>>(dasmReg[op2]&0x1f));
}

void interpOpDSrl32(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((QWORD)dasmReg[op1]>>(op2+32));
}

void interpOpDSll(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((QWORD)dasmReg[op1]<<op2);
}

void interpOpDSllV(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((QWORD)dasmReg[op1]<<(dasmReg[op2]&0x1f));
}

void interpOpDSll32(BYTE op0,BYTE op1,BYTE op2)
{
	dasmReg[op0]=((QWORD)dasmReg[op1]<<(op2+32));
}

void interpOpRs0Mf(BYTE op0,BYTE op1)
{
	dasmReg[op0]=(sDWORD)dasmCPR0[op1];
}

void interpOpRs0DMf(BYTE op0,BYTE op1)
{
	dasmReg[op0]=dasmCPR0[op1];
}

void interpOpRs0Mt(BYTE op0,BYTE op1)
{
	dasmCPR0[op1]=(sDWORD)dasmReg[op0];
}

void interpOpRs0DMt(BYTE op0,BYTE op1)
{
	dasmCPR0[op1]=dasmReg[op0];
}

void interpOpRs1Mf(BYTE op0,BYTE op1)
{
	dasmReg[op0]=(sDWORD)dasmCPR1[op1];
}

void interpOpRs1DMf(BYTE op0,BYTE op1)
{
	dasmReg[op0]=dasmCPR1[op1];
}

void interpOpRs1Mt(BYTE op0,BYTE op1)
{
	dasmCPR1[op1]=(sDWORD)dasmReg[op0];
}

void interpOpRs1DMt(BYTE op0,BYTE op1)
{
	dasmCPR1[op1]=dasmReg[op0];
}

void interpOpRs0Ct(BYTE op0,BYTE op1)
{
	dasmCPC0[op1]=(sDWORD)dasmReg[op0];
}

void interpOpRs1Ct(BYTE op0,BYTE op1)
{
	dasmCPC1[op1]=(sDWORD)dasmReg[op0];
}

void interpOpRs0DCt(BYTE op0,BYTE op1)
{
	dasmReg[op0]=dasmCPC0[op1];
}

void interpOpRs1DCt(BYTE op0,BYTE op1)
{
	dasmReg[op0]=dasmCPC1[op1];
}

void interpOpAndI(BYTE op0,BYTE op1,DWORD Imm)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]&(DWORD)Imm);
}

void interpOpOrI(BYTE op0,BYTE op1,DWORD Imm)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]|(DWORD)Imm);
}

void interpOpXorI(BYTE op0,BYTE op1,DWORD Imm)
{
	dasmReg[op0]=((sDWORD)dasmReg[op1]^(DWORD)Imm);
}

void interpOpAndIEqual(BYTE op0,DWORD Imm)
{
	dasmReg[op0]&=(DWORD)Imm;
}

void interpOpOrIEqual(BYTE op0,DWORD Imm)
{
	dasmReg[op0]|=(DWORD)Imm;
}

void interpOpXorIEqual(BYTE op0,DWORD Imm)
{
	dasmReg[op0]^=(DWORD)Imm;
}



