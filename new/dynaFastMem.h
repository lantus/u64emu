#ifndef DYNA_FAST_MEM
#define DYNA_FAST_MEM

extern int dynaCompileBuilderSB(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSH(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSW(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSD(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLD(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLW(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLWU(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLB(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLBU(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLH(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLHU(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLWC1(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLDC1(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLWC2(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLDC2(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLL(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLLD(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLDL(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLWL(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLDR(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderLWR(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);

extern int dynaCompileBuilderSWC1(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSDC1(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSWC2(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSDC2(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSC(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSCD(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSDL(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSWL(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSDR(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);
extern int dynaCompileBuilderSWR(BYTE *codeptr,BYTE op0,BYTE op1,DWORD Imm);

extern void dynaRuntimeBuilderSW(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSB(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSH(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSD(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLW(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLWU(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLD(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLB(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLBU(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLH(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLHU(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLWC1(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLL(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLLD(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLDC1(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaHelperLDL(DWORD address,DWORD op0);
extern void dynaHelperLWL(DWORD address,DWORD op0);
extern void dynaHelperLDR(DWORD address,DWORD op0);
extern void dynaHelperLWR(DWORD address,DWORD op0);

extern void dynaHelperSDL(DWORD address,DWORD op0);
extern void dynaHelperSWL(DWORD address,DWORD op0);
extern void dynaHelperSDR(DWORD address,DWORD op0);
extern void dynaHelperSWR(DWORD address,DWORD op0);

extern void dynaRuntimeBuilderLDL(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLWL(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLDR(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLWR(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLDC2(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderLWC2(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);

extern void dynaRuntimeBuilderSDL(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSWL(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSDR(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSWR(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSDC2(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSWC2(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);


extern void dynaRuntimeBuilderSWC1(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSDC1(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSWC2(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSDC2(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSC(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
extern void dynaRuntimeBuilderSCD(DWORD codeptr,DWORD op0,DWORD op1,DWORD Imm,DWORD StackPointer);
#endif
