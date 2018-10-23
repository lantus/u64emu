WORD dasmOpLui(BYTE *cp,BYTE op0,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	dasmIsValid[op0]=true;
	interpOpLui(op0,Imm);
	sprintf(com2,"%s=%X",dasmRegName[op0],(DWORD)dasmReg[op0]&0xffffffff);
	return(dasmOpGeneric1Imm(cp,"lui ",com1,com2,op0,Imm));
}

WORD dasmOpLb(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"lb ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLbu(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"lbu ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLh(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"lh ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLhu(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"lhu ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLw(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"lw ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLwu(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"lwu ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLd(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"ld ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLwl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"lwl ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLwr(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"lwr ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLdl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"ldl ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLdr(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"ldr ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"ll ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLld(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"lld ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLwc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"lwc1 ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLwc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"lwc2 ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLdc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"ldc1 ",com1,com2,op0,op1,Imm));
}

WORD dasmOpLdc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"ldc2 ",com1,com2,op0,op1,Imm));
}

// store functions


WORD dasmOpSb(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"sb ",com1,com2,op0,op1,Imm));
}

WORD dasmOpSh(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"sh ",com1,com2,op0,op1,Imm));
}

WORD dasmOpSw(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"sw ",com1,com2,op0,op1,Imm));
}

WORD dasmOpSd(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"sd ",com1,com2,op0,op1,Imm));
}

WORD dasmOpSwl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"swl ",com1,com2,op0,op1,Imm));
}

WORD dasmOpSwr(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"swr ",com1,com2,op0,op1,Imm));
}

WORD dasmOpSdl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"sdl ",com1,com2,op0,op1,Imm));
}

WORD dasmOpSdr(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"sdr ",com1,com2,op0,op1,Imm));
}


WORD dasmOpSwc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"swc1 ",com1,com2,op0,op1,Imm));
}

WORD dasmOpSwc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"swc2 ",com1,com2,op0,op1,Imm));
}

WORD dasmOpSdc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"sdc1 ",com1,com2,op0,op1,Imm));
}

WORD dasmOpSdc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"sdc2 ",com1,com2,op0,op1,Imm));
}

WORD dasmOpSc(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"sc ",com1,com2,op0,op1,Imm));
}

WORD dasmOpScd(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	return(dasmOpGenericMemory(cp,"scd ",com1,com2,op0,op1,Imm));
}

WORD dasmOpTlbR(BYTE *cp)
{
	return(sprintf((char *)cp,"tlbr"));
}

WORD dasmOpTlbWi(BYTE *cp)
{
	return(sprintf((char *)cp,"tlbwi"));
}

WORD dasmOpTlbWr(BYTE *cp)
{
	return(sprintf((char *)cp,"tlbwr"));
}

WORD dasmOpTlbP(BYTE *cp)
{
	return(sprintf((char *)cp,"tlbp"));
}