void interpOpLui(BYTE op0,DWORD imm)
{
	dasmReg[op0]=(sDWORD)(imm<<16);
}
