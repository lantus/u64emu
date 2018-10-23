
WORD dasmOpVmulf(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmulf",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}

WORD dasmOpVmulu(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmulu",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVrndp(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vrndp",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmulq(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmulq",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmudl(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmudl",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmudm(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmudm",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmudn(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmudn",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmudh(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmudh",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmacf(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmacf",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmacu(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmacu",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVrndn(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vrndn",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmacq(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmacq",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmadl(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmadl",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmadm(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmadm",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmadn(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmadn",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmadh(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmadh",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVadd(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vadd",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVsub(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vsub",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVsut(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vsut",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVabs(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vabs",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVaddc(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vaddc",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVsubc(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vsubc",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVaddb(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vaddb",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVsubb(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vsubb",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVaccb(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vaccb",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVsucb(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vsucb",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVsad(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vsad",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVsac(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vsac",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVsum(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vsum",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVsaw(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vsaw",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVlt(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vltp",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVeq(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"veqp",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVne(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vnep",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVge(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vgep",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVcl(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vclp",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVch(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vchp",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVcr(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vcrp",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmrg(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmrg",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVand(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vand",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVnand(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vnand",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVor(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vorp",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVnor(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vnor",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVxor(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vxor",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVnxor(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vnxor",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVrcp(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vrcp",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVrcpl(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vrcpl",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVrcph(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vrcph",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVmov(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vmov",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVrsq(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vrsq",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVrsql(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vrsql",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVrsqh(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vrsqh",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVnoop(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vnoop",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVextt(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vextt",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVextq(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vextq",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVextn(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vextn",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVinst(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vinst",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVinsq(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vinsq",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}


WORD dasmOpVinsn(BYTE *cp)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	return(dasmOpGenericVector(cp,"vinsn",com1,com2,MAKE_VD,MAKE_VS1,MAKE_VS2,MAKE_VSEL));
}

WORD dasmOpVectorSwc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	char op[128];
	short mult=0;
	switch(MAKE_RD)
	{
	case 0:
		{
			mult=1;
			strcpy(op,"sbv ");
			break;
		}
	case 1:
		{
			mult=2;
			strcpy(op,"ssv ");
			break;
		}
	case 2:
		{
			mult=4;
			strcpy(op,"slv ");
			break;
		}
	case 3:
		{
			mult=8;
			strcpy(op,"sdv ");
			break;
		}
	case 4:
		{
			mult=16;
			strcpy(op,"sqv ");
			break;
		}
	case 5:
		{
			mult=16;
			strcpy(op,"srv ");
			break;
		}
	case 6:
		{
			mult=8;
			strcpy(op,"spv ");
			break;
		}
	case 7:
		{
			mult=8;
			strcpy(op,"suv ");
			break;
		}
	case 8:
		{
			mult=16;
			strcpy(op,"shv ");
			break;
		}
	case 9:
		{
			mult=16;
			strcpy(op,"sfv ");
			break;
		}
	case 0xa:
		{
			mult=16;
			strcpy(op,"swv ");
			break;
		}
	case 0xb:
		{
			mult=16;
			strcpy(op,"stv ");
			break;
		}
	}
	char offset=dasmOpCode&0x7f;
	offset<<=1;
	offset>>=1;
	short index=offset*mult;
	return(dasmOpVectorMemory(cp,op,com1,com2,op0,op1,index,MAKE_VSEL));
}

WORD dasmOpVectorLwc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm)
{
	char com1[64],com2[64];
	strcpy(com1,"");
	strcpy(com2,"");

	if(dasmIsValid[op1])
	{
		dasmLookupIO(com1,((sDWORD)dasmReg[op1]+(sDWORD)Imm));
		sprintf(com2,"*(%X)",((sDWORD)dasmReg[op1]+(sDWORD)Imm));
	}
	char op[128];
	short mult=0;
	switch(MAKE_RD)
	{
	case 0:
		{
			mult=1;
			strcpy(op,"lbv ");
			break;
		}
	case 1:
		{
			mult=2;
			strcpy(op,"lsv ");
			break;
		}
	case 2:
		{
			mult=4;
			strcpy(op,"llv ");
			break;
		}
	case 3:
		{
			mult=8;
			strcpy(op,"ldv ");
			break;
		}
	case 4:
		{
			mult=16;
			strcpy(op,"lqv ");
			break;
		}
	case 5:
		{
			mult=16;
			strcpy(op,"lrv ");
			break;
		}
	case 6:
		{
			mult=8;
			strcpy(op,"lpv ");
			break;
		}
	case 7:
		{
			mult=8;
			strcpy(op,"luv ");
			break;
		}
	case 8:
		{
			mult=16;
			strcpy(op,"lhv ");
			break;
		}
	case 9:
		{
			mult=16;
			strcpy(op,"lfv ");
			break;
		}
	case 0xa:
		{
			mult=16;
			strcpy(op,"lwv ");
			break;
		}
	case 0xb:
		{
			mult=16;
			strcpy(op,"ltv ");
			break;
		}
	}
	char offset=dasmOpCode&0x7f;
	offset<<=1;
	offset>>=1;
	short index=offset*mult;
	return(dasmOpVectorMemory(cp,op,com1,com2,op0,op1,index,MAKE_VSEL));
}
