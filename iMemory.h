#ifndef IMEM_H
#define IMEM_H

#define RDRAMSIZE 8*1024*1024
#define SRAM (m->rdRam+RDRAMSIZE)
extern WORD iMemToDo;
extern WORD iMemTLBActive;


void iMemConstruct();
void iMemDestruct();
void iMemFinalCheck();
BYTE *iMemPhysReadAddr(DWORD VAddr);
BYTE *iMemPhysWriteAddr(DWORD VAddr);
BYTE iMemReadByte(DWORD where);
WORD iMemReadWord(DWORD where);
DWORD iMemReadDWord(DWORD where);
QWORD iMemReadQWord(DWORD where);
void iMemWriteByte(BYTE what,DWORD where);
void iMemWriteWord(WORD what,DWORD where);
void iMemWriteDWord(DWORD what,DWORD where);
void iMemWriteQWord(QWORD what,DWORD where);
void iMemInit();
void iMemClear();
void iMemFree();
void iMemDoDisplayRefresh();
void iMemUpdateAIReg();
void iMemUpdateVIReg();
void iMemUpdatePIReg();
void iMemUpdateMIReg();
void iMemUpdateSPReg();
void iMemUpdateDPReg();
void iMemCopyBootCode();
void iMemSave(FILE *tmp);
void iMemLoad(FILE *tmp);
void iMemLoadShort(FILE *tmp);

extern DWORD iMemWriteByteAddress;
extern DWORD iMemWriteWordAddress;
extern DWORD iMemWriteDWordAddress;
extern DWORD iMemWriteQWordAddress;
extern DWORD iMemReadByteAddress;
extern DWORD iMemReadWordAddress;
extern DWORD iMemReadDWordAddress;
extern DWORD iMemReadQWordAddress;

extern unsigned char *DSPPMem,*DSPDMem;
extern unsigned char dspReadByte(DWORD A);
extern unsigned int  dspReadWord(DWORD A);
extern DWORD dspReadDWord(DWORD A);
extern void dspWriteByte(DWORD A,BYTE V);
extern void dspWriteWord(DWORD A,WORD V);
extern void dspWriteDWord(DWORD A,DWORD V);

#endif
