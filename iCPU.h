#ifndef ICPU_H
#define ICPU_H

#define PATCH_SIN 16
#define PATCH_COSIN 17

extern void iCpuConstruct();
extern void iCpuDestruct();
extern void iCpuReset();
extern UINT8 iCpuThreadProc();
extern UINT8 iDspThreadProc();
extern void iOpSpecial();
extern void iOpRegimm();
extern void iOpCop0();
extern void iOpCop1();
extern void iOpCop2();
extern void iOpBc0();
extern void iOpCo0();
extern void iOpBc1();
extern void iOpBc2();
extern void iOpCo1();
extern void iOpVector();
extern void iCpuCheckInts();

extern void iOpReserved();
extern void iOpSyscall();
extern void iOpBreak();
extern void iOpInvalid();
extern void iOpCop2Reserved();
extern void iOpSync();
extern void iOpTge();
extern void iOpTgeu();
extern void iOpTlt();
extern void iOpTltu();
extern void iOpTeq();
extern void iOpTne();
extern void iOpCache();
extern void iOpTgei();
extern void iOpTgeiu();
extern void iOpTlti();
extern void iOpTltiu();
extern void iOpTeqi();
extern void iOpTnei();
extern void iCpuHelper(DWORD OpCode);
extern void iCpuVSYNC();
extern void iCpuCheckVSYNC();

extern HANDLE iCpuThreadId;
extern HANDLE iDspThreadId;
extern DWORD iCpuNextVSYNC;
extern bool iCpuResetVSYNC;
extern void iCpuSaveGame();
extern void iCpuLoadGame();
extern void iCpuCheckFPU();

extern void iCpuDoNextOp();
extern void iCpuStepDSP();
#endif
