#include "iMain.h"
#include "dynaCompiler.h"
#include "dynaNative.h"

#define R0 0
#define At 1
#define V0 2
#define V1 3
#define A0 4
#define A1 5
#define A2 6
#define A3 7
#define T0 8
#define T1 9
#define T2 10
#define T3 11
#define T4 12
#define T5 13
#define T6 14
#define T7 15
#define S0 16
#define S1 17
#define S2 18
#define S3 19
#define S4 20
#define S5 21
#define S6 22
#define S7 23
#define T8 24
#define T9 25
#define K0 26
#define K1 27
#define GP 28
#define SP 29
#define S8 30
#define RA 31


extern WORD hleCheckFunction(BYTE *cp,DWORD Address);
extern void hleWriteBlock();
extern void hleReadBlock();
extern void hleSprite1();
extern void hleScrollBlock(DWORD op0);
extern WORD hleCheckFunctionBGEZAL(BYTE *cp,DWORD Address,BYTE op0);
extern WORD hleCheckFunctionBLTZAL(BYTE *cp,DWORD Address,BYTE op0);
extern void hleBuildBG();
extern void hleFlatScroll();
extern BYTE *hleMakeP(DWORD offset);
extern void hleBackground2();
extern void hleSkyAnim();
extern void hleISR();
extern void hleISR2();
extern void hleUpdateScreen(WORD page);
extern void hleDSPCommand(WORD command);
extern void hleSendDSPCommand();
void hleFMV1();
void hleFMVSub1(short *src,short *dst);
extern WORD hlePatchScreenCopy(BYTE *cp);
extern void hleScreenCopy();
extern void hleFMV2();
extern void hleFMVSub2(DWORD *v0,QWORD *s0,DWORD *s1,int *s2);
extern void hleLine(WORD *dst,short x1,short x2,short u1,short u2,WORD *src);
extern void hleDrawTriangle(BYTE *dst,short x1,short y1,short x2,short y2,short x3,short y3,
						short u1,short v1,short u2,short v2, short u3, short v3,BYTE *src);
extern void hleFillTriangle(BYTE *dst,short x1,short y1,short x2,short y2,short x3,short y3,WORD color);
extern void hleTextureTriangle(BYTE *dst,short x1,short y1,short x2,short y2,short x3,short y3,
						short u1,short v1,short u2,short v2, short u3, short v3,BYTE *src);

extern void hleTri();
extern void hleClearRows();
extern void hleSkySub2();
extern void hleFillRows();
extern void hleSky();
extern WORD hlePatchPal(BYTE *cp);



