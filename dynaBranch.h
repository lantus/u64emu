#ifndef DYNA_BRANCH
#define DYNA_BRANCH

extern void StopMe();
extern WORD dynaOpILoop(BYTE *cp,DWORD NewPC);
extern WORD dynaOpEret(BYTE *cp);
extern WORD dynaOpJ(BYTE *cp,DWORD NewPC);
extern WORD dynaOpJr(BYTE *cp,BYTE op0);
extern WORD dynaOpJalr(BYTE *cp,BYTE op0,BYTE op1);
extern WORD dynaOpJal(BYTE *cp,DWORD NewPC);
extern WORD dynaOpBeq(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC);
extern WORD dynaOpBeqILoop(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC);
extern WORD dynaOpBne(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC);
extern WORD dynaOpBneILoop(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC);
extern WORD dynaOpBlez(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgtz(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBltz(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgez(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBeql(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC);
extern WORD dynaOpBnel(BYTE *cp,BYTE op0,BYTE op1,DWORD NewPC);
extern WORD dynaOpBlezl(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgtzl(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBltzl(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgezl(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBltzal(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgezal(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBltzall(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBgezall(BYTE *cp,BYTE op0,DWORD NewPC);
extern WORD dynaOpBct(BYTE *cp,DWORD NewPC);
extern WORD dynaOpBctl(BYTE *cp,DWORD NewPC);
extern WORD dynaOpBcf(BYTE *cp,DWORD NewPC);
extern WORD dynaOpBcfl(BYTE *cp,DWORD NewPC);

#endif
