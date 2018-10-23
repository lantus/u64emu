
#include "stdafx.h"
#include "math.h"
#include "ki.h"
#include "imain.h"


#include "dasmMain.h"
#include "dasmIns.h"



#define GPR0_OPTIMIZATION
//#define RD_EQUAL_RS


DWORD dasmOpCode;
DWORD dasmNextOpCode;
DWORD dasmPC;
BYTE dasmIsValid[32];
DWORD *dasmRam;
bool dasmRSPMode=false;
__int64 dasmReg[32];
__int64 dasmRegHi;
__int64 dasmRegLo;
__int64 dasmCPR0[32];
__int64 dasmCPR1[32];
__int64 dasmCPC0[32];
__int64 dasmCPC1[32];
char **dasmLookupName=NULL;
DWORD *dasmLookupAddy=NULL;
DWORD dasmNumLookups=0;

char dasmRegName[32][3]= {
	"R0","At","V0","V1","A0","A1","A2","A3",
	"T0","T1","T2","T3","T4","T5","T6","T7",
	"S0","S1","S2","S3","S4","S5","S6","S7",
	"T8","T9","K0","K1","GP","SP","S8","RA" };

char dasmCRegName[32][9]= {
	"INDEX   ","RANDOM  ","ENTRYLO0","ENTRYLO1","CONTEXT ","PAGEMASK","WIRED   ","___(7)__",
	"BADVADDR","COUNT   ","ENTRYHI ","COMPARE ","STATUS  ","CAUSE   ","EPC     ","PRID    ",
	"CONFIG  ","LLADDR  ","WATCHLO ","WATCHHI ","XCONTEXT","__(21)__","__(22)__","__(23)__",
	"__(24)__","__(25)__","ECC     ","CACHEERR","TAGLO   ","TAGHI   ","ERROREPC","__(31)__" };


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

#define DMAKE_RS           (((BYTE)(dasmOpCode >> 21)) & 0x1f)

#define DMAKE_RT            (((BYTE)(dasmOpCode >> 16)) & 0x1f)

#define DMAKE_RD            (((BYTE)(dasmOpCode >> 11)) & 0x1f)

#define DMAKE_SA            (((BYTE)(dasmOpCode >>  6)) & 0x1f)

#define DMAKE_F             ( (BYTE)(dasmOpCode)       & 0x3f)

#define DMAKE_I             ( (short)(dasmOpCode&0xffff) )
#define DMAKE_IU             ((WORD)(dasmOpCode&0xffff) )

#define DMAKE_VD           (((BYTE)(dasmOpCode >> 6)) & 0x1f)

#define DMAKE_VS1            (((BYTE)(dasmOpCode >> 11)) & 0x1f)

#define DMAKE_VS2            (((BYTE)(dasmOpCode >> 16)) & 0x1f)

#define DMAKE_VSEL            (((BYTE)(dasmOpCode >>  21)) & 0xf)

#define DMAKE_FMT DMAKE_RS
#define DMAKE_FT DMAKE_RT
#define DMAKE_FS DMAKE_RD
#define DMAKE_FD DMAKE_SA

#define FMT_S   16      /* 32bit binary floating-point */
#define FMT_D   17      /* 64bit binary floating-point */
#define FMT_W   20      /* 32bit binary fixed-point    */
#define FMT_L   21      /* 64bit binary fixed-point    */

#define DMAKE_O             ( dasmPC + (sDWORD)(DMAKE_I << 2) +4)
#define D____T           (dasmOpCode & 0x00ffffff)
#define DMAKE_T            (((dasmPC & 0xf0000000) | (D____T << 2) ))

//extern	RS4300iReg *m_Reg;		// owned by N64CPU

void dasmReset()
{
	for(BYTE i=0;i<32;i++)
	{
		dasmReg[i]=0;
		dasmIsValid[i]=false;
	}
	dasmIsValid[0]=true;
	dasmIsValid[29]=true;
//	dasmReg[29]=*(sQWORD *)&m_Reg->r[29*2];
	dasmRSPMode=false;
}

void dasmClose()
{
	for(DWORD i=0;i<dasmNumLookups;i++)
	{
		SafeFree(dasmLookupName[i]);
	}
	SafeFree(dasmLookupName);
	SafeFree(dasmLookupAddy);
	dasmNumLookups=0;
}
WORD dasmNOP(BYTE *cp)
{
	return(sprintf((char *)cp,"%X NOP",dasmPC));
}
   
void dasmChangeEndian(WORD Mode,BYTE *src,DWORD Length)
{
    DWORD     i;
    BYTE    change_endian[4];



    for(i=0; i<Length; i+=4)
    {
        change_endian[0] = src[i+0];
        change_endian[1] = src[i+1];
        change_endian[2] = src[i+2];
        change_endian[3] = src[i+3];
		if(Mode==0)
		{
			src[i+0] = change_endian[2];
			src[i+1] = change_endian[3];
			src[i+2] = change_endian[0];
			src[i+3] = change_endian[1];
		}
		else
		{
			src[i+0] = change_endian[3];
			src[i+1] = change_endian[2];
			src[i+2] = change_endian[1];
			src[i+3] = change_endian[0];
		}
	}
} 

char *dasmDo(DWORD *where,DWORD Address,DWORD EndAddress)
{
	DWORD size=16384;					// start with 16K
	BYTE *Text=(BYTE *)malloc(size);

	BYTE *dasmTargetPtr=Text;

	DWORD len=0;
	DWORD TotalLength=0;

	DWORD CurIns=0;

	dasmPC=Address;
	dasmRam=where;

	while(dasmPC<EndAddress)
	{
		dasmOpCode=*(dasmRam++);
		dasmNextOpCode=*dasmRam;
		len=Main[dasmOpCode>>26](dasmTargetPtr)+1;	// has null (0) terminator
		TotalLength+=len;
		if( (size-TotalLength) < 2048)			// running out of room, so get more
		{
			size+=16384;
			Text=(BYTE *)realloc(Text,size);
		}
		dasmPC+=4;
		dasmTargetPtr=&Text[TotalLength];
	}
	Text=(BYTE *)realloc(Text,TotalLength);		// shrink it down
	return (char *)Text;						// caller is tasked with freeing this memory
}

#define INS_LENGTH 10
#define COM_LENGTH 16
#define PARAM_LENGTH 16

char *dasmFormatIns(char *src)
{
	static char formatted[INS_LENGTH+1];
	char *dst=formatted;
	memset(formatted,32,INS_LENGTH);
	formatted[INS_LENGTH]=0;
	int count=0;
	while((*src)&&(count<INS_LENGTH))
	{
		*(dst++)=*(src++);
		count++;
	}
	return(formatted);		
}

char *dasmFormatParam(char *src)
{
	static char formatted[PARAM_LENGTH+1];
	char *dst=formatted;
	memset(formatted,32,PARAM_LENGTH);
	formatted[PARAM_LENGTH]=0;
	int count=0;
	while((*src)&&(count<PARAM_LENGTH))
	{
		*(dst++)=*(src++);
		count++;
	}
	return(formatted);		
}

char *dasmFormatComment(char *src)
{
	static char formatted[COM_LENGTH+1];
	char *dst=formatted;
	memset(formatted,32,COM_LENGTH);
	formatted[COM_LENGTH]=0;
	int count=0;
	while((*src)&&(count<COM_LENGTH))
	{
		*(dst++)=*(src++);
		count++;
	}
	return(formatted);		
}

char *dasmFormatComment2(char *src)
{
	static char formatted[COM_LENGTH+1];
	char *dst=formatted;
	memset(formatted,32,COM_LENGTH);
	formatted[COM_LENGTH]=0;
	int count=0;
	while((*src)&&(count<COM_LENGTH))
	{
		*(dst++)=*(src++);
		count++;
	}
	return(formatted);		
}

WORD dasmOpGeneric(BYTE *cp,char *ins)
{

	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(" "),
												dasmFormatComment(" "),dasmFormatComment2(" ")));
}

WORD dasmOpGeneric1Imm(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd,DWORD Imm)
{
	char tmp[32];
	sprintf(tmp,"%s,$%X",dasmRegName[rd],Imm);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGenericImm(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd,BYTE rs,DWORD Imm)
{
	char tmp[32];
	sprintf(tmp,"%s,%s,$%X",dasmRegName[rd],dasmRegName[rs],Imm);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGenericMemory(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd,BYTE rs,DWORD Imm)
{
	char tmp[32];
	sprintf(tmp,"%s,$%X(%s)",dasmRegName[rd],Imm,dasmRegName[rs]);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpVectorMemory(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd,BYTE rs,DWORD Imm,BYTE el)
{
	char tmp[32];
	sprintf(tmp,"V%02d[%d],$%X(%s)",rd,(int)el,Imm,dasmRegName[rs]);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGeneric1(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd)
{
	char tmp[32];
	sprintf(tmp,"%s",dasmRegName[rd]);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGeneric2(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd,BYTE rs)
{
	char tmp[32];
	sprintf(tmp,"%s,%s",dasmRegName[rd],dasmRegName[rs]);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGeneric3(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd,BYTE rt,BYTE rs)
{
	char tmp[32];
	sprintf(tmp,"%s,%s,%s",dasmRegName[rd],dasmRegName[rt],dasmRegName[rs]);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGenericShift(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd,BYTE rt,BYTE sa)
{
	char tmp[32];
	sprintf(tmp,"%s,%s,$%X",dasmRegName[rd],dasmRegName[rt],sa);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGenericFP2(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd,BYTE rs)
{
	char tmp[32];
	sprintf(tmp,"%d,%d",rd,rs);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGenericFP3(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd,BYTE rt,BYTE rs)
{
	char tmp[32];
	sprintf(tmp,"%d,%d,%d",rd,rt,rs);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}


WORD dasmOpGenericVector(BYTE *cp,char *ins,char *com1,char *com2,BYTE vd,BYTE vs1,BYTE vs2,BYTE vsel)
{
	char tmp[32];
	sprintf(tmp,"%d,%d,%d[%d]",vd,vs1,vs2,vsel);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGenericBranchI(BYTE *cp,char *ins,char *com1,char *com2,DWORD addy)
{
	char tmp[32];
	sprintf(tmp,"%X",addy);

	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGenericBranch(BYTE *cp,char *ins,char *com1,char *com2,BYTE op0)
{
	char tmp[32];
	sprintf(tmp,"%s",dasmRegName[op0]);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}


WORD dasmOpGenericBranch2(BYTE *cp,char *ins,char *com1,char *com2,BYTE op0,BYTE op1)
{
	char tmp[32];
	sprintf(tmp,"%s,%s",dasmRegName[op0],dasmRegName[op1]);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGenericCompare(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd,BYTE rs,DWORD addy)
{
	char tmp[32];
	sprintf(tmp,"%s,%s %X",dasmRegName[rd],dasmRegName[rs],addy);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

WORD dasmOpGenericCompareZero(BYTE *cp,char *ins,char *com1,char *com2,BYTE rd,DWORD addy)
{
	char tmp[32];
	sprintf(tmp,"%s %X",dasmRegName[rd],addy);
	
	return(sprintf((char *)cp,"%X %s %s %s %s",dasmPC,dasmFormatIns(ins),dasmFormatParam(tmp),
												dasmFormatComment(com1),dasmFormatComment2(com2)));
}

#include "interpGeneral.h"
#include "interpMemory.h"

#include "dasmLookups.h" // tries to be smart about load/store addresses and branch address
#include "dasmFP.h"		// floating point routines
#include "dasmMemory.h" // memory routines
#include "dasmGeneral.h" // general routines
#include "dasmBranch.h" // branch instructions
#include "dasmVector.h" // RSP vector instructions


WORD dasm_sra(BYTE *cp)
{
	return(dasmOpSra(cp,DMAKE_RD,DMAKE_RT,DMAKE_SA));
}

WORD dasm_sll(BYTE *cp)
{
	if((DMAKE_RD==0)&&(DMAKE_RT==0)&&(DMAKE_SA==0))
	{
		return(dasmNOP(cp));
	}
	return(dasmOpSll(cp,DMAKE_RD,DMAKE_RT,DMAKE_SA));
}

WORD dasm_reserved(BYTE *cp)
{
	return(sprintf((char *)cp,"Reserved "));
}

WORD dasm_srl(BYTE *cp)
{
	return(dasmOpSrl(cp,DMAKE_RD,DMAKE_RT,DMAKE_SA));
}

WORD dasm_sllv(BYTE *cp)
{
	WORD len=0;
	len+=dasmOpSllV(cp,DMAKE_RD,DMAKE_RT,DMAKE_RS);
	return(len);
}

WORD dasm_srlv(BYTE *cp)
{
	WORD len=0;
	len+=dasmOpSrlV(cp,DMAKE_RD,DMAKE_RT,DMAKE_RS);
	return(len);
}

WORD dasm_srav(BYTE *cp)
{
	WORD len=0;
	len+=dasmOpSraV(cp,DMAKE_RD,DMAKE_RT,DMAKE_RS);
	return(len);
}

WORD dasm_syscall(BYTE *cp)
{
	return(dasmOpGeneric(cp,"SysCall "));
}

WORD dasm_Break(BYTE *cp)
{
	return(dasmOpGeneric(cp,"Break "));
}

WORD dasm_sync(BYTE *cp)
{
	return(dasmOpGeneric(cp,"Sync "));
}

WORD dasm_mfhi(BYTE *cp)
{
	return(dasmOpMfHi(cp,DMAKE_RD));
}

WORD dasm_mthi(BYTE *cp)
{
	return(dasmOpMtHi(cp,DMAKE_RS));
}

WORD dasm_mflo(BYTE *cp)
{
	return(dasmOpMfLo(cp,DMAKE_RD));
}

WORD dasm_mtlo(BYTE *cp)
{
	return(dasmOpMtLo(cp,DMAKE_RS));
}

WORD dasm_dsllv(BYTE *cp)
{
	return(dasmOpDSllV(cp,DMAKE_RD,DMAKE_RT,DMAKE_RS));
}

WORD dasm_dsrlv(BYTE *cp)
{
	return(dasmOpDSrlV(cp,DMAKE_RD,DMAKE_RT,DMAKE_RS));
}

WORD dasm_dsrav(BYTE *cp)
{
	return(dasmOpDSraV(cp,DMAKE_RD,DMAKE_RT,DMAKE_RS));
}

WORD dasm_mult(BYTE *cp)
{
	return(dasmOpMult(cp,DMAKE_RS,DMAKE_RT));
}

WORD dasm_multu(BYTE *cp)
{
	return(dasmOpMultU(cp,DMAKE_RS,DMAKE_RT));
}

WORD dasm_div(BYTE *cp)
{
	return(dasmOpDiv(cp,DMAKE_RS,DMAKE_RT));
}

WORD dasm_divu(BYTE *cp)
{
	return(dasmOpDivU(cp,DMAKE_RS,DMAKE_RT));
}

WORD dasm_dmult(BYTE *cp)
{
	return(dasmOpDMult(cp,DMAKE_RS,DMAKE_RT));
}

WORD dasm_dmultu(BYTE *cp)
{
	return(dasmOpDMultU(cp,DMAKE_RS,DMAKE_RT));
}

WORD dasm_ddiv(BYTE *cp)
{
	return(dasmOpDDiv(cp,DMAKE_RS,DMAKE_RT));
}

WORD dasm_ddivu(BYTE *cp)
{
	return(dasmOpDDivU(cp,DMAKE_RS,DMAKE_RT));
}

WORD dasm_add(BYTE *cp)
{
#ifdef GPR0_OPTIMIZATION
	if(DMAKE_RS==0)
	{
		return(dasmOpRegMove(cp,DMAKE_RD,DMAKE_RT));
	}
	if(DMAKE_RT==0)
	{
		return(dasmOpRegMove(cp,DMAKE_RD,DMAKE_RS));
	}
#endif
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpAddEqual(cp,DMAKE_RD,DMAKE_RT));
	}
	return(dasmOpAdd(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_addu(BYTE *cp)
{
#ifdef GPR0_OPTIMIZATION
	if(DMAKE_RS==0)
	{
		return(dasmOpRegMove(cp,DMAKE_RD,DMAKE_RT));
	}
	if(DMAKE_RT==0)
	{
		return(dasmOpRegMove(cp,DMAKE_RD,DMAKE_RS));
	}
#endif
#ifdef RD_EQUAL_RS
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpAddEqual(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
	return(dasmOpAdd(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_sub(BYTE *cp)
{
#ifdef GPR0_OPTIMIZATION
	if(DMAKE_RT==0)
	{
		return(dasmOpRegMove(cp,DMAKE_RD,DMAKE_RS));
	}
#endif
#ifdef RD_EQUAL_RS
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpSubEqual(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
	return(dasmOpSub(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_subu(BYTE *cp)
{
#ifdef GPR0_OPTIMIZATION
	if(DMAKE_RT==0)
	{
		return(dasmOpRegMove(cp,DMAKE_RD,DMAKE_RS));
	}
#endif
#ifdef RD_EQUAL_RS
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpSubEqual(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
	return(dasmOpSub(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_and(BYTE *cp)
{
#ifdef RD_EQUAL_RS
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpAndEqual(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
	return(dasmOpAnd(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_or(BYTE *cp)
{
#ifdef GPR0_OPTIMIZATION
	if(DMAKE_RS==0)
	{
		return(dasmOpRegMove(cp,DMAKE_RD,DMAKE_RT));
	}
	if(DMAKE_RT==0)
	{
		return(dasmOpRegMove(cp,DMAKE_RD,DMAKE_RS));
	}
#endif
#ifdef RD_EQUAL_RS
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpOrEqual(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
	return(dasmOpOr(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_xor(BYTE *cp)
{
#ifdef RD_EQUAL_RS
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpXorEqual(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
	return(dasmOpXor(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_nor(BYTE *cp)
{
#ifdef RD_EQUAL_RS
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpNorEqual(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
	return(dasmOpNor(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_slt(BYTE *cp)
{
	return(dasmOpSlt(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_sltu(BYTE *cp)
{
	return(dasmOpSltU(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_dadd(BYTE *cp)
{
	WORD l=0;

#ifdef GPR0_OPTIMIZATION
	if(DMAKE_RS==0)
	{
		return(dasmOpRegDMove(cp,DMAKE_RD,DMAKE_RT));
	}
	if(DMAKE_RT==0)
	{
		return(dasmOpRegDMove(cp,DMAKE_RD,DMAKE_RS));
	}
#endif
#ifdef RD_EQUAL_RS
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpDAddEqual(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
	return(dasmOpDAdd(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_daddu(BYTE *cp)
{
	WORD l=0;
#ifdef GPR0_OPTIMIZATION
	if(DMAKE_RS==0)
	{
		return(dasmOpRegDMove(cp,DMAKE_RD,DMAKE_RT));
	}
	if(DMAKE_RT==0)
	{
		return(dasmOpRegDMove(cp,DMAKE_RD,DMAKE_RS));
	}
#endif
#ifdef RD_EQUAL_RS
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpDAddEqual(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
	return(dasmOpDAdd(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_dsub(BYTE *cp)
{
	WORD l=0;

#ifdef GPR0_OPTIMIZATION
	if(DMAKE_RS==0)
	{
		return(dasmOpRegDMove(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
#ifdef RD_EQUAL_RS
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpDSubEqual(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
	return(dasmOpDSub(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_dsubu(BYTE *cp)
{
	WORD l=0;

#ifdef GPR0_OPTIMIZATION
	if(DMAKE_RS==0)
	{
		return(dasmOpRegDMove(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
#ifdef RD_EQUAL_RS
	if(DMAKE_RD==DMAKE_RS)
	{
		return(dasmOpDSubEqual(cp,DMAKE_RD,DMAKE_RT));
	}
#endif
	return(dasmOpDSub(cp,DMAKE_RD,DMAKE_RS,DMAKE_RT));
}

WORD dasm_tge(BYTE *cp)
{
	return(sprintf((char *)cp,"TGE "));
}

WORD dasm_tgeu(BYTE *cp)
{
	return(sprintf((char *)cp,"TGEU "));
}

WORD dasm_tlt(BYTE *cp)
{
	return(sprintf((char *)cp,"TLT "));
}

WORD dasm_tltu(BYTE *cp)
{
	return(sprintf((char *)cp,"TLTU "));
}

WORD dasm_teq(BYTE *cp)
{
	return(sprintf((char *)cp,"TEQ "));
}

WORD dasm_tne(BYTE *cp)
{
	return(sprintf((char *)cp,"TNE "));
}

WORD dasm_dsll(BYTE *cp)
{
	WORD l=0;
	return(dasmOpDSll(cp,DMAKE_RD,DMAKE_RT,DMAKE_SA));
}

WORD dasm_dsrl(BYTE *cp)
{
	WORD l=0;
	return(dasmOpDSrl(cp,DMAKE_RD,DMAKE_RT,DMAKE_SA));
}

WORD dasm_dsra(BYTE *cp)
{
	WORD l=0;
	return(dasmOpDSra(cp,DMAKE_RD,DMAKE_RT,DMAKE_SA));
}

WORD dasm_dsll32(BYTE *cp)
{
	return(dasmOpDSll32(cp,DMAKE_RD,DMAKE_RT,DMAKE_SA));
}

WORD dasm_dsrl32(BYTE *cp)
{
	return(dasmOpDSrl32(cp,DMAKE_RD,DMAKE_RT,DMAKE_SA));
}

WORD dasm_dsra32(BYTE *cp)
{
	return(dasmOpDSra32(cp,DMAKE_RD,DMAKE_RT,DMAKE_SA));
}

WORD dasm_special(BYTE *cp)
{
	return(Special[dasmOpCode&0x3f](cp));
}

WORD dasm_regimm(BYTE *cp)
{
	return(Regimm[(dasmOpCode>>16)&0x1f](cp));
}

WORD dasm_addi(BYTE *cp)
{
#ifdef GPR0_OPTIMIZATION
	if(DMAKE_RS==0)
	{
		return(dasmOpLoadI(cp,DMAKE_RT,DMAKE_I));
	}
#endif
#ifdef RD_EQUAL_RS
	if(DMAKE_RT==DMAKE_RS)
	{
		return(dasmOpAddIEqual(cp,DMAKE_RT,DMAKE_I));
	}
	if(DMAKE_I==0)
	{
		return(dasmOpAddEqual(cp,DMAKE_RT,DMAKE_RS));
	}
#endif
	return(dasmOpAddI(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_addiu(BYTE *cp)
{
#ifdef GPR0_OPTIMIZATION
	if(DMAKE_RS==0)
	{
		return(dasmOpLoadI(cp,DMAKE_RT,DMAKE_I));
	}
#endif
#ifdef RD_EQUAL_RS
	if(DMAKE_RT==DMAKE_RS)
	{
		return(dasmOpAddIEqual(cp,DMAKE_RT,DMAKE_I));
	}
	if(DMAKE_I==0)
	{
		return(dasmOpAddEqual(cp,DMAKE_RT,DMAKE_RS));
	}
#endif
	return(dasmOpAddI(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_slti(BYTE *cp)
{
	return(dasmOpSltI(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_sltiu(BYTE *cp)
{
	return(dasmOpSltIU(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_andi(BYTE *cp)
{
#ifdef RD_EQUAL_RS
	if(DMAKE_RT==DMAKE_RS)
	{
		return(dasmOpAndIEqual(cp,DMAKE_RT,DMAKE_IU));
	}
#endif
	return(dasmOpAndI(cp,DMAKE_RT,DMAKE_RS,DMAKE_IU));
}

WORD dasm_ori(BYTE *cp)
{
#ifdef RD_EQUAL_RS
	if(DMAKE_RT==DMAKE_RS)
	{
		return(dasmOpOrIEqual(cp,DMAKE_RT,DMAKE_IU));
	}
#endif
	return(dasmOpOrI(cp,DMAKE_RT,DMAKE_RS,DMAKE_IU));
}

WORD dasm_xori(BYTE *cp)
{
#ifdef RD_EQUAL_RS
	if(DMAKE_RT==DMAKE_RS)
	{
		return(dasmOpXorIEqual(cp,DMAKE_RT,DMAKE_IU));
	}
#endif
	return(dasmOpXorI(cp,DMAKE_RT,DMAKE_RS,DMAKE_IU));
}

WORD dasm_lui(BYTE *cp)
{
	return(dasmOpLui(cp,DMAKE_RT,DMAKE_I));
}

WORD dasm_cop0(BYTE *cp)
{
	return(Cop0RS[(dasmOpCode>>21)&0x1f](cp));
}

WORD dasm_cop1(BYTE *cp)
{
	return(Cop1RS[(dasmOpCode>>21)&0x1f](cp));
}

WORD dasm_cop2(BYTE *cp)
{
	return(Cop2RS[(dasmOpCode>>21)&0x1f](cp));
//	return(sprintf((char *)cp,"Cop2 "));
}

WORD dasm_daddi(BYTE *cp)
{
	WORD l=0;
	return(dasmOpDAddI(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_daddiu(BYTE *cp)
{
	WORD l=0;
	return(dasmOpDAddI(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_ldl(BYTE *cp)
{
	return(dasmOpLdl(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_ldr(BYTE *cp)
{
	return(dasmOpLdr(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_lb(BYTE *cp)
{
	return(dasmOpLb(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_lh(BYTE *cp)
{
	return(dasmOpLh(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_lwl(BYTE *cp)
{
	return(dasmOpLwl(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_lw(BYTE *cp)
{
	return(dasmOpLw(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_lbu(BYTE *cp)
{
	return(dasmOpLbu(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_lhu(BYTE *cp)
{
	return(dasmOpLhu(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_lwr(BYTE *cp)
{
	return(dasmOpLwr(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_lwu(BYTE *cp)
{
	return(dasmOpLwu(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_sb(BYTE *cp)
{
	return(dasmOpSb(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_sh(BYTE *cp)
{
	return(dasmOpSh(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_swl(BYTE *cp)
{
	return(dasmOpSwl(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_sw(BYTE *cp)
{
	return(dasmOpSw(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_sdl(BYTE *cp)
{
	return(dasmOpSdl(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_sdr(BYTE *cp)
{
	return(dasmOpSdr(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_swr(BYTE *cp)
{
	return(dasmOpSwr(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_cache(BYTE *cp)
{
	return(dasmOpGeneric(cp,"Cache "));
}

WORD dasm_ll(BYTE *cp)
{
	return(dasmOpLl(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_lwc1(BYTE *cp)
{
	return(dasmOpLwc1(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_lwc2(BYTE *cp)
{
	if(dasmRSPMode)
		return(dasmOpVectorLwc2(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
	else
		return(dasmOpLwc2(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_lld(BYTE *cp)
{
	return(dasmOpLld(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_ldc1(BYTE *cp)
{
	return(dasmOpLdc1(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_ldc2(BYTE *cp)
{
	return(dasmOpLdc2(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_ld(BYTE *cp)
{
	return(dasmOpLd(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_sc(BYTE *cp)
{
	return(dasmOpSc(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_swc1(BYTE *cp)
{
	return(dasmOpSwc1(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_swc2(BYTE *cp)
{
	if(dasmRSPMode)
		return(dasmOpVectorSwc2(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
	else
		return(dasmOpSwc2(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_scd(BYTE *cp)
{
	return(dasmOpScd(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_sdc1(BYTE *cp)
{
	return(dasmOpSdc1(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_sdc2(BYTE *cp)
{
	return(dasmOpSdc2(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_sd(BYTE *cp)
{
	return(dasmOpSd(cp,DMAKE_RT,DMAKE_RS,DMAKE_I));
}

WORD dasm_tgei(BYTE *cp)
{
	return(sprintf((char *)cp,"TGEI "));
}

WORD dasm_tgeiu(BYTE *cp)
{
	return(sprintf((char *)cp,"TGEIU "));
}

WORD dasm_tlti(BYTE *cp)
{
	return(sprintf((char *)cp,"TLTI "));
}

WORD dasm_tltiu(BYTE *cp)
{
	return(sprintf((char *)cp,"TLTIU "));
}

WORD dasm_teqi(BYTE *cp)
{
	return(sprintf((char *)cp,"TEQI "));
}

WORD dasm_tnei(BYTE *cp)
{
	return(sprintf((char *)cp,"TNE "));
}

WORD dasm_cop0_rs_mf(BYTE *cp)
{
	return(dasmOpRs0Mf(cp,DMAKE_RT,DMAKE_RD));
}

WORD dasm_cop0_rs_dmf(BYTE *cp)
{
	return(dasmOpRs0DMf(cp,DMAKE_RT,DMAKE_RD));
}

WORD dasm_cop0_rs_cf(BYTE *cp)
{
	return(dasmOpRs0Cf(cp,DMAKE_RT,DMAKE_RD));
}

WORD dasm_cop0_rs_mt(BYTE *cp)
{
	return(dasmOpRs0Mt(cp,DMAKE_RD,DMAKE_RT));
}

WORD dasm_cop0_rs_dmt(BYTE *cp)
{
	return(dasmOpRs0DMt(cp,DMAKE_RD,DMAKE_RT));
}

WORD dasm_cop0_rs_ct(BYTE *cp)
{
	return(dasmOpRs0Ct(cp,DMAKE_RD,DMAKE_RT));
}

WORD dasm_cop0_rs_bc(BYTE *cp)
{
	return(Cop0RT[(dasmOpCode>>16)&0x1f](cp));
}

WORD dasm_cop0_rs_co(BYTE *cp)
{
	return(Cop0[dasmOpCode&0x3f](cp));
}

WORD dasm_cop0_rt_bcf(BYTE *cp)
{
	return(sprintf((char *)cp,"Bcf0 "));
}

WORD dasm_cop0_rt_bct(BYTE *cp)
{
	return(sprintf((char *)cp,"Bct0 "));
}

WORD dasm_cop0_rt_bcfl(BYTE *cp)
{
	return(sprintf((char *)cp,"Bcfl0 "));
}

WORD dasm_cop0_rt_bctl(BYTE *cp)
{
	return(sprintf((char *)cp,"Bctl0 "));
}

WORD dasm_cop0_tlbr(BYTE *cp)
{
	return(dasmOpTlbR(cp));
}

WORD dasm_cop0_tlbwi(BYTE *cp)
{
	return(dasmOpTlbWi(cp));
}

WORD dasm_cop0_tlbwr(BYTE *cp)
{
	return(dasmOpTlbWr(cp));
}

WORD dasm_cop0_tlbp(BYTE *cp)
{
	return(dasmOpTlbP(cp));
}

WORD dasm_cop0_eret(BYTE *cp)
{
	return(dasmOpEret(cp));
}

WORD dasm_cop1_rs_mf(BYTE *cp)
{
	return(dasmOpRs1Mf(cp,DMAKE_RT,DMAKE_RD));
}

WORD dasm_cop1_rs_dmf(BYTE *cp)
{
	return(dasmOpRs1DMf(cp,DMAKE_RT,DMAKE_RD));
}

WORD dasm_cop1_rs_cf(BYTE *cp)
{
	return(dasmOpRs1Cf(cp,DMAKE_RT,DMAKE_RD));
}

WORD dasm_cop1_rs_mt(BYTE *cp)
{
	return(dasmOpRs1Mt(cp,DMAKE_RD,DMAKE_RT));
}

WORD dasm_cop1_rs_dmt(BYTE *cp)
{
	return(dasmOpRs1DMt(cp,DMAKE_RD,DMAKE_RT));
}

WORD dasm_cop1_rs_ct(BYTE *cp)
{
	return(dasmOpRs1Ct(cp,DMAKE_RD,DMAKE_RT));
}

WORD dasm_cop1_rs_bc(BYTE *cp)
{
	return(Cop1RT[(dasmOpCode>>16)&0x1f](cp));
}

WORD dasm_cop1_rs_co(BYTE *cp)
{
	return(Cop1[dasmOpCode&0x3f](cp));
}

WORD dasm_cop1_add(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPAddS(cp,DMAKE_FD,DMAKE_FS,DMAKE_FT));
	}
	return(dasmOpFPAddD(cp,DMAKE_FD,DMAKE_FS,DMAKE_FT));
}

WORD dasm_cop1_sub(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPSubS(cp,DMAKE_FD,DMAKE_FS,DMAKE_FT));
	}
	return(dasmOpFPSubD(cp,DMAKE_FD,DMAKE_FS,DMAKE_FT));
}

WORD dasm_cop1_mul(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPMulS(cp,DMAKE_FD,DMAKE_FS,DMAKE_FT));
	}
	return(dasmOpFPMulD(cp,DMAKE_FD,DMAKE_FS,DMAKE_FT));
}

WORD dasm_cop1_div(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPDivS(cp,DMAKE_FD,DMAKE_FS,DMAKE_FT));
	}
	return(dasmOpFPDivD(cp,DMAKE_FD,DMAKE_FS,DMAKE_FT));
}

WORD dasm_cop1_sqrt(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPSqrtS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPSqrtD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_abs(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPAbsS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPAbsD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_mov(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPMovS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPMovD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_neg(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPNegS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPNegD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_roundl(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPRoundLS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPRoundLD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_truncl(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPTruncLS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPTruncLD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_ceill(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPCeilLS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPCeilLD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_floorl(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPFloorLS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPFloorLD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_roundw(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPRoundWS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPRoundWD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_truncw(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPTruncWS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPTruncWD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_ceilw(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPCeilWS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPCeilWD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_floorw(BYTE *cp)
{
	if(DMAKE_FMT==FMT_S)
	{
		return(dasmOpFPFloorWS(cp,DMAKE_FD,DMAKE_FS));
	}
	return(dasmOpFPFloorWD(cp,DMAKE_FD,DMAKE_FS));
}

WORD dasm_cop1_cvts(BYTE *cp)
{
	switch(DMAKE_FMT)
	{
	case FMT_S:
		return(sprintf((char *)cp,"CvtSFromS "));
		break;
	case FMT_D:
		return(dasmOpFPCvtSFromD(cp,DMAKE_FD,DMAKE_FS));
		break;
	case FMT_W:
		return(dasmOpFPCvtSFromW(cp,DMAKE_FD,DMAKE_FS));
		break;
	case FMT_L:
		return(dasmOpFPCvtSFromL(cp,DMAKE_FD,DMAKE_FS));
		break;
	}
	return(dasmNOP(cp));
}

WORD dasm_cop1_cvtd(BYTE *cp)
{
	switch(DMAKE_FMT)
	{
	case FMT_S:
		return(dasmOpFPCvtDFromS(cp,DMAKE_FD,DMAKE_FS));
		break;
	case FMT_D:
		return(sprintf((char *)cp,"CvtDFromD "));
		break;
	case FMT_W:
		return(dasmOpFPCvtDFromW(cp,DMAKE_FD,DMAKE_FS));
		break;
	case FMT_L:
		return(dasmOpFPCvtDFromL(cp,DMAKE_FD,DMAKE_FS));
		break;
	}
	return(dasmNOP(cp));
}

WORD dasm_cop1_cvtw(BYTE *cp)
{
	switch(DMAKE_FMT)
	{
	case FMT_S:
		return(dasmOpFPCvtWFromS(cp,DMAKE_FD,DMAKE_FS));
		break;
	case FMT_D:
		return(dasmOpFPCvtWFromD(cp,DMAKE_FD,DMAKE_FS));
		break;
	case FMT_W:
		return(sprintf((char *)cp,"CvtWFromW "));
		break;
	case FMT_L:
		return(dasmOpFPCvtWFromL(cp,DMAKE_FD,DMAKE_FS));
		break;
	}
	return(dasmNOP(cp));
}

WORD dasm_cop1_cvtl(BYTE *cp)
{
	switch(DMAKE_FMT)
	{
	case FMT_S:
		return(dasmOpFPCvtLFromS(cp,DMAKE_FD,DMAKE_FS));
		break;
	case FMT_D:
		return(dasmOpFPCvtLFromD(cp,DMAKE_FD,DMAKE_FS));
		break;
	case FMT_W:
		return(dasmOpFPCvtLFromW(cp,DMAKE_FD,DMAKE_FS));
		break;
	case FMT_L:
		return(sprintf((char *)cp,"CvtLFromL "));
		break;
	}
	return(dasmNOP(cp));
}

WORD dasm_cop1_c(BYTE *cp)
{
	switch(DMAKE_FMT)
	{
	case FMT_S:   /* c.*.s */
		{
		switch(DMAKE_F)
		{
		case 0x30:   /* c.f */
			return(dasmOpFPSetFalse(cp));
			break;
			
		case 0x31:   /* c.un */
			return(sprintf((char *)cp,"c.un "));
			break;
			
		case 0x32:   /* c.eq */
			return(dasmOpFPEqualS(cp,DMAKE_FS,DMAKE_FT));
			break;
			
		case 0x33:   /* c.ueq */
			return(dasmOpFPEqualS(cp,DMAKE_FS,DMAKE_FT));
			break;
			
		case 0x34:   /* c.olt */
		case 0x35:   /* c.ult */
		case 0x36:   /* c.ole */
		case 0x37:   /* c.ule */
		case 0x38:   /* c.sf */
		case 0x39:   /* c.ngle */
		case 0x3a:   /* c.seq */
		case 0x3b:   /* c.ngl */
			return(sprintf((char *)cp,"c.olt or others %X",DMAKE_F));
			break;
			
		case 0x3c:   /* c.lt */
			return(dasmOpFPLessThanS(cp,DMAKE_FS,DMAKE_FT));
			break;
			
		case 0x3d:   /* c.nge */
			return(dasmOpFPLessThanS(cp,DMAKE_FS,DMAKE_FT));
			break;
			
		case 0x3e:   /* c.le */
			return(dasmOpFPLessThanEqualS(cp,DMAKE_FS,DMAKE_FT));
			break;
			
		case 0x3f:   /* c.ngt */
			return(dasmOpFPLessThanEqualS(cp,DMAKE_FS,DMAKE_FT));
			break;
			
		} /* switch(__F) */
		
		break;
		}
		
		case FMT_D:   /* c.*.d */
			{
			switch(DMAKE_F)
			{
			case 0x30:   /* c.f */
				return(dasmOpFPSetFalse(cp));
				break;
				
			case 0x31:   /* c.un */
				return(sprintf((char *)cp,"c.un "));
				break;
				
			case 0x32:   /* c.eq */
				return(dasmOpFPEqualD(cp,DMAKE_FS,DMAKE_FT));
				break;
				
			case 0x33:   /* c.ueq */
				return(dasmOpFPEqualD(cp,DMAKE_FS,DMAKE_FT));
				break;
				
			case 0x34:   /* c.olt */
			case 0x35:   /* c.ult */
			case 0x36:   /* c.ole */
			case 0x37:   /* c.ule */
			case 0x38:   /* c.sf */
			case 0x39:   /* c.ngle */
			case 0x3a:   /* c.seq */
			case 0x3b:   /* c.ngl */
				return(sprintf((char *)cp,"c.olt or others "));
				break;
				
			case 0x3c:   /* c.lt */
				return(dasmOpFPLessThanD(cp,DMAKE_FS,DMAKE_FT));
				break;
				
			case 0x3d:   /* c.nge */
				return(dasmOpFPLessThanD(cp,DMAKE_FS,DMAKE_FT));
				break;
				
			case 0x3e:   /* c.le */
				return(dasmOpFPLessThanEqualD(cp,DMAKE_FS,DMAKE_FT));
				break;
				
			case 0x3f:   /* c.ngt */
				return(dasmOpFPLessThanEqualD(cp,DMAKE_FS,DMAKE_FT));
				break;
				
			} /* switch(__F) */
			}
				
    } /* switch(__FMT) */
	return(dasmNOP(cp));
}

WORD dasm_cop2_rs_not_implemented(BYTE *cp)
{
	return(sprintf((char *)cp,"Cop2Rs ni "));
}

WORD dasm_jr(BYTE *cp)
{
	WORD len=dasmOpJr(cp,DMAKE_RS);
	return(len);

}

WORD dasm_jalr(BYTE *cp)
{
	WORD len;
	len=dasmOpJalr(cp,DMAKE_RD,DMAKE_RS);
	return(len);
}


WORD dasm_j(BYTE *cp)
{
	DWORD t=DMAKE_T;
	if(((DMAKE_T&0x3fffff)==(dasmPC&0x3fffff))) //&&(dasmNextOpCode==0))
	{
		return(dasmOpILoop(cp,DMAKE_T));
	}
	WORD len=dasmOpJ(cp,DMAKE_T);
	return(len);

}

WORD dasm_jal(BYTE *cp)
{
	WORD len;
	len=dasmOpJal(cp,DMAKE_T);
	return(len);
}

WORD dasm_beq(BYTE *cp)
{
	if(((DMAKE_O&0x3fffff)==(dasmPC&0x3fffff))&&(dasmNextOpCode==0))
	{
		return(dasmOpBeqILoop(cp,DMAKE_RT,DMAKE_RS,DMAKE_O));
	}

	WORD len=0;
	len=dasmOpBeq(cp,DMAKE_RT,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bne(BYTE *cp)
{
	if(((DMAKE_O&0x3fffff)==(dasmPC&0x3fffff))) //&&(dasmNextOpCode==0))
	{
		return(dasmOpBneILoop(cp,DMAKE_RT,DMAKE_RS,DMAKE_O));
	}

	WORD len;
	len=dasmOpBne(cp,DMAKE_RT,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_blez(BYTE *cp)
{
	WORD len;
	len=dasmOpBlez(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bgtz(BYTE *cp)
{
	WORD len;
	len=dasmOpBgtz(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_beql(BYTE *cp)
{
	WORD len;
	len=dasmOpBeql(cp,DMAKE_RT,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bnel(BYTE *cp)
{
	WORD len;
	len=dasmOpBnel(cp,DMAKE_RT,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_blezl(BYTE *cp)
{
	WORD len;
	len=dasmOpBlezl(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bgtzl(BYTE *cp)
{
	WORD len;
	len=dasmOpBgtzl(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bltz(BYTE *cp)
{
	WORD len;
	len=dasmOpBltz(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bgez(BYTE *cp)
{
	WORD len;
	len=dasmOpBgez(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bltzl(BYTE *cp)
{
	WORD len;
	len=dasmOpBltzl(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bgezl(BYTE *cp)
{
	WORD len;
	len=dasmOpBgezl(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bltzal(BYTE *cp)
{
	WORD len;
	len=dasmOpBltzal(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bgezal(BYTE *cp)
{
	WORD len;
	len=dasmOpBgezal(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bltzall(BYTE *cp)
{
	WORD len;
	len=dasmOpBltzall(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_bgezall(BYTE *cp)
{
	WORD len;
	len=dasmOpBgezall(cp,DMAKE_RS,DMAKE_O);
	return(len);
}

WORD dasm_cop1_rt_bcf(BYTE *cp)
{
	WORD len;
	len=dasmOpBcf(cp,DMAKE_O);
	return(len);
}

WORD dasm_cop1_rt_bct(BYTE *cp)
{
	WORD len;
	len=dasmOpBct(cp,DMAKE_O);
	return(len);
}

WORD dasm_cop1_rt_bcfl(BYTE *cp)
{
	WORD len;
	len=dasmOpBcfl(cp,DMAKE_O);
	return(len);
}

WORD dasm_cop1_rt_bctl(BYTE *cp)
{
	WORD len;
	len=dasmOpBctl(cp,DMAKE_O);
	return(len);
}

WORD dasm_invalid(BYTE *cp)
{
	return(sprintf((char *)cp,"Invalid "));
}

WORD dasm_cop2_reserved(BYTE *cp)
{
	return(sprintf((char *)cp,"Cop2 Reserved"));
}

WORD dasm_cop2_mf(BYTE *cp)
{
	WORD len;
	return(dasmOpRs2Mf(cp,DMAKE_RT,DMAKE_RD));
}

WORD dasm_cop2_cf(BYTE *cp)
{
	WORD len;
	return(dasmOpRs2Cf(cp,DMAKE_RT,DMAKE_RD));
}

WORD dasm_cop2_mt(BYTE *cp)
{
	WORD len;
	return(dasmOpRs2Mt(cp,DMAKE_RT,DMAKE_RD));
}

WORD dasm_cop2_ct(BYTE *cp)
{
	WORD len;
	return(dasmOpRs2Ct(cp,DMAKE_RT,DMAKE_RD));
}

WORD dasm_cop2_vectop(BYTE *cp)
{
	return(Cop2Vector[dasmOpCode&0x3f](cp));
}

WORD dasm_cop2_vmulf(BYTE *cp)
{
	return(dasmOpVmulf(cp));
}

WORD dasm_cop2_vmulu(BYTE *cp)
{
	return(dasmOpVmulu(cp));
}

WORD dasm_cop2_vrndp(BYTE *cp)
{
	return(dasmOpVrndp(cp));
}

WORD dasm_cop2_vmulq(BYTE *cp)
{
	return(dasmOpVmulq(cp));
}

WORD dasm_cop2_vmudl(BYTE *cp)
{
	return(dasmOpVmudl(cp));
}

WORD dasm_cop2_vmudm(BYTE *cp)
{
	return(dasmOpVmudm(cp));
}

WORD dasm_cop2_vmudn(BYTE *cp)
{
	return(dasmOpVmudn(cp));
}

WORD dasm_cop2_vmudh(BYTE *cp)
{
	return(dasmOpVmudh(cp));
}

WORD dasm_cop2_vmacf(BYTE *cp)
{
	return(dasmOpVmacf(cp));
}

WORD dasm_cop2_vmacu(BYTE *cp)
{
	return(dasmOpVmacu(cp));
}

WORD dasm_cop2_vrndn(BYTE *cp)
{
	return(dasmOpVrndn(cp));
}

WORD dasm_cop2_vmacq(BYTE *cp)
{
	return(dasmOpVmacq(cp));
}

WORD dasm_cop2_vmadl(BYTE *cp)
{
	return(dasmOpVmadl(cp));
}

WORD dasm_cop2_vmadm(BYTE *cp)
{
	return(dasmOpVmadm(cp));
}

WORD dasm_cop2_vmadn(BYTE *cp)
{
	return(dasmOpVmadn(cp));
}

WORD dasm_cop2_vmadh(BYTE *cp)
{
	return(dasmOpVmadh(cp));
}

WORD dasm_cop2_vadd(BYTE *cp)
{
	return(dasmOpVadd(cp));
}

WORD dasm_cop2_vsub(BYTE *cp)
{
	return(dasmOpVsub(cp));
}

WORD dasm_cop2_vsut(BYTE *cp)
{
	return(dasmOpVsut(cp));
}

WORD dasm_cop2_vabs(BYTE *cp)
{
	return(dasmOpVabs(cp));
}

WORD dasm_cop2_vaddc(BYTE *cp)
{
	return(dasmOpVaddc(cp));
}

WORD dasm_cop2_vsubc(BYTE *cp)
{
	return(dasmOpVsubc(cp));
}

WORD dasm_cop2_vaddb(BYTE *cp)
{
	return(dasmOpVaddb(cp));
}

WORD dasm_cop2_vsubb(BYTE *cp)
{
	return(dasmOpVsubb(cp));
}

WORD dasm_cop2_vaccb(BYTE *cp)
{
	return(dasmOpVaccb(cp));
}

WORD dasm_cop2_vsucb(BYTE *cp)
{
	return(dasmOpVsucb(cp));
}

WORD dasm_cop2_vsad(BYTE *cp)
{
	return(dasmOpVsad(cp));
}

WORD dasm_cop2_vsac(BYTE *cp)
{
	return(dasmOpVsac(cp));
}

WORD dasm_cop2_vsum(BYTE *cp)
{
	return(dasmOpVsum(cp));
}

WORD dasm_cop2_vsaw(BYTE *cp)
{
	return(dasmOpVsaw(cp));
}

WORD dasm_cop2_vlt(BYTE *cp)
{
	return(dasmOpVlt(cp));
}

WORD dasm_cop2_veq(BYTE *cp)
{
	return(dasmOpVeq(cp));
}

WORD dasm_cop2_vne(BYTE *cp)
{
	return(dasmOpVne(cp));
}

WORD dasm_cop2_vge(BYTE *cp)
{
	return(dasmOpVge(cp));
}

WORD dasm_cop2_vcl(BYTE *cp)
{
	return(dasmOpVcl(cp));
}

WORD dasm_cop2_vch(BYTE *cp)
{
	return(dasmOpVch(cp));
}

WORD dasm_cop2_vcr(BYTE *cp)
{
	return(dasmOpVcr(cp));
}

WORD dasm_cop2_vmrg(BYTE *cp)
{
	return(dasmOpVmrg(cp));
}

WORD dasm_cop2_vand(BYTE *cp)
{
	return(dasmOpVand(cp));
}

WORD dasm_cop2_vnand(BYTE *cp)
{
	return(dasmOpVnand(cp));
}

WORD dasm_cop2_vor(BYTE *cp)
{
	return(dasmOpVor(cp));
}

WORD dasm_cop2_vnor(BYTE *cp)
{
	return(dasmOpVnor(cp));
}

WORD dasm_cop2_vxor(BYTE *cp)
{
	return(dasmOpVxor(cp));
}

WORD dasm_cop2_vnxor(BYTE *cp)
{
	return(dasmOpVnxor(cp));
}

WORD dasm_cop2_vrcp(BYTE *cp)
{
	return(dasmOpVrcp(cp));
}

WORD dasm_cop2_vrcpl(BYTE *cp)
{
	return(dasmOpVrcpl(cp));
}

WORD dasm_cop2_vrcph(BYTE *cp)
{
	return(dasmOpVrcph(cp));
}

WORD dasm_cop2_vmov(BYTE *cp)
{
	return(dasmOpVmov(cp));
}

WORD dasm_cop2_vrsq(BYTE *cp)
{
	return(dasmOpVrsq(cp));
}

WORD dasm_cop2_vrsql(BYTE *cp)
{
	return(dasmOpVrsql(cp));
}

WORD dasm_cop2_vrsqh(BYTE *cp)
{
	return(dasmOpVrsqh(cp));
}

WORD dasm_cop2_vnoop(BYTE *cp)
{
	return(dasmOpVnoop(cp));
}

WORD dasm_cop2_vextt(BYTE *cp)
{
	return(dasmOpVextt(cp));
}

WORD dasm_cop2_vextq(BYTE *cp)
{
	return(dasmOpVextq(cp));
}

WORD dasm_cop2_vextn(BYTE *cp)
{
	return(dasmOpVextn(cp));
}

WORD dasm_cop2_vinst(BYTE *cp)
{
	return(dasmOpVinst(cp));
}

WORD dasm_cop2_vinsq(BYTE *cp)
{
	return(dasmOpVinsq(cp));
}

WORD dasm_cop2_vinsn(BYTE *cp)
{
	return(dasmOpVinsn(cp));
}

