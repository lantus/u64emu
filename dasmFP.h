
WORD dasmOpFPAddS(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP3(cp,"fadds",com1,com2,op0,op1,op2));
}

WORD dasmOpFPAddD(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP3(cp,"faddd",com1,com2,op0,op1,op2));
}

WORD dasmOpFPSubS(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP3(cp,"fsubs",com1,com2,op0,op1,op2));
}

WORD dasmOpFPSubD(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP3(cp,"fsubd",com1,com2,op0,op1,op2));
}

WORD dasmOpFPMulS(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP3(cp,"fmuls",com1,com2,op0,op1,op2));
}

WORD dasmOpFPMulD(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP3(cp,"fmuld",com1,com2,op0,op1,op2));
}

WORD dasmOpFPDivS(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP3(cp,"fdivs",com1,com2,op0,op1,op2));
}

WORD dasmOpFPDivD(BYTE *cp,BYTE op0,BYTE op1,BYTE op2)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP3(cp,"fdivd",com1,com2,op0,op1,op2));
}


WORD dasmOpFPSqrtS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fsqrts",com1,com2,op0,op1));
}

WORD dasmOpFPSqrtD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fsqrtd",com1,com2,op0,op1));
}

WORD dasmOpFPAbsS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fabss",com1,com2,op0,op1));
}

WORD dasmOpFPAbsD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fabsd",com1,com2,op0,op1));
}

WORD dasmOpFPNegS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fnegs",com1,com2,op0,op1));
}

WORD dasmOpFPNegD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fnegd",com1,com2,op0,op1));
}

WORD dasmOpFPCvtSFromD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fvctsd",com1,com2,op0,op1));
}

WORD dasmOpFPCvtSFromW(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fcvtsw",com1,com2,op0,op1));
}


WORD dasmOpFPCvtSFromL(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fcvtsl",com1,com2,op0,op1));
}


WORD dasmOpFPCvtDFromS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fadds",com1,com2,op0,op1));
}

WORD dasmOpFPCvtDFromW(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fcvtdw",com1,com2,op0,op1));
}


WORD dasmOpFPCvtDFromL(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fcvtdl",com1,com2,op0,op1));
}



WORD dasmOpFPCvtWFromS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fcvtws",com1,com2,op0,op1));
}

WORD dasmOpFPCvtWFromD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fcvtwd",com1,com2,op0,op1));
}


WORD dasmOpFPCvtWFromL(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fcvtwl",com1,com2,op0,op1));
}


WORD dasmOpFPCvtLFromS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fcvtlw",com1,com2,op0,op1));
}

WORD dasmOpFPCvtLFromD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fcvtld",com1,com2,op0,op1));
}


WORD dasmOpFPCvtLFromW(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fctlw",com1,com2,op0,op1));
}


WORD dasmOpFPRoundLS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"froundls",com1,com2,op0,op1));
}


WORD dasmOpFPRoundLD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"froundld",com1,com2,op0,op1));
}


WORD dasmOpFPRoundWS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"froundws",com1,com2,op0,op1));
}


WORD dasmOpFPRoundWD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"froundwd",com1,com2,op0,op1));
}


WORD dasmOpFPTruncWS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"ftruncws",com1,com2,op0,op1));
}

WORD dasmOpFPTruncWD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"ftruncwd",com1,com2,op0,op1));
}

WORD dasmOpFPTruncLS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"ftruncls",com1,com2,op0,op1));
}

WORD dasmOpFPTruncLD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"ftruncld",com1,com2,op0,op1));
}

WORD dasmOpFPCeilLS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fceills",com1,com2,op0,op1));
}

WORD dasmOpFPCeilLD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fceild",com1,com2,op0,op1));
}

WORD dasmOpFPCeilWS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fceilws",com1,com2,op0,op1));
}

WORD dasmOpFPCeilWD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fceilwd",com1,com2,op0,op1));
}

WORD dasmOpFPFloorWD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"ffloorwd",com1,com2,op0,op1));
}

WORD dasmOpFPFloorWS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"ffloorws",com1,com2,op0,op1));
}

WORD dasmOpFPFloorLD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"ffloorld",com1,com2,op0,op1));
}

WORD dasmOpFPFloorLS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"ffloorls",com1,com2,op0,op1));
}

WORD dasmOpFPMovS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fmovs",com1,com2,op0,op1));
}

WORD dasmOpFPMovD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fmovd",com1,com2,op0,op1));
}

WORD dasmOpFPEqualS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"feqs",com1,com2,op0,op1));
}

WORD dasmOpFPLessThanS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"flts",com1,com2,op0,op1));
}

WORD dasmOpFPLessThanEqualS(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fltes",com1,com2,op0,op1));
}

WORD dasmOpFPEqualD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"feqd",com1,com2,op0,op1));
}

WORD dasmOpFPLessThanD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fltd",com1,com2,op0,op1));
}

WORD dasmOpFPLessThanEqualD(BYTE *cp,BYTE op0,BYTE op1)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"flted",com1,com2,op0,op1));
}

WORD dasmOpFPSetFalse(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericFP2(cp,"fsf",com1,com2,0,0));
}
