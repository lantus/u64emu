#ifndef DYNA_GENERAL
#define DYNA_GENERAL

extern WORD dynaOpSraEq(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpSrlEq(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpSllEq(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpSra(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpSrl(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpSll(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpSraV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpSrlV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpSllV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpMfHi(BYTE *cp,BYTE op0);
extern WORD dynaOpRegMove(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRegDMove(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpAddEqual(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpAdd(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpSubEqual(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpSub(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpMtHi(BYTE *cp,BYTE op0);
extern WORD dynaOpMfLo(BYTE *cp,BYTE op0);
extern WORD dynaOpMtLo(BYTE *cp,BYTE op0);
extern WORD dynaOpMult(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpMultU(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpDiv(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpDivU(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpSlt(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpSltU(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpSltI(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpSltIU(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpDAddEqual(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpDAdd(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpDSubEqual(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpDSub(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern void dynaHelpDMult(DWORD op0,DWORD op1);
extern void dynaHelpDMultU(DWORD op0,DWORD op1);
extern WORD dynaOpDMult(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpDMultU(BYTE *cp,BYTE op0,BYTE op1);
extern void dynaHelpDDiv(DWORD op0,DWORD op1);
extern void dynaHelpDDivU(DWORD op0,DWORD op1);
extern WORD dynaOpDDiv(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpDDivU(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpAddI(BYTE *cp,BYTE rt,BYTE rs,sDWORD Immediate);
extern WORD dynaOpAddIEqual(BYTE *cp,BYTE op0,DWORD Immediate);
extern WORD dynaOpLoadI(BYTE *cp,BYTE op0,DWORD Immediate);
extern WORD dynaOpDAddIEqual(BYTE *cp,BYTE rt,DWORD Immediate);
extern WORD dynaOpDAddI(BYTE *cp,BYTE rt,BYTE rs,DWORD Immediate);
extern WORD dynaOpDSllV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpDSll(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpDSll32(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpDSrlV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpDSrl(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpDSrl32(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpDSraV(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpDSra(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpDSra32(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpRs0Mf(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRs0DMf(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRs0Mt(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRs0DMt(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRs1Mf(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRs1DMf(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRs1Mt(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRs1DMt(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRs1Ct(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRs0Ct(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRs1Cf(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpRs0Cf(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpAndEqual(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpAnd(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpOrEqual(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpOr(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpNorEqual(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpNor(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpXorEqual(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpXor(BYTE *cp,BYTE op0,BYTE op1,BYTE op2);
extern WORD dynaOpAndI(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpOrIEqual(BYTE *cp,BYTE op0,DWORD Immediate);
extern WORD dynaOpOrI(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpXorIEqual(BYTE *cp,BYTE op0,DWORD Immediate);
extern WORD dynaOpXorI(BYTE *cp,BYTE op0,BYTE op1,DWORD Immediate);
extern WORD dynaOpAndIEqual(BYTE *cp,BYTE op0,DWORD Immediate);

#endif