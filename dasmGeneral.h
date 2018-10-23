
WORD dasmOpSra(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpSra(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGenericShift(cp,"sra",com1,com2,op0,op1,op2));
}

WORD dasmOpSrl(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpSrl(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGenericShift(cp,"srl",com1,com2,op0,op1,op2));
}

WORD dasmOpSll(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpSll(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGenericShift(cp,"sll",com1,com2,op0,op1,op2));
}

WORD dasmOpSraV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpSrav(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"srav",com1,com2,op0,op1,op2));
}

WORD dasmOpSrlV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpSrlv(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"srlv",com1,com2,op0,op1,op2));
}

WORD dasmOpSllV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpSllv(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"sllv",com1,com2,op0,op1,op2));
}

WORD dasmOpRegMove(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpRegMove(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric2(cp,"mov",com1,com2,op0,op1));
}

WORD dasmOpRegDMove(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpRegMoveD(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric2(cp,"movd",com1,com2,op0,op1));
}

WORD dasmOpAddEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpAddEqual(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric2(cp,"add eq",com1,com2,op0,op1));
}


WORD dasmOpAdd(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpAdd(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"add",com1,com2,op0,op1,op2));
}


WORD dasmOpSubEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpSubEqual(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric2(cp,"sub eq",com1,com2,op0,op1));
}


WORD dasmOpSub(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpSub(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"sub",com1,com2,op0,op1,op2));
}


WORD dasmOpAndEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpAndEqual(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric2(cp,"and eq",com1,com2,op0,op1));
}


WORD dasmOpAnd(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpAnd(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"and",com1,com2,op0,op1,op2));
}

WORD dasmOpOrEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpOrEqual(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric2(cp,"or eq",com1,com2,op0,op1));
}


WORD dasmOpOr(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpOr(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"or",com1,com2,op0,op1,op2));
}

WORD dasmOpNorEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpNorEqual(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric2(cp,"nor eq",com1,com2,op0,op1));
}


WORD dasmOpNor(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpNor(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"nor",com1,com2,op0,op1,op2));
}

WORD dasmOpXorEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpXorEqual(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric2(cp,"xor eq",com1,com2,op0,op1));
}


WORD dasmOpXor(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpXor(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"xor",com1,com2,op0,op1,op2));
}


WORD dasmOpMfHi(BYTE *cp,BYTE op0)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric1(cp,"mfhi",com1,com2,op0));
}

WORD dasmOpMtHi(BYTE *cp,BYTE op0)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric1(cp,"mthi",com1,com2,op0));
}


WORD dasmOpMfLo(BYTE *cp,BYTE op0)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric1(cp,"mflo",com1,com2,op0));
}


WORD dasmOpMtLo(BYTE *cp,BYTE op0)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric1(cp,"mtlo",com1,com2,op0));
}

WORD dasmOpMult(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"mult",com1,com2,op0,op1));
}

WORD dasmOpMultU(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"multu",com1,com2,op0,op1));
}


WORD dasmOpDiv(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"div",com1,com2,op0,op1));
}

WORD dasmOpDivU(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"divu",com1,com2,op0,op1));
}

WORD dasmOpSlt(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric3(cp,"slt",com1,com2,op0,op1,op2));
}

WORD dasmOpSltU(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric3(cp,"slt",com1,com2,op0,op1,op2));
}

WORD dasmOpSltI(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericImm(cp,"slti",com1,com2,op0,op1,Imm));
}

WORD dasmOpSltIU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericImm(cp,"sltiu",com1,com2,op0,op1,Imm));
}


WORD dasmOpAndIEqual(BYTE *cp,BYTE op0,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpAndIEqual(op0,Imm);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric1Imm(cp,"andi eq",com1,com2,op0,Imm));
}

WORD dasmOpAndI(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpAndI(op0,op1,Imm);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGenericImm(cp,"andi",com1,com2,op0,op1,Imm));
}

WORD dasmOpOrIEqual(BYTE *cp,BYTE op0,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpOrIEqual(op0,Imm);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric1Imm(cp,"ori eq",com1,com2,op0,Imm));
}

WORD dasmOpOrI(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpOrI(op0,op1,Imm);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGenericImm(cp,"ori",com1,com2,op0,op1,Imm));
}

WORD dasmOpXorIEqual(BYTE *cp,BYTE op0,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpXorIEqual(op0,Imm);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric1Imm(cp,"xori eq",com1,com2,op0,Imm));
}

WORD dasmOpXorI(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpXorI(op0,op1,Imm);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGenericImm(cp,"xori",com1,com2,op0,op1,Imm));
}

WORD dasmOpDAddEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpDAddEqual(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric2(cp,"dadd eq",com1,com2,op0,op1));
}


WORD dasmOpDAdd(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpDAdd(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"dadd",com1,com2,op0,op1,op2));
}


WORD dasmOpDSubEqual(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpDSubEqual(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric2(cp,"dsub eq",com1,com2,op0,op1));
}


WORD dasmOpDSub(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpDSub(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"dsub",com1,com2,op0,op1,op2));
}

WORD dasmOpDMult(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"dmult",com1,com2,op0,op1));
}

WORD dasmOpDMultU(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"dmultu",com1,com2,op0,op1));
}


WORD dasmOpDDiv(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"ddiv",com1,com2,op0,op1));
}

WORD dasmOpDDivU(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"ddivu",com1,com2,op0,op1));
}

WORD dasmOpAddI(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpAddI(op0,op1,Imm);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGenericImm(cp,"addi",com1,com2,op0,op1,Imm));
}


WORD dasmOpAddIEqual(BYTE *cp,BYTE op0,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpAddIEqual(op0,Imm);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric1Imm(cp,"addi eq",com1,com2,op0,Imm));
}


WORD dasmOpLoadI(BYTE *cp,BYTE op0,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmIsValid[op0]=true;
	interpOpLoadI(op0,Imm);
	sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	return(dasmOpGeneric1Imm(cp,"movi",com1,com2,op0,Imm));
}


WORD dasmOpDAddIEqual(BYTE *cp,BYTE op0,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpDAddIEqual(op0,Imm);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric1Imm(cp,"daddi eq",com1,com2,op0,Imm));
}


WORD dasmOpDAddI(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpDAddI(op0,op1,Imm);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGenericImm(cp,"daddi",com1,com2,op0,op1,Imm));
}


WORD dasmOpDSllV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&&dasmIsValid[op2])
	{
		dasmIsValid[op0]=true;
		interpOpDSllV(op0,op1,op2);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	return(dasmOpGeneric3(cp,"dsllv",com1,com2,op0,op1,op2));
}

WORD dasmOpDSll(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericImm(cp,"dsll",com1,com2,op0,op1,Imm));
}

WORD dasmOpDSll32(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericImm(cp,"dsll32",com1,com2,op0,op1,Imm+32));
}


WORD dasmOpDSrlV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric3(cp,"dsrlv",com1,com2,op0,op1,op2));
}

WORD dasmOpDSrl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericImm(cp,"dsrl",com1,com2,op0,op1,Imm));
}

WORD dasmOpDSrl32(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericImm(cp,"dsrl32",com1,com2,op0,op1,Imm+32));
}

WORD dasmOpDSraV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric3(cp,"dsrav",com1,com2,op0,op1,op2));
}

WORD dasmOpDSra(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericImm(cp,"dsra",com1,com2,op0,op1,Imm));
}

WORD dasmOpDSra32(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericImm(cp,"dsra32",com1,com2,op0,op1,Imm+32));
}

WORD dasmOpRs0Mf(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpRs0Mf(op0,op1);
		strcpy(com1,dasmCRegName[dasmReg[op1]&31]);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	
	return(dasmOpGeneric2(cp,"mf0",com1,com2,op0,op1));
}

WORD dasmOpRs0DMf(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmIsValid[op0]=true;
		interpOpRs0DMf(op0,op1);
		strcpy(com1,dasmCRegName[dasmReg[op1]&31]);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	
	return(dasmOpGeneric2(cp,"dmf0",com1,com2,op0,op1));
}


WORD dasmOpRs0Mt(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op0])
	{
		strcpy(com1,dasmCRegName[dasmReg[op0]&31]);
	}
	if(dasmIsValid[op1]&&dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpRs0Mt(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	
	return(dasmOpGeneric2(cp,"mt0",com1,com2,op0,op1));
}

WORD dasmOpRs0DMt(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op0])
	{
		strcpy(com1,dasmCRegName[dasmReg[op0]&31]);
	}
	if(dasmIsValid[op1]&&dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpRs0DMt(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	
	return(dasmOpGeneric2(cp,"dmt0",com1,com2,op0,op1));
}

WORD dasmOpRs1Mf(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&&dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpRs1Mf(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	
	return(dasmOpGeneric2(cp,"mf1",com1,com2,op0,op1));
}

WORD dasmOpRs1DMf(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&&dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpRs1DMf(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	
	return(dasmOpGeneric2(cp,"dmf1",com1,com2,op0,op1));
}

WORD dasmOpRs1Mt(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&&dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpRs1Mt(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	
	return(dasmOpGeneric2(cp,"mt1",com1,com2,op0,op1));
}

WORD dasmOpRs1DMt(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&&dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpRs1DMt(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	
	return(dasmOpGeneric2(cp,"dmt1",com1,com2,op0,op1));
}

WORD dasmOpRs1Ct(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"ct1",com1,com2,op0,op1));
}


WORD dasmOpRs0Ct(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"ct0",com1,com2,op0,op1));
}


WORD dasmOpRs1Cf(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"cf1",com1,com2,op0,op1));
}


WORD dasmOpRs0Cf(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"cf0",com1,com2,op0,op1));
}


WORD dasmOpRs2Mf(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&&dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpRs1Mf(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	
	return(dasmOpGeneric2(cp,"mf2",com1,com2,op0,op1));
}


WORD dasmOpRs2Mt(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1]&&dasmIsValid[op0])
	{
		dasmIsValid[op0]=true;
		interpOpRs1Mt(op0,op1);
		sprintf(com2,"%s=%X:%X",dasmRegName[op0],(DWORD)(dasmReg[op0]>>32),(DWORD)dasmReg[op0]&0xffffffff);
	}
	
	return(dasmOpGeneric2(cp,"mt2",com1,com2,op0,op1));
}

WORD dasmOpRs2Ct(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"ct2",com1,com2,op0,op1));
}


WORD dasmOpRs2Cf(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGeneric2(cp,"cf2",com1,com2,op0,op1));
}
