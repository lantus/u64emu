#ifndef DYNA_COMPILER
#define DYNA_COMPILER


//	Page=  (NewPC&0x003fc000)>>14;
//	Offset=(NewPC&0x00003ffc)>>2;
//#define PAGE_MASK 0x003f8000
//#define PAGE_SHIFT 15
//#define OFFSET_MASK 0x7ffc
//#define OFFSET_SHIFT 2

//#define PAGE_SIZE	4096*2
//#define NUM_PAGES	256/2			// i don't suggest that you change these, as other parts of the code, however limited, take
#define MEM_MASK 0x7fffff
#define MEM_SIZE 0x800000


/*
#define PAGE_SIZE	4096
#define NUM_PAGES	512			// i don't suggest that you change these, as other parts of the code, however limited, take
#define PAGE_MASK		0x007fc000
#define OFFSET_MASK		0x00003ffc
#define PAGE_SHIFT		14
#define OFFSET_SHIFT	2
*/


#define PAGE_SIZE 0x8000
#define NUM_PAGES	4	
#define PAGE_MASK		0x00060000
#define OFFSET_MASK		0x0001fffc
#define PAGE_SHIFT		17
#define OFFSET_SHIFT	2
/*
#define PAGE_SIZE 0x4000
#define NUM_PAGES	8	
#define PAGE_MASK		0x00070000
#define OFFSET_MASK		0x0000fffc
#define PAGE_SHIFT		16
#define OFFSET_SHIFT	2
*/
								// advantage of the bit patterns to calculate page/offsets
#define GPR0_OPTIMIZATION
#define RD_EQUAL_RS
#define BASIC_ILOOP
#define VARI_ILOOPn
#define ALL_ILOOPn

// these are the magic numbers.  they are the offsets into the CPU Register structure.  Dyna keeps a pointer to the
// base and then indexes in to get the various registers. On the Intel, EBP is set to the base.

	#define NATIVE_0 0		//EAX
	#define NATIVE_1 3		//EBX
	#define NATIVE_2 1		//ECX
	#define NATIVE_3 2		//EDX
	#define MEM_PTR 6		//ESI
	#define PC_PTR  7		//EDI
	#define REG_PTR  5		//EBP
	#define GPR_ 0					//   0
	#define CPR0_ GPR_+256			// 256
	#define CPR1_ CPR0_+256			// 512
	#define CPR2_ CPR1_+256			// 768
	#define CCR0_ CPR2_+256			//1024
	#define CCR1_ CCR0_+256			//1280
	#define CCR2_ CCR1_+256			//1536
	#define _FPR  CCR2_+256			//1792
	#define LO   _FPR+128			//1920
	#define HI   LO+8				//1928
	#define PC_   HI+8				//1936
	#define PCDELAY PC_+4			//1940
	#define DELAY   PCDELAY+4		//1944
	#define DORC DELAY+4			//1948
	#define CUR_ROUND_MODE  DORC+4	//1952
	#define CODE CUR_ROUND_MODE+4	//1956
	#define LAST_PC CODE+4			//1960
	#define TMP_ LAST_PC+4			//1964
	#define LL TMP_+4				//1968
	#define ICOUNT 1976
	#define CCOUNT 1984
	#define NCOUNT CCOUNT+8			//1992
	#define VCOUNT NCOUNT+8			//2000 (or 0 for you y2k fans)
	#define NEAREST VCOUNT+8		//2008	
	#define TRUNC NEAREST+4			//2012
	#define CEIL TRUNC+4			//2012
	#define FLOOR CEIL+4			//2012


// For Intel
#define NATIVE_REGS 4		// eax,ebx,ecx are available;  
							// edx,esi,edi & esp are reserved
							// edi always points to RegisterMemory
							// esi is reserved for registers used as a memory read pointer
							// edi is reserved for registers used as a memory write pointer
							// esp is the stack pointer

extern short dynaFPointer[32];
extern BYTE dynaFPPage[32];

typedef struct page_table
{
	BYTE *Offset[PAGE_SIZE];
	DWORD Value[PAGE_SIZE];
} dynaPageTableStruct;
extern dynaPageTableStruct dynaPageTable[NUM_PAGES];		// 256 pages, 4096 DWORD ptrs per page, ie; 16K per page * 256 pages = 4MB

typedef struct reg_map
{
	BYTE Reg;				// register number
	DWORD BirthDate;		// 'year' the register was mapped.
	BYTE Dirty;				// true if register is modified after loading; ie the results of an operation
} dynaRegMap;

extern DWORD dynaNextOpCode;
extern DWORD dynaPreCompileLength;
extern BYTE *dynaCompileTargetPtr;
extern DWORD *dynaRam;					//source for what we are compiling, must be global so that branch ops can get next instruction
extern WORD dynaSrc;
extern DWORD dynaRegPtr;				// this points to a data structure that contains the GPR,CPR,CCR,PC etc. for the 4300
extern DWORD dynaPCPtr;
extern DWORD dynaCurPage;
extern BYTE dynaForceFallOut;
extern DWORD dynaLengthTable[PAGE_SIZE];
extern BYTE *dynaRamPtr;

extern int dynaNumBPoints;
extern DWORD dynaBPoint[1024];
extern bool dynaBreakOnKnown;
extern DWORD dasmNumLookups;
extern DWORD *dasmLookupAddy;

extern DWORD InterpHelperAddress;
extern DWORD helperLoadDWordAddress;
extern DWORD helperLoadWordAddress;
extern DWORD helperLoadHalfAddress;
extern DWORD helperLoadByteAddress;
extern DWORD helperStoreDWordAddress;
extern DWORD helperStoreWordAddress;
extern DWORD helperStoreHalfAddress;
extern DWORD helperStoreByteAddress;
extern DWORD helperCheckIntsAddress;
extern DWORD helperLDLAddress;
extern DWORD helperLWLAddress;
extern DWORD helperLDRAddress;
extern DWORD helperLWRAddress;
extern DWORD bugFinderAddress;

//DWORD ICountPtr;
//DWORD CompareCountPtr;
extern DWORD SneekyAddress;
extern DWORD dynaPC;
extern DWORD dynaVCount;
extern DWORD dynaICount;
extern BYTE dynaInBranch;
extern BYTE *dynaHold;
extern BOOL dynaHLEAudio;
extern bool dynaStepMode;
extern bool dynaBreakSet;

extern volatile WORD NewTask;

extern volatile WORD dynaVTRACE;

extern DWORD dynaCheckIntsAddress;
extern dynaRegMap dynaMapToNative[32];	// index is MIPS GPR, value is the Native register currently mapped to
								// Dirty flag marks the register if it has been modified since load
								// 0xff if register is not mapped

extern dynaRegMap dynaMapToMips[NATIVE_REGS];	// index is Native register, value is the MIPS GPR currently mapped
										// Dirty flag marks the register if it has been modified since load
										// 0xff if register is not mapped
extern dynaRegMap buffer[2048];

extern BYTE dynaNumFreeRegs;			// number of 'free' Native registers

extern DWORD dynaBirthYear;			// the 'year' that any newly mapped registers will be 'born' on. used to determine
								// which registers should flush out when we need space in the Native register set
								// incremented everytime a register is loaded or modified.



extern void dynaInit();
extern void dynaDestroy();
extern BYTE dynaExecute(DWORD Address);
extern void dynaInvalidate(DWORD Start,DWORD Length);
extern BYTE dynaPreCompilePage(DWORD Address);
extern BYTE dynaCompilePage(DWORD Address);
extern WORD dynaCompile_sll(BYTE *dst);
extern WORD dynaCompile_srl(BYTE *dst);
extern WORD dynaCompile_sra(BYTE *dst);
extern WORD dynaCompile_sllv(BYTE *dst);
extern WORD dynaCompile_srlv(BYTE *dst);
extern WORD dynaCompile_srav(BYTE *dst);
extern WORD dynaCompile_jr(BYTE *dst);
extern WORD dynaCompile_jalr(BYTE *dst);
extern WORD dynaCompile_syscall(BYTE *dst);
extern WORD dynaCompile_Break(BYTE *dst);
extern WORD dynaCompile_reserved(BYTE *dst);
extern WORD dynaCompile_sync(BYTE *dst);
extern WORD dynaCompile_mfhi(BYTE *dst);
extern WORD dynaCompile_mthi(BYTE *dst);
extern WORD dynaCompile_mflo(BYTE *dst);
extern WORD dynaCompile_mtlo(BYTE *dst);
extern WORD dynaCompile_dsllv(BYTE *dst);
extern WORD dynaCompile_dsrlv(BYTE *dst);
extern WORD dynaCompile_dsrav(BYTE *dst);
extern WORD dynaCompile_mult(BYTE *dst);
extern WORD dynaCompile_multu(BYTE *dst);
extern WORD dynaCompile_div(BYTE *dst);
extern WORD dynaCompile_divu(BYTE *dst);
extern WORD dynaCompile_dmult(BYTE *dst);
extern WORD dynaCompile_dmultu(BYTE *dst);
extern WORD dynaCompile_ddiv(BYTE *dst);
extern WORD dynaCompile_ddivu(BYTE *dst);
extern WORD dynaCompile_add(BYTE *dst);
extern WORD dynaCompile_addu(BYTE *dst);
extern WORD dynaCompile_sub(BYTE *dst);
extern WORD dynaCompile_subu(BYTE *dst);
extern WORD dynaCompile_and(BYTE *dst);
extern WORD dynaCompile_or(BYTE *dst);
extern WORD dynaCompile_xor(BYTE *dst);
extern WORD dynaCompile_nor(BYTE *dst);
extern WORD dynaCompile_slt(BYTE *dst);
extern WORD dynaCompile_sltu(BYTE *dst);
extern WORD dynaCompile_dadd(BYTE *dst);
extern WORD dynaCompile_daddu(BYTE *dst);
extern WORD dynaCompile_dsub(BYTE *dst);
extern WORD dynaCompile_dsubu(BYTE *dst);
extern WORD dynaCompile_tge(BYTE *dst);
extern WORD dynaCompile_tgeu(BYTE *dst);
extern WORD dynaCompile_tlt(BYTE *dst);
extern WORD dynaCompile_tltu(BYTE *dst);
extern WORD dynaCompile_teq(BYTE *dst);
extern WORD dynaCompile_tne(BYTE *dst);
extern WORD dynaCompile_dsll(BYTE *dst);
extern WORD dynaCompile_dsrl(BYTE *dst);
extern WORD dynaCompile_dsra(BYTE *dst);
extern WORD dynaCompile_dsll32(BYTE *dst);
extern WORD dynaCompile_dsrl32(BYTE *dst);
extern WORD dynaCompile_dsra3(BYTE *dst);
extern WORD dynaCompile_special(BYTE *dst);
extern WORD dynaCompile_regimm(BYTE *dst);
extern WORD dynaCompile_j(BYTE *dst);
extern WORD dynaCompile_jal(BYTE *dst);
extern WORD dynaCompile_beq(BYTE *dst);
extern WORD dynaCompile_bne(BYTE *dst);
extern WORD dynaCompile_blez(BYTE *dst);
extern WORD dynaCompile_bgtz(BYTE *dst);
extern WORD dynaCompile_addi(BYTE *dst);
extern WORD dynaCompile_addiu(BYTE *dst);
extern WORD dynaCompile_slti(BYTE *dst);
extern WORD dynaCompile_sltiu(BYTE *dst);
extern WORD dynaCompile_andi(BYTE *dst);
extern WORD dynaCompile_ori(BYTE *dst);
extern WORD dynaCompile_xori(BYTE *dst);
extern WORD dynaCompile_lui(BYTE *dst);
extern WORD dynaCompile_cop0(BYTE *dst);
extern WORD dynaCompile_cop1(BYTE *dst);
extern WORD dynaCompile_cop2(BYTE *dst);
extern WORD dynaCompile_beql(BYTE *dst);
extern WORD dynaCompile_bnel(BYTE *dst);
extern WORD dynaCompile_blezl(BYTE *dst);
extern WORD dynaCompile_bgtzl(BYTE *dst);
extern WORD dynaCompile_daddi(BYTE *dst);
extern WORD dynaCompile_daddiu(BYTE *dst);
extern WORD dynaCompile_ldl(BYTE *dst);
extern WORD dynaCompile_ldr(BYTE *dst);
extern WORD dynaCompile_lb(BYTE *dst);
extern WORD dynaCompile_lh(BYTE *dst);
extern WORD dynaCompile_lwl(BYTE *dst);
extern WORD dynaCompile_lw(BYTE *dst);
extern WORD dynaCompile_lbu(BYTE *dst);
extern WORD dynaCompile_lhu(BYTE *dst);
extern WORD dynaCompile_lwr(BYTE *dst);
extern WORD dynaCompile_lwu(BYTE *dst);
extern WORD dynaCompile_sb(BYTE *dst);
extern WORD dynaCompile_sh(BYTE *dst);
extern WORD dynaCompile_swl(BYTE *dst);
extern WORD dynaCompile_sw(BYTE *dst);
extern WORD dynaCompile_sdl(BYTE *dst);
extern WORD dynaCompile_sdr(BYTE *dst);
extern WORD dynaCompile_swr(BYTE *dst);
extern WORD dynaCompile_cache(BYTE *dst);
extern WORD dynaCompile_ll(BYTE *dst);
extern WORD dynaCompile_lwc1(BYTE *dst);
extern WORD dynaCompile_lwc2(BYTE *dst);
extern WORD dynaCompile_lld(BYTE *dst);
extern WORD dynaCompile_ldc1(BYTE *dst);
extern WORD dynaCompile_ldc2(BYTE *dst);
extern WORD dynaCompile_ld(BYTE *dst);
extern WORD dynaCompile_sc(BYTE *dst);
extern WORD dynaCompile_swc1(BYTE *dst);
extern WORD dynaCompile_swc2(BYTE *dst);
extern WORD dynaCompile_scd(BYTE *dst);
extern WORD dynaCompile_sdc1(BYTE *dst);
extern WORD dynaCompile_sdc2(BYTE *dst);
extern WORD dynaCompile_s(BYTE *dst);
extern WORD dynaCompile_bltz(BYTE *dst);
extern WORD dynaCompile_bgez(BYTE *dst);
extern WORD dynaCompile_bltzl(BYTE *dst);
extern WORD dynaCompile_bgezl(BYTE *dst);
extern WORD dynaCompile_tgei(BYTE *dst);
extern WORD dynaCompile_tgeiu(BYTE *dst);
extern WORD dynaCompile_tlti(BYTE *dst);
extern WORD dynaCompile_tltiu(BYTE *dst);
extern WORD dynaCompile_teqi(BYTE *dst);
extern WORD dynaCompile_tnei(BYTE *dst);
extern WORD dynaCompile_bltzal(BYTE *dst);
extern WORD dynaCompile_bgezal(BYTE *dst);
extern WORD dynaCompile_bltzall(BYTE *dst);
extern WORD dynaCompile_bgezall(BYTE *dst);
extern WORD dynaCompile_cop0_rs_mf(BYTE *dst);
extern WORD dynaCompile_cop0_rs_dmf(BYTE *dst);
extern WORD dynaCompile_cop0_rs_cf(BYTE *dst);
extern WORD dynaCompile_cop0_rs_mt(BYTE *dst);
extern WORD dynaCompile_cop0_rs_dmt(BYTE *dst);
extern WORD dynaCompile_cop0_rs_ct(BYTE *dst);
extern WORD dynaCompile_cop0_rs_bc(BYTE *dst);
extern WORD dynaCompile_cop0_rs_co(BYTE *dst);
extern WORD dynaCompile_cop0_rt_bcf(BYTE *dst);
extern WORD dynaCompile_cop0_rt_bct(BYTE *dst);
extern WORD dynaCompile_cop0_rt_bcfl(BYTE *dst);
extern WORD dynaCompile_cop0_rt_bctl(BYTE *dst);
extern WORD dynaCompile_invalid(BYTE *dst);
extern WORD dynaCompile_cop0_tlbr(BYTE *dst);
extern WORD dynaCompile_cop0_tlbwi(BYTE *dst);
extern WORD dynaCompile_cop0_tlbwr(BYTE *dst);
extern WORD dynaCompile_cop0_tlbp(BYTE *dst);
extern WORD dynaCompile_cop0_eret(BYTE *dst);
extern WORD dynaCompile_cop1_rs_mf(BYTE *dst);
extern WORD dynaCompile_cop1_rs_dmf(BYTE *dst);
extern WORD dynaCompile_cop1_rs_cf(BYTE *dst);
extern WORD dynaCompile_cop1_rs_mt(BYTE *dst);
extern WORD dynaCompile_cop1_rs_dmt(BYTE *dst);
extern WORD dynaCompile_cop1_rs_ct(BYTE *dst);
extern WORD dynaCompile_cop1_rs_bc(BYTE *dst);
extern WORD dynaCompile_cop1_rs_co(BYTE *dst);
extern WORD dynaCompile_cop1_rt_bcf(BYTE *dst);
extern WORD dynaCompile_cop1_rt_bct(BYTE *dst);
extern WORD dynaCompile_cop1_rt_bcfl(BYTE *dst);
extern WORD dynaCompile_cop1_rt_bctl(BYTE *dst);
extern WORD dynaCompile_cop1_add(BYTE *dst);
extern WORD dynaCompile_cop1_sub(BYTE *dst);
extern WORD dynaCompile_cop1_mul(BYTE *dst);
extern WORD dynaCompile_cop1_div(BYTE *dst);
extern WORD dynaCompile_cop1_sqrt(BYTE *dst);
extern WORD dynaCompile_cop1_abs(BYTE *dst);
extern WORD dynaCompile_cop1_mov(BYTE *dst);
extern WORD dynaCompile_cop1_neg(BYTE *dst);
extern WORD dynaCompile_cop1_roundl(BYTE *dst);
extern WORD dynaCompile_cop1_truncl(BYTE *dst);
extern WORD dynaCompile_cop1_ceill(BYTE *dst);
extern WORD dynaCompile_cop1_floorl(BYTE *dst);
extern WORD dynaCompile_cop1_roundw(BYTE *dst);
extern WORD dynaCompile_cop1_truncw(BYTE *dst);
extern WORD dynaCompile_cop1_ceilw(BYTE *dst);
extern WORD dynaCompile_cop1_floorw(BYTE *dst);
extern WORD dynaCompile_cop1_cvts(BYTE *dst);
extern WORD dynaCompile_cop1_cvtd(BYTE *dst);
extern WORD dynaCompile_cop1_cvtw(BYTE *dst);
extern WORD dynaCompile_cop1_cvtl(BYTE *dst);
extern WORD dynaCompile_cop1_c(BYTE *dst);
extern WORD dynaCompile_cop2_rs_not_implemented(BYTE *dst);
extern WORD dynaCompile_dsra32(BYTE *dst);
extern WORD dynaCompile_sd(BYTE *dst);
extern WORD dynaFallOut(BYTE *dst);

typedef WORD (*ins_ptr)(BYTE *dst);
extern ins_ptr SpecialInstruction[64];
extern ins_ptr MainInstruction[64];
extern ins_ptr RegimmInstruction[32];
extern ins_ptr Cop0RSInstruction[32];
extern ins_ptr Cop0RTInstruction[32];
extern ins_ptr Cop0Instruction[64];
extern ins_ptr Cop1RSInstruction[32];
extern ins_ptr Cop1RTInstruction[32];
extern ins_ptr Cop1Instruction[64];
extern ins_ptr Cop2RSInstruction[32];

extern void dynaMarkRegDirty(BYTE MipsReg,BYTE IsDirty);
extern BYTE dynaIsUsed(BYTE MipsReg);
extern BYTE dynaGetFreeReg();
extern WORD dynaFlushReg(BYTE *cp,BYTE NReg);
extern WORD dynaKillNative(BYTE NReg);
extern WORD dynaTmpFlushReg(BYTE *cp,BYTE NReg);
extern WORD dynaTmpFlushRegPair(BYTE *cp,BYTE MipsReg);
extern void dynaSwitchMap(BYTE a,BYTE b);
extern WORD dynaMakeFreeReg(BYTE *cp,BYTE *NReg);
extern WORD dynaLoadReg(BYTE *cp,BYTE MipsReg);
extern WORD dynaLoadRegPair(BYTE *cp,BYTE MipsReg);
extern WORD dynaLoadRegForceNot(BYTE *cp,BYTE MipsReg,BYTE ExcludedNativeReg);
extern WORD dynaLoadRegForce(BYTE *cp,BYTE MipsReg,BYTE ForcedNativeReg);
extern DWORD dynaGetCompiledAddress(DWORD NewPC);
extern WORD dynaCompile_bgei(BYTE  *cp);
extern WORD dynaCompile_bgeiu(BYTE  *cp);
extern WORD dynaCompile_bli(BYTE  *cp);
extern WORD dynaCompile_bliu(BYTE  *cp);

#endif
