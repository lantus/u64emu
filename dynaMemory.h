#ifndef DYNA_MEMORY
#define DYNA_MEMORY


extern WORD dynaOpSw(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLw(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpSwSP(BYTE *cp,BYTE op0,DWORD Imm);
extern WORD dynaOpLwSP(BYTE *cp,BYTE op0,DWORD Imm);
extern WORD dynaOpSdSP(BYTE *cp,BYTE op0,DWORD Imm);
extern WORD dynaOpLdSP(BYTE *cp,BYTE op0,DWORD Imm);
extern WORD dynaOpLwU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLui(BYTE *cp,BYTE op0,DWORD Imm);
extern WORD dynaOpLh(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLb(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLhU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLbU(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLd(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpSh(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpSb(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpSd(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLdl(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpLdr(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpSdl(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpSdr(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpLwl(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpLwr(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpSwr(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpSwl(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpLl(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLld(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLwc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLwc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLdc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpLdc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpSwc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpSwc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpSdc1(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpSdc2(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpSc(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);
extern WORD dynaOpScd(BYTE *cp,BYTE op0,BYTE op1,DWORD Imm);

#endif