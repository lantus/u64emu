
WORD dynaOpSwSP(BYTE *cp,BYTE op0,DWORD Imm)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG(cp+l,29,PC_PTR);
	l+=LOAD_REG_IMM(cp+l,MEM_PTR,(DWORD)&r->GPR[op0*2]);
//	l+=AND_REG_IMM(cp+l,PC_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,PC_PTR,(DWORD) dynaRamPtr+Imm-0x88000000);
	l+=MOVSD(cp+l);
	return(l);
}

WORD dynaOpSdSP(BYTE *cp,BYTE op0,DWORD Imm)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG(cp+l,29,PC_PTR);
	l+=LOAD_REG_IMM(cp+l,MEM_PTR,(DWORD)&r->GPR[op0*2]);
//	l+=AND_REG_IMM(cp+l,PC_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,PC_PTR,(DWORD) dynaRamPtr+Imm-0x88000000);
	l+=MOVSD(cp+l);
	l+=MOVSD(cp+l);
	return(l);
}

WORD dynaOpLwSP(BYTE *cp,BYTE op0,DWORD Imm)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG(cp+l,29,MEM_PTR);
	l+=LOAD_REG_IMM(cp+l,PC_PTR,(DWORD)&r->GPR[op0*2]);
//	l+=AND_REG_IMM(cp+l,MEM_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,Imm+(DWORD)dynaRamPtr-0x88000000);
	l+=MOVSD(cp+l);
	return(l);
}

WORD dynaOpLdSP(BYTE *cp,BYTE op0,DWORD Imm)
{
	WORD l=0;

	l+=INC_PC_COUNTER(cp+l);
	l+=LOAD_REG(cp+l,29,MEM_PTR);
	l+=LOAD_REG_IMM(cp+l,PC_PTR,(DWORD)&r->GPR[op0*2]);
//	l+=AND_REG_IMM(cp+l,MEM_PTR,MEM_MASK);
	l+=ADD_REG_IMM(cp+l,MEM_PTR,Imm+(DWORD)dynaRamPtr-0x88000000);
	l+=MOVSD(cp+l);
	l+=MOVSD(cp+l);
	return(l);
}

