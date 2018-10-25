
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "iMain.h"

#include "iCPU.h"			// Core 4600 emulation routines
#include "dynaCompiler.h"
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines
#include "iATA.h"			// KI ATA-2 hard disk emulation

#include "adsp2100.h"
#include "hleDSP.h"

WORD *dspDMem;
DWORD *dspPMem;
WORD *dspRMem;
WORD dspBank;
WORD dspAuto;
WORD dspPoke;
WORD dspPeek=0;
WORD dspClkDiv;
WORD dspCtrl;
WORD dspIRQClear;
WORD dspAutoCount;
WORD dspIMASK=1;
DWORD dspAutoBase;
DWORD dspUpdateCount=0;
WORD peek1,peek2,peek3,peek4;
DWORD traceCount=0;
DWORD hleDSPVCurAddy;
WORD dspBitRev[0x4000];
int hleDSPPingTime=0;

extern DWORD stats;

void hleDSPConstruct()
{
	dspDMem=(WORD *)m->dspDMem;
	dspPMem=(DWORD *)m->dspPMem;
	dspRMem=(WORD *)m->dspRMem;
	for (int i = 0; i < 0x4000; i++)
	{
		WORD data = 0;

		data |= (i >> 13) & 0x0001;
		data |= (i >> 11) & 0x0002;
		data |= (i >> 9)  & 0x0004;
		data |= (i >> 7)  & 0x0008;
		data |= (i >> 5)  & 0x0010;
		data |= (i >> 3)  & 0x0020;
		data |= (i >> 1)  & 0x0040;
		data |= (i << 1)  & 0x0080;
		data |= (i << 3)  & 0x0100;
		data |= (i << 5)  & 0x0200;
		data |= (i << 7)  & 0x0400;
		data |= (i << 9)  & 0x0800;
		data |= (i << 11) & 0x1000;
		data |= (i << 13) & 0x2000;

		dspBitRev[i] = data;
	}
}

void hleDSPDestruct()
{
}

void hleDSPInit1()
{
	WORD *dst;
	DWORD *src;
	WORD count;

	//0x0800:	0x400108  	SI = $0010
	//0x0801:	0x0D0C18  	MSTAT = SI

	//0x0802:	0x340008  	L0 = $0000
	//0x0803:	0x340009  	L1 = $0000
	//0x0804:	0x34000A  	L2 = $0000
	//0x0805:	0x34000B  	L3 = $0000
	//0x0806:	0x380008  	L4 = $0000
	//0x0807:	0x380009  	L5 = $0000
	//0x0808:	0x38000A  	L6 = $0000
	//0x0809:	0x340004  	M0 = $0000
	//0x080A:	0x340015  	M1 = $0001
	//0x080B:	0x380004  	M4 = $0000
	//0x080C:	0x380015  	M5 = $0001
	//0x080D:	0x3B4002  	I6 = $3400

	//0x080E:	0x400000  	AX0 = $0000
	//0x080F:	0x400011  	AX1 = $0001
	//0x0810:	0x343000  	I0 = $0300
	//0x0811:	0x3C5005  	CNTR = $0500
	//0x0812:	0x14813E  	DO $0813 UNTIL CE
	//0x0813:	0x680001  	DM(I0,M1) = AX0
	memset(&dspDMem[0x300],0,0x500*2);

	//0x0814:	0x378000  	I0 = $3800
	//0x0815:	0x3C2005  	CNTR = $0200
	//0x0816:	0x14817E  	DO $0817 UNTIL CE
	//0x0817:	0x680001  	DM(I0,M1) = AX0
	memset(&dspDMem[0x3800],0,0x200*2);

	//0x0818:	0x404F08  	SI = $04F0
	//0x0819:	0x9060C8  	DM($060C) = SI
	//0x081A:	0x439108  	SI = $3910
	//0x081B:	0x905F88  	DM($05F8) = SI
	//0x081C:	0x905F98  	DM($05F9) = SI
	dspDMem[0x60c]=0x4f0;
	dspDMem[0x5f8]=0x3910;
	dspDMem[0x5f9]=0x3910;

	//0x081D:	0x384003  	I7 = $0400
	//0x081E:	0x381E0B  	L7 = $01E0
	dspAutoBase=0x400;
	dspAutoCount=0x1e0;

	//0x081F:	0x40009A  	AR = $0009
	//0x0820:	0x93FF1A  	DM($3FF1) = AR
	//0x0821:	0x44A9FA  	AR = $4A9F
	//0x0822:	0x93FF2A  	DM($3FF2) = AR
	//0x0823:	0x40C10A  	AR = $0C10
	//0x0824:	0x93FFFA  	DM($3FFF) = AR
	//0x0825:	0x3C03FC  	IFC = $003F
	//0x0826:	0x40E82A  	AR = $0E82
	//0x0827:	0x93FEFA  	DM($3FEF) = AR
	//0x0828:	0x70000D  	AX0 = DM(I7,M5)
	//0x0829:	0x0D0CB0  	TX1 = AX0
	//0x082A:	0x3C0004  	ICNTL = $0000
	//0x082B:	0x3C0203  	IMASK = $0020
//	adsp2100_set_imask(0x20);

	//0x082C:	0x404F08  	SI = $04F0
	//0x082D:	0x9060C8  	DM($060C) = SI
	//0x082E:	0x3B4000  	I4 = $3400
	//0x082F:	0x834008  	SI = DM($3400)
	dspIRQClear=true;

	//0x0830:	0x389001  	I5 = $0900
	//0x0831:	0x387000  	I4 = $0700
	//0x0832:	0x3C0805  	CNTR = $0080
	//0x0833:	0x14835E  	DO $0835 UNTIL CE
	//0x0834:	0x5000A5  	AR = PM(I5,M5)
	//0x0835:	0x7800A1  	DM(I4,M5) = AR
	count=0x80;
	dst=&dspDMem[0x700];
	src=(DWORD *)&dspPMem[0x900];
	DWORD tmp;
	while(count)
	{
		tmp=*(src++);
		tmp>>=8;
		tmp&=0xffff;
		*(dst++)=tmp;
		count--;
	}

	//0x0836:	0x389801  	I5 = $0980
	//0x0837:	0x387800  	I4 = $0780
	//0x0838:	0x3C0805  	CNTR = $0080
	//0x0839:	0x1483BE  	DO $083B UNTIL CE
	//0x083A:	0x5000A5  	AR = PM(I5,M5)
	//0x083B:	0x7800A1  	DM(I4,M5) = AR
	count=0x80;
	dst=&dspDMem[0x780];
	src=(DWORD *)&dspPMem[0x980];
	while(count)
	{
		tmp=*(src++);
		tmp>>=8;
		tmp&=0xffff;
		*(dst++)=tmp;
		count--;
	}

	//0x083C:	0x388F01  	I5 = $08F0
	//0x083D:	0x3862D0  	I4 = $062D
	//0x083E:	0x3C0105  	CNTR = $0010
	//0x083F:	0x14841E  	DO $0841 UNTIL CE
	//0x0840:	0x5000A5  	AR = PM(I5,M5)
	//0x0841:	0x7800A1  	DM(I4,M5) = AR
	count=0x10;
	dst=&dspDMem[0x62d];
	src=(DWORD *)&dspPMem[0x8f0];
	while(count)
	{
		tmp=*(src++);
		tmp>>=8;
		tmp&=0xffff;
		*(dst++)=tmp;
		count--;
	}

	//0x0842:	0x388D51  	I5 = $08D5
	//0x0843:	0x3B98C0  	I4 = $398C
	//0x0844:	0x3C0105  	CNTR = $0010
	//0x0845:	0x14847E  	DO $0847 UNTIL CE
	//0x0846:	0x5000A5  	AR = PM(I5,M5)
	//0x0847:	0x7800A1  	DM(I4,M5) = AR
	count=0x10;
	dst=&dspDMem[0x398c];
	src=(DWORD *)&dspPMem[0x8d5];
	while(count)
	{
		tmp=*(src++);
		tmp>>=8;
		tmp&=0xffff;
		*(dst++)=tmp;
		count--;
	}

	//0x0848:	0x388EC1  	I5 = $08EC
	//0x0849:	0x3B9F90  	I4 = $39F9
	//0x084A:	0x3C0045  	CNTR = $0004
	//0x084B:	0x1484DE  	DO $084D UNTIL CE
	//0x084C:	0x5000A5  	AR = PM(I5,M5)
	//0x084D:	0x7800A1  	DM(I4,M5) = AR
	count=0x4;
	dst=&dspDMem[0x39f9];
	src=(DWORD *)&dspPMem[0x8ec];
	while(count)
	{
		tmp=*(src++);
		tmp>>=8;
		tmp&=0xffff;
		*(dst++)=tmp;
		count--;
	}

	//0x084E:	0x388E51  	I5 = $08E5
	//0x084F:	0x386140  	I4 = $0614
	//0x0850:	0x3C0075  	CNTR = $0007
	//0x0851:	0x14853E  	DO $0853 UNTIL CE
	//0x0852:	0x5000A5  	AR = PM(I5,M5)
	//0x0853:	0x7800A1  	DM(I4,M5) = AR
	count=0x7;
	dst=&dspDMem[0x614];
	src=(DWORD *)&dspPMem[0x8e5];
	while(count)
	{
		tmp=*(src++);
		tmp>>=8;
		tmp&=0xffff;
		*(dst++)=tmp;
		count--;
	}

	//0x0854:	0x400005  	AY1 = $0000
	//0x0855:	0x4003D1  	AX1 = $003D
	//0x0856:	0x3B99C0  	I4 = $399C
	//0x0857:	0x1C897F  	CALL $0897
	hleDSPVMemcpy(0x399c,0x0,0x3d);

	//0x0858:	0x400025  	AY1 = $0002
	//0x0859:	0x400041  	AX1 = $0004
	//0x085A:	0x3B9DD0  	I4 = $39DD
	//0x085B:	0x1C897F  	CALL $0897
	hleDSPVMemcpy(0x39dd,0x2,0x4);

	//0x085C:	0x400045  	AY1 = $0004
	//0x085D:	0x400081  	AX1 = $0008
	//0x085E:	0x3B9E10  	I4 = $39E1
	//0x085F:	0x1C897F  	CALL $0897
	hleDSPVMemcpy(0x39e1,0x4,0x8);

	//0x0860:	0x400065  	AY1 = $0006
	//0x0861:	0x400201  	AX1 = $0020
	//0x0862:	0x383000  	I4 = $0300
	//0x0863:	0x1C897F  	CALL $0897
	hleDSPVMemcpy(0x300,0x6,0x20);

	//0x0864:	0x4000E5  	AY1 = $000E
	//0x0865:	0x400101  	AX1 = $0010
	//0x0866:	0x3B9E90  	I4 = $39E9
	//0x0867:	0x1C897F  	CALL $0897
	hleDSPVMemcpy(0x39e9,0xe,0x10);

	//0x0868:	0x400105  	AY1 = $0010
	//0x0869:	0x400101  	AX1 = $0010
	//0x086A:	0x383200  	I4 = $0320
	//0x086B:	0x1C897F  	CALL $0897
	hleDSPVMemcpy(0x320,0x10,0x10);

	//0x086C:	0x400125  	AY1 = $0012
	//0x086D:	0x400101  	AX1 = $0010
	//0x086E:	0x383300  	I4 = $0330
	//0x086F:	0x1C897F  	CALL $0897
	hleDSPVMemcpy(0x330,0x12,0x10);

	//0x0870:	0x400145  	AY1 = $0014
	//0x0871:	0x400101  	AX1 = $0010
	//0x0872:	0x3B9200  	I4 = $3920
	//0x0873:	0x1C897F  	CALL $0897
	hleDSPVMemcpy(0x3920,0x14,0x10);

	//0x0874:	0x400165  	AY1 = $0016
	//0x0875:	0x400101  	AX1 = $0010
	//0x0876:	0x3B9300  	I4 = $3930
	//0x0877:	0x1C897F  	CALL $0897
	hleDSPVMemcpy(0x3930,0x16,0x10);

	//0x0878:	0x400185  	AY1 = $0018
	//0x0879:	0x400101  	AX1 = $0010
	//0x087A:	0x3B9400  	I4 = $3940
	//0x087B:	0x1C897F  	CALL $0897
	hleDSPVMemcpy(0x3940,0x18,0x10);

	//0x087C:	0x4001A5  	AY1 = $001A
	//0x087D:	0x400101  	AX1 = $0010
	//0x087E:	0x3B9500  	I4 = $3950
	//0x087F:	0x1C897F  	CALL $0897
	hleDSPVMemcpy(0x3950,0x1a,0x10);

	//0x0880:	0x439DD4  	AY0 = $39DD
	//0x0881:	0x939DA4  	DM($39DA) = AY0
	//0x0882:	0x439E14  	AY0 = $39E1
	//0x0883:	0x939DB4  	DM($39DB) = AY0
	//0x0884:	0x403004  	AY0 = $0300
	//0x0885:	0x939DC4  	DM($39DC) = AY0
	dspDMem[0x39da]=0x39dd;
	dspDMem[0x39db]=0x39e1;
	dspDMem[0x39dc]=0x300;

	//0x0886:	0x343C00  	I0 = $03C0
	//0x0887:	0x4FFFF8  	SI = $FFFF
	//0x0888:	0x3C0045  	CNTR = $0004
	//0x0889:	0x1488AE  	DO $088A UNTIL CE
	//0x088A:	0x680081  	DM(I0,M1) = SI
	memset(&dspDMem[0x3c0],0xff,0x8);

	//0x088B:	0x40004A  	AR = $0004
	//0x088C:	0x93000A  	DM($3000) = AR
	//0x088D:	0x400FF4  	AY0 = $00FF
	//0x088E:	0x820468  	SI = DM($2046)
	//0x088F:	0x0F1008  	SR = LSHIFT SI BY 8 (LO)
	//0x0890:	0x82047A  	AR = DM($2047)
	//0x0891:	0x23820F  	AR = AR AND AY0
	//0x0892:	0x0F1A00  	SR = SR OR LSHIFT AR BY 0 (LO)
	//0x0893:	0x9060FE  	DM($060F) = SR0
	WORD b1,b2;
	b1=*(WORD *)&m->dspRMem[(4<<12)|0x46];
	b2=*(WORD *)&m->dspRMem[(4<<12)|0x47];
	b1<<=8;
	b2&=0xff;
	b1|=b2;
	dspDMem[0x60f]=b1;

	//0x0894:	0x400678  	SI = $0067
	//0x0895:	0x905FC8  	DM($05FC) = SI
	//0x0896:	0x0a000F  	RTS
	dspDMem[0x5fc]=0x67;
}

void hleDSPVMemcpy(WORD DstOffset,WORD SrcOffset,WORD Len)
{
	WORD *dst=(WORD *)&dspDMem[DstOffset];
	WORD index=*(WORD *)&m->dspRMem[SrcOffset+(3<<12)];

	//0x0897:	0x400FF4  	AY0 = $00FF
	//0x0898:	0x40003B  	MR0 = $0003
	//0x0899:	0x93000B  	DM($3000) = MR0
	//0x089A:	0x42000A  	AR = $2000
	//0x089B:	0x226A0F  	AR = AR + AY1
	//0x089C:	0x0D040A  	I0 = AR
	//0x089D:	0x6000A1  	AR = DM(I0,M1)
	//0x089E:	0x0F1208  	SR = LSHIFT AR BY 8 (LO)
	//0x089F:	0x6000A0  	AR = DM(I0,M0)
	//0x08A0:	0x23820F  	AR = AR AND AY0
	//0x08A1:	0x0F1A00  	SR = SR OR LSHIFT AR BY 0 (LO)
	//0x08A2:	0x0D047E  	M3 = SR0
	//0x08A3:	0x360000  	I0 = $2000
	//0x08A4:	0x090003  	MODIFY (I0,M3)
	index=(index>>8)|(index<<8);
	WORD *src=(WORD *)&m->dspRMem[index+(3<<12)];

	//0x08A5:	0x0D0C51  	CNTR = AX1
	//0x08A6:	0x148ACE  	DO $08AC UNTIL CE
	//0x08A7:	0x6000A1  	AR = DM(I0,M1)
	//0x08A8:	0x0F1208  	SR = LSHIFT AR BY 8 (LO)
	//0x08A9:	0x6000A1  	AR = DM(I0,M1)
	//0x08AA:	0x23820F  	AR = AR AND AY0
	//0x08AB:	0x0F1A00  	SR = SR OR LSHIFT AR BY 0 (LO)
	//0x08AC:	0x7800E1  	DM(I4,M5) = SR0
	WORD tmp;
	for(int i=0;i<Len;i++)
	{
		tmp=*(src++);
		tmp=(tmp>>8)|(tmp<<8);
		*(dst++)=tmp;
	}

	//0x08AD:	0x0a000F  	RTS
}

void hleDSPPCopy(WORD SrcPage,WORD SrcOffset,WORD len)
{
	DWORD *dst,*src,tmp;
	src=(DWORD *)&m->dspRMem[(SrcPage<<12)+SrcOffset];
	dst=(DWORD *)&m->dspPMem[0x800*4];

	while(len)
	{
		tmp=*(src++);
		tmp=(tmp&0xff0000)>>16|(tmp&0xff)<<16|(tmp&0xff00);
		*(dst++)=tmp;
		len--;
	}
}

void hleDSPInit()
{
	//0x0008:	0x400108  	SI = $0010
	//0x0009:	0x93FFF8  	DM($3FFF) = SI
	//0x000A:	0x422808  	SI = $2280
	//0x000B:	0x93FFE8  	DM($3FFE) = SI
	//0x000C:	0x340008  	L0 = $0000
	//0x000D:	0x340009  	L1 = $0000
	//0x000E:	0x380008  	L4 = $0000
	//0x000F:	0x340015  	M1 = $0001
	//0x0010:	0x380004  	M4 = $0000
	//0x0011:	0x380015  	M5 = $0001
	//0x0012:	0x400008  	SI = $0000
	//0x0013:	0x368000  	I0 = $2800
	//0x0014:	0x402000  	AX0 = $0200
	//0x0015:	0x1C3E8F  	CALL $03E8
	//0x0016:	0x1C800F  	CALL $0800
	hleDSPPCopy(0,0x800,0x200);
	hleDSPInit1();

	//0x0017:	0x400028  	SI = $0002
	//0x0018:	0x360000  	I0 = $2000
	//0x0019:	0x403000  	AX0 = $0300
	//0x001A:	0x1C3E8F  	CALL $03E8
	hleDSPPCopy(2,0x0,0x300);

	//0x001B:	0x345FF0  	I0 = $05FF
	//0x001C:	0x400FF0  	AX0 = $00FF
	//0x001D:	0x680001  	DM(I0,M1) = AX0
	//0x001E:	0x680001  	DM(I0,M1) = AX0
	//0x001F:	0x680001  	DM(I0,M1) = AX0
	//0x0020:	0x680001  	DM(I0,M1) = AX0
	dspDMem[0x5ff]=0xff;
	dspDMem[0x600]=0xff;
	dspDMem[0x601]=0xff;
	dspDMem[0x602]=0xff;
}

void hleDSPMain()
{
	WORD *dst;
	DWORD *src;
	WORD count;
	WORD si,i0,i3,i1,sr0,sr1,m3,m2,i6,af,mx0,mr0;
	short se;
	WORD ax1,ax0,m6,i2,i4,ay0,ay1,mr2,mr1;
	WORD my0,my1;
	DWORD tmp,vptr;
	int mr;
	DWORD sr;
	int j=0;
	WORD ar;
	DWORD page,BasePage;

	hleDSPVSetMem();

	//0x0021:	0x3C1025  	CNTR = $0102
	//0x0022:	0x378000  	I0 = $3800
	//0x0023:	0x14024E  	DO $0024 UNTIL CE
	//0x0024:	0xA00001  	DM(I0,M1) = $0000
	memset(&dspDMem[0x3800],0,0x102*2);

	//0x0025:	0x805F80  	AX0 = DM($05F8)
	//0x0026:	0x805F94  	AY0 = DM($05F9)
	//0x0027:	0x26E00F  	AF = AX0 - AY0
	//0x0028:	0x180640  	IF EQ JUMP $0064
	while(dspDMem[0x5f8]!=dspDMem[0x5f9])
	{
		//0x0029:	0x340108  	L0 = $0010
		//0x002A:	0x845F90  	I0 = DM($05F9)
		//0x002B:	0x6000A1  	AR = DM(I0,M1)
		//0x002C:	0x945F90  	DM($05F9) = I0
		i0=dspDMem[0x5f9];
		ar=dspDMem[i0];
		i0++;
		if((i0%0x10)==0)
			i0-=0x10;
		dspDMem[0x5f9]=i0;

		//0x002D:	0x340008  	L0 = $0000
		//0x002E:	0x8060F4  	AY0 = DM($060F)
		//0x002F:	0x26E20F  	AF = AR - AY0
		//0x0030:	0x180635  	IF GE JUMP $0063

		if(ar>=dspDMem[0x60f])
			continue;

		//0x0031:	0x0D002A  	MX0 = AR
		//0x0032:	0x400048  	SI = $0004
		//0x0033:	0x9390F8  	DM($390F) = SI
		//0x0034:	0x420408  	SI = $2040
		//0x0035:	0x939FF8  	DM($39FF) = SI
		//0x0036:	0x1C365F  	CALL $0365
		//0x0037:	0x0D03C7  	MR1 = PX
		//0x0038:	0x0D00BA  	MR0 = AR
		mx0=ar;

		hleDSPVSetPageOffset(4,0x40);
		hleDSPVRead24((DWORD *)&mr);

		//0x0039:	0x400036  	MY0 = $0003
		//0x003A:	0x21600F  	MR = MR + MX0 * MY0 (UU)
		mr+=mx0*3;

		//0x003B:	0x939FDC  	DM($39FD) = MR1
		//0x003C:	0x939FEB  	DM($39FE) = MR0
		//0x003D:	0x1C33FF  	CALL $033F
		//0x003E:	0x1C365F  	CALL $0365
		DWORD px;
		hleDSPVSetAbs(mr);
		hleDSPVRead24(&px);

		//0x003F:	0x0D0307  	AX0 = PX
		//0x0040:	0x400FF4  	AY0 = $00FF
		//0x0041:	0x26E00F  	AF = AX0 - AY0
		//0x0042:	0x180630  	IF EQ JUMP $0063
		if((px>>16)==0xff)
			continue;

		//0x0043:	0x0D0387  	SI = PX
		//0x0044:	0x939FD8  	DM($39FD) = SI
		//0x0045:	0x939FEA  	DM($39FE) = AR
		//0x0046:	0x1C33FF  	CALL $033F
		//0x0047:	0x1C34BF  	CALL $034B
		WORD tmp;
		hleDSPVSetAbs(px);
		hleDSPVRead8(&tmp);

		//0x0048:	0x400014  	AY0 = $0001
		//0x0049:	0x26E20F  	AF = AR - AY0
		//0x004A:	0x180521  	IF NE JUMP $0052
		if(tmp==1)
		{
			//0x004B:	0x1C34BF  	CALL $034B
			//0x004C:	0x0D00BA  	MR0 = AR
			//0x004D:	0x1C369F  	CALL $0369
			//0x004E:	0x8F9FD7  	PX = DM($39FD)
			//0x004F:	0x839FEA  	AR = DM($39FE)
			//0x0050:	0x1C371F  	CALL $0371
			//0x0051:	0x18063F  	JUMP $0063

			hleDSPVRead8(&tmp);
			hleDSPVGetAbs(&px);
			hleDSPResetPtrs(px,tmp);
			continue;
		}
		//0x0052:	0x400034  	AY0 = $0003
		//0x0053:	0x26E20F  	AF = AR - AY0
		//0x0054:	0x181CA2  	IF GT JUMP $01CA
		if(tmp<=3)
		{
			WORD ax0;

			//0x0055:	0x90607A  	DM($0607) = AR
			dspDMem[0x607]=tmp;

			//0x0056:	0x1C34BF  	CALL $034B
			//0x0057:	0x0D000A  	AX0 = AR
			hleDSPVRead8(&ax0);

			//0x0058:	0x1C35EF  	CALL $035E
			//0x0059:	0x0D008A  	SI = AR
			//0x005A:	0x405E84  	AY0 = $05E8
			//0x005B:	0x22600F  	AR = AX0 + AY0
			//0x005C:	0x0D040A  	I0 = AR
			//0x005D:	0x680080  	DM(I0,M0) = SI
			hleDSPVRead16(&si);
			dspDMem[ax0+0x5e8]=si;


			//0x005E:	0x405E04  	AY0 = $05E0
			//0x005F:	0x22600F  	AR = AX0 + AY0
			//0x0060:	0x0D040A  	I0 = AR
			//0x0061:	0x806078  	SI = DM($0607)
			//0x0062:	0x680080  	DM(I0,M0) = SI
			dspDMem[ax0+0x5e0]=dspDMem[0x607];
		}
		else
		{
			//ohoh
			int breakp=1;
		}
		//0x0063:	0x18025F  	JUMP $0025
	}

	hleDSPVFlush();
//	return;

	//0x0064:	0x40000A  	AR = $0000
	//0x0065:	0x90603A  	DM($0603) = AR
	//0x0066:	0x945FB4  	DM($05FB) = M0
	dspDMem[0x603]=0;
	dspDMem[0x5fb]=0;

	while(1)
	{
		//0x0067:	0x806034  	AY0 = DM($0603)
		//0x0068:	0x805FB9  	SE = DM($05FB)
		//0x0069:	0x40001A  	AR = $0001
		//0x006A:	0x0E120F  	SR = LSHIFT AR (LO)
		//0x006B:	0x23860F  	AR = SR0 AND AY0
		//0x006C:	0x180731  	IF NE JUMP $0073
		ay0=dspDMem[0x603];
		se=dspDMem[0x5fb];
		if(se>0)
			sr=(1<<se);
		else
			sr=(1>>(se*-1));
		ar=(sr&0xffff)&ay0;
		if(ar==0)
		{
			//0x006D:	0x1C19FF  	CALL $019F
			hleDSPPreDispatch();

			//0x006E:	0x80603E  	SR0 = DM($0603)
			//0x006F:	0x805FB9  	SE = DM($05FB)
			//0x0070:	0x40001A  	AR = $0001
			//0x0071:	0x0E1A0F  	SR = SR OR LSHIFT AR (LO)
			//0x0072:	0x90603E  	DM($0603) = SR0
			sr&=0xffff0000;
			sr|=dspDMem[0x603];
			se=dspDMem[0x5fb];
			if(se>0)
				sr|=(1<<se);
			else
				sr|=(1>>(se*-1));
			dspDMem[0x603]=sr&0xffff;
		}

		//0x0073:	0x805FB4  	AY0 = DM($05FB)
		//0x0074:	0x22200F  	AR = AY0 + 1
		//0x0075:	0x400044  	AY0 = $0004
		//0x0076:	0x26E20F  	AF = AR - AY0
		//0x0077:	0x221800  	IF EQ AR = 0
		//0x0078:	0x905FBA  	DM($05FB) = AR
		//0x0079:	0x80603A  	AR = DM($0603)
		//0x007A:	0x4000F4  	AY0 = $000F
		//0x007B:	0x26E20F  	AF = AR - AY0
		//0x007C:	0x180671  	IF NE JUMP $0067
		ay0=dspDMem[0x5fb];
		ar=ay0+1;
		if(ar==4) ar=0;
		dspDMem[0x5fb]=ar;
		ar=dspDMem[0x603];
		if(ar==0xf)
			break;
	}
}

void hleDSPPreDispatch()
{
	WORD i0;
	DWORD mr;

	//0x019F:	0x403800  	AX0 = $0380
	//0x01A0:	0x805FB4  	AY0 = DM($05FB)
	//0x01A1:	0x22600F  	AR = AX0 + AY0
	//0x01A2:	0x22620F  	AR = AR + AY0
	//0x01A3:	0x0D040A  	I0 = AR
	//0x01A4:	0x6000A1  	AR = DM(I0,M1)
	//0x01A5:	0x939FDA  	DM($39FD) = AR
	//0x01A6:	0x600040  	AY0 = DM(I0,M0)
	//0x01A7:	0x939FE4  	DM($39FE) = AY0
	//0x01A8:	0x23A20F  	AR = AR OR AY0
	//0x01A9:	0x0A0000  	IF EQ RTS
	//0x01AA:	0x0D00A4  	AR = AY0
	//0x01AB:	0x1C33FF  	CALL $033F
	i0=dspDMem[0x5fb]*2+0x380;
	mr=dspDMem[i0++]<<16;
	mr|=dspDMem[i0];
	if(mr==0)
		return;
	hleDSPVSetAbs(mr);
	hleDSPDispatch();
}

WORD hleDSPExp(WORD src)
{
	WORD count=0;
	WORD i,mask;
	mask=0x8000;
	for(i=0;i<16;i++)
	{
		if(mask&src)
		{
			count++;
		}
		else
			break;
		mask>>=1;
	}
	count=(count*-1);
	return(count);
}


void hleDSPMain2()
{
	WORD *dst;
	DWORD *src;
	WORD count;
	WORD si,i0,i3,i1,sr0,sr1,m3,m2,i6,af,mx0,mx1,mr0;
	short se;
	WORD ax1,ax0,m6,i2,i4,ay0,ay1,mr2,mr1,i5;
	WORD my0,my1;
	DWORD tmp,vptr;
	UINT64 mr;
	DWORD sr;
	WORD i,j,k,c1,c2,c3;
	WORD ar;
	DWORD page,BasePage;

	//0x007D:	0x343880  	I0 = $0388
	//0x007E:	0x343A01  	I1 = $03A0
	//0x007F:	0x3C0045  	CNTR = $0004
	//0x0080:	0x20980F  	MR = 0
	//0x0081:	0x805FD6  	MY0 = DM($05FD)
	//0x0082:	0x14087E  	DO $0087 UNTIL CE
	i0=0x388;
	i1=0x3a0;
	mr=0;
	my0=dspDMem[0x5fd];
	for(i=0;i<4;i++)
	{
		//0x0083:	0x600001  	AX0 = DM(I0,M1)
		//0x0084:	0x600041  	AY0 = DM(I0,M1)
		//0x0085:	0x600025  	MX0 = DM(I1,M1)
		//0x0086:	0x23A00F  	AR = AX0 OR AY0
		//0x0087:	0x216001  	IF NE MR = MR + MX0 * MY0 (UU)
		ax0=dspDMem[i0++];
		ay0=dspDMem[i0++];
		mx0=dspDMem[i1++];
		ar=ax0|ay0;
		if(ar)
		{
			mr=mr+mx0*my0;
		}
	}
	mr2=(mr>>32)&0xff;
	mr1=(mr>>16)&0xffff;
	mr0=mr&0xffff;

	//0x0088:	0x0F050E  	SR = LSHIFT MR2 BY 14 (HI)
	//0x0089:	0x0F0CFE  	SR = SR OR LSHIFT MR1 BY -2 (HI)
	//0x008A:	0x0E670F  	SE = EXP SR1 (HI)
	//0x008B:	0x0D0049  	AY0 = SE
	//0x008C:	0x40003A  	AR = $0003
	//0x008D:	0x22620F  	AR = AR + AY0
	//0x008E:	0x221802  	IF GT AR = 0
	sr=(mr2<<16)<<14;
	sr1=(sr>>16);
	ar|=(mr1<<16)>>2;
	se=hleDSPExp(sr1);
	se+=3;
	if(se)
		se=0;

	//0x008F:	0x4FFF84  	AY0 = $FFF8
	//0x0090:	0x26E20F  	AF = AR - AY0
	//0x0091:	0x220004  	IF LT AR = AY0
	//0x0092:	0x0D009A  	SE = AR
	if(se<(short)0xfff8)
		se=(short)0xfff8;

	//0x0093:	0x48000E  	SR0 = $8000
	//0x0094:	0x0E160F  	SR = LSHIFT SR0 (LO)
	//0x0095:	0x9390EE  	DM($390E) = SR0
	//0x0096:	0x233A0F  	AR = -AR
	//0x0097:	0x0D009A  	SE = AR
	sr=0x8000;
	sr=sr>>se;
	dspDMem[0x390e]=(sr&0xffff);
	se=se*-1;

	//0x0098:	0x3C0001  	MSTAT = $0000
	//0x0099:	0x343A01  	I1 = $03A0
	//0x009A:	0x3C0045  	CNTR = $0004
	//0x009B:	0x140A0E  	DO $00A0 UNTIL CE
	i1=0x3a0;
	for(i=0;i<4;i++)
	{
		//0x009C:	0x600024  	MX0 = DM(I1,M0)
		//0x009D:	0x20E00F  	MR = MX0 * MY0 (UU)
		//0x009E:	0x0E040F  	SR = LSHIFT MR1 (HI)
		//0x009F:	0x0E1B0F  	SR = SR OR LSHIFT MR0 (LO)
		//0x00A0:	0x6800F5  	DM(I1,M1) = SR1
		mx0=dspDMem[i1];
		mr=mx0*my0;
		mr<<=1;
		mr0=mr&0xffff;
		mr1=(mr>>16)&0xffff;
		sr=(mr1<<16)<<se;
		sr|=(mr0<<se);
		dspDMem[i1++]=(sr>>16)&0xffff;
	}

	//0x00A1:	0x3C0181  	MSTAT = $0018
	//0x00A2:	0x22180F  	AR = 0
	ar=0;
	while(1)
	{
		//0x00A3:	0x905FBA  	DM($05FB) = AR
		//0x00A4:	0x1C168F  	CALL $0168
		dspDMem[0x5fb]=ar;
		hleDSPProcess(ar);

		//0x00A5:	0x805FB4  	AY0 = DM($05FB)
		//0x00A6:	0x22200F  	AR = AY0 + 1
		//0x00A7:	0x400044  	AY0 = $0004
		//0x00A8:	0x26E20F  	AF = AR - AY0
		//0x00A9:	0x180A31  	IF NE JUMP $00A3
		ar=dspDMem[0x5fb];
		ar++;
		if(ar==4)
			break;
	}
	int imr;
	int temp;

	//0x00AA:	0x3C0081  	MSTAT = $0008
	//0x00AB:	0x378800  	I0 = $3880
	//0x00AC:	0x480007  	MY1 = $8000
	i0=0x3880;
	my1=0x8000;

	//0x00AD:	0x6000A0  	AR = DM(I0,M0)
	//0x00AE:	0x208A0F  	MR = AR * MY1 (SS)
	//0x00AF:	0x6800C1  	DM(I0,M1) = MR1
	ar=dspDMem[i0];
	imr=(short)ar*(short)my1;
	dspDMem[i0++]=(imr>>15);

	//0x00B0:	0x600040  	AY0 = DM(I0,M0)
	//0x00B1:	0x22A00F  	AR = -AY0
	//0x00B2:	0x208A0F  	MR = AR * MY1 (SS)
	//0x00B3:	0x6800C1  	DM(I0,M1) = MR1
	ay0=dspDMem[i0];
	ar=-ay0;
	imr=(short)ar*(short)my1;
	dspDMem[i0++]=(imr>>15);

	//0x00B4:	0x3C0405  	CNTR = $0040
	//0x00B5:	0x378000  	I0 = $3800
	//0x00B6:	0x379001  	I1 = $3900
	//0x00B7:	0x37FFF6  	M2 = $3FFF
	//0x00B8:	0x37FFD7  	M3 = $3FFD
	//0x00B9:	0x480007  	MY1 = $8000
	//0x00BA:	0x140C9E  	DO $00C9 UNTIL CE
	i0=0x3800;
	i1=0x3900;
	my1=0x8000;
	for(i=0;i<0x40;i++)
	{
		//0x00BB:	0x600000  	AX0 = DM(I0,M0)
		//0x00BC:	0x600044  	AY0 = DM(I1,M0)
		//0x00BD:	0x22600F  	AR = AX0 + AY0
		//0x00BE:	0x208A0F  	MR = AR * MY1 (SS)
		//0x00BF:	0x6AE0C1  	AR = AX0 - AY0, DM(I0,M1) = MR1
		//0x00C0:	0x208A0F  	MR = AR * MY1 (SS)
		//0x00C1:	0x6800C5  	DM(I1,M1) = MR1
		ax0=dspDMem[i0];
		ay0=dspDMem[i1];
		ar=ax0+ay0;
		imr=(short)ar*(short)my1;
		dspDMem[i0++]=(imr>>15);
		ar=ax0-ay0;
		imr=(short)ar*(short)my1;
		dspDMem[i1++]=(imr>>15);


		//0x00C2:	0x600000  	AX0 = DM(I0,M0)
		//0x00C3:	0x600044  	AY0 = DM(I1,M0)
		//0x00C4:	0x22600F  	AR = AX0 + AY0
		//0x00C5:	0x208A0F  	MR = AR * MY1 (SS)
		//0x00C6:	0x6AE0C7  	AR = AX0 - AY0, DM(I1,M3) = MR1
		//0x00C7:	0x208A0F  	MR = AR * MY1 (SS)
		//0x00C8:	0x6800C1  	DM(I0,M1) = MR1
		//0x00C9:	//0x000000  	NOP
		ax0=dspDMem[i0];
		ay0=dspDMem[i1];
		ar=ax0+ay0;
		imr=(short)ar*(short)my1;
		dspDMem[i1]=(imr>>15);
		i1-=3;
		ar=ax0-ay0;
		imr=(short)ar*(short)my1;
		dspDMem[i0++]=(imr>>15);
	}
	//0x00CA:	0x340780  	I0 = $0078
	//0x00CB:	0x340381  	I1 = $0038
	//0x00CC:	0x3B8000  	I4 = $3800
	//0x00CD:	0x3B9011  	I5 = $3901
	//0x00CE:	0x3C0081  	MSTAT = $0008
	//0x00CF:	0x340806  	M2 = $0080
	//0x00D0:	0x3BFFF6  	M6 = $3FFF
	//0x00D1:	0x380027  	M7 = $0002
	//0x00D2:	0x0C00C0  	ENA BIT_REV 
	//0x00D3:	0x3C0405  	CNTR = $0040
	//0x00D4:	0x140E1E  	DO $00E1 UNTIL CE
	i0=0x78;
	i1=0x38;
	i4=0x3800;
	i5=0x3901;
	for(i=0;i<0x40;i++)
	{
		//0x00D5:	0x600062  	MY0 = DM(I0,M2)
		//0x00D6:	0x600076  	MY1 = DM(I1,M2)
		//0x00D7:	0x700036  	MX1 = DM(I5,M6)
		my0=dspDMem[dspBitRev[i0]];
		i0+=0x80;
		my1=dspDMem[dspBitRev[i1]];
		i1+=0x80;
		mx1=dspDMem[i5--];

		//0x00D8:	0x708925  	MR = MX1 * MY1 (SS), MX0 = DM(I5,M5)
		imr=(short)mx1*(short)my1;
		mx0=dspDMem[i5++];
		imr<<=1;

		//0x00D9:	0x706041  	MR = MR - MX0 * MY0 (RND), AY0 = DM(I4,M5)
		temp=((short)mx0*(short)my0);
		temp<<=1;
		imr=imr-temp;
		temp&=0xffff;
		imr+=0x8000;
		if(temp==0x8000)
		{
			imr&= ~((UINT64)0x10000);
		}

		ay0=dspDMem[i4++];

		//0x00DA:	0x28810C  	MR = MX1 * MY0 (SS), AX0 = MR1
		ax0=(imr>>16);
		imr=(short)mx1*(short)my0;
		imr<<=1;

		//0x00DB:	0x704850  	MR = MR + MX0 * MY1 (RND), AY1 = DM(I4,M4)
		temp=(short)mx0*(short)my1;
		temp<<=1;
		imr=imr+temp;
		temp&=0xffff;
		imr+=0x8000;
		if(temp==0x8000)
		{
			imr&= ~((UINT64)0x10000);
		}

		ay1=dspDMem[i4];
		mr1=(imr>>16);

		//0x00DC:	0x22680F  	AR = AX0 + AY1
		//0x00DD:	0x7AE8A2  	AR = AX0 - AY1, DM(I4,M6) = AR
		//0x00DE:	0x7A64A6  	AR = MR1 + AY0, DM(I5,M6) = AR
		//0x00DF:	0x7B24A3  	AR = AY0 - MR1, DM(I4,M7) = AR
		//0x00E0:	0x7800A6  	DM(I5,M6) = AR
		ar=ax0+ay1;
		dspDMem[i4--]=ar;
		ar=ax0-ay1;
		dspDMem[i5--]=ar;
		ar=mr1+ay0;
		dspDMem[i4]=ar;
		i4+=2;
		ar=ay0-mr1;
		dspDMem[i5--]=ar;
		//0x00E1:	//0x000000  	NOP
	}
	//0x00E2:	0x0C0080  	DIS BIT_REV 
	//0x00E3:	0x0C2000  	DIS M_MODE 
	//0x00E4:	0x378000  	I0 = $3800
	//0x00E5:	0x378802  	I2 = $3880
	//0x00E6:	0x37FFF6  	M2 = $3FFF
	//0x00E7:	0x37FFD7  	M3 = $3FFD
	//0x00E8:	0x3C0405  	CNTR = $0040
	//0x00E9:	0x140F3E  	DO $00F3 UNTIL CE
	i0=0x3800;
	i2=0x3880;
	for(i=0;i<0x40;i++)
	{
		//0x00EA:	0x600001  	AX0 = DM(I0,M1)
		//0x00EB:	0x600048  	AY0 = DM(I2,M0)
		ax0=dspDMem[i0++];
		ay0=dspDMem[i2];

		//0x00EC:	0x626012  	AR = AX0 + AY0, AX1 = DM(I0,M2)
		ar=ax0+ay0;
		ax1=dspDMem[i0--];

		//0x00ED:	0x6AE0A1  	AR = AX0 - AY0, DM(I0,M1) = AR
		dspDMem[i0++]=ar;
		ar=ax0-ay0;

		//0x00EE:	0x6800A9  	DM(I2,M1) = AR
		//0x00EF:	0x600058  	AY1 = DM(I2,M0)
		dspDMem[i2++]=ar;
		ay1=dspDMem[i2];

		//0x00F0:	0x22690F  	AR = AX1 + AY1
		//0x00F1:	0x6800A1  	DM(I0,M1) = AR
		ar=ax1+ay1;
		dspDMem[i0++]=ar;

		//0x00F2:	0x22E90F  	AR = AX1 - AY1
		//0x00F3:	0x6800A9  	DM(I2,M1) = AR
		ar=ax1-ay1;
		dspDMem[i2++]=ar;
	}
	//0x00F4:	0x40002A  	AR = $0002
	//0x00F5:	0x9063DA  	DM($063D) = AR
	//0x00F6:	0x400408  	SI = $0040
	//0x00F7:	0x9063E8  	DM($063E) = SI
	//0x00F8:	0x0F10FF  	SR = LSHIFT SI BY -1 (LO)
	//0x00F9:	0x9063FE  	DM($063F) = SR0
	//0x00FA:	0x37FFF4  	M0 = $3FFF
	//0x00FB:	0x3C0065  	CNTR = $0006
	//0x00FC:	0x14120E  	DO $0120 UNTIL CE
	dspDMem[0x63d]=0x2;
	si=0x40;
	dspDMem[0x63e]=si;
	si>>=1;
	dspDMem[0x63f]=si;
	for(i=0;i<6;i++)
	{
		//0x00FD:	0x387800  	I4 = $0780
		//0x00FE:	0x387001  	I5 = $0700
		//0x00FF:	0x378000  	I0 = $3800
		//0x0100:	0x378001  	I1 = $3800
		//0x0101:	0x8063E4  	AY0 = DM($063E)
		//0x0102:	0x0D0464  	M2 = AY0
		//0x0103:	0x090006  	MODIFY (I1,M2)
		//0x0104:	0x0D0521  	I2 = I1
		//0x0105:	0x23000F  	AR = AY0 - 1
		//0x0106:	0x0D047A  	M3 = AR
		//0x0107:	0x8C63D5  	CNTR = DM($063D)
		//0x0108:	0x14119E  	DO $0119 UNTIL CE
		i4=0x780;
		i5=0x700;
		i0=0x3800;
		i1=0x3800;
		ay0=dspDMem[0x63e];
		m2=ay0;
		i1+=m2;
		i2=i1;
		ar=ay0-1;
		m3=ar;
		c1=dspDMem[0x63d];
		for(j=0;j<c1;j++)
		{
			//0x0109:	0x8C63F5  	CNTR = DM($063F)
			//0x010A:	0x700061  	MY0 = DM(I4,M5)
			//0x010B:	0x700075  	MY1 = DM(I5,M5)
			//0x010C:	0x600025  	MX0 = DM(I1,M1)
			//0x010D:	0x14116E  	DO $0116 UNTIL CE
			my0=dspDMem[i4++];
			my1=dspDMem[i5++];
			mx0=dspDMem[i1++];
			c2=dspDMem[0x63f];
			for(k=0;k<c2;k++)
			{
				//0x010E:	0x608035  	MR = MX0 * MY0 (SS), MX1 = DM(I1,M1)
				imr=(short)mx0*(short)my0;
				imr<<=1;
				mx1=dspDMem[i1++];

				//0x010F:	0x606941  	MR = MR - MX1 * MY1 (RND), AY0 = DM(I0,M1)
				temp=(short)mx1*(short)my1;
				temp<<=1;
				imr=imr-temp;
				temp&=0xffff;
				imr+=0x8000;
				if(temp==0x8000)
				{
					imr&= ~((UINT64)0x10000);
				}
				ay0=dspDMem[i0++];

				//0x0110:	0x28810C  	MR = MX1 * MY0 (SS), AX0 = MR1
				ax0=(imr>>16);
				imr=(short)mx1*(short)my0;
				imr<<=1;

				//0x0111:	0x604850  	MR = MR + MX0 * MY1 (RND), AY1 = DM(I0,M0)
				temp=(short)mx0*(short)my1;
				temp<<=1;
				imr=imr+temp;
				temp&=0xffff;
				imr+=0x8000;
				if(temp==0x8000)
				{
					imr&= ~((UINT64)0x10000);
				}
				ay1=dspDMem[i0--];
				mr1=(imr>>16);

				//0x0112:	0x632025  	AR = AY0 - AX0, MX0 = DM(I1,M1)
				ar=ay0-ax0;
				mx0=dspDMem[i1++];

				//0x0113:	0x6A60A1  	AR = AX0 + AY0, DM(I0,M1) = AR
				dspDMem[i0++]=ar;
				ar=ax0+ay0;

				//0x0114:	0x6B2CA9  	AR = AY1 - MR1, DM(I2,M1) = AR
				dspDMem[i2++]=ar;
				ar=ay1-mr1;

				//0x0115:	0x6A6CA1  	AR = MR1 + AY1, DM(I0,M1) = AR
				//0x0116:	0x6800A9  	DM(I2,M1) = AR
				dspDMem[i0++]=ar;
				ar=mr1+ay1;
				dspDMem[i2++]=ar;
			}
			//0x0117:	0x09000A  	MODIFY (I2,M2)
			//0x0118:	0x090007  	MODIFY (I1,M3)
			//0x0119:	0x090002  	MODIFY (I0,M2)
			i2+=m2;
			i1+=m3;
			i0+=m2;
		}
		//0x011A:	0x8063D8  	SI = DM($063D)
		//0x011B:	0x0F1001  	SR = LSHIFT SI BY 1 (LO)
		//0x011C:	0x9063DE  	DM($063D) = SR0
		//0x011D:	0x8063F8  	SI = DM($063F)
		//0x011E:	0x9063E8  	DM($063E) = SI
		//0x011F:	0x0F10FF  	SR = LSHIFT SI BY -1 (LO)
		//0x0120:	0x9063FE  	DM($063F) = SR0
		si=dspDMem[0x63d];
		si<<=1;
		dspDMem[0x63d]=si;
		si=dspDMem[0x63f];
		dspDMem[0x63e]=si;
		si>>=1;
		dspDMem[0x63f]=si;
	}
	//0x0121:	0x340004  	M0 = $0000
	//0x0122:	0x378000  	I0 = $3800
	//0x0123:	0x3C1005  	CNTR = $0100
	//0x0124:	0x8390E6  	MY0 = DM($390E)
	//0x0125:	0x14129E  	DO $0129 UNTIL CE
//return;
	i0=0x3800;
	my0=dspDMem[0x390e];
	int  limr;
	WORD ov;
	for(i=0;i<0x100;i++)
	{
		//0x0126:	0x600020  	MX0 = DM(I0,M0)
		//0x0127:	0x20A00F  	MR = MX0 * MY0 (SU)
		//0x0128:	//0x010000  	IF MV SAT MR
		mx0=dspDMem[i0];
		limr=(int)(short)mx0*(DWORD)(WORD)my0;
		limr<<=1;
		//0x0129:	0x6800C1  	DM(I0,M1) = MR1
		dspDMem[i0++]=(limr>>16);
	}
/*
*/

	//0x012A:	0x40800A  	AR = $0800
	//0x012B:	0x93000A  	DM($3000) = AR
	//0x012C:	0x0D02A3  	AR = I7
	//0x012D:	0x404F04  	AY0 = $04F0
	//0x012E:	0x26E20F  	AF = AR - AY0
	//0x012F:	0x181364  	IF LT JUMP $0136
	//0x0130:	0x0D02A3  	AR = I7
	//0x0131:	0x26E20F  	AF = AR - AY0
	//0x0132:	0x181305  	IF GE JUMP $0130
	//0x0133:	0x404F0A  	AR = $04F0
	//0x0134:	0x9060CA  	DM($060C) = AR
	//0x0135:	0x1813BF  	JUMP $013B
	//0x0136:	0x0D02A3  	AR = I7
	//0x0137:	0x26E20F  	AF = AR - AY0
	//0x0138:	0x181364  	IF LT JUMP $0136
	//0x0139:	0x40400A  	AR = $0400
	//0x013A:	0x9060CA  	DM($060C) = AR
	//hleDSPUpdateAudio();
}


void hleDSPMain3()
{
	WORD *dst;
	DWORD *src;
	WORD count;
	WORD si,i0,i3,i1,sr0,sr1,m3,m2,i6,af,mx0,mx1,mr0;
	short se;
	WORD ax1,ax0,m6,i2,i4,ay0,ay1,mr2,mr1,i5;
	WORD my0,my1;
	DWORD tmp,vptr;
	DWORD sr;
	WORD i,j,k,c1,c2,c3;
	WORD ar;
	DWORD page,BasePage;

	//0x013B:	0x340070  	I0 = $0007
	//0x013C:	0x360071  	I1 = $2007
	//0x013D:	0x340406  	M2 = $0040
	//0x013E:	0x0C00C0  	ENA BIT_REV 
	//0x013F:	0x3862D1  	I5 = $062D
	//0x0140:	0x3863C2  	I6 = $063C
	//0x0141:	0x3861D0  	I4 = $061D
	//0x0142:	0x3BFFF6  	M6 = $3FFF
	//0x0143:	0x380027  	M7 = $0002
	//0x0144:	0x3C0085  	CNTR = $0008
	//0x0145:	0x14153E  	DO $0153 UNTIL CE
	i0=0x7;
	i1=0x2007;
	m2=0x40;
	i5=0x62d;
	i6=0x63c;
	i4=0x61d;
	int imr,temp;
	for(i=0;i<8;i++)
	{
		//0x0146:	0x700065  	MY0 = DM(I5,M5)
		//0x0147:	0x600020  	MX0 = DM(I0,M0)
		my0=dspDMem[i5++];
		mx0=dspDMem[dspBitRev[i0]];

		//0x0148:	0x70A021  	MR = MX0 * MY0 (SU), MX0 = DM(I4,M5)
		imr=(short)mx0*my0;
		imr<<=1;
		mx0=dspDMem[i4++];

		//0x0149:	0x70006A  	MY0 = DM(I6,M6)
		//0x014A:	0x21200F  	MR = MR + MX0 * MY0 (SU)
		my0=dspDMem[i6--];
		temp=(short)mx0*my0;
		temp<<=1;
		imr+=temp;

		//0x014B:	0x20580F  	MR = MR + MX0 * 0 (RND)
		//0x014C:	0x6800C2  	DM(I0,M2) = MR1
		temp=(short)mx0*(short)0;
		temp<<=1;
		imr=imr+temp;
		temp&=0xffff;
		imr+=0x8000;
		if(temp==0x8000)
		{
			imr&= ~((UINT64)0x10000);
		}
		dspDMem[dspBitRev[i0]]=(imr>>16);
		i0+=0x40;

		//0x014D:	0x700065  	MY0 = DM(I5,M5)
		//0x014E:	0x600024  	MX0 = DM(I1,M0)
		my0=dspDMem[i5++];
		mx0=dspDMem[dspBitRev[i1]];

		//0x014F:	0x70A021  	MR = MX0 * MY0 (SU), MX0 = DM(I4,M5)
		//0x0150:	0x70006A  	MY0 = DM(I6,M6)
		imr=(short)mx0*my0;
		imr<<=1;
		mx0=dspDMem[i4++];
		my0=dspDMem[i6--];

		//0x0151:	0x21200F  	MR = MR + MX0 * MY0 (SU)
		temp=(short)mx0*my0;
		temp<<=1;
		imr+=temp;

		//0x0152:	0x20580F  	MR = MR + MX0 * 0 (RND)
		temp=(short)mx0*(short)0;
		temp<<=1;
		imr=imr+temp;
		temp&=0xffff;
		imr+=0x8000;
		if(temp==0x8000)
		{
			imr&= ~((UINT64)0x10000);
		}
		//0x0153:	0x6800C6  	DM(I1,M2) = MR1
		dspDMem[dspBitRev[i1]]=(imr>>16);
		i1+=0x40;

	}
	//0x0154:	0x8860C0  	I4 = DM($060C)
	//0x0155:	0x340070  	I0 = $0007
	//0x0156:	0x360071  	I1 = $2007
	//0x0157:	0x3C0785  	CNTR = $0078
	//0x0158:	0x1415CE  	DO $015C UNTIL CE
	i4=dspDMem[0x60c];
	i0=0x7;
	i1=0x2007;
	for(i=0;i<0x78;i++)
	{
		//0x0159:	0x6000A2  	AR = DM(I0,M2)
		//0x015A:	0x7800A1  	DM(I4,M5) = AR
		//0x015B:	0x6000A6  	AR = DM(I1,M2)
		//0x015C:	0x7800A1  	DM(I4,M5) = AR
		dspDMem[i4++]=dspDMem[dspBitRev[i0]];
		i0+=0x40;
		dspDMem[i4++]=dspDMem[dspBitRev[i1]];
		i1+=0x40;
	}

	//0x015D:	0x3861D0  	I4 = $061D
	//0x015E:	0x3C0085  	CNTR = $0008
	//0x015F:	0x14163E  	DO $0163 UNTIL CE
	i4=0x61d;
	for(i=0;i<8;i++)
	{
		//0x0160:	0x6000A2  	AR = DM(I0,M2)
		//0x0161:	0x7800A1  	DM(I4,M5) = AR
		//0x0162:	0x6000A6  	AR = DM(I1,M2)
		//0x0163:	0x7800A1  	DM(I4,M5) = AR
		dspDMem[i4++]=dspDMem[dspBitRev[i0]];
		i0+=0x40;
		dspDMem[i4++]=dspDMem[dspBitRev[i1]];
		i1+=0x40;
	}
	//0x0164:	0x0C0080  	DIS BIT_REV 
	//0x0165:	0x3C0181  	MSTAT = $0018
	//0x0166:	0x1C802F  	CALL $0802
	//0x0167:	0x18021F  	JUMP $0021
	hleDSPDecode();
}


void hleDSPZeroPtrs(WORD offset)
{
	//0x340,0x344,0x348,0x34c,0x350,0x354,0x358,0x35c=0
	//0x0332:	0x403504  	AY0 = $0350
	//0x0333:	0x22630F  	AR = MR0 + AY0
	//0x0334:	0x0D040A  	I0 = AR
	//0x0335:	0x403404  	AY0 = $0340
	//0x0336:	0x22630F  	AR = MR0 + AY0
	//0x0337:	0x0D041A  	I1 = AR
	//0x0338:	0x340046  	M2 = $0004
	//0x0339:	0x400008  	SI = $0000
	//0x033A:	0x3C0045  	CNTR = $0004
	//0x033B:	0x1433DE  	DO $033D UNTIL CE
	//0x033C:	0x680082  	DM(I0,M2) = SI
	//0x033D:	0x680086  	DM(I1,M2) = SI
	//0x033E:	0x0a000F  	RTS
	WORD *dst=(WORD *)&dspDMem[0x340+offset];
	*(dst++)=0;
	dst+=3;
	*(dst++)=0;
	dst+=3;
	*(dst++)=0;
	dst+=3;
	*(dst++)=0;
	dst+=3;
	*(dst++)=0;
	dst+=3;
	*(dst++)=0;
	dst+=3;
	*(dst++)=0;
	dst+=3;
	*(dst++)=0;
	dst+=3;
}

void hleMakeVPtr()
{
	//take 39fd and 39fe and make vptr, store in 390f,39ff
	//0x033F:	0x839FD8  	SI = DM($39FD)
	//0x0340:	0x0F0009  	SR = LSHIFT SI BY 9 (HI)
	//0x0341:	0x839FE8  	SI = DM($39FE)
	//0x0342:	0x0F08F9  	SR = SR OR LSHIFT SI BY -7 (HI)
	//0x0343:	0x0F17FB  	SR = LSHIFT SR1 BY -5 (LO)
	//0x0344:	0x9390FE  	DM($390F) = SR0
	//0x0345:	0x0F1004  	SR = LSHIFT SI BY 4 (LO)
	//0x0346:	0x0F16FC  	SR = LSHIFT SR0 BY -4 (LO)
	//0x0347:	0x420008  	SI = $2000
	//0x0348:	0x0F1800  	SR = SR OR LSHIFT SI BY 0 (LO)
	//0x0349:	0x939FFE  	DM($39FF) = SR0
	//0x034A:	0x0a000F  	RTS
	WORD hi,lo;
	hi=dspDMem[0x39fd];
	lo=dspDMem[0x39fe];
	DWORD tmp=(*(DWORD *)&dspDMem[0x39fd])&0xffffff;
	DWORD test=(hi<<16)|lo;

}

//get byte from vptr, inc vptr and check page inc
//0x034B:	0x8390F4  	AY0 = DM($390F)
//0x034C:	0x930004  	DM($3000) = AY0
//0x034D:	0x839FF5  	AY1 = DM($39FF)
//0x034E:	0x0D0405  	I0 = AY1
//0x034F:	0x6000A1  	AR = DM(I0,M1)
//0x0350:	0x0F1208  	SR = LSHIFT AR BY 8 (LO)
//0x0351:	0x0F16F8  	SR = LSHIFT SR0 BY -8 (LO)
//0x0352:	0x0D00AE  	AR = SR0
//0x0353:	0x0D0110  	AX1 = I0
//0x0354:	0x939FF1  	DM($39FF) = AX1
//0x0355:	0x430005  	AY1 = $3000
//0x0356:	0x26E90F  	AF = AX1 - AY1
//0x0357:	//0x0a0004  	IF LT RTS
//0x0358:	0x42000A  	AR = $2000
//0x0359:	0x939FFA  	DM($39FF) = AR
//0x035A:	0x22200F  	AR = AY0 + 1
//0x035B:	0x9390FA  	DM($390F) = AR
//0x035C:	0x0D00AE  	AR = SR0
//0x035D:	0x0a000F  	RTS

//get word from vptr
//0x035E:	0x1C34BF  	CALL $034B
//0x035F:	0x0D00BA  	MR0 = AR
//0x0360:	0x1C34BF  	CALL $034B
//0x0361:	0x0D00EA  	SR0 = AR
//0x0362:	0x0F1B08  	SR = SR OR LSHIFT MR0 BY 8 (LO)
//0x0363:	0x0D00AE  	AR = SR0
//0x0364:	0x0a000F  	RTS

//get 24 bits from vptr
//0x0365:	0x1C34BF  	CALL $034B
//0x0366:	0x0D0C7A  	PX = AR
//0x0367:	0x1C35EF  	CALL $035E
//0x0368:	0x0a000F  	RTS

//put vptr at 39fd,39fe
//0x0369:	0x839FFE  	SR0 = DM($39FF)
//0x036A:	0x0F1604  	SR = LSHIFT SR0 BY 4 (LO)
//0x036B:	0x0F16FC  	SR = LSHIFT SR0 BY -4 (LO)
//0x036C:	0x8390F8  	SI = DM($390F)
//0x036D:	0x0F180C  	SR = SR OR LSHIFT SI BY 12 (LO)
//0x036E:	0x939FDF  	DM($39FD) = SR1
//0x036F:	0x939FEE  	DM($39FE) = SR0
//0x0370:	0x0a000F  	RTS

//
void hleDSPResetPtrs(DWORD value,WORD offset)
{

	//0x0371:	0x0D008A  	SI = AR
	//0x0372:	0x403A84  	AY0 = $03A8
	//0x0373:	0x22630F  	AR = MR0 + AY0
	//0x0374:	0x0D040A  	I0 = AR
	//0x0375:	0xA00000  	DM(I0,M0) = $0000
	dspDMem[offset+0x3a8]=0;

	//0x0376:	0x403B44  	AY0 = $03B4
	//0x0377:	0x22630F  	AR = MR0 + AY0
	//0x0378:	0x0D040A  	I0 = AR
	//0x0379:	0xA00000  	DM(I0,M0) = $0000
	dspDMem[offset+0x3b4]=0;

	//0x037A:	0x403804  	AY0 = $0380
	//0x037B:	0x26630F  	AF = MR0 + AY0
	//0x037C:	0x22730F  	AR = MR0 + AF
	//0x037D:	0x0D040A  	I0 = AR
	//0x037E:	0x0D03A7  	AR = PX
	//0x037F:	0x6800A1  	DM(I0,M1) = AR
	//0x0380:	0x680080  	DM(I0,M0) = SI
	dspDMem[offset+0x380+offset]=(value>>16);
	dspDMem[offset+0x380+offset+1]=(value&0xffff);

	//0x0381:	0x403FC4  	AY0 = $03FC
	//0x0382:	0x22630F  	AR = MR0 + AY0
	//0x0383:	0x0D040A  	I0 = AR
	//0x0384:	0xA00000  	DM(I0,M0) = $0000
	dspDMem[offset+0x3fc]=0;

	//0x0385:	0x403884  	AY0 = $0388
	//0x0386:	0x26630F  	AF = MR0 + AY0
	//0x0387:	0x22730F  	AR = MR0 + AF
	//0x0388:	0x0D040A  	I0 = AR
	//0x0389:	0xA00001  	DM(I0,M1) = $0000
	//0x038A:	0xA00000  	DM(I0,M0) = $0000
	dspDMem[offset+0x388+offset]=0;
	dspDMem[offset+0x388+offset+1]=0;

	//0x038B:	0x0D009B  	SE = MR0
	//0x038C:	0x40001A  	AR = $0001
	//0x038D:	0x0E120F  	SR = LSHIFT AR (LO)
	//0x038E:	0x27660F  	AF = NOT SR0
	//0x038F:	0x80603A  	AR = DM($0603)
	//0x0390:	0x23920F  	AR = AR AND AF
	//0x0391:	0x90603A  	DM($0603) = AR
	dspDMem[0x603]&=~(1<<offset);

	//0x0392:	0x1C332F  	CALL $0332
	hleDSPZeroPtrs(offset);
	//0x0393:	0x0a000F  	RTS
}


void hleDSPDecode()
{
	WORD i0,i1,i2,i3,i4,i;
	DWORD sr,mr;
	short ar;
	WORD ax0,ax1,ay0,ay1;
	WORD mx0,my0,mr1,sr1,sr0,mr0;
	WORD count,t2,t3;

	//0x0a7E:	0x343400  	I0 = $0340
	//0x0a7F:	0x343501  	I1 = $0350
	//0x0a80:	0x343602  	I2 = $0360
	//0x0a81:	0x343703  	I3 = $0370
	i0=0x340;
	i1=0x350;
	i2=0x360;
	i3=0x370;

	//0x0a82:	0x3C0105  	CNTR = $0010
	//0x0a83:	0x14A9EE  	DO $0A9E UNTIL CE
	count=0x10;
	while(count)
	{
		//0x0a84:	0x600040  	AY0 = DM(I0,M0)
		//0x0a85:	0x22000F  	AR = AY0
		//0x0a86:	0x18A9A3  	IF LE JUMP $0A9A
		ay0=dspDMem[i0];
		ar=ay0;
		if(ar>0)
		{
			//0x0a87:	0x23000F  	AR = AY0 - 1
			//0x0a88:	0x6800A0  	DM(I0,M0) = AR
			//0x0a89:	0x18A8D1  	IF NE JUMP $0A8D
			ar=ay0-1;
			dspDMem[i0]=ar;
			if(ar==0)
			{
				//0x0a8A:	0x6000AC  	AR = DM(I3,M0)
				//0x0a8B:	0x6800A4  	DM(I1,M0) = AR
				//0x0a8C:	0x18A9AF  	JUMP $0A9A
				ar=dspDMem[i3];
				dspDMem[i1]=ar;
				break;
			}
			//0x0a8D:	0x6000A4  	AR = DM(I1,M0)
			//0x0a8E:	0x600048  	AY0 = DM(I2,M0)
			//0x0a8F:	0x22620F  	AR = AR + AY0
			//0x0a90:	0x41FFF4  	AY0 = $1FFF
			//0x0a91:	0x26E20F  	AF = AR - AY0
			//0x0a92:	0x18A953  	IF LE JUMP $0A95
			ar=dspDMem[i1];
			ay0=dspDMem[i2];
			ar+=(short)ay0;
			if(ar>0x1fff)
			{
				//0x0a93:	0x41FFFA  	AR = $1FFF
				//0x0a94:	0x18A99F  	JUMP $0A99
				ar=0x1fff;
			}
			else
			{
				//0x0a95:	0x4E0014  	AY0 = $E001
				//0x0a96:	0x26E20F  	AF = AR - AY0
				//0x0a97:	0x18A995  	IF GE JUMP $0A99
				//0x0a98:	0x4E001A  	AR = $E001
				if(ar<(short)0xe001)
				{
					ar=(short)0xe001;
				}
			}
			//0x0a99:	0x6800A4  	DM(I1,M0) = AR
			dspDMem[i1]=ar;
		}
		//0x0a9A:	0x090001  	MODIFY (I0,M1)
		//0x0a9B:	0x090005  	MODIFY (I1,M1)
		//0x0a9C:	0x090009  	MODIFY (I2,M1)
		//0x0a9D:	0x09000D  	MODIFY (I3,M1)
		//0x0a9E:	0x000000  	NOP
		i0++;
		i1++;
		i2++;
		i3++;
		count--;
	}
	//0x0a9F:	0x343500  	I0 = $0350
	//0x0aA0:	0x343A01  	I1 = $03A0
	//0x0aA1:	0x345FF2  	I2 = $05FF
	//0x0aA2:	0x3C0045  	CNTR = $0004
	//0x0aA3:	0x14AC7E  	DO $0AC7 UNTIL CE
	i0=0x350;
	i1=0x3a0;
	i2=0x5ff;
	count=4;
	while(count)
	{
		//0x0aA4:	0x40000A  	AR = $0000
		//0x0aA5:	0x3C0045  	CNTR = $0004
		//0x0aA6:	0x14AA8E  	DO $0AA8 UNTIL CE
		ar=0;
		for(WORD i=0;i<4;i++)
		{
			//0x0aA7:	0x600041  	AY0 = DM(I0,M1)
			//0x0aA8:	0x22620F  	AR = AR + AY0
			ar+=(short)dspDMem[i0];
			i0++;
		}
		//0x0aA9:	0x41FFF4  	AY0 = $1FFF
		//0x0aAA:	0x26E20F  	AF = AR - AY0
		//0x0aAB:	0x18AAE3  	IF LE JUMP $0AAE
		//0x0aAC:	0x41FFFA  	AR = $1FFF
		//0x0aAD:	0x18AB2F  	JUMP $0AB2
		//0x0aAE:	0x4E0014  	AY0 = $E001
		//0x0aAF:	0x26E20F  	AF = AR - AY0
		//0x0aB0:	0x18AB25  	IF GE JUMP $0AB2
		//0x0aB1:	0x4E001A  	AR = $E001
		if(ar>0x1fff)
			ar=0x1fff;
		else if(ar<(short)0xe001)
			ar=(short)0xe001;

		//0x0aB2:	0x6000E9  	SR0 = DM(I2,M1)
		//0x0aB3:	0x0F1607  	SR = LSHIFT SR0 BY 7 (LO)
		//0x0aB4:	0x0D002E  	MX0 = SR0
		sr=dspDMem[i2]<<7;
		i2++;
		mx0=sr&0xffff;

		//0x0aB5:	0x0F12FA  	SR = LSHIFT AR BY -6 (LO)
		//0x0aB6:	0x400804  	AY0 = $0080
		//0x0aB7:	0x22660F  	AR = SR0 + AY0
		//0x0aB8:	0x0D00FA  	SR1 = AR
		WORD t=ar;
		sr=t>>6;
		ar=(sr&0xffff)+0x80;
		sr1=ar;

		//0x0aB9:	0x3C0085  	CNTR = $0008
		//0x0aBA:	0x47C946  	MY0 = $7C94
		//0x0aBB:	0x3C0081  	MSTAT = $0008
		//0x0aBC:	0x14AC4E  	DO $0AC4 UNTIL CE
		my0=0x7c94;
		for(i=0;i<8;i++)
		{
			//0x0aBD:	0x0F07FF  	SR = LSHIFT SR1 BY -1 (HI)
			sr=((DWORD)sr1<<15);
			sr1=(sr>>16);

			//0x0aBE:	0x267E0F  	AF = SR0
			//0x0aBF:	0x0D00C2  	MR1 = MX0
			//0x0aC0:	0x20E400  	IF EQ MR = MR1 * MY0 (UU)
			mr1=mx0;
			if((sr&0xffff)==0)
			{
				mr=(DWORD)mr1*(DWORD)my0;
				mr<<=1;
				mr1=(mr>>16);
			}

			//0x0aC1:	0x0D002C  	MX0 = MR1
			//0x0aC2:	0x0D00C6  	MR1 = MY0
			//0x0aC3:	0x28E4E2  	MR = MR1 * MY0 (UU), SR0 = MX0
			//0x0aC4:	0x0D006C  	MY0 = MR1
			mx0=mr1;
			mr1=my0;
			mr=(DWORD)mr1*(DWORD)my0;
			mr<<=1;
			sr0=mx0;
			my0=(mr>>16);
		}
		//0x0aC5:	0x3C0181  	MSTAT = $0018
		//0x0aC6:	0x0F1601  	SR = LSHIFT SR0 BY 1 (LO)
		//0x0aC7:	0x6800E5  	DM(I1,M1) = SR0
		sr=sr0<<1;
		dspDMem[i1]=sr&0xffff;
		i1++;
		count--;
	}

	//0x0aC8:	0x805FCF  	SR1 = DM($05FC)
	//0x0aC9:	0x2E7FEF  	AF = SR1, SR0 = SR1
	//0x0aCA:	0x18ADA0  	IF EQ JUMP $0ADA
	sr1=dspDMem[0x5fc];
	sr0=sr1;
	if(sr1)
	{
		//0x0aCB:	0x3C0085  	CNTR = $0008
		//0x0aCC:	0x43FFF2  	MX0 = $3FFF
		//0x0aCD:	0x47D986  	MY0 = $7D98
		//0x0aCE:	0x3C0081  	MSTAT = $0008
		//0x0aCF:	0x14AD7E  	DO $0AD7 UNTIL CE
		mx0=0x3fff;
		my0=0x7d98;
		for(i=0;i<8;i++)
		{
			//0x0aD0:	0x0F07FF  	SR = LSHIFT SR1 BY -1 (HI)
			sr=((DWORD)sr1<<15);
			sr1=(sr>>16);

			//0x0aD1:	0x267E0F  	AF = SR0
			//0x0aD2:	0x0D00C2  	MR1 = MX0
			//0x0aD3:	0x20E400  	IF EQ MR = MR1 * MY0 (UU)
			mr1=mx0;
			if((sr&0xffff)==0)
			{
				mr=(DWORD)mr1*(DWORD)my0;
//				mr<<=1;
				mr1=(mr>>15);
			}

			//0x0aD4:	0x0D002C  	MX0 = MR1
			//0x0aD5:	0x0D00C6  	MR1 = MY0
			//0x0aD6:	0x28E4E2  	MR = MR1 * MY0 (UU), SR0 = MX0
			//0x0aD7:	0x0D006C  	MY0 = MR1
			mx0=mr1;
			mr1=my0;
			mr=(DWORD)mr1*(DWORD)my0;
//			mr<<=1;
			sr0=mx0;
			my0=(mr>>15);
		}
		//0x0aD8:	0x3C0181  	MSTAT = $0018
		//0x0aD9:	0x0F1601  	SR = LSHIFT SR0 BY 1 (LO)
		sr=sr0<<1;
		sr0=sr&0xffff;

	}
	//0x0aDA:	0x905FDE  	DM($05FD) = SR0
	//0x0aDB:	0x343A80  	I0 = $03A8
	//0x0aDC:	0x343B41  	I1 = $03B4
	//0x0aDD:	0x343B02  	I2 = $03B0
	//0x0aDE:	0x343AC3  	I3 = $03AC
	//0x0aDF:	0x37FFF6  	M2 = $3FFF
	//0x0aE0:	0x3C0045  	CNTR = $0004
	//0x0aE1:	0x14AF6E  	DO $0AF6 UNTIL CE
	dspDMem[0x5fd]=sr0;
	i0=0x3a8;
	i1=0x3b4;
	i2=0x3b0;
	i3=0x3ac;
	count=4;
	while(count)
	{
		//0x0aE2:	0x600040  	AY0 = DM(I0,M0)
		//0x0aE3:	0x22200F  	AR = AY0 + 1
		//0x0aE4:	0x6800A1  	DM(I0,M1) = AR
		ay0=dspDMem[i0];
		ar=ay0+1;
		dspDMem[i0]=ar;
		i0++;

		//0x0aE5:	0x60005D  	AY1 = DM(I3,M1)
		//0x0aE6:	0x26180F  	AF = 0
		//0x0aE7:	0x6000A5  	AR = DM(I1,M1)
		//0x0aE8:	0x227A0F  	AR = AR
		//0x0aE9:	0x18AF50  	IF EQ JUMP $0AF5
		ay1=dspDMem[i3];
		i3++;
		ar=dspDMem[i1];
		i1++;
		if(ar!=0)
		{
			//0x0aEA:	0x600048  	AY0 = DM(I2,M0)
			//0x0aEB:	0x23000F  	AR = AY0 - 1
			//0x0aEC:	0x6800A9  	DM(I2,M1) = AR
			//0x0aED:	0x18AF61  	IF NE JUMP $0AF6
			ay0=dspDMem[i2];
			ar=ay0-1;
			dspDMem[i2]=ar;
			i2++;
			if(ar==0)
			{
				//0x0aEE:	0x3B4000  	I4 = $3400
				//0x0aEF:	0x780050  	DM(I4,M4) = AY1
				//0x0aF0:	0x090006  	MODIFY (I1,M2)
				//0x0aF1:	0x09000A  	MODIFY (I2,M2)
				//0x0aF2:	0x6000A5  	AR = DM(I1,M1)
				//0x0aF3:	0x6800A9  	DM(I2,M1) = AR
				//0x0aF4:	0x18AF6F  	JUMP $0AF6
				dspPeek=ay1;
				dspDMem[0x3400]=ay1;
				i1--;
				i2--;
				ar=dspDMem[i1];
				i1++;
				dspDMem[i2]=ar;
				i2++;
			}
		}
		else
		{
			//0x0aF5:	0x090009  	MODIFY (I2,M1)
			i2++;
		}
		//0x0aF6:	0x000000  	NOP
		count--;
	}
	//0x0aF7:	0x8060B4  	AY0 = DM($060B)
	//0x0aF8:	0x22200F  	AR = AY0 + 1
	//0x0aF9:	0x9060BA  	DM($060B) = AR
	//0x0aFA:	0x4000D4  	AY0 = $000D
	//0x0aFB:	0x26E20F  	AF = AR - AY0
	//0x0aFC:	0x220005  	IF GE AR = AY0
	//0x0aFD:	0x9060BA  	DM($060B) = AR
	//0x0aFE:	0x0a000F  	RTS
	ar=dspDMem[0x60b];
	ar++;
	if(ar>0xd)
		ar=0xd;
	dspDMem[0x60b]=ar;
}

void hleDSPVSetPageOffset(WORD page,WORD offset)
{
	dspDMem[0x390f]=page;
	dspDMem[0x39ff]=offset|0x2000;

	if(offset&0x2000) offset^=0x2000;
	page=((page & 0xff00)>>1)|page&0x7f;
	hleDSPVCurAddy=offset|(page<<12);
}

void hleDSPVSetAbs(DWORD addy)
{
	DWORD page,offset;

	offset=addy&0xfff;
	page=(addy&0xfffff000)>>12;
	hleDSPVSetPageOffset(page,offset);
}

void hleDSPVGetAbs(DWORD *addy)
{
	DWORD page,offset;

	offset=hleDSPVCurAddy&0xfff;
	page=(hleDSPVCurAddy&0xfffff000)>>12;
	page=(page&0x7f)|((page&0xf80)<<1);
	*addy=(page<<12)|offset;
}

void hleDSPVFlush()
{
	DWORD page,offset;


	offset=hleDSPVCurAddy&0xfff;
	page=(hleDSPVCurAddy&0xfffff000)>>12;
	page=(page&0x7f)|((page&0xf80)<<1);
	dspDMem[0x390f]=page;
	dspDMem[0x39ff]=offset|0x2000;
}

void hleDSPVGetPageOffset(WORD *page,WORD *offset)
{
	*offset=(hleDSPVCurAddy&0xfff)|0x2000;
	*page=(hleDSPVCurAddy&0xfffff000)>>12;
	*page=((*page)&0x7f)|(((*page)&0xf80)<<1);
}


void hleDSPVSetMem()
{
	DWORD page,offset;

	page=dspDMem[0x390f];
	offset=dspDMem[0x39ff];
	if(offset&0x2000) offset^=0x2000;
	hleDSPVSetPageOffset(page,offset);
}

void hleDSPVRead24(DWORD *val)
{
	DWORD tmp;
	DWORD offset;

	tmp=*(DWORD *)&m->dspRMem[hleDSPVCurAddy];
	*val=((tmp&0xff0000)>>16)|(tmp&0xff00)|((tmp&0xff)<<16);
	offset=hleDSPVCurAddy&0xfff;
	if(offset>0xffc)
	{
		DWORD page;
		offset+=3;
		offset&=0xfff;
		page=(hleDSPVCurAddy&0xfffff000)>>12;
		page=(page&0x7f)|((page&0xf80)<<1);
		page++;
		hleDSPVSetPageOffset(page,offset);
		return;
	}
	hleDSPVCurAddy+=3;
}

void hleDSPVRead16(WORD *val)
{
	WORD tmp;
	DWORD offset;

	tmp=*(WORD *)&m->dspRMem[hleDSPVCurAddy];
	*val=(tmp<<8)|((tmp&0xff00)>>8);
	offset=hleDSPVCurAddy&0xfff;
	if(offset>0xffd)
	{
		DWORD page;
		offset+=2;
		offset&=0xfff;
		page=(hleDSPVCurAddy&0xfffff000)>>12;
		page=(page&0x7f)|((page&0xf80)<<1);
		page++;
		hleDSPVSetPageOffset(page,offset);
		return;
	}
	hleDSPVCurAddy+=2;
}

void hleDSPVRead8(WORD *val)
{
	WORD tmp;
	DWORD offset;

	*val=*(BYTE *)&m->dspRMem[hleDSPVCurAddy];
	offset=hleDSPVCurAddy&0xfff;
	if(offset>0xffe)
	{
		DWORD page;
		offset+=1;
		offset&=0xfff;
		page=(hleDSPVCurAddy&0xfffff000)>>12;
		page=(page&0x7f)|((page&0xf80)<<1);
		page++;
		hleDSPVSetPageOffset(page,offset);
		return;
	}
	hleDSPVCurAddy++;
}

void hleDSPCopySample(DWORD *page)
{
	WORD ax0,ax1,ay1,ay0,i0,i1,mr1,i2,si;
	short ar;
	short se;
	DWORD sr;

	//0x0a49:	0x3799C2  	I2 = $399C
	//0x0a4A:	0x600008  	AX0 = DM(I2,M0)
	//0x0a4B:	0x400FF1  	AX1 = $00FF
	//0x0a4C:	0x400105  	AY1 = $0010
	//0x0a4D:	0x846431  	I1 = DM($0643)
	//0x0a4E:	0x400014  	AY0 = $0001
	i2=0x399c;
	ax0=dspDMem[i2];
	ax1=0xff;
	ay1=0x10;
	i1=dspDMem[0x643];
	ay0=1;

Label0xa4f:
	//0x0a4F:	0x346400  	I0 = $0640
	//0x0a50:	0x1CA27F  	CALL $0A27
	i0=0x640;

	{
		//0x0a27:	0x6000A0  	AR = DM(I0,M0)
		//0x0a28:	0x626284  	AR = AR + AY0, SI = DM(I1,M0)
		//0x0a29:	0x0D009A  	SE = AR
		//0x0a2A:	0x1290A0  	SR = LSHIFT SI (LO), DM(I0,M0) = AR
		ar=dspDMem[i0];
		ar+=ay0;
		si=dspDMem[i1];
		se=(short)ar;
		if(se>0)
			sr=(DWORD)si<<se;
		else
			sr=si>>(se*-1);

		dspDMem[i0]=ar;

		//0x0a2B:	0x2AEACF  	AR = AR - AY1, MR1 = SR1
		//0x0a2C:	0x18A474  	IF LT JUMP $0A47
		ar-=ay1;
		mr1=(sr>>16);
		if(ar>=0)
		{
			//0x0a2D:	0x6800A0  	DM(I0,M0) = AR
			//0x0a2E:	0x340029  	L1 = $0002
			//0x0a2F:	0x400089  	SE = $0008
			//0x0a30:	0x400FF5  	AY1 = $00FF
			//0x0a31:	0x879FF0  	I0 = DM($39FF)
			//0x0a32:	0x600081  	SI = DM(I0,M1)
			//0x0a33:	0x1210A1  	SR = LSHIFT SI (LO), AR = DM(I0,M1)
			dspDMem[i0]=ar;
			ay1=0xff;
			i0=dspDMem[0x39ff];
			si=*(WORD *)&m->dspRMem[(i0^0x2000)|(*page)];
			i0++;
			sr=si<<8;
			ar=*(WORD *)&m->dspRMem[(i0^0x2000)|(*page)];
			i0++;

			//0x0a34:	0x238A0F  	AR = AR AND AY1
			//0x0a35:	0x0F1A00  	SR = SR OR LSHIFT AR BY 0 (LO)
			//0x0a36:	0x6800E5  	DM(I1,M1) = SR0
			ar=ar&ay1;
			sr=sr|(DWORD)(WORD)ar;
			dspDMem[i1]=(sr&0xffff);
			i1++;
			if((i1%2)==0)
				i1-=2;

			//0x0a37:	0x0D01E0  	SR0 = I0
			//0x0a38:	0x0F1603  	SR = LSHIFT SR0 BY 3 (LO)
			//0x0a39:	0x267E0F  	AF = SR0
			//0x0a3A:	0x400105  	AY1 = $0010
			//0x0a3B:	0x18A455  	IF GE JUMP $0A45
			sr=i0<<3;
			ay1=0x10;
			if(sr&0x8000)
			{
				//0x0a3C:	0x370007  	M3 = $3000
				//0x0a3D:	0x090003  	MODIFY (I0,M3)
				i0-=0x1000;

				//0x0a3E:	0x0D0474  	M3 = AY0
				//0x0a3F:	0x8390F4  	AY0 = DM($390F)
				//0x0a40:	0x22200F  	AR = AY0 + 1
				//0x0a41:	0x0D0147  	AY0 = M3
				ar=dspDMem[0x390f];
				ar++;

				//0x0a42:	0x9390FA  	DM($390F) = AR
				//0x0a43:	0x93000A  	DM($3000) = AR
				//0x0a44:	0x340009  	L1 = $0000
				dspDMem[0x390f]=ar;
				dspDMem[0x3000]=ar;
				dspBank = ((ar & 0x0f00)>>1)|ar&0x7f;
				(*page)=(dspBank<<12);
				hleDSPVSetPageOffset(ar,i0);

			}
			//0x0a45:	0x979FF0  	DM($39FF) = I0
			//0x0a46:	0x946431  	DM($0643) = I1
			dspDMem[0x39ff]=i0;
			dspDMem[0x643]=i1;
		}
		//0x0a47:	0x23840F  	AR = MR1 AND AY0
		ar=mr1&ay0;
	}


	//0x0a51:	0x18A550  	IF EQ JUMP $0A55
	if(ar)
	{
		//0x0a52:	0x0D0470  	M3 = AX0
		//0x0a53:	0x09000B  	MODIFY (I2,M3)
		//0x0a54:	0x18A56F  	JUMP $0A56
		i2+=ax0;
	}
	else
	{
		//0x0a55:	0x090009  	MODIFY (I2,M1)
		i2++;
	}

	//0x0a56:	0x600008  	AX0 = DM(I2,M0)
	//0x0a57:	0x22780F  	AR = AX0
	//0x0a58:	0x18A4F5  	IF GE JUMP $0A4F
	ax0=dspDMem[i2];
	ar=ax0;
	if(ar>=0)
		goto Label0xa4f;

	//0x0a59:	0x0D0041  	AY0 = AX1
	//0x0a5A:	0x23820F  	AR = AR AND AY0
	//0x0a5B:	0x4001E4  	AY0 = $001E
	//0x0a5C:	0x26E20F  	AF = AR - AY0
	//0x0a5D:	0x18A612  	IF GT JUMP $0A61
	ar&=ax1;
	if(ar<=0x1e)
	{
		//0x0a5E:	0x4000F4  	AY0 = $000F
		//0x0a5F:	0x22E20F  	AR = AR - AY0
		//0x0a60:	0x18A63F  	JUMP $0A63
		ar-=0xf;
	}
	else
	{
		//0x0a61:	0x4002E4  	AY0 = $002E
		//0x0a62:	0x22E20F  	AR = AR - AY0
		ar-=0x2e;
	}
	//0x0a63:	0x805FB8  	SI = DM($05FB)
	//0x0a64:	0x0F1004  	SR = LSHIFT SI BY 4 (LO)
	//0x0a65:	0x0D047E  	M3 = SR0
	//0x0a66:	0x346440  	I0 = $0644
	//0x0a67:	0x090003  	MODIFY (I0,M3)
	si=dspDMem[0x5fb];
	sr=si<<4;
	i0=0x644+(sr&0xffff);

	//0x0a68:	0x846847  	M3 = DM($0684)
	//0x0a69:	0x090003  	MODIFY (I0,M3)
	i0+=dspDMem[0x684];

	//0x0a6A:	0x600040  	AY0 = DM(I0,M0)
	//0x0a6B:	0x22620F  	AR = AR + AY0
	//0x0a6C:	0x6800A0  	DM(I0,M0) = AR
	dspDMem[i0]+=ar;
	//0x0a6D:	//0x0a000F  	RTS
}

void hleDSPIntService()
{
	WORD t1;
	short ar,t2;
	WORD *dspDMem;
	dspDMem=(WORD *)m->dspDMem;

/*
	while(!dspIMASK)
	{
		Sleep(1);
	}
*/
	//	theApp.LogMessage("DSP %X",dspPeek);

	//0x0394:	0x0C0030  	ENA SEC_REG 
	//0x0395:	0x0D0222  	MX0 = I6
	//0x0396:	0x0D0337  	MX1 = PX
	//0x0397:	0x3B4002  	I6 = $3400
	//0x0398:	0x8060BA  	AR = DM($060B)
	//0x0399:	0x4000D4  	AY0 = $000D
	//0x039A:	0x22E20F  	AR = AR - AY0
	//0x039B:	0x1839D4  	IF LT JUMP $039D
	if(dspDMem[0x60b]>=0xd)
	{
		//0x039C:	0x985FA4  	DM($05FA) = M4
		dspDMem[0x5fa]=0;
	}
	//0x039D:	0x805FA4  	AY0 = DM($05FA)
	//0x039E:	0x22000F  	AR = AY0
	//0x039F:	0x183A51  	IF NE JUMP $03A5
	t1=dspDMem[0x5fa];
	if(t1==0)
	{
		//0x03A0:	0x7000E8  	SR0 = DM(I6,M4)
		//0x03A1:	0x0F1608  	SR = LSHIFT SR0 BY 8 (LO)
		//0x03A2:	0x9060DE  	DM($060D) = SR0
		//0x03A3:	0x985FA5  	DM($05FA) = M5
		//0x03A4:	0x183E4F  	JUMP $03E4
		dspDMem[0x60d]=dspPeek<<8;
		dspDMem[0x5fa]=1;
		goto IntServiceExit2;
	}

	//0x03A5:	0x23000F  	AR = AY0 - 1
	//0x03A6:	0x183C51  	IF NE JUMP $03C5
	if(t1==1)
	{
		//0x03A7:	0x7000E8  	SR0 = DM(I6,M4)
		//0x03A8:	0x0F1608  	SR = LSHIFT SR0 BY 8 (LO)
		//0x03A9:	0x0F16F8  	SR = LSHIFT SR0 BY -8 (LO)
		//0x03AA:	0x8060DF  	SR1 = DM($060D)
		//0x03AB:	0x0F1F00  	SR = SR OR LSHIFT SR1 BY 0 (LO)
		//0x03AC:	0x400004  	AY0 = $0000
		//0x03AD:	0x22660F  	AR = SR0 + AY0
		//0x03AE:	0x183E34  	IF LT JUMP $03E3
		ar=dspDMem[0x60d]|(dspPeek&0xff);
		if(ar<0)
			goto IntServiceExit1;

		//0x03AF:	0x455AA4  	AY0 = $55AA
		//0x03B0:	0x22E60F  	AR = SR0 - AY0
		//0x03B1:	0x183B35  	IF GE JUMP $03B3
		//0x03B2:	0x183B7F  	JUMP $03B7
		if(ar>=0x55aa)
		{
			//0x03B3:	0x455AE4  	AY0 = $55AE
			//0x03B4:	0x22E60F  	AR = SR0 - AY0
			//0x03B5:	0x183BF3  	IF LE JUMP $03BF
			t2=ar-0x55ae;
			if(t2<=0)
			{
				//0x03BF:	0x455AA4  	AY0 = $55AA
				//0x03C0:	0x22E60F  	AR = SR0 - AY0
				//0x03C1:	0x905FEA  	DM($05FE) = AR
				//0x03C2:	0x40002A  	AR = $0002
				//0x03C3:	0x905FAA  	DM($05FA) = AR
				//0x03C4:	0x183E4F  	JUMP $03E4
				ar-=0x55aa;
				dspDMem[0x5fe]=ar;
				dspDMem[0x5fa]=2;
				goto IntServiceExit2;
			}
			//0x03B6:	0x183E3F  	JUMP $03E3
			goto IntServiceExit1;
		}

		//0x03B7:	0x885F82  	I6 = DM($05F8)
		//0x03B8:	0x38010A  	L6 = $0010
		//0x03B9:	0x7800E9  	DM(I6,M5) = SR0
		//0x03BA:	0x38000A  	L6 = $0000
		//0x03BB:	0x985F82  	DM($05F8) = I6
		t1=dspDMem[0x5f8];
		dspDMem[t1]=ar;
		t1++;
		if((t1%16)==0)
			t1-=16;
		dspDMem[0x5f8]=t1;

		//0x03BC:	0x3B4002  	I6 = $3400
		//0x03BD:	0x985FA4  	DM($05FA) = M4
		//0x03BE:	0x183E4F  	JUMP $03E4
		dspDMem[0x5fa]=0;
		goto IntServiceExit1;
	}

	//0x03C5:	0x40002A  	AR = $0002
	//0x03C6:	0x23220F  	AR = AY0 - AR
	//0x03C7:	0x183CF1  	IF NE JUMP $03CF
	if(t1==2)
	{
		//0x03C8:	0x7000E8  	SR0 = DM(I6,M4)
		//0x03C9:	0x0F1608  	SR = LSHIFT SR0 BY 8 (LO)
		//0x03CA:	0x0F16F8  	SR = LSHIFT SR0 BY -8 (LO)
		//0x03CB:	0x9060DE  	DM($060D) = SR0
		//0x03CC:	0x40003A  	AR = $0003
		//0x03CD:	0x905FAA  	DM($05FA) = AR
		//0x03CE:	0x183E4F  	JUMP $03E4
		dspDMem[0x60d]=dspPeek&0xff;
		dspDMem[0x5fa]=3;
		goto IntServiceExit2;
	}

	//0x03CF:	0x7000E8  	SR0 = DM(I6,M4)
	//0x03D0:	0x0F1608  	SR = LSHIFT SR0 BY 8 (LO)
	//0x03D1:	0x0F16F8  	SR = LSHIFT SR0 BY -8 (LO)
	//0x03D2:	0x8060D4  	AY0 = DM($060D)
	//0x03D3:	0x2BC604  	AR = SR0 XOR AY0, AX0 = AY0
	t1=dspDMem[0x60d];
	ar=dspDMem[0x60d]^(dspPeek&0xff);

	//0x03D4:	0x400FF4  	AY0 = $00FF
	//0x03D5:	0x22E20F  	AR = AR - AY0
	//0x03D6:	0x183E31  	IF NE JUMP $03E3
	if(ar!=0xff)
	{
		goto IntServiceExit1;
	}

	//0x03D7:	0x805FE4  	AY0 = DM($05FE)
	//0x03D8:	0x22000F  	AR = AY0
	//0x03D9:	0x183E20  	IF EQ JUMP $03E2
	if(dspDMem[0x5fe])
	{
		//0x03DA:	0x405FF4  	AY0 = $05FF
		//0x03DB:	0x805FEA  	AR = DM($05FE)
		//0x03DC:	0x22620F  	AR = AR + AY0
		//0x03DD:	0x400014  	AY0 = $0001
		//0x03DE:	0x22E20F  	AR = AR - AY0
		//0x03DF:	0x0D082A  	I6 = AR
		//0x03E0:	0x780008  	DM(I6,M4) = AX0
		//0x03E1:	0x183E3F  	JUMP $03E3
		ar=dspDMem[0x5fe];
		ar+=0x5ff;
		ar--;
		dspDMem[ar]=t1;
	}
	else
	{
		//0x03E2:	0x905FC0  	DM($05FC) = AX0
		dspDMem[0x5fc]=t1;
	}

IntServiceExit1:
	//0x03E3:	0x985FA4  	DM($05FA) = M4
	dspDMem[0x5fa]=0;

IntServiceExit2:
	//0x03E4:	0x9860B4  	DM($060B) = M4
	dspDMem[0x60b]=0;

	//0x03E5:	0x0D0822  	I6 = MX0
	//0x03E6:	0x0D0C73  	PX = MX1
	//0x03E7:	0x0A001F  	??? (0A001F)
}

void hleDSPSkipSample(WORD m3,DWORD page)
{
	WORD i0;
//	m3=adsp2100_get_reg(ADSP2100_M3);

	//0x0a6E:	0x0D01E7  	SR0 = M3
	//0x0a6F:	0x0F1601  	SR = LSHIFT SR0 BY 1 (LO)
	m3<<=1;

	//0x0a70:	0x0D047E  	M3 = SR0
	//0x0a71:	0x360000  	I0 = $2000
	//0x0a72:	0x090003  	MODIFY (I0,M3)
	i0=0x2000+m3;

	//0x0a73:	0x400FF4  	AY0 = $00FF
	//0x0a74:	0x6000A1  	AR = DM(I0,M1)
	//0x0a75:	0x0F1208  	SR = LSHIFT AR BY 8 (LO)
	//0x0a76:	0x6000A1  	AR = DM(I0,M1)
	//0x0a77:	0x23820F  	AR = AR AND AY0
	//0x0a78:	0x0F1A00  	SR = SR OR LSHIFT AR BY 0 (LO)
	//0x0a79:	0x0D047E  	M3 = SR0
	m3=*(WORD *)&m->dspRMem[(i0^0x2000)|page];
	m3=(m3<<8)|((m3>>8)&0xff);

	//0x0a7A:	0x360000  	I0 = $2000
	//0x0a7B:	0x090003  	MODIFY (I0,M3)
	//0x0a7C:	0x946130  	DM($0613) = I0
	//0x0a7D:	0x0A000F  	RTS
	dspDMem[0x613]=0x2000+m3;

//	adsp2100_set_reg(ADSP2100_M3,m3);
}

void hleDSPResetBlocks(WORD mr0)
{
	//0x0332:	0x403504  	AY0 = $0350
	//0x0333:	0x22630F  	AR = MR0 + AY0
	//0x0334:	0x0D040A  	I0 = AR
	//0x0335:	0x403404  	AY0 = $0340
	//0x0336:	0x22630F  	AR = MR0 + AY0
	//0x0337:	0x0D041A  	I1 = AR
	//0x0338:	0x340046  	M2 = $0004
	//0x0339:	0x400008  	SI = $0000
	//0x033A:	0x3C0045  	CNTR = $0004
	//0x033B:	0x1433DE  	DO $033D UNTIL CE
	//0x033C:	0x680082  	DM(I0,M2) = SI
	//0x033D:	0x680086  	DM(I1,M2) = SI
	//0x033E:	0x0A000F  	RTS
	WORD i0=0x350+mr0;
	WORD i1=0x340+mr0;
	for(int i=0;i<4;i++)
	{
		dspDMem[i0]=0;
		dspDMem[i1]=0;
		i0+=4;
		i1+=4;
	}
}

void hleDSPNextSample()
{
	WORD si,i0,i3,i1,sr0,sr1,m3,m2,i6;
	DWORD mr,sr,tmp;
	WORD ar;
	BYTE i = 0;

	//0x0803:	0x805FB8  	SI = DM($05FB)
	//0x0804:	0x0D0468  	M2 = SI
	//0x0805:	0x0F1001  	SR = LSHIFT SI BY 1 (LO)
	//0x0806:	0x0D047E  	M3 = SR0
	si=dspDMem[0x5fb];
	m2=si;
	m3=si<<1;

	//0x0807:	0x343880  	I0 = $0388
	//0x0808:	0x090003  	MODIFY (I0,M3)
	//0x0809:	0x600081  	SI = DM(I0,M1)
	//0x080A:	0x939FD8  	DM($39FD) = SI
	//0x080B:	0x600080  	SI = DM(I0,M0)
	//0x080C:	0x939FE8  	DM($39FE) = SI
	//0x080D:	0x1C33FF  	CALL $033F
	i0=0x388+m3;
	mr=(dspDMem[i0]<<16);
	i0++;
	mr|=dspDMem[i0];
	hleDSPVSetAbs(mr);

	//0x080E:	0x346893  	I3 = $0689
	//0x080F:	0x09000F  	MODIFY (I3,M3)
	//0x0810:	0x1C35EF  	CALL $035E
	//0x0811:	0x6800AD  	DM(I3,M1) = AR
	//0x0812:	0x1C35EF  	CALL $035E
	//0x0813:	0x6800AC  	DM(I3,M0) = AR
	i3=0x689+m3;
	hleDSPVRead16(&ar);
	dspDMem[i3]=ar;
	i3++;
	hleDSPVRead16(&ar);
	dspDMem[i3]=ar;


	//0x0814:	0x346850  	I0 = $0685
	//0x0815:	0x090002  	MODIFY (I0,M2)
	//0x0816:	0xA00000  	DM(I0,M0) = $0000
	dspDMem[0x685+m2]=0;

	//0x0817:	0x343880  	I0 = $0388
	//0x0818:	0x090003  	MODIFY (I0,M3)
	i0=0x388+m3;

	//0x0819:	0x1C369F  	CALL $0369
	//0x081A:	0x839FD8  	SI = DM($39FD)
	//0x081B:	0x680081  	DM(I0,M1) = SI
	//0x081C:	0x839FE8  	SI = DM($39FE)
	//0x081D:	0x680080  	DM(I0,M0) = SI
	hleDSPVGetAbs(&mr);
	dspDMem[0x388+m3]=(mr>>0x10)&0xffff;
	dspDMem[0x388+m3+1]=mr&0xffff;

	//0x081E:	0x343C40  	I0 = $03C4
	//0x081F:	0x090003  	MODIFY (I0,M3)
	//0x0820:	0x600081  	SI = DM(I0,M1)
	//0x0821:	0x906108  	DM($0610) = SI
	//0x0822:	0x6000A0  	AR = DM(I0,M0)
	//0x0823:	0x0D082A  	I6 = AR
	//0x0824:	0x90607A  	DM($0607) = AR
	i0=0x3c4+m3;
	si=dspDMem[i0];
	i0++;
	dspDMem[0x610]=si;
	ar=dspDMem[i0];
	i6=ar;
	dspDMem[0x607]=ar;

	//0x0825:	0x400104  	AY0 = $0010
	//0x0826:	0x22620F  	AR = AR + AY0
	//0x0827:	0x90611A  	DM($0611) = AR
	dspDMem[0x611]=ar+0x10;

	//0x0828:	0x0D02A2  	AR = I6
MoveRepeat:
	ar=i6;

	//0x0829:	0x806114  	AY0 = DM($0611)
	//0x082A:	0x22E20F  	AR = AR - AY0
	//0x082B:	0x188360  	IF EQ JUMP $0836
	ar-=dspDMem[0x611];
	if(ar!=0)
	{
		//0x082C:	0x8390F8  	SI = DM($390F)
		//0x082D:	0x80610A  	AR = DM($0610)
		//0x082E:	0x93000A  	DM($3000) = AR
		//0x082F:	0x7000A9  	AR = DM(I6,M5)
		//0x0830:	0x930008  	DM($3000) = SI
		//0x0831:	0x4007F4  	AY0 = $007F
		//0x0832:	0x23820F  	AR = AR AND AY0
		//0x0833:	0x26E20F  	AF = AR - AY0
		//0x0834:	0x188380  	IF EQ JUMP $0838

		DWORD page=dspDMem[0x610];
		page=((page & 0xff00)>>1)|page&0x7f;
		page<<=12;
		ar=*(BYTE *)&m->dspRMem[(0x2000^i6)+page];
		i6++;
		ar&=0x7f;
		if(ar!=0x7f)
			goto MoveRepeat;
		//0x0835:	0x18828F  	JUMP $0828

		//0x0838:	0x0D0202  	AX0 = I6
		//0x0839:	0x806074  	AY0 = DM($0607)
		//0x083A:	0x26E00F  	AF = AX0 - AY0
		//0x083B:	0x23100F  	AR = AF - 1
		ar=i6-dspDMem[0x607];
		ar--;
	}
	else
	{
		//0x0836:	0x40010A  	AR = $0010
		//0x0837:	0x1883CF  	JUMP $083C
		ar=0x10;
	}
	//0x083C:	0x379090  	I0 = $3909
	//0x083D:	0x090002  	MODIFY (I0,M2)
	//0x083E:	0x6800A0  	DM(I0,M0) = AR
	dspDMem[0x3909+m2]=ar;

	//0x083F:	0x0D0116  	AX1 = M2
	//0x0840:	0x0D0147  	AY0 = M3
	//0x0841:	0x22610F  	AR = AX1 + AY0
	//0x0842:	0x0D047A  	M3 = AR
	//0x0843:	0x379600  	I0 = $3960
	//0x0844:	0x090003  	MODIFY (I0,M3)
	//0x0845:	0x0D0104  	AX0 = M0
	//0x0846:	0x3C0035  	CNTR = $0003
	//0x0847:	0x14848E  	DO $0848 UNTIL CE
	//0x0848:	0x680001  	DM(I0,M1) = AX0
	for(int i=0;i<3;i++)
		dspDMem[m2+m3+0x3960+i]=0;

	//0x0849:	0x0D0186  	SI = M2
	//0x084A:	0x0F1004  	SR = LSHIFT SI BY 4 (LO)
	//0x084B:	0x0D047E  	M3 = SR0
	m3=m2<<4;

	//0x084C:	0x346440  	I0 = $0644
	//0x084D:	0x090003  	MODIFY (I0,M3)
	//0x084E:	0x0D0104  	AX0 = M0
	//0x084F:	0x3C0105  	CNTR = $0010
	//0x0850:	0x14851E  	DO $0851 UNTIL CE
	//0x0851:	0x680001  	DM(I0,M1) = AX0
	for(i=0;i<16;i++)
	{
		dspDMem[0x644+m3+i]=0;
	}
	//0x0852:	0x0A000F  	RTS
	hleDSPVFlush();
}


void hleDSPMama()
{
	WORD si,i0,i3,i1,sr0,sr1,m3,m2,i6,af,mx0,mr0;
	short se;
	WORD ax1,ax0,m6,i2,i4,ay0,ay1,mr2,mr1;
	WORD my0,my1;
	DWORD tmp,vptr;
	int mr;
	DWORD sr;
	int j=0;
	WORD ar;
	DWORD page,BasePage;
	WORD offset;
	BYTE i = 0;

	//0x0853:	0x805FB8  	SI = DM($05FB)
	//0x0854:	0x0D0468  	M2 = SI
	//0x0855:	0x0F1001  	SR = LSHIFT SI BY 1 (LO)
	//0x0856:	0x0D047E  	M3 = SR0
	//0x0857:	0x343C40  	I0 = $03C4
	//0x0858:	0x090003  	MODIFY (I0,M3)
	m2=dspDMem[0x5fb];
	m3=m2<<1;
	i0=0x3c4+m3;


	//0x0859:	0x600081  	SI = DM(I0,M1)
	//0x085A:	0x906108  	DM($0610) = SI
	//0x085B:	0x6000A0  	AR = DM(I0,M0)
	//0x085C:	0x0D082A  	I6 = AR
	//0x085D:	0x986072  	DM($0607) = I6
	//0x085E:	0x400104  	AY0 = $0010
	//0x085F:	0x22620F  	AR = AR + AY0
	//0x0860:	0x90611A  	DM($0611) = AR
	//0x0861:	0x930008  	DM($3000) = SI
	//0x0862:	0x7000A9  	AR = DM(I6,M5)
	//0x0863:	0x400804  	AY0 = $0080
	//0x0864:	0x23820F  	AR = AR AND AY0
	//0x0865:	0x188680  	IF EQ JUMP $0868
	si=dspDMem[i0++];	//page
	dspDMem[0x610]=si;
	i6=dspDMem[i0];
	dspDMem[0x607]=i6;
	ar=i6+0x10;
	dspDMem[0x611]=ar;		//start of data?

	page=((si & 0xff00)>>1)|si&0x7f;
	page<<=12;
	if((i6>=0x2000)&&(i6<=0x2fff))
		ar=*(BYTE *)&m->dspRMem[(0x2000^i6)+page];
	else
		ar=dspDMem[i6];
	i6++;
	ar=ar&0x80;
	if(ar)
	{
		//0x0866:	0x979084  	DM($3908) = M0
		//0x0867:	0x18869F  	JUMP $0869
		dspDMem[0x3908]=0;
	}
	else
	{
		//0x0868:	0x979085  	DM($3908) = M1
		dspDMem[0x3908]=1;
	}

	//0x0869:	0x7000A9  	AR = DM(I6,M5)
	//0x086A:	0x23820F  	AR = AR AND AY0
	//0x086B:	0x0F1201  	SR = LSHIFT AR BY 1 (LO)
	if((i6>=0x2000)&&(i6<=0x2fff))
		ar=*(BYTE *)&m->dspRMem[(0x2000^i6)+page];
	else
		ar=dspDMem[i6];
	i6++;
	ar&=0x80;
	sr=ar<<1;

	//0x086C:	0x7000A8  	AR = DM(I6,M4)
	//0x086D:	0x23820F  	AR = AR AND AY0
	//0x086E:	0x0F1A00  	SR = SR OR LSHIFT AR BY 0 (LO)
	//0x086F:	0x93904E  	DM($3904) = SR0
	if((i6>=0x2000)&&(i6<=0x2fff))
		ar=*(BYTE *)&m->dspRMem[(0x2000^i6)+page];
	else
		ar=dspDMem[i6];
	ar&=0x80;
	sr|=ar;
	dspDMem[0x3904]=sr&0xffff;

	//0x0870:	0x886072  	I6 = DM($0607)
	//0x0871:	0x343880  	I0 = $0388
	//0x0872:	0x090003  	MODIFY (I0,M3)
	//0x0873:	0x600081  	SI = DM(I0,M1)
	//0x0874:	0x939FD8  	DM($39FD) = SI
	//0x0875:	0x600080  	SI = DM(I0,M0)
	//0x0876:	0x939FE8  	DM($39FE) = SI
	//0x0877:	0x1C33FF  	CALL $033F
	//0x0878:	0x8390F8  	SI = DM($390F)
	//0x0879:	0x930008  	DM($3000) = SI
	i6=dspDMem[0x607];
	i0=0x388+m3;
	vptr=dspDMem[i0++]<<16;
	vptr|=dspDMem[i0];
	page=(vptr&0xfffff000)>>12;
	dspDMem[0x390f]=page;

	page=((page & 0xff00)>>1)|page&0x7f;
	page<<=12;
	BasePage=page;
	dspDMem[0x39ff]=vptr&0xfff|0x2000;
	hleDSPVSetAbs(vptr);

	//0x087A:	0x346850  	I0 = $0685
	//0x087B:	0x090002  	MODIFY (I0,M2)
	//0x087C:	0x600080  	SI = DM(I0,M0)
	//0x087D:	0x906408  	DM($0640) = SI
	//0x087E:	0x439028  	SI = $3902
	//0x087F:	0x906438  	DM($0643) = SI
	i0=0x685+m2;
	dspDMem[0x640]=dspDMem[i0];
	dspDMem[0x643]=0x3902;

	//0x0880:	0x346890  	I0 = $0689
	//0x0881:	0x090003  	MODIFY (I0,M3)
	//0x0882:	0x600081  	SI = DM(I0,M1)
	//0x0883:	0x939028  	DM($3902) = SI
	//0x0884:	0x600080  	SI = DM(I0,M0)
	//0x0885:	0x939038  	DM($3903) = SI
	i0=0x689+m3;
	dspDMem[0x3902]=dspDMem[i0++];
	dspDMem[0x3903]=dspDMem[i0];

	//0x0886:	0x343A00  	I0 = $03A0
	//0x0887:	0x090002  	MODIFY (I0,M2)
	//0x0888:	0x600070  	MY1 = DM(I0,M0)
	//0x0889:	0x0D0116  	AX1 = M2
	//0x088A:	0x0D0147  	AY0 = M3
	//0x088B:	0x22610F  	AR = AX1 + AY0
	//0x088C:	0x0D047A  	M3 = AR
	i0=0x3a0+m2;
	my1=dspDMem[i0];
	m3=m2+m3;

	//0x088D:	0x379600  	I0 = $3960
	//0x088E:	0x090003  	MODIFY (I0,M3)
	//0x088F:	0x805FB8  	SI = DM($05FB)
	//0x0890:	0x0F1004  	SR = LSHIFT SI BY 4 (LO)
	//0x0891:	0x0D047E  	M3 = SR0
	//0x0892:	0x346443  	I3 = $0644
	//0x0893:	0x09000F  	MODIFY (I3,M3)
	i0=0x3960+m3;
	si=dspDMem[0x5fb];
	sr=si<<4;
	m3=sr&0xffff;
	i3=0x644+m3;

	//0x0894:	0x379051  	I1 = $3905
	//0x0895:	0x83904A  	AR = DM($3904)
	//0x0896:	0x227A0F  	AR = AR
	//0x0897:	0x1889A2  	IF GT JUMP $089A
	i1=0x3905;
	ar=dspDMem[0x3904];
	short sar=(short)dspDMem[0x3904];
//	if((ar&0x7fff)!=ar)
	if(sar<=0)
	{
		//0x0898:	0x439201  	AX1 = $3920
		//0x0899:	0x188A5F  	JUMP $08A5
		ax1=0x3920;
	}
	else
	{
		//0x089A:	0x400014  	AY0 = $0001
		//0x089B:	0x22E20F  	AR = AR - AY0
		//0x089C:	0x1889F2  	IF GT JUMP $089F
		sar--;
		if(sar<=0)
		{
			//0x089D:	0x439301  	AX1 = $3930
			//0x089E:	0x188A5F  	JUMP $08A5
			ax1=0x3930;
		}
		else
		{
			sar-=2;
			//0x089F:	0x400024  	AY0 = $0002
			//0x08A0:	0x22E20F  	AR = AR - AY0
			//0x08A1:	0x188A42  	IF GT JUMP $08A4
			if(sar<=0)
			{
				//0x08A2:	0x439401  	AX1 = $3940
				//0x08A3:	0x188A5F  	JUMP $08A5
				ax1=0x3940;
			}
			else
			{
				//0x08A4:	0x439501  	AX1 = $3950
				ax1=0x3950;
			}
		}
	}
	//0x08A5:	0x3C0035  	CNTR = $0003
	//0x08A6:	0x148AEE  	DO $08AE UNTIL CE
	for(i=0;i<3;i++)
	{
		//0x08A7:	0x60004D  	AY0 = DM(I3,M1)
		//0x08A8:	0x22610F  	AR = AX1 + AY0
		//0x08A9:	0x0D042A  	I2 = AR
		i2=ax1+dspDMem[i3++];

		//0x08AA:	0x600048  	AY0 = DM(I2,M0)
		//0x08AB:	0x600001  	AX0 = DM(I0,M1)
		//0x08AC:	0x22600F  	AR = AX0 + AY0
		//0x08AD:	0x0F12FF  	SR = LSHIFT AR BY -1 (LO)
		//0x08AE:	0x6800E5  	DM(I1,M1) = SR0
		ay0=dspDMem[i2];
		ax0=dspDMem[i0++];
		dspDMem[i1++]=(ax0+ay0)>>1;
	}

	//0x08AF:	0x946844  	DM($0684) = M0
	//0x08B0:	0x379090  	I0 = $3909
	//0x08B1:	0x090002  	MODIFY (I0,M2)
	//0x08B2:	0x6000A0  	AR = DM(I0,M0)
	//0x08B3:	0x0D0C5A  	CNTR = AR
	//0x08B4:	0x148B9E  	DO $08B9 UNTIL CE
	dspDMem[0x684]=0;
	i0=0x3909+m2;
	ar=dspDMem[i0];
	WORD count=ar;
	for(i=0;i<count;i++)
	{
		//0x08B5:	0x1CA49F  	CALL $0A49
		hleDSPCopySample(&BasePage);

		//0x08B6:	0x846840  	I0 = DM($0684)
		//0x08B7:	0x090001  	MODIFY (I0,M1)
		//0x08B8:	0x946840  	DM($0684) = I0
		//0x08B9:	0x83801A  	AR = DM($3801)
		dspDMem[0x684]++;
		ar=dspDMem[0x3801];
	}
	//0x08BA:	0x90642A  	DM($0642) = AR
	//0x08BB:	0x378013  	I3 = $3801
	//0x08BC:	0x946844  	DM($0684) = M0
	dspDMem[0x642]=ar;
	i3=0x3801;
	dspDMem[0x684]=0;
	while(1)
	{
		//0x08BD:	0x0D02A2  	AR = I6
		//0x08BE:	0x806114  	AY0 = DM($0611)
		//0x08BF:	0x22E20F  	AR = AR - AY0
		//0x08C0:	0x18A040  	IF EQ JUMP $0A04
		ar=i6;
		if(ar==dspDMem[0x611])
			break;

		//0x08C1:	0x8390F8  	SI = DM($390F)
		//0x08C2:	0x80610A  	AR = DM($0610)
		//0x08C3:	0x93000A  	DM($3000) = AR
		//0x08C4:	0x7000A9  	AR = DM(I6,M5)
		//0x08C5:	0x930008  	DM($3000) = SI
		//0x08C6:	0x4007F4  	AY0 = $007F
		//0x08C7:	0x23820F  	AR = AR AND AY0
		//0x08C8:	0x26E20F  	AF = AR - AY0
		//0x08C9:	0x18A040  	IF EQ JUMP $0A04
		page=dspDMem[0x610];
		page=((page & 0xff00)>>1)|page&0x7f;
		page<<=12;
		ar=*(BYTE *)&m->dspRMem[(0x2000^i6)+page];
		i6++;
		page=BasePage;
		ar&=0x7f;
		if(ar==0x7f)
			break;

		//0x08CA:	0x0F12FA  	SR = LSHIFT AR BY -6 (LO)
		//0x08CB:	0x90612E  	DM($0612) = SR0
		//0x08CC:	0x9390DA  	DM($390D) = AR
		sr=ar>>6;
		dspDMem[0x612]=sr&0xffff;
		dspDMem[0x390d]=ar;

		//0x08CD:	0x3798C0  	I0 = $398C
		//0x08CE:	0x846847  	M3 = DM($0684)
		//0x08CF:	0x090003  	MODIFY (I0,M3)
		//0x08D0:	0x6000A0  	AR = DM(I0,M0)
		//0x08D1:	0x0D086A  	M6 = AR
		//0x08D2:	0x80612A  	AR = DM($0612)
		//0x08D3:	0x227A0F  	AR = AR
		//0x08D4:	0x188D71  	IF NE JUMP $08D7
		i0=0x398c;
		m3=dspDMem[0x684];
		i0+=m3;
		ar=dspDMem[i0];
		m6=ar;
		ar=dspDMem[0x612];
		if(ar==0)
		{
			//0x08D5:	0x340016  	M2 = $0001
			//0x08D6:	0x188DBF  	JUMP $08DB
			m2=1;
		}
		else
		{
			//0x08D7:	0x340026  	M2 = $0002
			//0x08D8:	0x0D0286  	SI = M6
			//0x08D9:	0x0F10FF  	SR = LSHIFT SI BY -1 (LO)
			//0x08DA:	0x0D086E  	M6 = SR0
			m2=0x2;
			m6>>=1;
		}
		//0x08DB:	0x805FB8  	SI = DM($05FB)
		//0x08DC:	0x0F1004  	SR = LSHIFT SI BY 4 (LO)
		//0x08DD:	0x0D047E  	M3 = SR0
		//0x08DE:	0x346440  	I0 = $0644
		//0x08DF:	0x090003  	MODIFY (I0,M3)
		m3=dspDMem[0x5fb]<<4;
		i0=0x644+m3;

		//0x08E0:	0x846847  	M3 = DM($0684)
		//0x08E1:	0x090003  	MODIFY (I0,M3)
		//0x08E2:	0x6000A0  	AR = DM(I0,M0)
		//0x08E3:	0x227A0F  	AR = AR
		//0x08E4:	0x188E91  	IF NE JUMP $08E9
		m3=dspDMem[0x684];
		i0+=m3;
		ar=dspDMem[i0];
		if(ar==0)
		{
			//0x08E5:	0x0D0903  	I4 = I3
			//0x08E6:	0x090012  	MODIFY (I4,M6)
			//0x08E7:	0x0D0630  	I3 = I4
			//0x08E8:	0x18A00F  	JUMP $0A00
			i4=i3;
			i4+=m6;
			i3=i4;
			goto ShortExit;
		}
		//0x08E9:	0x90641A  	DM($0641) = AR
		//0x08EA:	0x8390DE  	SR0 = DM($390D)
		//0x08EB:	0x83908A  	AR = DM($3908)
		//0x08EC:	0x227A0F  	AR = AR
		//0x08ED:	0x1890D1  	IF NE JUMP $090D
		dspDMem[0x641]=ar;
		sr=dspDMem[0x390d];
		ar=dspDMem[0x3908];
		if(ar==0)
		{
			//0x08EE:	0x80684A  	AR = DM($0684)
			//0x08EF:	0x400034  	AY0 = $0003
			//0x08F0:	0x26E20F  	AF = AR - AY0
			//0x08F1:	0x188FB5  	IF GE JUMP $08FB
			ar=dspDMem[0x684];
			if(ar<3)
			{
				//0x08F2:	0x0D047A  	M3 = AR
				//0x08F3:	0x379050  	I0 = $3905
				//0x08F4:	0x090003  	MODIFY (I0,M3)
				m3=ar;
				i0=0x3905+m3;

				//0x08F5:	0x600040  	AY0 = DM(I0,M0)
				//0x08F6:	0x8390D0  	AX0 = DM($390D)
				//0x08F7:	0x22600F  	AR = AX0 + AY0
				//0x08F8:	0x9390DA  	DM($390D) = AR
				//0x08F9:	0x439E94  	AY0 = $39E9
				//0x08FA:	0x18901F  	JUMP $0901
				ay0=dspDMem[i0];
				ax0=dspDMem[0x390d];
				ar=ax0+ay0;
				dspDMem[0x390d]=ar;
				ay0=0x39e9;
			}
			else
			{
				//0x08FB:	0x400064  	AY0 = $0006
				//0x08FC:	0x26E20F  	AF = AR - AY0
				//0x08FD:	0x189005  	IF GE JUMP $0900
				if(ar<6)
				{
					//0x08FE:	0x403204  	AY0 = $0320
					//0x08FF:	0x18901F  	JUMP $0901
					ay0=0x320;
				}
				else
				{
					//0x0900:	0x403304  	AY0 = $0330
					ay0=0x330;
				}
			}
			//0x0901:	0x806410  	AX0 = DM($0641)
			//0x0902:	0x22600F  	AR = AX0 + AY0
			//0x0903:	0x0D040A  	I0 = AR
			ax0=dspDMem[0x641];
			i0=ax0+ay0;

			//0x0904:	0x600040  	AY0 = DM(I0,M0)
			//0x0905:	0x0D0084  	SI = AY0
			//0x0906:	0x0F10F8  	SR = LSHIFT SI BY -8 (LO)
			//0x0907:	0x90641E  	DM($0641) = SR0
			ay0=dspDMem[i0];
			sr=ay0>>8;
			dspDMem[0x641]=sr&0xffff;

			//0x0908:	0x400FF0  	AX0 = $00FF
			//0x0909:	0x23800F  	AR = AX0 AND AY0
			//0x090A:	0x8390D5  	AY1 = DM($390D)
			//0x090B:	0x226A0F  	AR = AR + AY1
			//0x090C:	0x0D00EA  	SR0 = AR
			ar=ay0&0xff;
			ar+=dspDMem[0x390d];
			sr=ar;
		}
		//0x090D:	0x400035  	AY1 = $0003
		//0x090E:	0x238E0F  	AR = SR0 AND AY1
		ar=sr&0x3;

		//0x090F:	0x439F95  	AY1 = $39F9
		//0x0910:	0x226A0F  	AR = AR + AY1
		//0x0911:	0x0D040A  	I0 = AR
		//0x0912:	0x600080  	SI = DM(I0,M0)
		//0x0913:	0x0F160A  	SR = LSHIFT SR0 BY 10 (LO)
		//0x0914:	0x0F16F4  	SR = LSHIFT SR0 BY -12 (LO)
		ar+=0x39f9;
		i0=ar;
		si=dspDMem[i0];
		sr=(sr&0xffff)<<10;
		sr=(sr&0xffff)>>12;

		//0x0915:	0x4000F5  	AY1 = $000F
		//0x0916:	0x22EE0F  	AR = SR0 - AY1
		//0x0917:	0x0D009A  	SE = AR
		//0x0918:	0x0E100F  	SR = LSHIFT SI (LO)
		//0x0919:	0x0D006E  	MY0 = SR0
		se=(sr&0xffff)-0xf;
		if(se>0)
			sr=si<<se;
		else
			sr=si>>(se*-1);
		my0=sr&0xffff;

		//0x091A:	0x3796C2  	I2 = $396C
		//0x091B:	0x80641A  	AR = DM($0641)
		//0x091C:	0x400065  	AY1 = $0006
		//0x091D:	0x26EA0F  	AF = AR - AY1
		//0x091E:	0x189C72  	IF GT JUMP $09C7
		i2=0x396c;
		ar=dspDMem[0x641];
		if(ar<=6)
		{
			//0x091F:	0x8390F8  	SI = DM($390F)
			//0x0920:	0x930008  	DM($3000) = SI
			//0x0921:	0x846417  	M3 = DM($0641)
			page=BasePage;
			m3=dspDMem[0x641];

			//0x0922:	0x346140  	I0 = $0614
			//0x0923:	0x090003  	MODIFY (I0,M3)
			//0x0924:	0x600040  	AY0 = DM(I0,M0)
			//0x0925:	0x9061B4  	DM($061B) = AY0
			dspDMem[0x61b]=dspDMem[0x614+m3];

			//0x0926:	0x0D01A7  	AR = M3
			//0x0927:	0x400014  	AY0 = $0001
			//0x0928:	0x2AE2B4  	AR = AR - AY0, MR0 = AY0
			//0x0929:	0x0D009A  	SE = AR
			//0x092A:	0x0E130F  	SR = LSHIFT MR0 (LO)
			//0x092B:	0x9061CE  	DM($061C) = SR0
			ar=m3;
			ar--;
			sr=(1<<ar);
			dspDMem[0x61c]=sr&0xffff;

			//0x092C:	0x40003B  	MR0 = $0003
			//0x092D:	0x93000B  	DM($3000) = MR0
			//0x092E:	0x0D01A7  	AR = M3
			//0x092F:	0x400034  	AY0 = $0003
			//0x0930:	0x26E20F  	AF = AR - AY0
			//0x0931:	0x1897A2  	IF GT JUMP $097A
			page=(3<<12);
			ar=m3;
			if(ar<=3)
			{
				//0x0932:	0x379D90  	I0 = $39D9
				//0x0933:	0x090003  	MODIFY (I0,M3)
				//0x0934:	0x600080  	SI = DM(I0,M0)
				//0x0935:	0x906138  	DM($0613) = SI
				i0=0x39d9+m3;
				dspDMem[0x613]=dspDMem[i0];

				//0x0936:	0x846431  	I1 = DM($0643)
				//0x0937:	0x340029  	L1 = $0002
				//0x0938:	0x400105  	AY1 = $0010
				//0x0939:	0x400080  	AX0 = $0008
				//0x093A:	0x806409  	SE = DM($0640)
				//0x093B:	0x8061B4  	AY0 = DM($061B)
				//0x093C:	0x40010A  	AR = $0010
				//0x093D:	0x23220F  	AR = AY0 - AR
				//0x093E:	0x0D00DA  	MR2 = AR
				//0x093F:	0x8B9FF0  	I4 = DM($39FF)
				//0x0940:	0x0D00A9  	AR = SE
				//0x0941:	0x600085  	SI = DM(I1,M1)
				//0x0942:	0x0D0E56  	CNTR = M6
				//0x0943:	0x14978E  	DO $0978 UNTIL CE
				i1=dspDMem[0x643];
				ay1=0x10;
				ax0=0x08;
				se=dspDMem[0x640];
				ar=dspDMem[0x61b]-0x10;
				mr2=ar;
				i4=dspDMem[0x39ff];
				ar=se;
				si=dspDMem[i1++];
				if((i1%2)==0)
					i1-=2;
				for(i=0;i<m6;i++)
				{
					//0x0944:	0x120085  	SR = LSHIFT SI (HI), SI = DM(I1,M1)
					if(se>0)
						sr=(si<<16)<<se;
					else
						sr=(si<<16)>>(se*-1);
					si=dspDMem[i1++];
					if((i1%2)==0)
						i1-=2;

					//0x0945:	0x10189D  	SR = SR OR LSHIFT SI (LO), SE = MR2
					if(se>0)
						sr|=(si<<se);
					else
						sr|=(si>>(se*-1));
					se=mr2;

					//0x0946:	0x0E170F  	SR = LSHIFT SR1 (LO)
					if(se>0)
						sr=(sr>>16)<<se;
					else
						sr=(sr>>16)>>(se*-1);

					//0x0947:	0x400FF4  	AY0 = $00FF
					//0x0948:	0x0D047E  	M3 = SR0
					//0x0949:	0x846130  	I0 = DM($0613)
					ay0=0xff;
					m3=sr&0xffff;
					i0=dspDMem[0x613];

					//0x094A:	0x0D009A  	SE = AR
					//0x094B:	0x090003  	MODIFY (I0,M3)
					//0x094C:	0x6000A1  	AR = DM(I0,M1)
					//0x094D:	0x0F1208  	SR = LSHIFT AR BY 8 (LO)
					//0x094E:	0x23820F  	AR = AR AND AY0
					//0x094F:	0x400804  	AY0 = $0080
					//0x0950:	0x27820F  	AF = AR AND AY0
					//0x0951:	0x189590  	IF EQ JUMP $0959
					se=ar;
					i0+=m3;
					if((i0>=0x2000)&&(i0<=0x2fff))
						ar=*(WORD *)&m->dspRMem[(i0^0x2000)+page];
					else
						ar=dspDMem[i0];
					i0++;
					sr=ar<<8;
					ar&=0xff;
					if(ar&0x80)
					{
						//0x0952:	0x0D01A4  	AR = M0
						//0x0953:	0x6800A9  	DM(I2,M1) = AR
						//0x0954:	0x6800A9  	DM(I2,M1) = AR
						//0x0955:	0x0D0345  	AY0 = CNTR
						//0x0956:	0x2B004F  	AR = AY0 - 1, AY0 = SR1
						//0x0957:	0x0D0CDA  	OWRCNTR = AR
						//0x0958:	0x1895CF  	JUMP $095C
						ar=0;
						dspDMem[i2++]=0;
						dspDMem[i2++]=0;
						ay0=sr>>16;
						i++;
					}
					else
					{
						//0x0959:	0x8061C4  	AY0 = DM($061C)
						//0x095A:	0x2AE24F  	AR = AR - AY0, AY0 = SR1
						//0x095B:	0x6800A9  	DM(I2,M1) = AR
						ay0=dspDMem[0x61c];
						ar=ar-ay0;
						ay0=sr>>16;
						dspDMem[i2++]=ar;
					}
					//0x095C:	0x0D00A9  	AR = SE
					//0x095D:	0x22620F  	AR = AR + AY0
					//0x095E:	0x2EEA9A  	AF = AR - AY1, SE = AR
					//0x095F:	0x189784  	IF LT JUMP $0978
					ar=se;
					ar+=ay0;
					se=ar;
					if(ar>=ay1)
					{
						//0x0960:	0x0D0090  	SE = AX0
						//0x0961:	0x8390F8  	SI = DM($390F)
						//0x0962:	0x930008  	DM($3000) = SI
						se=ax0;
						page=BasePage;

						//0x0963:	0x400FF4  	AY0 = $00FF
						//0x0964:	0x700081  	SI = DM(I4,M5)
						//0x0965:	0x1010BA  	SR = LSHIFT SI (LO), MR0 = AR
						si=*(WORD *)&m->dspRMem[(i4^0x2000)+page];
						i4++;
						if(se>0)
							sr=si<<se;
						else
							sr=si>>(se*-1);
						mr0=ar;

						//0x0966:	0x7000A1  	AR = DM(I4,M5)
						//0x0967:	0x23820F  	AR = AR AND AY0
						//0x0968:	0x0F1A00  	SR = SR OR LSHIFT AR BY 0 (LO)
						//0x0969:	0x6AEBE5  	AR = MR0 - AY1, DM(I1,M1) = SR0
						ar=*(WORD *)&m->dspRMem[(i4^0x2000)+page];
						i4++;
						ar&=0xff;
						sr|=ar;
						ar=mr0-ay1;
						dspDMem[i1++]=sr&0xffff;
						if((i1%2)==0)
							i1-=2;

						//0x096A:	0x400038  	SI = $0003
						//0x096B:	0x930008  	DM($3000) = SI
						//0x096C:	0x0D02E0  	SR0 = I4
						//0x096D:	0x0F1603  	SR = LSHIFT SR0 BY 3 (LO)
						//0x096E:	0x2E7E9A  	AF = SR0, SE = AR
						//0x096F:	0x189785  	IF GE JUMP $0978
						page=(3<<12);
						sr=i4<<3;
						se=ar;
						if(sr&0x8000)
						{
							//0x0970:	0x3B0004  	M4 = $3000
							//0x0971:	0x090010  	MODIFY (I4,M4)
							//0x0972:	0x380004  	M4 = $0000
							//0x0973:	0x8390F4  	AY0 = DM($390F)
							//0x0974:	0x22200F  	AR = AY0 + 1
							//0x0975:	0x9390FA  	DM($390F) = AR
							//0x0976:	0x22EB0F  	AR = MR0 - AY1
							//0x0977:	0x0D009A  	SE = AR
							i4-=0x1000;
							WORD p=dspDMem[0x390f];
							p++;
							dspDMem[0x390f]=p;
							BasePage=((p&0xff00)>>1)|(p&0x7f);
							BasePage<<=12;

							ar=mr0-ay1;
							se=ar;

						}
					}
					//0x0978:	0x600085  	SI = DM(I1,M1)
					si=dspDMem[i1++];
					if((i1%2)==0)
						i1-=2;
				}
				//0x0979:	0x189BFF  	JUMP $09BF
			}
			else
			{
				//page=(3<<12) at this point
				//0x097A:	0x1CA6EF  	CALL $0A6E
				hleDSPSkipSample(m3,page);

				//0x097B:	0x846431  	I1 = DM($0643)
				//0x097C:	0x340029  	L1 = $0002
				//0x097D:	0x400105  	AY1 = $0010
				//0x097E:	0x400080  	AX0 = $0008
				//0x097F:	0x806409  	SE = DM($0640)
				//0x0980:	0x8061B4  	AY0 = DM($061B)
				i1=dspDMem[0x643];
				ay1=0x10;
				ax0=0x08;
				se=dspDMem[0x640];
				ay0=dspDMem[0x61b];

				//0x0981:	0x40010A  	AR = $0010
				//0x0982:	0x23220F  	AR = AY0 - AR
				//0x0983:	0x0D00DA  	MR2 = AR
				//0x0984:	0x8B9FF0  	I4 = DM($39FF)
				//0x0985:	0x0D00A9  	AR = SE
				//0x0986:	0x600085  	SI = DM(I1,M1)
				//0x0987:	0x0D0E56  	CNTR = M6
				//0x0988:	0x149BEE  	DO $09BE UNTIL CE
				ar=ay0-0x10;
				mr2=ar;
				i4=dspDMem[0x39ff];
				ar=se;
				si=dspDMem[i1++];
				if((i1%2)==0)
					i1-=2;
				for(i=0;i<m6;i++)
				{
					//0x0989:	0x120085  	SR = LSHIFT SI (HI), SI = DM(I1,M1)
					if(se>0)
						sr=(si<<16)<<se;
					else
						sr=(si<<16)>>(se*-1);
					si=dspDMem[i1++];
					if((i1%2)==0)
						i1-=2;

					//0x098A:	0x10189D  	SR = SR OR LSHIFT SI (LO), SE = MR2
					if(se>0)
						sr|=(si<<se);
					else
						sr|=(si>>(se*-1));
					se=mr2;

					//0x098B:	0x0E170F  	SR = LSHIFT SR1 (LO)
					if(se>0)
						sr=(sr>>16)<<se;
					else
						sr=(sr>>16)>>(se*-1);

					//0x098C:	0x400FF4  	AY0 = $00FF
					//0x098D:	0x0F1601  	SR = LSHIFT SR0 BY 1 (LO)
					//0x098E:	0x0D047E  	M3 = SR0
					//0x098F:	0x846130  	I0 = DM($0613)
					sr=(sr&0xffff)<<1;
					m3=sr&0xffff;
					i0=dspDMem[0x613];

					//0x0990:	0x0D009A  	SE = AR
					//0x0991:	0x090003  	MODIFY (I0,M3)
					//0x0992:	0x6000A1  	AR = DM(I0,M1)
					//0x0993:	0x638210  	AR = AR AND AY0, AX1 = DM(I0,M0)
					//0x0994:	0x2B81EA  	AR = AX1 AND AY0, SR0 = AR
					//0x0995:	0x400804  	AY0 = $0080
					//0x0996:	0x27820F  	AF = AR AND AY0
					//0x0997:	0x1899F0  	IF EQ JUMP $099F
					se=ar;
					i0+=m3;
					if((i0>=0x2000)&&(i0<=0x2fff))
						ar=*(WORD *)&m->dspRMem[(i0^0x2000)+page];
					else
						ar=dspDMem[i0];
					i0++;
					ar&=0xff;
					if((i0>=0x2000)&&(i0<=0x2fff))
						ax1=*(WORD *)&m->dspRMem[(i0^0x2000)+page];
					else
						ax1=dspDMem[i0];
					sr0=ar;
					ar=ax1&0xff;
					if(ar&0x80)
					{
						//0x0998:	0x0D01A4  	AR = M0
						//0x0999:	0x6800A9  	DM(I2,M1) = AR
						//0x099A:	0x6800A9  	DM(I2,M1) = AR
						//0x099B:	0x0D0345  	AY0 = CNTR
						//0x099C:	0x2B004E  	AR = AY0 - 1, AY0 = SR0
						//0x099D:	0x0D0CDA  	OWRCNTR = AR
						//0x099E:	0x189A2F  	JUMP $09A2
						dspDMem[i2++]=0;
						dspDMem[i2++]=0;
						i++;
						ay0=sr0;
					}
					else
					{
						//0x099F:	0x8061C4  	AY0 = DM($061C)
						//0x09A0:	0x2AE24E  	AR = AR - AY0, AY0 = SR0
						//0x09A1:	0x6800A9  	DM(I2,M1) = AR
						ay0=dspDMem[0x61c];
						ar-=ay0;
						ay0=sr0;
						dspDMem[i2++]=ar;
					}
					//0x09A2:	0x0D00A9  	AR = SE
					//0x09A3:	0x22620F  	AR = AR + AY0
					//0x09A4:	0x2EEA9A  	AF = AR - AY1, SE = AR
					//0x09A5:	0x189BE4  	IF LT JUMP $09BE
					ar=se;
					ar+=ay0;
					se=ar;
					if(ar>=ay1)
					{
						//0x09A6:	0x0D0090  	SE = AX0
						//0x09A7:	0x8390F8  	SI = DM($390F)
						//0x09A8:	0x930008  	DM($3000) = SI
						se=ax0;
						page=BasePage;

						//0x09A9:	0x400FF4  	AY0 = $00FF
						//0x09AA:	0x700081  	SI = DM(I4,M5)
						//0x09AB:	0x1010BA  	SR = LSHIFT SI (LO), MR0 = AR
						si=*(WORD *)&m->dspRMem[(i4^0x2000)+page];
						i4++;
						if(se>0)
							sr=si<<se;
						else
							sr=si>>(se*-1);
						mr0=ar;

						//0x09AC:	0x7000A1  	AR = DM(I4,M5)
						//0x09AD:	0x23820F  	AR = AR AND AY0
						//0x09AE:	0x0F1A00  	SR = SR OR LSHIFT AR BY 0 (LO)
						//0x09AF:	0x6AEBE5  	AR = MR0 - AY1, DM(I1,M1) = SR0
						ar=*(WORD *)&m->dspRMem[(i4^0x2000)+page];
						i4++;
						ar&=0xff;
						sr|=ar;
						ar=mr0-ay1;
						dspDMem[i1++]=sr&0xffff;
						if((i1%2)==0)
							i1-=2;

						//0x09B0:	0x400038  	SI = $0003
						//0x09B1:	0x930008  	DM($3000) = SI
						page=(3<<12);

						//0x09B2:	0x0D02E0  	SR0 = I4
						//0x09B3:	0x0F1603  	SR = LSHIFT SR0 BY 3 (LO)
						//0x09B4:	0x2E7E9A  	AF = SR0, SE = AR
						//0x09B5:	0x189BE5  	IF GE JUMP $09BE
						sr=(i4<<3);
						se=ar;
						if(sr&0x8000)
						{
							//0x09B6:	0x3B0004  	M4 = $3000
							//0x09B7:	0x090010  	MODIFY (I4,M4)
							//0x09B8:	0x380004  	M4 = $0000
							//0x09B9:	0x8390F4  	AY0 = DM($390F)
							//0x09BA:	0x22200F  	AR = AY0 + 1
							//0x09BB:	0x9390FA  	DM($390F) = AR
							//0x09BC:	0x22EB0F  	AR = MR0 - AY1
							//0x09BD:	0x0D009A  	SE = AR
							i4-=0x1000;
							WORD p=dspDMem[0x390f];
							p++;
							dspDMem[0x390f]=p;
							BasePage=((p&0xff00)>>1)|(p&0x7f);
							BasePage<<=12;

							ar=mr0-ay1;
							se=ar;
						}
					}
					//0x09BE:	0x600085  	SI = DM(I1,M1)
					si=dspDMem[i1++];
					if((i1%2)==0)
						i1-=2;
				}
			}
			//0x09BF:	0x090005  	MODIFY (I1,M1)
			i1++;
			if((i1%2)==0)
				i1-=2;

			//0x09C0:	0x946431  	DM($0643) = I1
			//0x09C1:	0x90640A  	DM($0640) = AR
			//0x09C2:	0x9B9FF0  	DM($39FF) = I4
			//0x09C3:	0x8390F8  	SI = DM($390F)
			//0x09C4:	0x930008  	DM($3000) = SI
			//0x09C5:	0x340009  	L1 = $0000
			//0x09C6:	0x189F7F  	JUMP $09F7
			dspDMem[0x643]=i1;
			dspDMem[0x640]=ar;
			dspDMem[0x39ff]=i4;
			page=BasePage;
		}
		else
		{
			//0x09C7:	0x879FF0  	I0 = DM($39FF)
			//0x09C8:	0x846431  	I1 = DM($0643)
			//0x09C9:	0x8B90F0  	I4 = DM($390F)
			//0x09CA:	0x9B0000  	DM($3000) = I4
			i0=dspDMem[0x39ff];
			i1=dspDMem[0x643];
			i4=dspDMem[0x390f];
			page=BasePage;

			//0x09CB:	0x370007  	M3 = $3000
			//0x09CC:	0x340029  	L1 = $0002
			//0x09CD:	0x806414  	AY0 = DM($0641)
			//0x09CE:	0x23000F  	AR = AY0 - 1
			//0x09CF:	0x0D004A  	AY0 = AR
			ay0=dspDMem[0x641]-1;

			//0x09D0:	0x4000FA  	AR = $000F
			//0x09D1:	0x23220F  	AR = AY0 - AR
			//0x09D2:	0x2A20DA  	AR = AY0 + 1, MR2 = AR
			//0x09D3:	0x0D004A  	AY0 = AR
			ar=ay0-0x0f;
			mr2=ar;
			ar=ay0+1;
			ay0=ar;

			//0x09D4:	0x400105  	AY1 = $0010
			//0x09D5:	0x400080  	AX0 = $0008
			//0x09D6:	0x400FF1  	AX1 = $00FF
			//0x09D7:	0x806409  	SE = DM($0640)
			//0x09D8:	0x80640A  	AR = DM($0640)
			//0x09D9:	0x0D0E56  	CNTR = M6
			//0x09DA:	0x600085  	SI = DM(I1,M1)
			//0x09DB:	0x149F0E  	DO $09F0 UNTIL CE
			ay1=0x10;
			ax0=0x08;
			ax1=0xff;
			se=dspDMem[0x640];
			ar=se;
			si=dspDMem[i1++];
			if((i1%2)==0)
				i1-=2;
			for(i=0;i<m6;i++)
			{
				//0x09DC:	0x120085  	SR = LSHIFT SI (HI), SI = DM(I1,M1)
				if(se>0)
					sr=(si<<16)<<se;
				else
					sr=(si<<16)>>(se*-1);
				si=dspDMem[i1++];
				if((i1%2)==0)
					i1-=2;

				//0x09DD:	0x10189D  	SR = SR OR LSHIFT SI (LO), SE = MR2
				if(se>0)
					sr|=(si<<se);
				else
					sr|=(si>>(se*-1));
				se=mr2;

				//0x09DE:	0x0E370F  	SR = ASHIFT SR1 (LO)
				int sr1=(int)(short)((sr>>16)&0xffff);
				if(se>0)
					sr1<<=se;
				else
					sr1>>=(se*-1);
				//0x09DF:	0x6A62E9  	AR = AR + AY0, DM(I2,M1) = SR0
				//0x09E0:	0x2EEA9A  	AF = AR - AY1, SE = AR
				//0x09E1:	0x189F04  	IF LT JUMP $09F0
				ar=ar+ay0;
				dspDMem[i2++]=sr1;
				se=ar;
				if(ar>=ay1)
				{
					//0x09E2:	0x0D0090  	SE = AX0
					//0x09E3:	0x667981  	AF = AX1, SI = DM(I0,M1)
					//0x09E4:	0x1010BA  	SR = LSHIFT SI (LO), MR0 = AR
					se=ax0;
					af=ax1;
					si=*(WORD *)&m->dspRMem[(i0^0x2000)+page];
					i0++;
					if(se>0)
						sr=si<<se;
					else
						sr=si>>(se*-1);
					mr0=ar;

					//0x09E5:	0x6000A1  	AR = DM(I0,M1)
					//0x09E6:	0x23920F  	AR = AR AND AF
					//0x09E7:	0x0F1A00  	SR = SR OR LSHIFT AR BY 0 (LO)
					ar=*(WORD *)&m->dspRMem[(i0^0x2000)+page];
					i0++;
					ar&=af;
					sr|=ar;

					//0x09E8:	0x6AEBE5  	AR = MR0 - AY1, DM(I1,M1) = SR0
					//0x09E9:	0x0D01E0  	SR0 = I0
					//0x09EA:	0x0F1603  	SR = LSHIFT SR0 BY 3 (LO)
					//0x09EB:	0x2E7E9A  	AF = SR0, SE = AR
					//0x09EC:	0x189F05  	IF GE JUMP $09F0
					ar=mr0-ay1;
					dspDMem[i1++]=sr&0xffff;
					if((i1%2)==0)
						i1-=2;
					sr=i0<<3;
					se=ar;
					if(sr&0x8000)
					{
						//0x09ED:	0x090003  	MODIFY (I0,M3)
						//0x09EE:	0x090011  	MODIFY (I4,M5)
						//0x09EF:	0x9B0000  	DM($3000) = I4
						i0-=0x1000;
						i4++;
						page=((i4&0xff00)>>1)|(i4&0x7f);
						page<<=12;
					}
				}
				//0x09F0:	0x600085  	SI = DM(I1,M1)
				si=dspDMem[i1++];
				if((i1%2)==0)
					i1-=2;
			}
			//0x09F1:	0x090005  	MODIFY (I1,M1)
			//0x09F2:	0x340009  	L1 = $0000
			i1++;
			if((i1%2)==0)
				i1-=2;

			//0x09F3:	0x90640A  	DM($0640) = AR
			//0x09F4:	0x979FF0  	DM($39FF) = I0
			//0x09F5:	0x946431  	DM($0643) = I1
			//0x09F6:	0x9B90F0  	DM($390F) = I4
			dspDMem[0x640]=ar;
			dspDMem[0x39ff]=i0;
			dspDMem[0x643]=i1;
			dspDMem[0x390f]=i4;
			BasePage=page;
			
		}
		//0x09F7:	0x3796C2  	I2 = $396C
		//0x09F8:	0x0D0E56  	CNTR = M6
		//0x09F9:	0x149FFE  	DO $09FF UNTIL CE
		i2=0x396c;
		for(j=0;j<m6;j++)
		{
			//0x09FA:	0x667AA9  	AF = AR, AR = DM(I2,M1)
			//0x09FB:	0x20A20F  	MR = AR * MY0 (SU)
			int t1;
			int t2;
			af=ar;
			ar=dspDMem[i2];
			t1=(int)(short)dspDMem[i2];
			i2++;
			t2=(DWORD)(WORD)my0;
			mr=t1*t2;

			//0x09FC:	0x0D002B  	MX0 = MR0
			//0x09FD:	0x6000CC  	MR1 = DM(I3,M0)
			//0x09FE:	0x21280F  	MR = MR + MX0 * MY1 (SU)
			//0x09FF:	0x6800CE  	DM(I3,M2) = MR1
			mx0=mr&0xffff;
			mr&=0x0000ffff;
			mr|=(dspDMem[i3]<<16);
			t1=(int)(short)mx0;
			t2=(DWORD)(WORD)my1;
			mr+=(t1*t2);
			dspDMem[i3]=(mr>>16)&0xffff;
			i3+=m2;
		}
ShortExit:
		//0x0a00:	0x846840  	I0 = DM($0684)
		//0x0a01:	0x090001  	MODIFY (I0,M1)
		//0x0a02:	0x946840  	DM($0684) = I0
		//0x0a03:	0x188BDF  	JUMP $08BD
		dspDMem[0x684]++;
	}

	//0x0a04:	0x378000  	I0 = $3800
	//0x0a05:	0x600041  	AY0 = DM(I0,M1)
	//0x0a06:	0x806425  	AY1 = DM($0642)
	//0x0a07:	0x6000A0  	AR = DM(I0,M0)
	//0x0a08:	0x6AEA50  	AR = AR - AY1, DM(I0,M0) = AY1
	//0x0a09:	0x22620F  	AR = AR + AY0
	//0x0a0A:	0x93800A  	DM($3800) = AR
	i0=0x3800;
	ay0=dspDMem[i0++];
	ay1=dspDMem[0x642];
	ar=dspDMem[i0];
	ar=ar-ay1;
	dspDMem[i0]=ay1;
	ar=ar+ay0;
	dspDMem[0x3800]=ar;

	//0x0a0B:	0x805FB8  	SI = DM($05FB)
	//0x0a0C:	0x0D0468  	M2 = SI
	//0x0a0D:	0x0F1001  	SR = LSHIFT SI BY 1 (LO)
	//0x0a0E:	0x0D047E  	M3 = SR0
	//0x0a0F:	0x346850  	I0 = $0685
	//0x0a10:	0x090002  	MODIFY (I0,M2)
	si=dspDMem[0x5fb];
	m2=si;
	m3=si<<1;
	i0=0x685+m2;

	//0x0a11:	0x806408  	SI = DM($0640)
	//0x0a12:	0x680080  	DM(I0,M0) = SI
	//0x0a13:	0x343883  	I3 = $0388
	//0x0a14:	0x09000F  	MODIFY (I3,M3)
	dspDMem[i0]=dspDMem[0x640];
	i3=0x388+m3;

	//0x0a15:	0x1C369F  	CALL $0369
	//0x0a16:	0x839FD8  	SI = DM($39FD)
	//0x0a17:	0x68008D  	DM(I3,M1) = SI
	//0x0a18:	0x839FE8  	SI = DM($39FE)
	//0x0a19:	0x68008C  	DM(I3,M0) = SI
	offset=dspDMem[0x39ff];
	page=dspDMem[0x390f];
	hleDSPVSetPageOffset(page,offset);
	hleDSPVGetAbs(&vptr);

	dspDMem[i3++]=(vptr>>16)&0xffff;
	dspDMem[i3]=(vptr>>0)&0xffff;

	//0x0a1A:	0x846431  	I1 = DM($0643)
	//0x0a1B:	0x340029  	L1 = $0002
	//0x0a1C:	0x346890  	I0 = $0689
	//0x0a1D:	0x090003  	MODIFY (I0,M3)
	i1=dspDMem[0x643];
	i0=0x689+m3;

	//0x0a1E:	0x600085  	SI = DM(I1,M1)
	//0x0a1F:	0x680081  	DM(I0,M1) = SI
	//0x0a20:	0x600084  	SI = DM(I1,M0)
	//0x0a21:	0x680080  	DM(I0,M0) = SI
	//0x0a22:	0x340009  	L1 = $0000
	si=dspDMem[i1++];
	if((i1%2)==0)
		i1-=2;
	dspDMem[i0++]=si;
	si=dspDMem[i1];
	dspDMem[i0]=si;
	dspBank=BasePage>>12;

	//0x0a23:	0x0A000F  	RTS
}

bool hleDSPFuncA(WORD m3,WORD m2)
{
	//0x01D6:	0x343800  	I0 = $0380
	//0x01D7:	0x090003  	MODIFY (I0,M3)
	//0x01D8:	0xA00001  	DM(I0,M1) = $0000
	//0x01D9:	0xA00000  	DM(I0,M0) = $0000
	//0x01DA:	0x343880  	I0 = $0388
	//0x01DB:	0x090003  	MODIFY (I0,M3)
	//0x01DC:	0xA00001  	DM(I0,M1) = $0000
	//0x01DD:	0xA00000  	DM(I0,M0) = $0000
	//0x01DE:	0x343B40  	I0 = $03B4
	//0x01DF:	0x090002  	MODIFY (I0,M2)
	//0x01E0:	0xA00000  	DM(I0,M0) = $0000
	//0x01E1:	0x343FC0  	I0 = $03FC
	//0x01E2:	0x090002  	MODIFY (I0,M2)
	//0x01E3:	0xA00000  	DM(I0,M0) = $0000
	//0x01E4:	0x0D01B6  	MR0 = M2
	WORD i0;
	i0=0x380+m3;
	dspDMem[i0++]=0;
	dspDMem[i0]=0;

	i0=0x388+m3;
	dspDMem[i0++]=0;
	dspDMem[i0]=0;

	i0=0x3b4+m2;
	dspDMem[i0]=0;

	i0=0x3fc+m2;
	dspDMem[i0]=0;

	//0x01E5:	0x1C332F  	CALL $0332
	hleDSPResetBlocks(m2);
	//0x01E6:	0x0A000F  	RTS
	return(true);
}

void hleDSPDispatch()
{
	DWORD tmp;
	WORD ay0,ay1,ax0,ax1;
	DWORD mr,sr;
	WORD ar,m2,m3,mr1,mr0;
	WORD i0,i1,i2,i3,i4;

	//0x01AC:	0x1C35EF  	CALL $035E
	//0x01AD:	0x0D005A  	AY1 = AR
	//0x01AE:	0x26280F  	AF = AY1 + 1
	//0x01AF:	0x181B70  	IF EQ JUMP $01B7
	hleDSPVSetMem();
	hleDSPVRead16(&ar);
	hleDSPVFlush();
	ay1=ar;
	if(ay1!=0xffff)
	{
		//0x01B0:	0x403A80  	AX0 = $03A8
		//0x01B1:	0x805FB4  	AY0 = DM($05FB)
		//0x01B2:	0x22600F  	AR = AX0 + AY0
		//0x01B3:	0x0D040A  	I0 = AR
		//0x01B4:	0x6000A0  	AR = DM(I0,M0)
		//0x01B5:	0x26EA0F  	AF = AR - AY1
		//0x01B6:	0x181C50  	IF EQ JUMP $01C5
		i0=dspDMem[0x5fb]+0x3a8;
		ar=dspDMem[i0];
		if(ar==ay1)
		{
			//0x01C5:	0xA00000  	DM(I0,M0) = $0000
			//0x01C6:	0x1C34BF  	CALL $034B
			//0x01C7:	0x400104  	AY0 = $0010
			//0x01C8:	0x26E20F  	AF = AR - AY0
			//0x01C9:	0x181CE4  	IF LT JUMP $01CE
			dspDMem[i0]=0;
			hleDSPVRead8(&ar);
			hleDSPVFlush();
			if(ar<0x10)
			{
				bool Exit;
				//0x01CE:	0x403104  	AY0 = $0310
				//0x01CF:	0x2A625A  	AR = AR + AY0, AY1 = AR
				//0x01D0:	0x0D080A  	I4 = AR
				//0x01D1:	0x805FBA  	AR = DM($05FB)
				//0x01D2:	0x0D046A  	M2 = AR
				//0x01D3:	0x0F1201  	SR = LSHIFT AR BY 1 (LO)
				//0x01D4:	0x0D047E  	M3 = SR0
				//0x01D5:	0x0B000F  	JUMP (I4)
				ay0=0x310;
				ay1=ar;
//				ar=ar+ay0;
				i4=ar;
				ar=dspDMem[0x5fb];
				m2=ar;
				m3=m2<<1;
				switch(i4)
				{
				case 0:
					{
						Exit=hleDSPFuncA(m3,m2);
						break;
					}
				case 1:
					{
						Exit=hleDSPFuncB(m3,m2);
						break;
					}
				case 2:
					{
						Exit=hleDSPFuncC();
						break;
					}
				case 3:
					{
						Exit=hleDSPFuncD();
						break;
					}
				case 4:
					{
						Exit=hleDSPFuncE();
						break;
					}
				case 5:
					{
						Exit=hleDSPFuncF();
						break;
					}
				case 6:
					{
						Exit=hleDSPFuncG();
						break;
					}
				case 7:
				case 8:
				case 9:
					{
						Exit=hleDSPFuncH(ay1);
						break;
					}
				case 0xa:
				case 0xb:
				case 0xc:
					{
						Exit=hleDSPFuncI(ay1);
						break;
					}
				case 0xd:
					{
						Exit=hleDSPFuncJ();
						break;
					}
				case 0xe:
					{
						Exit=hleDSPFuncK();
						break;
					}
				case 0xf:
					{
						Exit=hleDSPFuncL();
						break;
					}
				}
				if(!Exit)
				{
					hleDSPDispatch();
				}
			}
			else
			{
				//fatal error, invalid jump table entry
			}
			return;
		}
	}
	//0x01B7:	0x1C369F  	CALL $0369
	//0x01B8:	0x403800  	AX0 = $0380
	//0x01B9:	0x805FB4  	AY0 = DM($05FB)
	//0x01BA:	0x22600F  	AR = AX0 + AY0
	//0x01BB:	0x22620F  	AR = AR + AY0
	//0x01BC:	0x0D040A  	I0 = AR
	//0x01BD:	0x839FDC  	MR1 = DM($39FD)
	//0x01BE:	0x839FEB  	MR0 = DM($39FE)
	//0x01BF:	0x4FFFE2  	MX0 = $FFFE
	//0x01C0:	0x400016  	MY0 = $0001
	//0x01C1:	0x21000F  	MR = MR + MX0 * MY0 (SS)
	//0x01C2:	0x6800C1  	DM(I0,M1) = MR1
	//0x01C3:	0x6800B0  	DM(I0,M0) = MR0
	//0x01C4:	0x0A000F  	RTS
	DWORD addy;

	hleDSPVGetAbs(&addy);
	i0=dspDMem[0x5fb]*2+0x380;
	addy-=2;
	dspDMem[i0++]=(addy>>16)&0xffff;
	dspDMem[i0]=(addy>>0)&0xffff;
	return;

}

	
bool hleDSPFuncB(WORD m3,WORD m2)
{
	WORD mr1,mr0,ar,si,af,i0,i1,i2;
	DWORD tmp,mr;
	WORD px;

	hleDSPVSetMem();

	//0x01E7:	0x1C34BF  	CALL $034B
	//0x01E8:	0x0D00CA  	MR1 = AR
	hleDSPVRead8(&mr1);

	//0x01E9:	0x1C365F  	CALL $0365
	hleDSPVRead24(&tmp);
	ar=tmp&0xffff;
	px=(tmp>>16);

	//0x01EA:	0x8390F8  	SI = DM($390F)
	//0x01EB:	0x906098  	DM($0609) = SI
	//0x01EC:	0x839FF8  	SI = DM($39FF)
	//0x01ED:	0x9060A8  	DM($060A) = SI
	//0x01EE:	0x0D0387  	SI = PX
	//0x01EF:	0x939FD8  	DM($39FD) = SI
	//0x01F0:	0x939FEA  	DM($39FE) = AR
	//0x01F1:	0x1C33FF  	CALL $033F
	//0x01F2:	0x1C35EF  	CALL $035E
	//0x01F3:	0x0D008A  	SI = AR
	WORD page,offset;
	hleDSPVGetPageOffset(&page,&offset);
	dspDMem[0x609]=page;
	dspDMem[0x60a]=offset;
	dspDMem[0x39fd]=px;
	dspDMem[0x39fe]=ar;
	hleDSPVSetAbs(tmp);		//see above, this is (px<<16)|ar
	hleDSPVRead16(&ar);
	si=ar;

	//0x01F4:	0x403B84  	AY0 = $03B8
	//0x01F5:	0x22640F  	AR = MR1 + AY0
	//0x01F6:	0x0D040A  	I0 = AR
	//0x01F7:	0x680080  	DM(I0,M0) = SI
	dspDMem[mr1+0x3b8]=si;

	//0x01F8:	0x403BC4  	AY0 = $03BC
	//0x01F9:	0x22640F  	AR = MR1 + AY0
	//0x01FA:	0x0D040A  	I0 = AR
	//0x01FB:	0x680080  	DM(I0,M0) = SI
	dspDMem[mr1+0x3bc]=si;

	//0x01FC:	0x403C44  	AY0 = $03C4
	//0x01FD:	0x26640F  	AF = MR1 + AY0
	//0x01FE:	0x22740F  	AR = MR1 + AF
	//0x01FF:	0x0D040A  	I0 = AR
	//0x0200:	0x8390F8  	SI = DM($390F)
	//0x0201:	0x680081  	DM(I0,M1) = SI
	//0x0202:	0x839FF8  	SI = DM($39FF)
	//0x0203:	0x680080  	DM(I0,M0) = SI
	//0x0204:	0x1C34BF  	CALL $034B
	//0x0205:	0x3C00F5  	CNTR = $000F
	//0x0206:	0x14208E  	DO $0208 UNTIL CE
	ar=mr1*2+0x3c4;
	i0=ar;
	hleDSPVGetPageOffset(&page,&offset);
	dspDMem[i0++]=page;
	dspDMem[i0]=offset;
	hleDSPVRead8(&ar);
	for(int i=0;i<15;i++)
	{
		//0x0207:	0x1C34BF  	CALL $034B
		//0x0208:	0x000000  	NOP
		hleDSPVRead8(&ar);
	}

	//0x0209:	0x1C369F  	CALL $0369
	//0x020A:	0x403884  	AY0 = $0388
	//0x020B:	0x26640F  	AF = MR1 + AY0
	//0x020C:	0x22740F  	AR = MR1 + AF
	//0x020D:	0x0D041A  	I1 = AR
	//0x020E:	0x403904  	AY0 = $0390
	//0x020F:	0x26640F  	AF = MR1 + AY0
	//0x0210:	0x22740F  	AR = MR1 + AF
	//0x0211:	0x0D042A  	I2 = AR
	//0x0212:	0x839FD8  	SI = DM($39FD)
	//0x0213:	0x839FEA  	AR = DM($39FE)
	//0x0214:	0x680085  	DM(I1,M1) = SI
	//0x0215:	0x6800A4  	DM(I1,M0) = AR
	//0x0216:	0x680089  	DM(I2,M1) = SI
	//0x0217:	0x6800A8  	DM(I2,M0) = AR
	hleDSPVGetAbs(&tmp);
	i1=mr1*2+0x388;
	i2=mr1*2+0x390;
	dspDMem[i1++]=(tmp>>16);
	dspDMem[i1]=tmp&0xffff;
	dspDMem[i2++]=(tmp>>16);
	dspDMem[i2]=tmp&0xffff;

	//0x0218:	0x806098  	SI = DM($0609)
	//0x0219:	0x9390F8  	DM($390F) = SI
	//0x021A:	0x8060A8  	SI = DM($060A)
	//0x021B:	0x939FF8  	DM($39FF) = SI
	page=dspDMem[0x609];
	offset=dspDMem[0x60a];
	hleDSPVSetPageOffset(page,offset);

	//0x021C:	0x1C34BF  	CALL $034B
	//0x021D:	0x0D008A  	SI = AR
	//0x021E:	0x403984  	AY0 = $0398
	//0x021F:	0x22640F  	AR = MR1 + AY0
	//0x0220:	0x0D041A  	I1 = AR
	//0x0221:	0x680084  	DM(I1,M0) = SI
	hleDSPVRead8(&si);
	dspDMem[mr1+0x398]=si;

	//0x0222:	0x403C04  	AY0 = $03C0
	//0x0223:	0x22640F  	AR = MR1 + AY0
	//0x0224:	0x0D040A  	I0 = AR
	//0x0225:	0x6000A0  	AR = DM(I0,M0)
	//0x0226:	0x267A0F  	AF = AR
	//0x0227:	0x182374  	IF LT JUMP $0237
	i0=mr1+0x3c0;
	ar=dspDMem[i0];
	if(!(ar&0x8000))
	{
		//0x0228:	0x805FB5  	AY1 = DM($05FB)
		//0x0229:	0x26EA0F  	AF = AR - AY1
		//0x022A:	0x182370  	IF EQ JUMP $0237
		if(ar!=dspDMem[0x5fb])
		{
			//0x022B:	0x400046  	MY0 = $0004
			//0x022C:	0x0D00BA  	MR0 = AR
			//0x022D:	0x21640F  	MR = MR + MR1 * MY0 (UU)
			mr=(mr1<<16)|ar;
			mr=ar+(mr1*4);
			mr0=mr&0xffff;

			//0x022E:	0x400008  	SI = $0000
			//0x022F:	0x403504  	AY0 = $0350
			//0x0230:	0x22630F  	AR = MR0 + AY0
			//0x0231:	0x0D041A  	I1 = AR
			//0x0232:	0x680084  	DM(I1,M0) = SI
			dspDMem[mr0+0x350]=0;

			//0x0233:	0x403404  	AY0 = $0340
			//0x0234:	0x22630F  	AR = MR0 + AY0
			//0x0235:	0x0D041A  	I1 = AR
			//0x0236:	0x680084  	DM(I1,M0) = SI
			dspDMem[mr0+0x340]=0;
		}
	}
	//0x0237:	0x805FB4  	AY0 = DM($05FB)
	//0x0238:	0x680040  	DM(I0,M0) = AY0
	//0x0239:	0x181ACF  	JUMP $01AC
	dspDMem[i0]=dspDMem[0x5fb];
	hleDSPVFlush();
	return(false);
}

bool hleDSPFuncC()
{
	WORD mr0,i0,i1,ar;

	hleDSPVSetMem();

	//0x023A:	0x1C34BF  	CALL $034B
	//0x023B:	0x0D00BA  	MR0 = AR
	hleDSPVRead8(&ar);
	mr0=ar;

	//0x023C:	0x403884  	AY0 = $0388
	//0x023D:	0x26630F  	AF = MR0 + AY0
	//0x023E:	0x22730F  	AR = MR0 + AF
	//0x023F:	0x0D041A  	I1 = AR
	//0x0240:	0x6000A5  	AR = DM(I1,M1)
	//0x0241:	0x227A0F  	AR = AR
	//0x0242:	0x182461  	IF NE JUMP $0246
	i1=mr0*2+0x388;
	ar=dspDMem[i1++];
	if(ar==0)
	{
		//0x0243:	0x6000A5  	AR = DM(I1,M1)
		//0x0244:	0x227A0F  	AR = AR
		//0x0245:	0x1824E0  	IF EQ JUMP $024E
		ar=dspDMem[i1++];
		if(ar==0)
			goto GotosRFun;
	}

	//0x0246:	0x403884  	AY0 = $0388
	//0x0247:	0x26630F  	AF = MR0 + AY0
	//0x0248:	0x22730F  	AR = MR0 + AF
	//0x0249:	0x0D041A  	I1 = AR
	//0x024A:	0x400008  	SI = $0000
	//0x024B:	0x680085  	DM(I1,M1) = SI
	//0x024C:	0x680085  	DM(I1,M1) = SI
	//0x024D:	0x1C332F  	CALL $0332
	i1=mr0*2+0x388;
	dspDMem[i1++]=0;
	dspDMem[i1++]=0;
	hleDSPZeroPtrs(mr0);

GotosRFun:
	//0x024E:	0x403B44  	AY0 = $03B4
	//0x024F:	0x22630F  	AR = MR0 + AY0
	//0x0250:	0x0D041A  	I1 = AR
	//0x0251:	0x400008  	SI = $0000
	//0x0252:	0x680085  	DM(I1,M1) = SI
	i1=mr0+0x3b4;
	dspDMem[i1++]=0;

	//0x0253:	0x403804  	AY0 = $0380
	//0x0254:	0x26630F  	AF = MR0 + AY0
	//0x0255:	0x22730F  	AR = MR0 + AF
	//0x0256:	0x0D041A  	I1 = AR
	//0x0257:	0x680085  	DM(I1,M1) = SI
	//0x0258:	0x680085  	DM(I1,M1) = SI
	i1=mr0*2+0x380;
	dspDMem[i1++]=0;
	dspDMem[i1++]=0;

	//0x0259:	0x805FBA  	AR = DM($05FB)
	//0x025A:	0x403804  	AY0 = $0380
	//0x025B:	0x26620F  	AF = AR + AY0
	//0x025C:	0x22720F  	AR = AR + AF
	//0x025D:	0x0D041A  	I1 = AR
	//0x025E:	0x6000A5  	AR = DM(I1,M1)
	//0x025F:	0x227A0F  	AR = AR
	//0x0260:	0x182641  	IF NE JUMP $0264
	i1=dspDMem[0x5fb]*2+0x380;
	ar=dspDMem[i1++];
	hleDSPVFlush();
	if(ar==0)
	{
		//0x0261:	0x6000A4  	AR = DM(I1,M0)
		//0x0262:	0x227A0F  	AR = AR
		//0x0263:	0x0A0000  	IF EQ RTS
		ar=dspDMem[i1];
		if(ar==0)
		{
//			adsp2100_set_pc(0x30f);		//temp, set pc to any RTS instruction
			return(true);
		}
	}
	//0x0264:	0x181ACF  	JUMP $01AC
	return(false);
//	adsp2100_set_pc(0x1ac);
}

void hleDSPProcess(WORD ar)
{
	DWORD sr;
	WORD i0,i1,ax1,ay1,af,m2,m3;

	//0x0168:	0x0F1201  	SR = LSHIFT AR BY 1 (LO)
	//0x0169:	0x0D047E  	M3 = SR0
	//0x016A:	0x343880  	I0 = $0388
	//0x016B:	0x090003  	MODIFY (I0,M3)
	//0x016C:	0x600011  	AX1 = DM(I0,M1)
	//0x016D:	0x600050  	AY1 = DM(I0,M0)
	//0x016E:	0x27A90F  	AF = AX1 OR AY1
	//0x016F:	0x0A0000  	IF EQ RTS
	sr=ar<<1;
	m3=(sr&0xffff);
	i0=0x388+m3;
	ax1=dspDMem[i0++];
	ay1=dspDMem[i0];
	if((ax1|ay1)==0)
		return;

	//0x0170:	0x343900  	I0 = $0390
	//0x0171:	0x090003  	MODIFY (I0,M3)
	//0x0172:	0x667911  	AF = AX1, AX1 = DM(I0,M1)
	//0x0173:	0x66F110  	AF = AX1 - AF, AX1 = DM(I0,M0)
	//0x0174:	0x181771  	IF NE JUMP $0177
	i0=0x390+m3;
	af=ax1;
	ax1=dspDMem[i0++];
	af=ax1-af;
	ax1=dspDMem[i0];
	if(af==0)
	{
		//0x0175:	0x26E90F  	AF = AX1 - AY1
		//0x0176:	0x1C8000  	IF EQ CALL $0800
		af=ax1-ay1;
		if(af==0)
			hleDSPNextSample();

	}
	//0x0177:	0x1C801F  	CALL $0801
	hleDSPMama();

	//0x0178:	0x805FB8  	SI = DM($05FB)
	//0x0179:	0x0D0468  	M2 = SI
	//0x017A:	0x0F1001  	SR = LSHIFT SI BY 1 (LO)
	//0x017B:	0x0D047E  	M3 = SR0
	m2=dspDMem[0x5fb];
	m3=m2<<1;

	//0x017C:	0x343B80  	I0 = $03B8
	//0x017D:	0x090002  	MODIFY (I0,M2)
	//0x017E:	0x600050  	AY1 = DM(I0,M0)
	//0x017F:	0x23080F  	AR = AY1 - 1
	//0x0180:	0x6800A0  	DM(I0,M0) = AR
	//0x0181:	0x0A0001  	IF NE RTS
	i0=0x3b8+m2;
	ay1=dspDMem[i0];
	ar=ay1-1;
	dspDMem[i0]=ar;
	if(ar)
		return;

	//0x0182:	0x343BC1  	I1 = $03BC
	//0x0183:	0x090006  	MODIFY (I1,M2)
	//0x0184:	0x6000A4  	AR = DM(I1,M0)
	//0x0185:	0x6800A0  	DM(I0,M0) = AR
	i1=0x3bc+m2;
	dspDMem[i0]=dspDMem[i1];

	//0x0186:	0x343900  	I0 = $0390
	//0x0187:	0x090003  	MODIFY (I0,M3)
	//0x0188:	0x343881  	I1 = $0388
	//0x0189:	0x090007  	MODIFY (I1,M3)
	//0x018A:	0x600081  	SI = DM(I0,M1)
	//0x018B:	0x680085  	DM(I1,M1) = SI
	//0x018C:	0x600080  	SI = DM(I0,M0)
	//0x018D:	0x680084  	DM(I1,M0) = SI
	i0=0x390+m3;
	i1=0x388+m3;
	dspDMem[i1++]=dspDMem[i0++];
	dspDMem[i1]=dspDMem[i0];

	//0x018E:	0x343980  	I0 = $0398
	//0x018F:	0x090002  	MODIFY (I0,M2)
	//0x0190:	0x600050  	AY1 = DM(I0,M0)
	//0x0191:	0x22080F  	AR = AY1
	//0x0192:	0x0A0000  	IF EQ RTS
	i0=0x398+m2;
	ay1=dspDMem[i0];
	ar=ay1;
	if(ar==0)
		return;

	//0x0193:	0x23080F  	AR = AY1 - 1
	//0x0194:	0x181970  	IF EQ JUMP $0197
	ar--;
	if(ar!=0)
	{
		//0x0195:	0x6800A0  	DM(I0,M0) = AR
		//0x0196:	0x0A000F  	RTS
		dspDMem[i0]=ar;
		return;
	}

	//0x0197:	0x343880  	I0 = $0388
	//0x0198:	0x090003  	MODIFY (I0,M3)
	//0x0199:	0xA00001  	DM(I0,M1) = $0000
	//0x019A:	0xA00000  	DM(I0,M0) = $0000
	//0x019B:	0x343C00  	I0 = $03C0
	//0x019C:	0x090002  	MODIFY (I0,M2)
	//0x019D:	0xAFFFF0  	DM(I0,M0) = $FFFF
	i0=0x388+m3;
	dspDMem[i0++]=0;
	dspDMem[i0]=0;
	i0=0x3c0+m2;
	dspDMem[i0]=0xffff;

	//0x019E:	0x0A000F  	RTS
}

void hleDSPUpdateAudio()
{
	if(hleDSPPingTime==0) hleDSPPingTime=getTime()+7;
	WORD tmp=dspDMem[0x60c];
	if(tmp==0x400)
	{
		int diff;

		tmp=0x4f0;
		dspDMem[0x60c]=tmp;
		adsp2100_set_reg(ADSP2100_I7,0x400);
		dspUpdateCount++;
		theApp.m_EmuObj->UpdateAudio(0x400);
		diff=hleDSPPingTime-getTime();
		if(diff>0)
		{
			svcSleepThread(diff);
			hleDSPPingTime+=7;
		}
		else
		{
			hleDSPPingTime+=7;
			if(diff<-500) hleDSPPingTime=getTime()+7;
		}
	}
	else
	{
		int diff;
		tmp=0x400;
		dspDMem[0x60c]=tmp;
		adsp2100_set_reg(ADSP2100_I7,0x4f0);
		theApp.m_EmuObj->UpdateAudio(0x4f0);
		diff=hleDSPPingTime-getTime();
		if(diff>0)
		{
			svcSleepThread(diff);
			hleDSPPingTime+=7;
		}
		else
		{
			hleDSPPingTime+=7;
			if(diff<-500) hleDSPPingTime=getTime()+7;
		}
	}
}

void hleDSPUpdatePtr(WORD ar)
{
	//0x0320:	0x3C0003  	IMASK = $0000
	//0x0321:	0x845F80  	I0 = DM($05F8)
	//0x0322:	0x340108  	L0 = $0010
	//0x0323:	0x6800A1  	DM(I0,M1) = AR
	//0x0324:	0x340008  	L0 = $0000
	//0x0325:	0x945F80  	DM($05F8) = I0
	//0x0326:	0x3C0203  	IMASK = $0020
	dspIMASK=0;
	WORD i0=dspDMem[0x5f8];
	dspDMem[i0++]=ar;
	if((i0%0x10)==0)
		i0-=0x10;
	dspDMem[0x5f8]=i0;
	dspIMASK=1;
	//0x0327:	0x0A000F  	RTS
}

bool hleDSPFuncD()
{
	WORD ar;

	//0x0265:	0x1C35EF  	CALL $035E
	//0x0266:	0x1C320F  	CALL $0320
	//0x0267:	0x181ACF  	JUMP $01AC
	hleDSPVSetMem();
	hleDSPVRead16(&ar);
	hleDSPUpdatePtr(ar);
	hleDSPVFlush();
	return(false);
}

bool hleDSPFuncE()
{
	WORD ar;

	//0x0268:	0x1C34BF  	CALL $034B
	//0x0269:	0x0D002A  	MX0 = AR
	//0x026A:	0x3B4000  	I4 = $3400
	//0x026B:	0x7800A0  	DM(I4,M4) = AR
	//0x026C:	0x181ACF  	JUMP $01AC
	hleDSPVSetMem();
	hleDSPVRead8(&ar);
	dspDMem[0x3400]=ar;
	dspPeek=ar;
	hleDSPVFlush();
	return(false);
}

bool hleDSPFuncF()
{
	WORD ar,se,i0,i1,mr0;
	DWORD mr;

	//0x026D:	0x1C34BF  	CALL $034B
	//0x026E:	0x0D00BA  	MR0 = AR
	//0x026F:	0x405E04  	AY0 = $05E0
	//0x0270:	0x22620F  	AR = AR + AY0
	//0x0271:	0x0D041A  	I1 = AR
	//0x0272:	0x6000A4  	AR = DM(I1,M0)
	//0x0273:	0x227A0F  	AR = AR
	//0x0274:	0x181AC0  	IF EQ JUMP $01AC
	hleDSPVSetMem();
	hleDSPVRead8(&ar);
	mr0=ar;
	i1=ar+0x5e0;
	ar=dspDMem[i1];
	if(ar==0)
	{
		hleDSPVFlush();
		return(false);
	}

	//0x0275:	0x400008  	SI = $0000
	//0x0276:	0x680084  	DM(I1,M0) = SI
	//0x0277:	0x400024  	AY0 = $0002
	//0x0278:	0x26E20F  	AF = AR - AY0
	//0x0279:	0x182801  	IF NE JUMP $0280
	dspDMem[i1]=0;
	if(ar==2)
	{
		//0x027A:	0x405E84  	AY0 = $05E8
		//0x027B:	0x22630F  	AR = MR0 + AY0
		//0x027C:	0x0D041A  	I1 = AR
		//0x027D:	0x6000A4  	AR = DM(I1,M0)
		//0x027E:	0x1C320F  	CALL $0320
		//0x027F:	0x181ACF  	JUMP $01AC
		i1=mr0+0x5e8;
		ar=dspDMem[i1];
		hleDSPUpdatePtr(ar);
		hleDSPVFlush();
		return(false);
	}

	//0x0280:	0x405E84  	AY0 = $05E8
	//0x0281:	0x22630F  	AR = MR0 + AY0
	//0x0282:	0x0D041A  	I1 = AR
	//0x0283:	0x6000A4  	AR = DM(I1,M0)
	//0x0284:	0x90608A  	DM($0608) = AR
	//0x0285:	0x0F12F8  	SR = LSHIFT AR BY -8 (LO)
	//0x0286:	0x4039C4  	AY0 = $039C
	//0x0287:	0x22660F  	AR = SR0 + AY0
	//0x0288:	0x0D040A  	I0 = AR
	i1=mr0+0x5e8;
	ar=dspDMem[i1];
	dspDMem[0x608]=ar;
	ar=ar>>8;
	ar+=0x39c;
	i0=ar;

	//0x0289:	0x600080  	SI = DM(I0,M0)
	//0x028A:	0x0D0098  	SE = SI
	//0x028B:	0x8390F8  	SI = DM($390F)
	//0x028C:	0x906108  	DM($0610) = SI
	//0x028D:	0x839FF8  	SI = DM($39FF)
	//0x028E:	0x906118  	DM($0611) = SI
	//0x028F:	0x400048  	SI = $0004
	//0x0290:	0x9390F8  	DM($390F) = SI
	//0x0291:	0x420438  	SI = $2043
	//0x0292:	0x939FF8  	DM($39FF) = SI
	//0x0293:	0x1C365F  	CALL $0365
	//0x0294:	0x0D03C7  	MR1 = PX
	//0x0295:	0x0D00BA  	MR0 = AR
	WORD page,offset;

	se=dspDMem[i0];
	hleDSPVGetPageOffset(&page,&offset);
	dspDMem[0x610]=page;
	dspDMem[0x611]=offset;
	hleDSPVSetPageOffset(4,0x43);
	hleDSPVRead24(&mr);
	

	//0x0296:	0x80608A  	AR = DM($0608)
	//0x0297:	0x400FF4  	AY0 = $00FF
	//0x0298:	0x23820F  	AR = AR AND AY0
	//0x0299:	0x400036  	MY0 = $0003
	//0x029A:	0x21620F  	MR = MR + AR * MY0 (UU)
	//0x029B:	0x939FDC  	DM($39FD) = MR1
	//0x029C:	0x939FEB  	DM($39FE) = MR0
	//0x029D:	0x1C33FF  	CALL $033F
	//0x029E:	0x1C365F  	CALL $0365
	//0x029F:	0x0D03C7  	MR1 = PX
	//0x02A0:	0x0D00BA  	MR0 = AR
	ar=dspDMem[0x608];
	ar&=0xff;
	mr+=ar*3;
	hleDSPVSetAbs(mr);
	hleDSPVRead24(&mr);

	//0x02A1:	0x0D0029  	MX0 = SE
	//0x02A2:	0x400026  	MY0 = $0002
	//0x02A3:	0x21600F  	MR = MR + MX0 * MY0 (UU)
	//0x02A4:	0x939FDC  	DM($39FD) = MR1
	//0x02A5:	0x939FEB  	DM($39FE) = MR0
	//0x02A6:	0x1C33FF  	CALL $033F
	//0x02A7:	0x1C35EF  	CALL $035E
	//0x02A8:	0x1C320F  	CALL $0320
	mr+=se*2;
	hleDSPVSetAbs(mr);
	hleDSPVRead16(&ar);
	hleDSPUpdatePtr(ar);

	//0x02A9:	0x806108  	SI = DM($0610)
	//0x02AA:	0x9390F8  	DM($390F) = SI
	//0x02AB:	0x806118  	SI = DM($0611)
	//0x02AC:	0x939FF8  	DM($39FF) = SI
	//0x02AD:	0x181ACF  	JUMP $01AC
	page=dspDMem[0x610];
	offset=dspDMem[0x611];
	hleDSPVSetPageOffset(page,offset);
	return(false);
}

bool hleDSPFuncG()
{
	//0x02AE:	0x181ACF  	JUMP $01AC
	return(false);
}


void hleDSPMakeP()
{
	WORD ar;
	int sr;
	short tmp;

	//0x0328:	0x1C34BF  	CALL $034B
	//0x0329:	0x90604A  	DM($0604) = AR
	//0x032A:	0x1C34BF  	CALL $034B
	//0x032B:	0x0F1208  	SR = LSHIFT AR BY 8 (LO)
	//0x032C:	0x0F36FE  	SR = ASHIFT SR0 BY -2 (LO)
	//0x032D:	0x90605E  	DM($0605) = SR0
	//0x032E:	0x0A000F  	RTS
	hleDSPVSetMem();
	hleDSPVRead8(&ar);
	dspDMem[0x604]=ar;
	hleDSPVRead8(&ar);
	sr=ar<<8;
	tmp=(short)(sr&0xffff);
	tmp>>=2;
	dspDMem[0x605]=tmp;
	hleDSPVFlush();
}


bool hleDSPFuncH(WORD ay1)
{
	WORD ar;

	//0x02AF:	0x40007A  	AR = $0007
	//0x02B0:	0x232A0F  	AR = AY1 - AR
	//0x02B1:	0x90606A  	DM($0606) = AR
	//0x02B2:	0x1C328F  	CALL $0328
	//0x02B3:	0x400000  	AX0 = $0000
	//0x02B4:	0x182BAF  	JUMP $02BA
	ar=ay1-7;
	dspDMem[0x606]=ar;
	hleDSPMakeP();

	return(hleDSPFuncHIMain(0));
}

//0x032F:	0x1C35EF  	CALL $035E
//0x0330:	0x0D000A  	AX0 = AR
//0x0331:	0x0A000F  	RTS


bool hleDSPFuncI(WORD ay1)
{
	WORD ar,ax0;

	//0x02B5:	0x4000AA  	AR = $000A
	//0x02B6:	0x232A0F  	AR = AY1 - AR
	//0x02B7:	0x90606A  	DM($0606) = AR
	//0x02B8:	0x1C328F  	CALL $0328
	//0x02B9:	0x1C32FF  	CALL $032F
	ar=ay1-0xa;
	dspDMem[0x606]=ar;
	hleDSPVSetMem();
	hleDSPMakeP();
	hleDSPVRead16(&ax0);
	hleDSPVFlush();

	return(hleDSPFuncHIMain(ax0));
}

bool hleDSPFuncHIMain(WORD ax0)
{
	short ar;
	int sr;
	DWORD mr;
	WORD i0,i1,i2,m2;
	WORD ax1,mx0,mx1,my0,my1,ay0,ay1;
	WORD af;

	//0x02BA:	0x806042  	MX0 = DM($0604)
	//0x02BB:	0x400046  	MY0 = $0004
	//0x02BC:	0x805FBB  	MR0 = DM($05FB)
	//0x02BD:	0x21600F  	MR = MR + MX0 * MY0 (UU)
	mx0=dspDMem[0x604];
	my0=4;
	mr=dspDMem[0x5fb];
	mr+=mx0*my0;

	//0x02BE:	0x0D046B  	M2 = MR0
	//0x02BF:	0x343400  	I0 = $0340
	//0x02C0:	0x090002  	MODIFY (I0,M2)
	//0x02C1:	0x680000  	DM(I0,M0) = AX0
	m2=mr&0xffff;
	i0=0x340+m2;
	dspDMem[i0]=ax0;

	//0x02C2:	0x343501  	I1 = $0350
	//0x02C3:	0x090006  	MODIFY (I1,M2)
	//0x02C4:	0x343702  	I2 = $0370
	//0x02C5:	0x09000A  	MODIFY (I2,M2)
	//0x02C6:	0x806061  	AX1 = DM($0606)
	//0x02C7:	0x80605A  	AR = DM($0605)
	//0x02C8:	0x667954  	AF = AX1, AY1 = DM(I1,M0)
	//0x02C9:	0x182D40  	IF EQ JUMP $02D4
	i1=0x350+m2;
	i2=0x370+m2;
	ax1=dspDMem[0x606];
	ar=dspDMem[0x605];
	ay1=dspDMem[i1];
	af=ax1;
	if(af)
	{
		//0x02CA:	0x27100F  	AF = AF - 1
		//0x02CB:	0x233A01  	IF NE AR = -AR
		af=af-1;
		if(af)
			ar=-ar;

		//0x02CC:	0x2A6A4A  	AR = AR + AY1, AY0 = AR
		//0x02CD:	0x41FFF5  	AY1 = $1FFF
		//0x02CE:	0x26EA0F  	AF = AR - AY1
		//0x02CF:	0x220802  	IF GT AR = AY1
		//0x02D0:	0x4E0015  	AY1 = $E001
		//0x02D1:	0x26EA0F  	AF = AR - AY1
		//0x02D2:	0x220804  	IF LT AR = AY1
		//0x02D3:	0x182D6F  	JUMP $02D6
		ay0=ar;
		ar+=ay1;
		if(ar>0x1fff)
			ar=0x1fff;
		if(ar<(short)0xe001)
			ar=(short)0xe001;
	}
	else
	{
		//0x02D4:	0x2AEA1A  	AR = AR - AY1, AX1 = AR
		//0x02D5:	0x2A794A  	AR = AX1, AY0 = AR
		ax1=ar;
		ar=ar-ay1;
		ay0=ar;
		ar=ax1;
	}

	//0x02D6:	0x6E78A8  	AF = AX0, DM(I2,M0) = AR
	//0x02D7:	0x182DA1  	IF NE JUMP $02DA
	af=ax0;
	dspDMem[i2]=ar;
	if(af==0)
	{
		//0x02D8:	0x6800A4  	DM(I1,M0) = AR
		//0x02D9:	0x182E7F  	JUMP $02E7
		dspDMem[i1]=ar;
	}
	else
	{
		//0x02DA:	0x0D00E4  	SR0 = AY0
		//0x02DB:	0x0F3601  	SR = ASHIFT SR0 BY 1 (LO)
		//0x02DC:	0x23E60F  	AR = ABS SR0
		//0x02DD:	0x2E184A  	AF = 0, AY0 = AR
		sr=(short)ay0<<1;
		ar=(short)(sr&0xffff);
		if(ar<0) ar*=-1;
		ay0=ar;

		//0x02DE:	0x021000  	DIVS AF,AX0
		//0x02DF:	0x3C00F5  	CNTR = $000F
		//0x02E0:	0x142E1E  	DO $02E1 UNTIL CE
		//0x02E1:	0x031000  	DIVQ AX0
		//0x02E2:	0x0D00A4  	AR = AY0
		ar=ay0/ax0;

		//0x02E3:	0x233A0A  	IF NEG AR = -AR
		//0x02E4:	0x343602  	I2 = $0360
		//0x02E5:	0x09000A  	MODIFY (I2,M2)
		//0x02E6:	0x6800A8  	DM(I2,M0) = AR
		if(ar<0)
			ar*=-1;
		i2=0x360+m2;
		dspDMem[i2]=ar;
	}
	//0x02E7:	0x181ACF  	JUMP $01AC
	return(false);
}

bool hleDSPFuncJ()
{
	//0x02E8:	0x181ACF  	JUMP $01AC
	return(false);
}

bool hleDSPFuncK()
{
	WORD si,page,offset;

	//0x02E9:	0x1C34BF  	CALL $034B
	//0x02EA:	0x0D008A  	SI = AR
	//0x02EB:	0x8390FF  	SR1 = DM($390F)
	//0x02EC:	0x839FFE  	SR0 = DM($39FF)
	//0x02ED:	0x182FEF  	JUMP $02FE
	hleDSPVSetMem();
	hleDSPVRead8(&si);
	hleDSPVFlush();
	hleDSPVGetPageOffset(&page,&offset);


	return(hleDSPFuncKLMain(si,page,offset));
}


bool hleDSPFuncL()
{
	WORD si,page,offset,ar,i1,ay0;

	//0x02EE:	0x4FFF40  	AX0 = $FFF4
	//0x02EF:	0x1C305F  	CALL $0305
	hleDSPIncPtr(0xfff4,&i1);

	//0x02F0:	0x37FFC6  	M2 = $3FFC
	//0x02F1:	0x090006  	MODIFY (I1,M2)
	i1-=4;

	//0x02F2:	0x6000E6  	SR0 = DM(I1,M2)
	//0x02F3:	0x6000F6  	SR1 = DM(I1,M2)
	//0x02F4:	0x600084  	SI = DM(I1,M0)
	offset=dspDMem[i1];
	i1-=4;
	page=dspDMem[i1];
	i1-=4;
	si=dspDMem[i1];

	//0x02F5:	0x0D0048  	AY0 = SI
	//0x02F6:	0x27000F  	AF = AY0 - 1
	//0x02F7:	0x181AC0  	IF EQ JUMP $01AC
	ay0=si;
	if(ay0==1)
	{
		return(false);
	}
	//0x02F8:	0x26000F  	AF = AY0
	//0x02F9:	0x182FC0  	IF EQ JUMP $02FC
	if(ay0)
	{
		//0x02FA:	0x23000F  	AR = AY0 - 1
		//0x02FB:	0x0D008A  	SI = AR
		ar=ay0-1;
		si=ar;
	}

	//0x02FC:	0x9390FF  	DM($390F) = SR1
	//0x02FD:	0x939FFE  	DM($39FF) = SR0
	hleDSPVSetPageOffset(page,offset);
	return(hleDSPFuncKLMain(si,page,offset));
}

bool hleDSPFuncKLMain(WORD si,WORD page,WORD offset)
{
	WORD i1;

	//0x02FE:	0x4000C0  	AX0 = $000C
	//0x02FF:	0x1C305F  	CALL $0305
	hleDSPIncPtr(0xc,&i1);

	//0x0300:	0x340047  	M3 = $0004
	//0x0301:	0x680087  	DM(I1,M3) = SI
	//0x0302:	0x6800F7  	DM(I1,M3) = SR1
	//0x0303:	0x6800E7  	DM(I1,M3) = SR0
	dspDMem[i1]=si;
	i1+=4;
	dspDMem[i1]=page;
	i1+=4;
	dspDMem[i1]=offset;
	i1+=4;

	//0x0304:	0x181ACF  	JUMP $01AC
	return(false);
}

bool hleDSPIncPtr(WORD ax0,WORD *i1)
{
	WORD m2,i0,ar,ay0;

	//0x0305:	0x845FB6  	M2 = DM($05FB)
	//0x0306:	0x343FC0  	I0 = $03FC
	//0x0307:	0x090002  	MODIFY (I0,M2)
	//0x0308:	0x600040  	AY0 = DM(I0,M0)
	m2=dspDMem[0x5fb];
	i0=m2+0x3fc;
	ay0=dspDMem[i0];


	//0x0309:	0x403CCA  	AR = $03CC
	//0x030A:	0x22620F  	AR = AR + AY0
	//0x030B:	0x0D041A  	I1 = AR
	//0x030C:	0x090006  	MODIFY (I1,M2)
	*i1=0x3cc+ay0+m2;

	//0x030D:	0x22600F  	AR = AX0 + AY0
	//0x030E:	0x6800A0  	DM(I0,M0) = AR
	ar=ax0+ay0;
	dspDMem[i0]=ar;

	//0x030F:	0x0A000F  	RTS
	return(true);
}
