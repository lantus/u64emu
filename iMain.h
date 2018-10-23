#ifndef IMAIN_H
#define IMAIN_H


#define NO_DELAY        0
#define DO_DELAY        1
#define EXEC_DELAY      2
#define RELOAD_PC		3

#define LINK_OFFSET 4

#define INC_AMOUNT  625000*2
#define VTRACE_TIME 625000*2

#define EXC_CODE__MASK  0x7c

#define BOOT_STAGE0 0
#define NORMAL_GAME 1
#define SAVE_GAME 2
#define LOAD_GAME 3
#define EXIT_EMU 4
#define HALT_GAME 5
#define RESUME_GAME 6
#define GAME_HALTED 7
#define DEBUG_BREAK 8
#define DEBUG_RESUME 9
#define DEBUG_STEPMODE 10
#define DEBUG_SINGLESTEP 11
#define ERROR_BREAK 12
#define DEBUG_FASTSTEP 13

#define MI_INTR_NO  0x00
#define MI_INTR_ALL 0x3f
#define MI_INTR_SP  0x01
#define MI_INTR_SI  0x02
#define MI_INTR_AI  0x04
#define MI_INTR_VI  0x08
#define MI_INTR_PI  0x10
#define MI_INTR_DP  0x20

typedef struct tr_rominfo
{       
        char  *real_name;
        char  *dmem_name;
        char  *imem_name;
        
        WORD Validation;       /* 0x00 */
        BYTE  Compression;      /* 0x02 */
        BYTE  Unknown1;         /* 0x03 */
        DWORD  Clockrate;        /* 0x04 */
        DWORD  ProgramCounter;   /* 0x08 */
        DWORD  Release;          /* 0x0c */
        
        DWORD  Crc1;             /* 0x10 */
        DWORD  Crc2;             /* 0x14 */
        DWORD Unknown2;         /* 0x18 */
        DWORD Unknown2a;         /* 0x18 */
        
        BYTE  Name[20];         /* 0x20 - 0x33 */
        
        BYTE  Unknown3;         /* 0x34 */
        BYTE  Unknown4;         /* 0x35 */
        BYTE  Unknown5;         /* 0x36 */
        BYTE  Unknown6;         /* 0x37 */
        BYTE  Unknown7;         /* 0x38 */
        BYTE  Unknown8;         /* 0x39 */
        BYTE  Unknown9;         /* 0x3a */
        BYTE  ManufacturerId;   /* 0x3b */
        WORD CartridgeId;      /* 0x3c */
        BYTE  CountryCode;      /* 0x3e */
        BYTE  Unknown10;        /* 0x3f */
} N64RomInfoStruct;



typedef struct rs4300i_tlb
{
        DWORD    hh;
        DWORD    hl;
        DWORD    lh;
        DWORD    ll;
        BIT     g;
} RS4300iTlb;



typedef struct tr_rom
{
        BYTE    *Image;                 /* pointer to rom image                 */
        long    Length;                 /* length of rom image                  */

        BYTE    *Header;                /* pointer to header (same as 'image')  */
        BYTE    *BootCode;              /* pointer to bootcode                  */
        DWORD    *PrgCode;               /* pointer to prgcode                   */

        DWORD    PrgCodeBaseOrig;      /* base of prgcode (from header)        */
        DWORD    PrgCodeBase;           /* base of (prgcode & 0x1fffffff)       */
        long    PrgCodeLength;         /* length of prgcode                    */
		N64RomInfoStruct Info;
} N64RomStruct;


typedef struct rs4300i_reg
{
		sDWORD GPR[64];		// general purpose registers
							// address mode
							// 64 bit:  *(sQWORD)&GPR[index*2]
							// 32 bit: GPR[index*2];
							// this 64/32 address mode is used for all register groups with the exception
							// of FPR (CPR1) which are the floating point registers
 
		sDWORD CPR0[64];	// co-processor 0 registers
		sDWORD CPR1[64];	// co-processor 1 registers
		sDWORD CPR2[64];	// co-processor 2 registers

		sDWORD CCR0[64];	// co-processor 0 control registers 
		sDWORD CCR1[64];	// co-processor 1 control registers 
		sDWORD CCR2[64];	// co-processor 2 control registers 

		sDWORD FPR[32];		// used instead of CPR1 - floating point registers

        QWORD   Lo, Hi;     // multiply and divide registers
        
        DWORD    PC;        // program counter 

		// the following structure members are for the emulation engine only, they are not directly
		// part of the 4300
        
        DWORD    PCDelay;   // delayed program counter 
        int     Delay;      // this is for correct emulation of the 6 stage pipeline
                            // if(delay == NO_DELAY)   { next instr is pc + 4 } 
                            // if(delay == DO_DELAY)   { next instr is the delayed instruction } 
                            // if(delay == EXEC_DELAY) { exec instr @ delayed pc - set in switch} 
        
        int     DoOrCheckSthg;
                            // if '!0' then the rs4300i has to do or check sthg 
                            // look at the defines that follow this struct 

        DWORD CurRoundMode;	// current rounding mode for the FPU
        DWORD    Code;      // current code (needed for speed) 

        DWORD LastPC;		// last address of execution
		DWORD Break;		// break address
        BIT     Llbit;		// link bit
		BYTE pad[3];		// just pads
		QWORD ICount;		// instruction count
		DWORD CompareCount;	// count for compare
		QWORD NextIntCount;	// count for next interrupt (VTrace or Compare)
		QWORD VTraceCount;	// count of next VTrace
		DWORD RoundMode;	// value to place Intel FPU in Round Mode
		DWORD TruncMode;	// value to place Intel FPU in Truncation Mode
		DWORD CeilMode;		// value to place Intel FPU in Ceiling Mode
		DWORD FloorMode;	// value to place Intel FPU in Floor Mode
		RS4300iTlb Tlb[48];


} RS4300iReg; /* struct rs4300i_reg */


typedef struct mem_struct
{
        BYTE    *rdRam;
        BYTE    *spDmem;
        BYTE    *spImem;
        BYTE    *piRom;
        BYTE    *piRam;
        BYTE    *piRamW;

        BYTE    *rdReg;
        BYTE    *spReg;
        BYTE    *dpcReg;
        BYTE    *dpsReg;
        BYTE    *miReg;
        BYTE    *miRegW;
        BYTE    *viReg;
        BYTE    *aiReg;
        BYTE    *piReg;
        BYTE    *piRegW;
        BYTE    *riReg;
        BYTE    *siReg;
        BYTE    *NullMem;
		BYTE	*atReg;
		BYTE	*dspPMem;
		BYTE    *dspDMem;
		BYTE	*dspRMem;

        DWORD    miRegModeRO;
        DWORD    miRegIntrMaskRO;

        DWORD    spRegPC;
        DWORD    spRegIbist;

        DWORD    spRegStatusRO;

        DWORD    dpcRegStatusRO;

        DWORD    piRegStatusRO;
} N64Mem;


extern volatile WORD NewTask;
extern volatile WORD DspTask;

extern WORD iFPUMode;
extern RS4300iReg *r;
extern N64Mem *m;
extern N64RomStruct *rom;

extern DWORD iOpCode;
extern DWORD iNextOpCode;
extern DWORD iPC;

#define INDEX           0
#define RANDOM          1
#define ENTRYLO0        2
#define ENTRYLO1        3
#define CONTEXT         4
#define PAGEMASK        5
#define WIRED           6
#define BADVADDR        8
#define COUNT           9
#define ENTRYHI         10
#define COMPARE         11
#define STATUS          12
#define CAUSE           13
#define EPC             14
#define PRID            15
#define CONFIG          16
#define LLADDR          17
#define WATCHLO         18
#define WATCHHI         19
#define XCONTEXT        20
#define ECC             26
#define CACHEERR        27
#define TAGLO           28
#define TAGHI           29
#define ERROREPC        30

#define MAKE_RS           (((BYTE)(iOpCode >> 21)) & 0x1f)

#define MAKE_RT            (((BYTE)(iOpCode >> 16)) & 0x1f)

#define MAKE_RD            (((BYTE)(iOpCode >> 11)) & 0x1f)

#define MAKE_SA            (((BYTE)(iOpCode >>  6)) & 0x1f)

#define MAKE_F             ( (BYTE)(iOpCode)       & 0x3f)

#define MAKE_I             ( (sDWORD) (short)(iOpCode&0xffff) )
#define MAKE_IU             ((WORD)(iOpCode&0xffff) )

#define MAKE_VD           (((BYTE)(iOpCode >> 6)) & 0x1f)

#define MAKE_VS1            (((BYTE)(iOpCode >> 11)) & 0x1f)

#define MAKE_VS2            (((BYTE)(iOpCode >> 16)) & 0x1f)

#define MAKE_VSEL            (((BYTE)(iOpCode >>  21)) & 0xf)

#define MAKE_FMT MAKE_RS
#define MAKE_FT MAKE_RT
#define MAKE_FS MAKE_RD
#define MAKE_FD MAKE_SA

#define FMT_S   16      /* 32bit binary floating-point */
#define FMT_D   17      /* 64bit binary floating-point */
#define FMT_W   20      /* 32bit binary fixed-point    */
#define FMT_L   21      /* 64bit binary fixed-point    */

#define MAKE_O             ( r->PC + (sDWORD)(MAKE_I << 2) )
#define ____T           (iOpCode & 0x00ffffff)
#define MAKE_T            (((r->PC & 0xff000000) | (____T << 2) ))

extern void iMainConstruct(char *filename);
extern void iMainDestruct();
extern void iMainThreadProc();
extern void iMainReset();
extern void iMainStopCPU();
extern void iMainStartCPU();
extern void iMainResetDSP();


#endif