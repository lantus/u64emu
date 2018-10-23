#ifndef DYNA_BRANCH_SP
#define DYNA_BRANCH_SP


extern WORD dynaOpBeqSamePage(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC);
extern WORD dynaOpBneSamePage(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC);
extern WORD dynaOpBlezSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgtzSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBltzSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgezSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBeqlSamePage(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC);
extern WORD dynaOpBnelSamePage(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC);
extern WORD dynaOpBlezlSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgtzlSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBltzlSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgezlSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBltzalSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgezalSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBltzallSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgezallSamePage(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBctSamePage(BYTE *cp,DWORD NewPC);
extern WORD dynaOpBctlSamePage(BYTE *cp,DWORD NewPC);
extern WORD dynaOpBcfSamePage(BYTE *cp,DWORD NewPC);
extern WORD dynaOpBcflSamePage(BYTE *cp,DWORD NewPC);
extern WORD dynaOpBgei(BYTE *cp,BYTE op0,BYTE op1,DWORD imm,DWORD NewPC);
extern WORD dynaOpBgeiu(BYTE *cp,BYTE op0,BYTE op1,DWORD imm,DWORD NewPC);
extern WORD dynaOpBli(BYTE *cp,BYTE op0,BYTE op1,DWORD imm,DWORD NewPC);
extern WORD dynaOpBliu(BYTE *cp,BYTE op0,BYTE op1,DWORD imm,DWORD NewPC);
extern WORD dynaOpBge(BYTE *cp,BYTE op0,BYTE op1,BYTE op2,DWORD NewPC);
extern WORD dynaOpBgeu(BYTE *cp,BYTE op0,BYTE op1,BYTE op2,DWORD NewPC);
extern WORD dynaOpBl(BYTE *cp,BYTE op0,BYTE op1,BYTE op2,DWORD NewPC);
extern WORD dynaOpBlu(BYTE *cp,BYTE op0,BYTE op1,BYTE op2,DWORD NewPC);


#endif
