extern void hleDSPVMemcpy(WORD DstOffset,WORD SrcOffset,WORD Len);
extern void hleDSPConstruct();
extern void hleDSPInit1();
extern void hleDSPMain();
extern void hleDSPZeroPtrs(WORD offset);
extern void hleMakeVPtr();
extern void hleDSPDecode();
extern void hleDSPIntService();
extern void hleDSPDestruct();
extern void hleDSPSkipSample(WORD m3,DWORD page);
extern void hleDSPResetPtrs(DWORD value,WORD offset);
extern void hleDSPNextSample();
extern void hleDSPMama();
extern void hleDSPResetBlocks(WORD mr0);
extern void hleDSPCopySample(DWORD *page);
extern void hleDSPProcess(WORD ar);
extern void hleDSPMain2();
extern WORD hleDSPExp(WORD src);
extern void hleDSPUpdateAudio();
extern void hleDSPMain3();
extern void hleDSPInit();


extern bool hleDSPFuncA(WORD m3,WORD m2);
extern bool hleDSPFuncB(WORD m3,WORD m2);
extern bool hleDSPFuncC();
extern bool hleDSPFuncD();
extern bool hleDSPFuncE();
extern bool hleDSPFuncF();
extern bool hleDSPFuncG();
extern void hleDSPUpdatePtr(WORD ar);
extern bool hleDSPFuncHIMain(WORD ax0);
extern bool hleDSPFuncI(WORD ay1);
extern bool hleDSPFuncH(WORD ay1);
extern bool hleDSPFuncJ();
extern bool hleDSPFuncK();
extern bool hleDSPFuncL();
extern bool hleDSPFuncKLMain(WORD si,WORD page,WORD offset);
extern bool hleDSPIncPtr(WORD ax0,WORD *i1);
extern void hleDSPDispatch();
extern void hleDSPPreDispatch();





extern void hleDSPVRead24(DWORD *val);
extern void hleDSPVRead16(WORD *val);
extern void hleDSPVRead8(WORD *val);
extern void hleDSPVSetMem();
extern void hleDSPVSetPageOffset(WORD page,WORD offset);
extern void hleDSPVSetAbs(DWORD addy);
extern void hleDSPVGetAbs(DWORD *addy);
extern void hleDSPVGetPageOffset(WORD *page,WORD *offset);
extern void hleDSPVFlush();


extern WORD dspBank;
extern WORD dspAuto;
extern WORD dspPoke;
extern WORD dspPeek;
extern WORD dspClkDiv;
extern WORD dspCtrl;
extern WORD dspIRQClear;
extern void dspReset();
extern WORD dspAutoCount;
extern DWORD dspAutoBase;
extern DWORD dspUpdateCount;
extern DWORD hleDSPVCurAddy;





