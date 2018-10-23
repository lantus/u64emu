
WORD dasmOpILoop(BYTE *cp,DWORD NewPC)
{
	return(sprintf((char *)cp,"I-Loop %X",NewPC));
}

WORD dasmOpEret(BYTE *cp)
{
	return(sprintf((char *)cp,"eret"));
}


WORD dasmOpJ(BYTE *cp,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericBranchI(cp,"jmp ",com1,com2,NewPC));
}


WORD dasmOpJr(BYTE *cp,BYTE op0)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");
	DWORD NewPC=0;

	if(dasmIsValid[op0])
	{
		dasmLookupFunction(com1,NewPC);
	}
	return(dasmOpGenericBranch(cp,"jr ",com1,com2,op0));
}

WORD dasmOpJalr(BYTE *cp,BYTE op0,BYTE op1)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");
	DWORD NewPC=0;

	if(dasmIsValid[op0])
	{
		dasmLookupFunction(com1,NewPC);
	}
	dasmReg[op1]=dasmPC+8;
	dasmIsValid[op1]=true;
	return(dasmOpGenericBranch2(cp,"jalr ",com1,com2,op0,op1));
}


WORD dasmOpJal(BYTE *cp,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	dasmReg[31]=dasmPC+8;
	dasmIsValid[31]=true;
	return(dasmOpGenericBranchI(cp,"jal ",com1,com2,NewPC));
}

WORD dasmOpBeq(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompare(cp,"beq ",com1,com2,op0,op1,NewPC));
}


WORD dasmOpBeqILoop(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompare(cp,"beq iloop",com1,com2,op0,op1,NewPC));
}

WORD dasmOpBne(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompare(cp,"bne ",com1,com2,op0,op1,NewPC));
}


WORD dasmOpBneILoop(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompare(cp,"bne ",com1,com2,op0,op1,NewPC));
}

WORD dasmOpBlez(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompareZero(cp,"blez ",com1,com2,op0,NewPC));
}


WORD dasmOpBgtz(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompareZero(cp,"bgtz ",com1,com2,op0,NewPC));
}


WORD dasmOpBltz(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompareZero(cp,"bltz ",com1,com2,op0,NewPC));
}


WORD dasmOpBgez(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompareZero(cp,"bgez ",com1,com2,op0,NewPC));
}


WORD dasmOpBeql(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompare(cp,"beql ",com1,com2,op0,op1,NewPC));
}


WORD dasmOpBnel(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompare(cp,"bnel ",com1,com2,op0,op1,NewPC));
}

WORD dasmOpBlezl(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompareZero(cp,"blezl ",com1,com2,op0,NewPC));
}


WORD dasmOpBgtzl(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompareZero(cp,"bgtzl ",com1,com2,op0,NewPC));
}


WORD dasmOpBltzl(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompareZero(cp,"bltzl ",com1,com2,op0,NewPC));
}


WORD dasmOpBgezl(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericCompareZero(cp,"bgezl ",com1,com2,op0,NewPC));
}

WORD dasmOpBltzal(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	dasmReg[31]=dasmPC+8;
	return(dasmOpGenericCompareZero(cp,"bltzal ",com1,com2,op0,NewPC));
}


WORD dasmOpBgezal(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	dasmReg[31]=dasmPC+8;
	return(dasmOpGenericCompareZero(cp,"bgezal ",com1,com2,op0,NewPC));
}


WORD dasmOpBltzall(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	dasmReg[31]=dasmPC+8;
	return(dasmOpGenericCompareZero(cp,"bltzall ",com1,com2,op0,NewPC));
}


WORD dasmOpBgezall(BYTE *cp,BYTE op0,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	dasmReg[31]=dasmPC+8;
	return(dasmOpGenericCompareZero(cp,"bgezall ",com1,com2,op0,NewPC));
}


WORD dasmOpBct(BYTE *cp,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericBranchI(cp,"bct ",com1,com2,NewPC));
}


WORD dasmOpBctl(BYTE *cp,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericBranchI(cp,"bctl ",com1,com2,NewPC));
}


WORD dasmOpBcf(BYTE *cp,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericBranchI(cp,"bcf ",com1,com2,NewPC));
}


WORD dasmOpBcfl(BYTE *cp,DWORD NewPC)
{
	WORD l=0;
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmLookupFunction(com1,NewPC);
	return(dasmOpGenericBranchI(cp,"bcfl ",com1,com2,NewPC));
}

