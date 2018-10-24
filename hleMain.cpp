
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "hleMain.h"
#include "iMemory.h"
#include "hleDSP.h"
#include "iCPU.h"

//#define SHOW_NAME(a) theApp.LogMessage(a)
#define SHOW_NAME(a) 
extern WORD inputs[4];
void hleFMVSub2Patch();
bool hleFMVDelay=false;
void hleKeyInput();


void hleICache()
{
	return;
	 
}

void hleVirtualToPhysical()
{
	DWORD addr=r->GPR[A0*2];
	theApp.LogMessage("addr %X",addr);
	if(addr<0x00400000)
		addr|=0x80000000;
	else if(addr<0x05000000)
		addr|=0xa0000000;
	else addr&=0x3fffff;
	r->GPR[V0*2]=addr;
}

void hlePiRawStartDma()
{
	DWORD ReadWrite,Source,Dest,Len;
	ReadWrite=r->GPR[A0*2];
	Source=r->GPR[A1*2];
	Dest=r->GPR[A2*2];
	Len=r->GPR[A3*2];
}

void hlePiStartDma()
{
	DWORD ReadWrite,Source,Dest,Len;
	ReadWrite=r->GPR[A0*2];
	Source=r->GPR[A1*2];
	Dest=r->GPR[A2*2];
	Len=r->GPR[A3*2];
}

void hleCos()
{
	double result=cos((float) *(float *)&r->FPR[12]);
	*(float *)&r->FPR[0]=(float) result;
}

float ident[16]={1.f,0.f,0.f,0.f, 0.f,1.f,0.f,0.f, 0.f,0.f,1.f,0.f, 0.f,0.f,0.f,1.f};
WORD identl[32]={0,1,0,0, 1,0,0,0, 0,0,0,1, 0,0,1,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

void hle_ll_mul()
{
	DWORD *src=(DWORD *)&r->GPR[A0*2];
	INT64 *dst=(INT64 *)&r->GPR[V0*2];

	INT64 t1,t2;
	t1=(src[0]<<32)|src[2];
	t2=(src[4]<<32)|src[6];
	t1=t1*t2;
	t2=t1;
	t1<<=32;
	t1>>=32;
	t2>>=32;
	*(dst++)=t2;
	*(dst++)=t1;
}

DWORD hleMakeAddrIndex(BYTE Reg)
{
	return((DWORD)(r->GPR[Reg*2]&0x3fffff));
}

void hleNormalize()
{
	//not tested
	float *src=(float *)&r->GPR[A1*2];
	float *dst=src;

	float vx,vy,vz;
	vx=*(src++);
	src++;
	vy=*(src++);
	src++;
	vz=*src;
    if ((vx== 0) && (vy == 0) && (vz == 0))
		return;
    float inv_mod = (float)(1.0 / sqrt(vx * vx + vy * vy + vz * vz));
	*(dst++)=vx*inv_mod;
	dst++;
	*(dst++)=vy*inv_mod;
	dst++;
	*dst=vz*inv_mod;
}

void hlePerspectiveF()
{
	DWORD addr=hleMakeAddrIndex(A0);
	float *src=(float *)&r->GPR[A1*2];
	float *src2=(float *)&m->rdRam[r->GPR[SP*2]&0x3fffff];
	src2+=4;
	float *dst=(float *)&m->rdRam[addr];
	memcpy(dst,ident,16*4);
	float fovy=src[2];		// A2
	float tmp=fovy;
	float aspect=src[4];	// A3
	float Zn=src2[0];		// a4
	float Zf=src2[1];
	float scale=src2[2];
	fovy*=(6.28318530718f/720.f);
//	fovy/=2.f;

	float CotFovY=(float)cos(fovy)/(float)sin(fovy);
	dst[0]=(CotFovY/aspect)*scale;
	dst[5]=CotFovY*scale;
	float q=(Zf+Zn)/(Zn-Zf);
	dst[10]=q*scale;
	dst[14]=(2.f*Zn*Zf)/(Zn-Zf);
	dst[15]=0.f;
	dst[11]=-1.f;
}

void hleMtxIdentF()
{
	DWORD addr=hleMakeAddrIndex(A0);
	memcpy(&m->rdRam[addr],ident,16*4);
}

void hleMtxIdent()
{
	DWORD addr=hleMakeAddrIndex(A0);
	memcpy(&m->rdRam[addr],identl,16*4);
}

void hleScaleF()
{
	DWORD addr=hleMakeAddrIndex(A0);
	DWORD *src=(DWORD *)&r->GPR[A1*2];
	DWORD *dst=(DWORD *)&m->rdRam[addr];
	memcpy(dst,ident,16*4);
	*dst=*(src++);
	src++;
	dst+=5;
	*dst=*(src++);
	src++;
	dst+=5;
	*dst=*src;
	dst+=5;
	*dst=0x3f800000;
}

void hleTranslateF()
{
	DWORD addr=hleMakeAddrIndex(A0);
	DWORD *src=(DWORD *)&r->GPR[A1*2];
	DWORD *dst=(DWORD *)&m->rdRam[addr];
	memcpy(dst,ident,16*4);
	dst+=12;
	*(dst++)=*(src++);
	src++;
	*(dst++)=*(src++);
	src++;
	*dst=*src;
}

void hleRotateF()
{
	//obviously not tested
}

void hleMemCopy()
{
	DWORD src;
	DWORD dst;
	src=hleMakeAddrIndex(A1);
	dst=hleMakeAddrIndex(A0);
	sDWORD count=r->GPR[A2*2]&0xffff;
	while(count)
	{
		count--;
		m->rdRam[(dst+count)^3]=m->rdRam[(src+count)^3];
	}
	r->GPR[(V0*2)]=r->GPR[(A0*2)];
	r->GPR[(V0*2)+1]=r->GPR[(A0*2)+1];
}

void hleMtxF2L()
{
/*
	DWORD *src=(DWORD *)((r->GPR[A0*2]&0x3fffff)+m->rdRam);
	DWORD *dst=(DWORD *)((r->GPR[A1*2]&0x3fffff)+m->rdRam);
	for(WORD i=0;i<16;i++)
		*(dst++)=*(src++);
	return;
*/
	WORD *dst1=(WORD *)((r->GPR[A1*2]&0x3fffff)+m->rdRam);
	WORD *dst2=(WORD *)((r->GPR[A1*2]&0x3fffff)+32+m->rdRam);
	float *src=(float *)((r->GPR[A0*2]&0x3fffff)+m->rdRam);
	DWORD val1,val2;
	for(WORD i=0;i<8;i++)
	{
		val1=(DWORD)(*(src++)*65536.f);
		val2=(DWORD)(*(src++)*65536.f);
		*(dst1++)=(WORD)(val2>>16);
		*(dst2++)=(WORD)(val2);
		*(dst1++)=(WORD)(val1>>16);
		*(dst2++)=(WORD)(val1);
	}
}

void hleSin()
{
	double result=sin((float) *(float *)&r->FPR[12]);
	*(float *)&r->FPR[0]=(float) result;
}
 
void hleDelay()
{
	DWORD delay;
	DWORD start;
	delay=*(DWORD *)&m->rdRam[0x8f5b4];
	start=*(DWORD *)&m->rdRam[0x8f5bc];
	r->ICount=start+delay;
	r->PC+=4;
}

void hleDelayFrames()
{
	DWORD count=r->GPR[A0*2];
	while(count)
	{
		iCpuVSYNC();
	}
}

void hleDelayFrame()
{
	iCpuVSYNC();
}

void hleWaitVSYNC()
{
//	iCpuVSYNC();		//i removed this for version 3.01
						//with this here, the update rate was 60hz, however, the game played at half that - 10/05/02rcp
	r->GPR[V1*2]=0x400;
}

void hleSprite1()
{
//	r->ICount+=15000;
	BYTE *src;
	WORD *dst;
	DWORD tmp=(DWORD)r->GPR[V1*2];
	src=(BYTE *)&m->rdRam[tmp&0x7fffff];
	DWORD keep=(DWORD)r->GPR[V0*2];
	tmp=keep&0x7fffff;
	if((keep>>24)==0x88)
		dst=(WORD *)(m->rdRam+tmp);
	else
		dst=(WORD *)(SRAM+tmp-0x0000);

	tmp=(DWORD)r->GPR[S6*2];

	dst+=(0x40/sizeof(WORD));
	src+=(0x22/sizeof(BYTE));
	*dst=(WORD)tmp&0xffff;
	dst--;
	BYTE *pal=(BYTE *)&m->rdRam[0x90000];
	BYTE index;
	WORD pixel;

	for(WORD count=0;count<31;count++)
	{
		index=*src;
		index=0;
		src--;
		pixel=*(WORD *)&pal[index];
		*dst=pixel;
		dst--;
	}
	index=*src;
	src--;
	pixel=*(WORD *)&pal[index];
	r->GPR[S6*2]=(DWORD)pixel;
}

void hleWriteBlock()
{
//	r->ICount+=150;

	DWORD *dst=(DWORD *)hleMakeP(r->GPR[K0*2]);
	DWORD *src=(DWORD *)&r->GPR[A0*2];
	for(DWORD t=0;t<8;t++)
	{
		memcpy(dst,src,16);
		dst+=(640/4);
		src+=4;
	}
}

void hleReadBlock()
{
//	r->ICount+=150;
	DWORD *src=(DWORD *)hleMakeP(r->GPR[T8*2]);
	DWORD *dst=(DWORD *)&r->GPR[A0*2];
	for(DWORD t=0;t<8;t++)
	{
		memcpy(dst,src,16);
		src+=(640/4);
		dst+=4;
	}
}

BYTE *hleMakeP(DWORD offset)
{
	BYTE *src;
/*
	DWORD page=offset>>24;
	page^=0x8800000;
	offset&=0x7fffff;
	page&=0x800000;
	src=(BYTE *)page+offset+(DWORD)m->rdRam;
*/
	switch(offset>>24)
	{
	case 0x80:
		{
			offset&=0x7fffff;
			if((offset>=0x20000)&&(offset<0xb0000))
				src=(BYTE *)(SRAM+(offset&0x7fffff));
			else
				src=(BYTE *)(m->rdRam+(offset&0x7fffff));
			break;
		}
	case 0x88:
		{
			src=(BYTE *)(m->rdRam+(offset&0x7fffff));
			break;
		}
	}
/*
*/
	return(src);
}

void hleReadBlock2()
{
//	r->ICount+=150;
	DWORD *dst=(DWORD *)&r->GPR[A0*2];
	DWORD *src=(DWORD *)hleMakeP(r->GPR[T8*2]);
	DWORD p1,p2,p3,p4;
	DWORD t1,t2,t3,t4;
	DWORD *src2=(DWORD *)hleMakeP(r->GPR[T8*2]+2);
	for(DWORD t=0;t<8;t++)
	{
		p1=*(src++);
		p1+=0x04210421;
		p1&=0x7bde7bde;
		t1=*(src2++);
		t1&=0x7bde7bde;
		p1+=t1;
		p1>>=1;
		*(dst++)=p1;

		p1=*(src++);
		p1+=0x04210421;
		p1&=0x7bde7bde;
		t1=*(src2++);
		t1&=0x7bde7bde;
		p1+=t1;
		p1>>=1;
		*(dst++)=p1;

		p1=*(src++);
		p1+=0x04210421;
		p1&=0x7bde7bde;
		t1=*(src2++);
		t1&=0x7bde7bde;
		p1+=t1;
		p1>>=1;
		*(dst++)=p1;

		p1=*(src++);
		p1+=0x04210421;
		p1&=0x7bde7bde;
		t1=*(src2++);
		t1&=0x7bde7bde;
		p1+=t1;
		p1>>=1;
		*(dst++)=p1;


		src+=((640/4)-4);
		src2+=((640/4)-4);
	}
}

void hleDummy()
{
	int DummyIBe=1;
	DummyIBe++;
}

void hleRandom()
{
/*
5BDC lui        A2,$FFFF8808                      A2=88080000     
5BE0 lw         A2,$7268(A2)                      *(88087268)     
5BE4 srl        A3,A2,$9                          A3=0:440400     
5BE8 sll        T0,A2,$17                         T0=0:0          
5BEC xor        A2,A3,A2                          A2=FFFFFFFF:884C
5BF0 mf0        A3,T1            INDEX            A3=0:0          
5BF4 xor        A2,A2,T0                          A2=FFFFFFFF:884C
5BF8 xor        A2,A2,A3                          A2=FFFFFFFF:884C
5BFC lui        At,$FFFF8808                      At=88080000     
5C00 jr         RA                                                
5C04 sw         A2,$7268(At)                      *(88087268)     
*/
	DWORD a2,a3,t0;
	a2=*(DWORD *)&m->rdRam[0x87268];
	a3=a2>>9;
	t0=a3<<23;
	a2=a2^a3;
	a3=r->ICount;
	a3+=1230;
	r->ICount=a3;
	a2=a2^t0;
	a2=a2^a3;
	*(DWORD *)&m->rdRam[0x87268]=a2;
}



WORD hlePatchScreenCopy(BYTE *cp)
{
	return 0;
}

WORD hlePatchPal(BYTE *cp)
{
	return 0;
 
}

void hleScreenCopy()
{
	DWORD *src=(DWORD *)hleMakeP(0x884A0000);
	DWORD *dst=(DWORD *)hleMakeP((*(DWORD *)&m->rdRam[0x40DC8]));
	memcpy(dst,src,0x25800);
}


WORD hleCheckFunction(BYTE *cp,DWORD Address)
{
	 return 0;
}

void hleScrollBlock(DWORD op0)
{
	sQWORD val=*(sQWORD *)&r->GPR[op0*2+0];
	if(val>=0)
	{
		hleReadBlock();
	}
	else
	{
		hleReadBlock2();
	}
}

void hleCheckWriteBlock(DWORD op0)
{
	sQWORD val=*(sQWORD *)&r->GPR[op0*2];
	if(val<0)
	{
		hleReadBlock2();
//		hleWriteBlock();
	}
}

WORD hleCheckFunctionBGEZAL(BYTE *cp,DWORD Address,BYTE op0)
{
	 return 0;

}

WORD hleCheckFunctionBLTZAL(BYTE *cp,DWORD Address,BYTE op0)
{
 
	return 0;

}

/*

  0x30ccc - write 16x8 block to screen (ram?)

  input: k0=dst address, gpr[4]->gpr[19] data to store

  0x30d10 - read 16x8 block from screen into gpr[4]->gpr[19]

  input: t8=src address, gpr[4]->gpr[19] data to store


  0x32330 - something with FMV
  inputs:t1-t7 and s3 from a3


  t1=(t1*8)-t1+(t1*16);



  0x3c158 - mid level FMV
	input: t3 - length?
			a3: dst?
			a2: src?

  t1=0;
  t2=0x880338c0
  dst[0->15]=0;
  dst[32->47]=0; etc. 8 total times  16x8 block with a 32 byte pitch
  calls 3228c( v0=t0[0]) t0=40f10????


  sprites
  0x1111c - different dst

  0x3108c - builds background in off screen buffer (i guess)
  inputs: fpr[4],

  */

/*

  This function pieces together the background.  It reads from a group of bitmap images
  stored in the upper parts of ram and assembles the 320x240 bitmap given the current
  camera position and angle.  this data is NOT sent directly to the screen, it is
  further massaged by hleScrollBlock() that basically interpolates between various
  pre-rendered camera positions and angles.  In quite a few instances, this routine
  and hleScrollBlock() yield a 2x speed improvement.

  */

void hleBuildBG()
{
	sDWORD a0;
	sDWORD a1,a2;
	DWORD *m_a0;
//3108C mf1        A0,A0                                             
//31090 NOP
//31094 bltz       A0 31158                                          
	a0=r->FPR[4];
	r->ICount+=15000;
	if(a0>=0)
	{
		//3109C lui        T2,$FFFF8002                      T2=80020000     
		//310AC ori        T2,T2,$CE00                       T2=0:8002CE00   
		DWORD *t2=(DWORD *)(SRAM+0x02ce00);
		//310A0 mf1        A0,R0                                             
		//310A4 mf1        A1,At                                             
		//310A8 mf1        A2,V0                                             
		//310B0 movi       A3,$2                             A3=0:2          
		a0=r->FPR[0];
		a1=r->FPR[1];
		a2=r->FPR[2];
		DWORD t1,t3;
		DWORD *src;
		for(DWORD a3=0;a3<2;a3++)
		{
//			310B4 movi       T0,$50                            T0=0:50         
			for(DWORD t0=0;t0<0x50;t0++)
			{
//				310B8 lwu        T1,$0(A0)                                         
				//this pointer can change.  in end level sequences, it seems to read directly from
				//rdRam rather than offscreen ram.
				if(((a0>>24)&0xff)==0x88)
					m_a0=(DWORD *)((a0&0x7fffff)+m->rdRam);
				else
					m_a0=(DWORD *)((a0&0x7fffff)-0x0000+SRAM);

				t1=m_a0[0];
//				310BC slti       At,A1,$F                                          
//				310C0 sllv       T1,T1,A1                                          
//				310C4 bne        R0,At 310E0                                       
//				310C8 srl        T1,T1,$E                                          
				t1<<=a1;
				t1>>=0xe;
				if(a1>=0xf)
				{
	//				310CC lwu        T3,$4(A0)                                         
					t3=m_a0[1];
	//				310D0 movi       T4,$2E                            T4=0:2E         
	//				310D4 sub        T4,T4,A1                                          
	//				310D8 srlv       T3,T3,T4                                          
	//				310DC or         T1,T1,T3                                          
					t3>>=(0x2e - a1);
					t1|=t3;
				}
//				310E0 add        T1,T1,T1                                          
//				310E4 add        T1,T1,A2                                          
				t1+=t1;
				t1+=a2;
//				310EC ldr        T6,$0(T1)                                         
//				310F0 addi       A1,A1,$12                                         
//				310F4 slti       At,A1,$20                                         
//				310F8 sd         T6,$0(T2)                         *(8002CE00)     
//				31100 ldr        T6,$280(T1)                                       
//				31104 sd         T6,$280(T2)                       *(8002D080)     
//				3110C ldr        T6,$500(T1)                                       
//				31110 sd         T6,$500(T2)                       *(8002D300)     
//				31118 ldr        T6,$780(T1)                                       
				//this pointer can change.  in end level sequences, it seems to read directly from
				//rdRam rather than offscreen ram.
				if(((t1>>24)&0xff)==0x88)
					src=(DWORD *)((t1&0x7fffff)+m->rdRam);
				else
					src=(DWORD *)((t1&0x7fffff)-0x0000+SRAM);
				t2[0]=src[0];
				t2[4/4]=src[4/4];
				t2[0x280/4]=src[0x280/4];
				t2[0x284/4]=src[0x284/4];
				t2[0x500/4]=src[0x500/4];
				t2[0x504/4]=src[0x504/4];
				t2[0x780/4]=src[0x780/4];
				t2[0x784/4]=src[0x784/4];
//				3111C bne        R0,At 3112C                                       
//				31120 sd         T6,$780(T2)                       *(8002D580)     
				a1+=0x12;
				if(a1>=0x20)
				{
	//				31124 andi       A1,A1,$1F                                         
	//				31128 addi       A0,A0,$4                                          
					a1&=0x1f;
					a0+=4;
				}
//				3112C addi       T0,T0,$FFFFFFFF                   T0=0:4F         
//				31130 bne        R0,T0 310B8                                       
//				31134 addi       T2,T2,$8                          T2=FFFFFFFF:8002
				t2+=2;
			}
//			31138 addi       A3,A3,$FFFFFFFF                   A3=0:1          
//			3113C bne        R0,A3 310B4                                       
//			31140 addi       T2,T2,$780                        T2=FFFFFFFF:8002
			t2+=0x1e0;
		}
//		31144 mt1        R0,A0                                             
//		31148 mt1        At,A1                                             
		r->FPR[0]=a0;
		r->FPR[1]=a1;
//		3114C lui        GP,$FFFF8002                      GP=80020000     
//		31150 jr         RA                                                
//		31154 ori        GP,GP,$CE00                       GP=0:8002CE00   
		r->GPR[GP*2]=0x8002ce00;
		r->GPR[GP*2+1]=0xffffffff;
		return;
	}
//	31158 mf1        A0,A1                                             
//	3115C mf1        GP,V0                                             
	a0=r->FPR[A1];
	*(sQWORD *)&r->GPR[GP*2]=(sQWORD)r->FPR[V0];
	a0=r->GPR[K0*2]-a0;
	r->GPR[GP*2]+=a0;
//	31160 sub        A0,K0,A0                                          
//	31164 jr         RA                                                
//	31168 add        GP,GP,A0                                          
}

/*
0xc87c - rle ?
0xcd14 -
0xc778 - 
0xc744 - 
0xcb38 -
0xcac0 -
0xca04 -

  parents?
0xcd90 -
0xce30 -
*/

void hleFlatScroll()
{

	r->ICount+=15000;
	//C744 lh         A1,$6(S8)                         *(88030006)     
	//C748 lh         T0,$4(S8)                         *(88030004)     
	//C74C lh         A2,$8(S8)                         *(88030008)     
	//C750 mult       A1,T0                                             
	//C754 sll        A3,A2,$9                                          
	//C758 sll        A2,A2,$7                                          
	//C75C add        A2,A2,A3                                          
	//C760 add        A2,A2,V1                                          
	//C764 lh         A3,$A(S8)                         *(8803000A)     
	//C768 addi       T0,T0,$FFFFFD80                   T0=FFFFFFFF:8802
	//C76C mflo       A1                                                
	//C770 add        A1,A1,A0                          A1=0:10690004   
	//C774 NOP
	//C778 addi       T1,A2,$280                                        
	//C77C ldl        T2,$7(A1)                         *(1069000B)     
	//C780 ldr        T2,$0(A1)                         *(10690004)     
	//C784 addi       A2,A2,$8                                          
	//C788 addi       A1,A1,$8                          A1=0:1069000C   
	//C78C bne        T1,A2 C77C                                        
	//C790 sd         T2,$FFFFFFF8(A2)                                  
	//C794 addi       A3,A3,$FFFFFFFF                   A3=0:9F         
	//C798 bne        R0,A3 C778                                        
	//C79C add        A1,A1,T0                          A1=FFFFFFFF:986B
	//C7A0 jr         RA                                                
	//C7A4 NOP

	WORD *ps8=(WORD *)hleMakeP(r->GPR[S8*2]);
	ps8+=2;					//0->4
	DWORD gap=(DWORD)*(ps8++);		//4
	DWORD i1=(DWORD)*(ps8++);		//6
	DWORD i2=(DWORD)*(ps8++);		//8
	WORD count=*(ps8++);	//0xa
	DWORD *src=(DWORD *)hleMakeP(r->GPR[A0*2]+i1*gap);
	DWORD *dst=(DWORD *)hleMakeP(r->GPR[V1*2]+i2*640);
	gap-=640;
	memcpy(dst,src,0x280);
	count--;
	src+=(gap/4);
	dst+=(0x280/4);
	src+=(0x280/4);
	while(count)
	{
		memcpy(dst,src,0x280);
		count--;
		src+=(gap/4);
		dst+=(0x280/4);
		src+=(0x280/4);
	}
}

/*
13BC NOP
13C0 jal        8B8          - input, player movement
13C4 NOP
13C8 jal        C020         - mama function, maybe AI             
13CC NOP
13D0 jal        2038                                              
13D4 NOP
13D8 jal        964                                               
13DC NOP
13E0 jal        8CBC         - collision detect
13E4 NOP
13E8 jal        D064                                              
13EC NOP
13F0 jal        CDC                                               
13F4 NOP
13F8 movi       A0,$1                             A0=0:1          
13FC lui        At,$FFFF8808                      At=88080000     
1400 jal        1434                                              
*/

/*
206f0 - wait vtrace
*/

/*
audio commands

  0xfa0 - if less than then  *(8808622B) must be nonzero: some noted values 0x50, 0x46, 0x82, 
  *(8808F5E2) must be nonzero: 
  *(88086244) must be nonzero:
  0x1fc
  0x9cc
  0x120b
  0x56c
  0x55aa
  0x57a
  0x5ee
  0x5ed
  0xdbd
  0x14cb

*/


void hleBuildBG2()
{
	sDWORD a0;
	sDWORD a1,a2;
	DWORD *m_a0;
	a0=r->FPR[4];
	r->ICount+=15000;
	if(a0>=0)
	{
		DWORD *t2=(DWORD *)(SRAM+0x02ce00);
		a0=r->FPR[0];
		a1=r->FPR[1];
		a2=r->FPR[2];
		DWORD t1,t3;
		DWORD *src;
		if(((a0>>24)&0xff)==0x88)
			m_a0=(DWORD *)((a0&0x7fffff)+m->rdRam);
		else
			m_a0=(DWORD *)((a0&0x7fffff)-0x0000+SRAM);
		BYTE *base;
		if(((m_a0[0]>>24)&0xff)==0x88)
			base=m->rdRam;
		else
			base=SRAM;
		for(DWORD a3=0;a3<2;a3++)
		{
			for(DWORD t0=0;t0<0x50;t0++)
			{

				t1=m_a0[0];
				t1<<=a1;
				t1>>=0xe;
				if(a1>=0xf)
				{
					t3=m_a0[1];
					t3>>=(0x2e - a1);
					t1|=t3;
				}
				t1+=t1;
				t1+=a2;
				src=(DWORD *)(base+(t1&0x7fffff));
/*
				if(((t1>>24)&0xff)==0x88)
					src=(DWORD *)((t1&0x7fffff)+m->rdRam);
				else
					src=(DWORD *)((t1&0x7fffff)-0x0000+SRAM);
*/
				t2[0]=src[0];
				t2[4/4]=src[4/4];
				t2[0x280/4]=src[0x280/4];
				t2[0x284/4]=src[0x284/4];
				t2[0x500/4]=src[0x500/4];
				t2[0x504/4]=src[0x504/4];
				t2[0x780/4]=src[0x780/4];
				t2[0x784/4]=src[0x784/4];
				a1+=0x12;
				if(a1>=0x20)
				{
					a1&=0x1f;
					a0+=4;
					m_a0++;
				}
				t2+=2;
			}
			t2+=0x1e0;
		}
		r->FPR[0]=a0;
		r->FPR[1]=a1;
		r->GPR[GP*2]=0x8002ce00;
		r->GPR[GP*2+1]=0xffffffff;
		return;
	}
	a0=r->FPR[A1];
	*(sQWORD *)&r->GPR[GP*2]=(sQWORD)r->FPR[V0];
	a0=r->GPR[K0*2]-a0;
	r->GPR[GP*2]+=a0;
}

void hleBackground2()
{

	DWORD *pa0,*pa1,*pv0,*pv1;
	sDWORD a0,a1,a2,a3;
	sDWORD t0,t1,t2,t3,t4,t5;
	sDWORD v0,v1;
	sDWORD s2;

	//inputs
	a0=(sDWORD)r->GPR[A0*2];
	a2=(sDWORD)r->GPR[A2*2];
	a3=(sDWORD)r->GPR[A3*2];
	v0=(sDWORD)r->GPR[V0*2];
	v0=*(sDWORD *)hleMakeP(v0+0xdc8);
	v1=(sDWORD)r->GPR[V1*2];

	//18cA0 sw         T2,$4(SP)                         *(12C)          
	//18cA4 sw         T3,$8(SP)                         *(130)          
	//18cAC sw         T4,$C(SP)                         *(134)          
	t0=(sDWORD)r->GPR[T0*2];
	t1=(sDWORD)r->GPR[T1*2];
	t2=(sDWORD)r->GPR[T2*2];
	t3=(sDWORD)r->GPR[T3*2];
	t4=(sDWORD)r->GPR[T4*2];

	//18cA8 bgez       A1 //18cE0                                          
	a1=(sDWORD)r->GPR[A1*2];
	if(a1<0)
	{
		//18cB0 add        T2,A1,A3                          T2=0:100CCEF    
		//18cB4 blez       T2 //18dC8                                          
		t2=a1+a3;
		if(t2<=0)
			return;

		//18cBC sub        T2,R0,A1                          T2=0:7F77       
		//18cC0 sub        A3,A3,T2                          A3=0:1004C89    
		//18cC4 sll        T2,T2,$1                          T2=0:FEEE       
		//18cC8 addi       T3,A2,$4                          T3=FFFFFFFF:FFFF
		t2=-a1;
		a3=a3-t2;
		t2<<=1;
		t3=a2+4;

		//18cCC mult       T2,T3                                             
		//18cD0 mov        A1,R0                             A1=0:8066       
		//18cD4 mflo       T2                                                
		//18cD8 add        V1,V1,T2                          V1=0:1499D      
		//18cDC NOP
		t2*=t3;
		v1+=t2;
		a1=0;
	}
	//18cE0 slti       At,A1,$F0                                         
	//18cE4 beq        R0,At //18dC8                                       
	//18cE8 NOP
	if(a1>=0xf0)
		return;

	//18cEC add        T2,A1,A3                          T2=0:100CCEF    
	//18cF0 addi       T2,T2,$FFFFFF10                   T2=0:100CBFF    
	//18cF4 blez       T2 //18d00                                          
	//18cF8 NOP
	t2=a1+a3;
	t2-=240;
	if(t2>0)
	{
		//18cFC sub        A3,A3,T2                          A3=FFFFFFFF:FFFF
		a3-=t2;
	}
	//18d00 sll        T0,A1,$9                          T0=0:100CC00    
	//18d04 sll        A1,A1,$7                          A1=0:403300     
	//18d08 add        A1,A1,T0                          A1=0:140FF00    
	//18d0C add        V0,V0,A1                          V0=FFFFFFFF:8944
	v0+=a1*640;
	//18d10 bgez       A0 //18d24                                          
	//18d14 mov        A1,R0                             A1=0:8066       
	a1=0;
	if(a0<0)
	{
		//18d18 add        A0,A0,A2                          A0=FFFFFFFF:FFFF
		//18d1C bltz       A0 //18d18                                          
		//18d20 NOP
		a0+=a2;
		while(a0<0) a0+=a2;
	}

	//18d24 slt        At,A0,A2                                          
	//18d28 bne        R0,At //18d40                                       
	//18d2C NOP
	if(a0>=a2)
	{
		//18d30 sub        A0,A0,A2                          A0=0:EF0        
		//18d34 slt        At,A0,A2                                          
		//18d38 beq        R0,At //18d30                                       
		a0-=a2;
		while(a0>=a2) a0-=a2;
		//18d3C NOP
	}
	//18d40 addi       T0,A2,$FFFFFEC4                   T0=FFFFFFFF:FFFF
	t0=a2-316;
	//18d44 slt        At,T0,A0                                          
	//18d48 bne        R0,At //18dE0                                       
	//18d4C NOP
	if(t0>=a0)
	{
		//18d50 addi       A2,A2,$4                          A2=FFFFFFFF:FFFF
		//18d54 mult       A2,A1                                             
		//18d6C mflo       T0                                                
		a2+=4;
		t0=a2*a1;
		//18d58 sll        A0,A0,$1                          A0=0:1DE0       
		//18d5C add        V1,V1,A0                          V1=0:1677D      
		//18d60 addi       A2,A2,$FFFFFEC0                   A2=FFFFFFFF:FFFF
		//18d64 sll        A2,A2,$1                          A2=FFFFFFFF:FFFE
		//18d68 movi       T2,$14                            T2=0:14         
		//18d70 sll        T0,T0,$1                          T0=FFFFFFFF:FFFE
		//18d74 add        V1,V1,T0                          V1=0:6617       
		a0<<=1;
		v1+=a0;
		a2-=320;
		a2<<=1;
		t0<<=1;
		v1+=t0;
		t2=0x14;

		pv1=(DWORD *)hleMakeP(v1);
		pv0=(DWORD *)hleMakeP(v0);
		pv1+=2;
		while(a3)
		{
			//18d78 mov        A0,T2                             A0=0:14         
			a0=0x14;
			while(a0)
			{
				memcpy(pv0,pv1,32);
				//18d7C ldl        A1,$F(V1)                         *(6626)         
				//18d80 ldr        A1,$8(V1)                         *(661F)         
				//18d84 addi       A0,A0,$FFFFFFFF                   A0=0:13         
				//18d88 addi       V0,V0,$20                         V0=FFFFFFFF:8944
				//18d8C sd         A1,$FFFFFFE0(V0)                  *(8944FF08)     

				//18d90 ldl        A1,$17(V1)                        *(662E)         
				//18d94 ldr        A1,$10(V1)                        *(6627)         
				//18d98 addi       V1,V1,$20                         V1=0:6637       
				//18d9C sd         A1,$FFFFFFE8(V0)                  *(8944FF10)     

				//18dA0 ldl        A1,$FFFFFFFF(V1)                  *(6636)         
				//18dA4 ldr        A1,$FFFFFFF8(V1)                  *(662F)         
				//18dA8 sd         A1,$FFFFFFF0(V0)                  *(8944FF//18)     
				//18dAC ldl        A1,$7(V1)                         *(663E)         
				//18dB0 ldr        A1,$0(V1)                         *(6637)         
				//18dB4 bne        R0,A0 //18d7C                                       
				//18dB8 sd         A1,$FFFFFFF8(V0)                  *(8944FF20)     
				a0--;
				pv1+=(32/4);
				pv0+=(32/4);
				v1+=32;
			}
			//18dBC addi       A3,A3,$FFFFFFFF                   A3=FFFFFFFF:FFFF
			//18dC0 bne        R0,A3 //18d78                                       
			//18dC4 add        V1,V1,A2                          V1=FFFFFFFF:FFFF
			a3--;
			pv1+=(a2/4);
			v1+=a2;
		}

		r->GPR[V1*2]=v1;
		return;
	}

	//18dE0 sub        T0,A2,A0                          T0=FFFFFFFF:FFFE
	//18dE4 addi       A2,A2,$4                          A2=FFFFFFFF:FFFE
	//18dE8 mult       A2,A1                                             
	//18dEC sra        T1,T0,$2                          T1=FFFFFFFF:FFFF
	//18dF0 addi       T1,T1,$1                          T1=FFFFFFFF:FFFF
	//18dF4 movi       T2,$50                            T2=0:50         
	//18dF8 sll        T5,T1,$2                          T5=FFFFFFFF:FFFE
	//18dFC sub        T2,T2,T1                          T2=0:40AE       
	////18e00 sll        T4,T2,$2                          T4=0:102B8      
	////18e04 sub        T3,A2,T5                          T3=0:16         
	////18e08 sub        T4,A2,T4                          T4=FFFFFFFF:FFFD
	////18e0C sll        A0,A0,$1                          A0=0:26         
	////18e10 mflo       A1                                                
	t0=a2-a0;
	a2+=4;
	a1=a2*a1;
	t1=t0>>2;
	t1++;
	t2=0x50;
	t5=t1<<2;
	t2-=t1;
	t4=t2<<2;
	t3=a2-t5;
	t4=a2-t4;
	a0<<=1;

	////18e14 sll        A1,A1,$1                          A1=0:100CC      
	////18e18 add        V1,V1,A1                          V1=0:659D       
	////18e1C sub        A1,T5,T0                          A1=0:1          
	////18e20 mov        S2,A1                             S2=0:1          
	////18e24 sll        A1,A1,$1                          A1=0:2          
	////18e28 sll        T3,T3,$1                          T3=0:2C         
	////18e2C sll        T4,T4,$1                          T4=FFFFFFFF:FFFB
	////18e30 add        A0,A0,V1                          A0=0:65C3       
	////18e38 add        A1,A1,V1                          A1=0:659F       
	a1<<=1;
	v1+=a1;
	a1=t5-t0;
	s2=a1;
	a1<<=1;
	t3<<=1;
	t4<<=1;
	a0+=v1;
	a1+=v1;
	pa0=(DWORD *)hleMakeP(a0);
	pa1=(DWORD *)hleMakeP(a1);
	pv0=(DWORD *)hleMakeP(v0);
	pa0+=2;
	pa1+=2;
	
	////18e34 beq        R0,T2 ////18eA4                                       
	if(t2)
	{
		while(a3)
		{
			////18e3C mov        T6,T1                             T6=FFFFFFFF:FFFF
//			while(t6)
			{
				////18e40 ldl        A2,$F(A0)                         *(65D2)         
				////18e44 ldr        A2,$8(A0)                         *(65CB)         
				////18e48 addi       T6,T6,$FFFFFFFF                   T6=FFFFFFFF:FFFF
				////18e4C addi       V0,V0,$8                          V0=FFFFFFFF:8944
				////18e50 addi       A0,A0,$8                          A0=0:65CB       
				////18e54 bne        R0,T6 ////18e40                                       
				////18e58 sd         A2,$FFFFFFF8(V0)                  *(8944FF28)     
				memcpy(pv0,pa0,t1*8);

			}
			pv0+=(t1*2);
			pa0+=(t1*2);
			////18e5C add        A0,A0,T3                          A0=0:65F7       
			////18e60 mov        T6,T2                             T6=0:40AE       
			pa0+=(t3/4);
//			while(t6)
			{
				////18e64 ldl        A2,$F(A1)                         *(65AE)         
				////18e68 ldr        A2,$8(A1)                         *(65A7)         
				////18e6C addi       T6,T6,$FFFFFFFF                   T6=0:40AD       
				////18e70 addi       V0,V0,$8                          V0=FFFFFFFF:8944
				////18e74 addi       A1,A1,$8                          A1=0:65A7       
				////18e78 bne        R0,T6 ////18e64                                       
				////18e7C sd         A2,$FFFFFFF8(V0)                  *(8944FF30)     
			memcpy(pv0,pa1,t2*8);
			}
			pv0+=(t2*2);
			pa1+=(t2*2);
			////18e80 addi       A3,A3,$FFFFFFFF                   A3=FFFFFFFF:FFFF
			////18e84 bne        R0,A3 ////18e3C                                       
			////18e88 add        A1,A1,T4                          A1=FFFFFFFF:FFFC
			pa1+=(t4/4);
			a3--;
		}
		r->GPR[V1*2]=v1;
		return;
	}

	while(a3)
	{
		////18eA4 mov        T6,T1                             T6=FFFFFFFF:FFFF
//		while(t6)
		{
			////18eA8 ldl        A2,$F(A0)                         *(6606)         
			////18eAC ldr        A2,$8(A0)                         *(65FF)         
			////18eB0 addi       T6,T6,$FFFFFFFF                   T6=FFFFFFFF:FFFF
			////18eB4 addi       V0,V0,$8                          V0=FFFFFFFF:8944
			////18eB8 addi       A0,A0,$8                          A0=0:65FF       
			////18eBC bne        R0,T6 ////18eA8                                       
			////18eC0 sd         A2,$FFFFFFF8(V0)                  *(8944FF38)     
			memcpy(pv0,pa0,t1*8);
		}
		pv0+=(t1*2);
		pa0+=(t1*2);
		pa0+=(t3/4);
		////18eC4 addi       A3,A3,$FFFFFFFF                   A3=FFFFFFFF:FFFF
		////18eC8 bne        R0,A3 ////18eA4                                       
		////18eCC add        A0,A0,T3                          A0=0:662B       
		a3--;
	}
	r->GPR[V1*2]=v1;
}

void hleSkyAnim()
{
	sDWORD t0,t1,t2,t3,t4,t5,t6,t7;
	sDWORD a1,a2,a3;
	sDWORD s0;
	sDWORD v1=r->GPR[V1*2];

	sDWORD s8=r->GPR[S8*2];

	r->ICount+=15000;
	//c87C lui        T3,$FFFF8808                      T3=88080000     
	//c880 lbu        T3,$622C(T3)                      *(8808622C)     
	BYTE *tmp=hleMakeP(0x8808622c);
	t3=(WORD)*tmp;
	
	//c884 lui        T0,$FFFF8803                      T0=88030000     
	//c888 lb         T0,$4F46(T0)                      *(88034F46)     
	t0=(short)*(char *)hleMakeP(0x88034f46);

	//c88C lh         A3,$18(S8)                        *(88030018)     
	a3=*(short *)hleMakeP(s8+0x18);

	//c890 addi       T3,T3,$1                          T3=FFFFFFFF:8808
	//c894 add        T0,T0,T3                          T0=0:100B0001   
	t3++;
	t0+=t3;

	//c898 lui        A2,$FFFF8808                      A2=88080000     
	//c89C slt        At,T0,A3                                          
	//c8A0 lh         A1,$16(S8)                        *(88030016)     
	//c8A8 lh         A2,$7B18(A2)                      *(88087B18)     
	//c8A4 bne        R0,At c8B0                                        
	a1=*(short *)hleMakeP(s8+0x16);
	a2=*(short *)hleMakeP(0x88087b18);
	if(t0>=a3)
	{
		//c8AC mov        T0,R0                             T0=0:440        
		t0=0;
	}
	//c8B0 lui        At,$FFFF8803                      At=88030000     
	//c8B4 bne        R0,T0 //c8D8                                        
	//c8B8 sb         T0,$4F46(At)                      *(88034F46)     
	tmp=hleMakeP(0x88034f46);
	*tmp=t0&0xff;
	if(!t0)
	{
		//c8BC addi       A2,A2,$1                          A2=FFFFFFFF:8808
		//c8C0 slt        At,A2,A1                                          
		//c8C4 bne        R0,At //c8D0                                        
		//c8c8 NOP
		a2++;
		if(a2>=a1)
		{
			//c8CC mov        A2,R0                             A2=0:440        
			a2=0;
		}
		//c8D0 lui        At,$FFFF8808                      At=88080000     
		//c8D4 sh         A2,$7B18(At)                      *(88087B18)     
		tmp=hleMakeP(0x88087b18);
		*(WORD *)tmp=(WORD)a2;
	}

	//c8D8 lui        A1,$FFFF8860                      A1=88600000     
	//c8DC sll        A2,A2,$2                          A2=0:1100       
	//c8E0 ori        A1,A1,$40                         A1=0:88600040   
	//c8E4 add        A1,A1,A2                          A1=FFFFFFFF:8860
	//c8E8 lh         A2,$12(S8)                        *(88030012)     
	//c8EC lh         S0,$20(S8)                        *(88030020)     
	//c8F0 lw         A1,$0(A1)                         *(88601140)     
	a1=0x88600000;
	a2<<=2;
	a1|=0x40;
	a1+=a2;
	a2=*(short *)hleMakeP(s8+0x12);
	s0=*(short *)hleMakeP(s8+0x20);
	a1=*(sDWORD *)hleMakeP(a1);

	//c8F4 sll        A3,A2,$7                          A3=0:88000      
	//c8F8 sll        A2,A2,$9                          A2=0:220000     
	//c8FC add        A2,A2,A3                          A2=0:2A8000     
	//c900 add        A2,A2,V1                          A2=FFFFFFFF:985C
	//c904 mov        T7,A2                             T7=FFFFFFFF:985C
	//c908 beq        R0,S0 //c920                                        
	//c90C addi       A1,A1,$4                          A1=FFFFFFFF:8860
	a2=v1+a2*640;
	t7=a2;
	a1+=4;
	if(s0)
	{
		//c910 lui        T0,$FFFF8803                      T0=88030000     
		//c914 lb         T0,$4F46(T0)                      *(88034F46)     
		//c918 sll        T0,T0,$2                          T0=0:200C0000   
		//c91C sub        A2,A2,T0                          A2=0:78508487   
		t0=(short)*(char *)hleMakeP(0x88034f46);
		t0<<=2;
		a2-=t0;
	}
	//c920 lh         A3,$14(S8)                        *(88030014)     
	//c924 addi       T0,A2,$27E                        T0=0:78508705   
	//c928 mov        T1,A2                             T1=0:78508487   
	a3=*(short *)hleMakeP(s8+0x14);
	t0=a2+0x27e;
	t1=a2;

	//c92C lbu        T2,$0(A1)                         *(88601144)     
	BYTE *pa1=(BYTE *)hleMakeP(a1);
	WORD *pa2=(WORD *)hleMakeP(a2);
	t2=*(pa1++);
	while(1)
	{
		//c930 lbu        T3,$1(A1)                         *(88601145)     
		//c934 lbu        T4,$2(A1)                         *(88601146)     
		//c938 addi       A1,A1,$3                          A1=FFFFFFFF:8860
		//c93C sll        T3,T3,$8                          T3=0:8000100    
		//c940 add        T3,T3,T4                          T3=0:730F6E87   
		t3=*(pa1++);
		t4=*(pa1++);
		t3<<=8;
		t3+=t4;

		//c944 sh         T3,$0(A2)                         *(78508487)     
		//c948 beq        T0,A2 //c960                                        
		//c94C addi       A2,A2,$2                          A2=0:78508489   
		//c950 bgtz       T2 //c944                                           
		//c954 addi       T2,T2,$FFFFFFFF                   T2=0:13D        
		//c958 jmp        //c930                                              
		//c95C lbu        T2,$0(A1)                         *(88601147)     
		t2++;
		while(t2)
		{
			*(pa2++)=(WORD)t3;
			a2+=2;
			if(t0==a2)
			{
				//c960 addi       T1,T1,$280                        T1=0:78508707   
				//c964 mov        A2,T1                             A2=0:78508707   
				//c968 addi       A3,A3,$FFFFFFFF                   A3=0:87FFF      
				//c970 addi       T0,A2,$27E                        T0=0:78508985   
				//c96C bne        R0,A3 //c950                                        
				t1+=0x280;
				a2=t1;
				pa2=(WORD *)hleMakeP(a2);
				a3--;
				t0=a2+0x280;
				if(a3==0)
				{
					goto IamNotUsingAGotoItJustLooksLikeIt;
				}
			}
			t2--;
		}
		t2=*(pa1++);
	}
IamNotUsingAGotoItJustLooksLikeIt:


	//c974 lh         A2,$1E(S8)                        *(8803001E)     
	//c978 bne        R0,A2 //c988                                        
	//c97C NOP
	//c980 jr         RA                                                
	//c984 NOP
	a2=*(short *)hleMakeP(s8+0x1e);
	if(a2==0)
	{
		return;
	}

	//c988 mov        A2,T7                             A2=FFFFFFFF:985C
	//c98C mov        T1,A2                             T1=FFFFFFFF:985C
	//c990 addi       A2,A2,$FFFFEC00                   A2=FFFFFFFF:985C
	//c994 mov        T2,A2                             T2=FFFFFFFF:985C
	//c998 movi       T6,$4                             T6=0:4          
	a2=t7;
	t1=a2;
	a2+=0xffffec00;
	t2=a2;
	t6=4;

	DWORD *pt1=(DWORD *)hleMakeP(t1);
	DWORD *pt2=(DWORD *)hleMakeP(t2);
	while(t6)
	{
		//c99C movi       T5,$3                             T5=0:3          
		t5=3;
		while(t5)
		{
			//c9A0 addi       T4,T2,$280                        T4=FFFFFFFF:985C
//			while(t4!=t2)
			{
				//c9A4 ld         T3,$0(T1)                         *(985C8487)     
				//c9A8 addi       T2,T2,$8                          T2=FFFFFFFF:985C
				//c9AC addi       T1,T1,$8                          T1=FFFFFFFF:985C
				//c9B0 bne        T4,T2 //c9A4                                        
				//c9B4 sd         T3,$FFFFFFF8(T2)                  *(985C7087)     
				memcpy(pt2,pt1,0x280);
			}
			t2+=0x280;
			pt2+=(0x280/4);
			//c9B8 addi       T5,T5,$FFFFFFFF                   T5=0:2          
			//c9BC bne        R0,T5 //c9A0                                        
			//c9C0 addi       T1,T1,$FFFFFD80                   T1=FFFFFFFF:985C
			t5--;
		}
		//c9C4 addi       T6,T6,$FFFFFFFF                   T6=0:3          
		//c9C8 bne        R0,T6 //c99C                                        
		//c9CC addi       T1,T1,$280                        T1=FFFFFFFF:985C
		t1+=0x280;
		pt1+=(0x280/4);
		t6--;
	}
	//c9D0 lh         A3,$14(S8)                        *(88030014)     
	//c9D4 mov        A2,T7                             A2=FFFFFFFF:985C
	//c9D8 sll        T0,A3,$7                          T0=0:43FFF80    
	//c9DC sll        A3,A3,$9                          A3=0:10FFFE00   
	//c9E0 add        A3,A3,T0                          A3=0:153FFD80   
	//c9E4 add        A2,A3,A2                          A2=FFFFFFFF:AD9C
	//c9E8 addi       A3,A2,$1900                       A3=FFFFFFFF:AD9C
	//c9EC ld         T3,$FFFFFD80(A2)                  *(AD9C7F87)     
	//c9F0 addi       A2,A2,$8                          A2=FFFFFFFF:AD9C
	//c9F4 bne        A3,A2 //c9EC                                        
	//c9F8 sd         T3,$FFFFFFF8(A2)                  *(AD9C8207)     
	a3=*(short *)hleMakeP(s8+0x14);
	a2=t7+a3*640;
	BYTE *dst=hleMakeP(a2);
	BYTE *src=dst-640;
	memcpy(dst,src,0x1900);
}


void hleISR()
{
	DWORD sp=0x8808F470;
	DWORD a0,a1,v0,v1,ra;
	BYTE *bRam=(BYTE *)m->rdRam;
	WORD *wRam=(WORD *)m->rdRam;
	DWORD *dRam=(DWORD *)m->rdRam;
	
	
	r->CPR0[2*13]|=0x400;
	r->CPR0[2*13]^=0x400;
	r->CPR0[2*13]|=0x800;
	r->CPR0[2*13]^=0x800;
	//26754 mf0        V1,T1                                             
	//26758 NOP
	//2675C lui        A0,$C                             A0=C0000        
	//26760 ori        A0,A0,$B735                       A0=0:CB735      
	//26764 add        V1,V1,A0                                          
	//26768 mt0        T3,V1                                             
//	r->CompareCount+=0xcb735;
	r->CompareCount+=0xcb735;
	r->NextIntCount=r->CompareCount;
	
	//2676C NOP
	//26770 lui        V0,$FFFF8809                      V0=88090000     
	//26774 lbu        V0,$FFFFF4D1(V0)                  *(8808F4D1)     
	//26778 addi       V0,V0,$1                          V0=FFFFFFFF:8809
	//2677C slti       At,V0,$3C                                         
	//26780 bne        R0,At 2678C                                       
	v0=bRam[0x8f4d1];
	v0++;
	if(v0>=0x3c)
	{
		//26784 NOP
		//26788 mov        V0,R0                             V0=0:0          
		v0=0;
	}
	//2678C lui        At,$FFFF8809                      At=88090000     
	//26790 bne        R0,V0 267C0                                       
	//26794 sb         V0,$FFFFF4D1(At)                  *(8808F4D1)     
	bRam[0x8f4d1]=v0;
	if(v0==0)
	{
		//26798 lui        V0,$FFFF8804                      V0=88040000     
		//2679C lwu        V0,$FFFFA1EC(V0)                  *(8803A1EC)     
		//267A0 lui        At,$FFFF8804                      At=88040000     
		//267A4 addi       V0,V0,$1                          V0=FFFFFFFF:8804
		//267A8 sw         V0,$FFFFA1EC(At)                  *(8803A1EC)     
		v0=dRam[0x3a1ec/4];
		v0++;
		dRam[0x3a1ec/4]=v0;
		
		//267AC lui        V0,$FFFF8809                      V0=88090000     
		//267B0 lwu        V0,$FFFFF3D8(V0)                  *(8808F3D8)     
		//267B4 lui        At,$FFFF8809                      At=88090000     
		//267B8 addi       V0,V0,$1                          V0=FFFFFFFF:8809
		//267BC sw         V0,$FFFFF3D8(At)                  *(8808F3D8)     
		v0=dRam[0x8f3d8/4];
		v0++;
		dRam[0x8f3d8/4]=v0;
	}
	
	//267C0 lui        V1,$FFFF8808                      V1=88080000     
	//267C4 lb         V1,$6244(V1)                      *(88086244)     
	//267C8 beq        R0,V1 267DC                                       
	//267CC NOP
	BYTE tb=bRam[0x86244];
	if(tb)
	{
		//267D0 addi       V1,V1,$FFFFFFFF                   V1=FFFFFFFF:8807
		//267D4 lui        At,$FFFF8808                      At=88080000     
		//267D8 sb         V1,$6244(At)                      *(88086244)     
		tb--;
		bRam[0x86244]=tb;
	}
	
	//267DC lui        V1,$FFFF8809                      V1=88090000     
	//267E0 lb         V1,$FFFFF4D0(V1)                  *(8808F4D0)     
	//267E4 bne        R0,V1 26BC8                                       
	//267E8 NOP
	tb=bRam[0x8f4d0];
	if(tb==0)
	{
		//267EC lui        V0,$FFFF8804                      V0=88040000     
		//267F0 addi       V0,V0,$FFFFC4C0                   V0=FFFFFFFF:8803
		//267F4 movi       RA,$5                             RA=0:5          
		v0=0x3C4C0;
		ra=5;
		
Label267f8:
		//267F8 lw         A0,$C(V0)                         *(8803C4CC)     
		//267FC lw         A1,$1C(V0)                        *(8803C4DC)     
		//26800 lw         V1,$8(V0)                         *(8803C4C8)     
		//26804 lw         A0,$0(A0)                         *(CB735)        
		//26808 nor        A0,A0,R0                          A0=FFFFFFFF:FFF3
		//2680C and        A0,A0,A1                                          
		//26810 beq        R0,A0 //2681C                                       
		//26814 NOP
		a0=dRam[(v0+0x0c)/4];
		a1=dRam[(v0+0x1c)/4];
		v1=dRam[(v0+0x08)/4];
		a0=dRam[(a0&0x7fffff)/4];
		a0=~a0;
		a0=a0&a1;
		if(a0)
		{
			//26818 movi       A0,$1                             A0=0:1          
			a0=1;
		}
		
		//2681C sll        V1,V1,$1                          V1=0:10120000   
		//26820 or         V1,V1,A0                          V1=0:10120001   
		//26824 andi       A0,V1,$3FFF                       A0=0:1          
		//26828 movi       At,$FFF                           At=0:FFF        
		//2682C mov        A1,R0                             A1=0:0          
		//26830 bne        At,A0 //2683C                                       
		//26834 sw         V1,$8(V0)                         *(8803C4C8)     
		v1<<=1;
		v1=v1|a0;
		a0=v1&0x3fff;
		a1=0;
		dRam[(v0+0x08)/4]=v1;
		if(a0==0xfff)
		{
			//26838 sw         A0,$4(V0)                         *(8803C4C4)     
			dRam[(v0+0x04)/4]=a0;
		}
		
		//2683C andi       A0,V1,$1F                         A0=0:1          
		//26840 movi       At,$10                            At=0:10         
		//26844 bne        At,A0 //2686C                                       
		//26848 NOP
		a0=v1&0x1f;
		if(a0==0x10)
		{
			//2684C lw         A0,$4(V0)                         *(8803C4C4)     
			//26850 bne        R0,A0 //2686C                                       
			//26854 NOP
			a0=dRam[(v0+0x04)/4];
			if(a0==0)
			{
				//26858 lw         A0,$0(V0)                         *(8803C4C0)     
				//2685C beq        R0,A0 //2686C                                       
				//26860 NOP
				a0=dRam[(v0+0x00)/4];
				if(a0)
				{
					//26864 movi       A1,$1                             A1=0:1          
					//26868 sw         R0,$0(V0)                         *(8803C4C0)     
					dRam[(v0+0x00)/4]=0;
					a1=1;
				}
			}
		}
		
		//2686C lw         V1,$8(V0)                         *(8803C4C8)     
		//26870 movi       At,$1                             At=0:1          
		//26874 andi       V1,V1,$F                          V1=0:1          
		//26878 bne        At,V1 //26884                                       
		//2687C NOP
		v1=dRam[(v0+0x08)/4];
		v1&=0xf;
		if(v1==0x1)
		{
			//26880 sw         V1,$0(V0)                         *(8803C4C0)     
			dRam[(v0+0x00)/4]=v1;
		}
		
		//26884 bne        R0,V1 26894                                       
		//26888 NOP
		if(v1==0)
		{
			//2688C sw         R0,$4(V0)                         *(8803C4C4)     
			//26890 sw         R0,$0(V0)                         *(8803C4C0)     
			dRam[(v0+0x04)/4]=0;
			dRam[(v0+0x00)/4]=0;
		}
		
		//26894 beq        R0,A1 26AA0                                       
		//26898 NOP
		if(a1)
		{
			//2689C lui        V1,$FFFF8804                      V1=88040000     
			//268A0 lw         V1,$FFFFA16C(V1)                  *(8803A16C)     
			//268A4 beq        R0,V1 26904                                       
			//268A8 NOP
			v1=dRam[0x3a16c/4];
			if(v1)
			{
				//268AC lui        V1,$FFFF8809                      V1=88090000     
				//268B0 lb         V1,$FFFFF4D2(V1)                  *(8808F4D2)     
				//268B4 beq        R0,V1 268E4                                       
				//268B8 movi       At,$1                             At=0:1          
				v1=bRam[0x8f4d2];
				a1=1;
				if(v1)
				{
					//268BC bne        At,RA 268E4                                       
					//268C0 NOP
					if(ra==1)
					{
						//268C4 lw         V1,$14(V0)                        *(8803C4D4)     
						//268C8 lui        A0,$FFFF8809                      A0=88090000     
						//268CC lh         A0,$FFFFF4C2(A0)                  *(8808F4C2)     
						//268D0 lw         V1,$0(V1)                         *(88090000)     
						//268D4 lui        At,$FFFF8809                      At=88090000     
						//268D8 add        A0,A0,V1                          A0=0:10120000   
						//268DC jmp        26948                                             
						//268E0 sh         A0,$FFFFF4C2(At)                  *(8808F4C2)     
						v1=dRam[(v0+0x14)/4];
						a0=wRam[0x8f4c2/2];
						v1=dRam[(v1&0x7fffff)];
						a0+=v1;
						wRam[0x8f4c2/2]=a0;
						goto Label26948;
					}
				}
				//268E4 lw         V1,$14(V0)                        *(8803C4D4)     
				//268E8 lui        A0,$FFFF8809                      A0=88090000     
				//268EC lh         A0,$FFFFF4C0(A0)                  *(8808F4C0)     
				//268F0 lw         V1,$0(V1)                         *(88090000)     
				//268F4 lui        At,$FFFF8809                      At=88090000     
				//268F8 add        A0,A0,V1                          A0=0:10120000   
				//268FC jmp        26948                                             
				//26900 sh         A0,$FFFFF4C0(At)                  *(8808F4C0)     
				v1=dRam[(v0+0x14)/4];
				a0=wRam[0x8f4c0/2];
				v1=dRam[(v1&0x7fffff)];
				a0+=v1;
				wRam[0x8f4c0/2]=a0;
				goto Label26948;
			}
			//26904 lui        V1,$FFFF8809                      V1=88090000     
			//26908 lb         V1,$FFFFF4D2(V1)                  *(8808F4D2)     
			//2690C beq        R0,V1 26934                                       
			//26910 movi       At,$1                             At=0:1          
			v1=bRam[0x8f4d2];
			a1=1;
			if(v1)
			{
				//26914 bne        At,RA 26934                                       
				//26918 NOP
				if(ra==1)
				{
					//2691C lui        V1,$FFFF8809                      V1=88090000     
					//26920 lh         V1,$FFFFF4C2(V1)                  *(8808F4C2)     
					//26924 lui        At,$FFFF8809                      At=88090000     
					//26928 addi       V1,V1,$1                          V1=FFFFFFFF:8809
					//2692C jmp        //26948                                             
					//26930 sh         V1,$FFFFF4C2(At)                  *(8808F4C2)     
					v1=wRam[0x8f4c2/2];
					v1=dRam[(v1&0x7fffff)];
					v1++;
					wRam[0x8f4c2/2]=v1;
					goto Label26948;
				}
			}
			//26934 lui        V1,$FFFF8809                      V1=88090000     
			//26938 lh         V1,$FFFFF4C0(V1)                  *(8808F4C0)     
			//2693C lui        At,$FFFF8809                      At=88090000     
			//26940 addi       V1,V1,$1                          V1=FFFFFFFF:8809
			//26944 sh         V1,$FFFFF4C0(At)                  *(8808F4C0)     
			v1=wRam[0x8f4c0/2];
			v1=dRam[(v1&0x7fffff)];
			v1++;
			wRam[0x8f4c0/2]=v1;
Label26948:
			//26948 lw         V1,$18(V0)                        *(8803C4D8)     
			//2694C lui        At,$FFFF8809                      At=88090000     
			//26950 lw         A0,$0(V1)                         *(88090001)     
			//26954 addi       A0,A0,$1                          A0=0:10120001   
			//26958 sw         A0,$0(V1)                         *(88090001)     
			v1=dRam[(v0+0x18)/4];
			a0=dRam[(v1&0x7fffff)/4];
			a0++;
			dRam[(v1&0x7fffff)/4]=a0;
			
			//2695C lw         V1,$10(V0)                        *(8803C4D0)     
			//26960 lui        A0,$FFFF8804                      A0=88040000     
			//26964 lw         A0,$FFFFA1A0(A0)                  *(8803A1A0)     
			//26968 lw         V1,$0(V1)                         *(88090001)     
			//2696C sw         A1,$FFFFF4B8(At)                  *(8808F4B8)     
			v1=dRam[(v0+0x18)/4];
			a0=dRam[0x3a1a0/4];
			v1=dRam[(v1&0x7fffff)/4];
			dRam[0x8f4b8/4]=a1;
			
			//26970 lui        A1,$FFFF8809                      A1=88090000     
			//26974 lw         A1,$FFFFF4CC(A1)                  *(8808F4CC)     
			//26978 add        A1,A1,V1                          A1=0:10120001   
			//2697C slt        At,A1,A0                                          
			//26980 beq        R0,At //26990                                       
			//26984 lui        At,$FFFF8809                      At=88090000     
			a1=dRam[0x8f4cc];
			a1+=v1;
			if(a1<a0)
			{
				//26988 jmp        26A1C                                             
				//2698C sw         A1,$FFFFF4CC(At)                  *(8808F4CC)     
				dRam[0x8f4cc]=a1;
				goto Label26a1c;
			}
			
			//26990 lui        A0,$FFFF8804                      A0=88040000     
			//26994 lw         A0,$FFFFA170(A0)                  *(8803A170)     
			//26998 beq        R0,A0 26AA0                                       
			//2699C NOP
			a0=dRam[0x3a170];
			if(a0)
			{
				//269A0 addi       SP,SP,$FFFFFFF8                   SP=FFFFFFFF:8808
				//269A4 sw         RA,$0(SP)                         *(8808F4E8)     
				//269A8 sw         V1,$4(SP)                         *(8808F4EC)     
				sp-=8;
				dRam[((sp+0x00)&0x7fffff)/4]=ra;
				dRam[((sp+0x04)&0x7fffff)/4]=v1;
				
Label269ac:
				//269AC mov        V1,A1                             V1=0:10120001   
				//269B0 sub        V1,V1,A0                          V1=FFFFFFFF:880E
				//269B4 bltz       V1 26A00                                          
				//269B8 NOP
				v1=a1;
				v1-=a0;
				if((int)v1>=0)
				{
					//269BC lui        V1,$FFFF8809                      V1=88090000     
					//269C0 lw         V1,$FFFFF3E8(V1)                  *(8808F3E8)     
					//269C4 lui        At,$FFFF8809                      At=88090000     
					//269C8 sub        A1,A1,A0                          A1=FFFFFFFF:880E
					//269CC addi       V1,V1,$1                          V1=FFFFFFFF:8809
					//269D0 sw         V1,$FFFFF3E8(At)                  *(8808F3E8)     
					v1=dRam[0x8f3e8/4];
					a1-=a0;
					v1++;
					dRam[0x8f3e8/4]=v1;
					
					//269D4 lui        V1,$FFFF8804                      V1=88040000     
					//269D8 lw         V1,$FFFFA1B8(V1)                  *(8803A1B8)     
					//269DC lui        At,$FFFF8804                      At=88040000     
					//269E0 addi       V1,V1,$1                          V1=FFFFFFFF:8804
					//269E4 sw         V1,$FFFFA1B8(At)                  *(8803A1B8)     
					v1=dRam[0x3a1b8/4];
					v1++;
					dRam[0x3a1b8/4]=v1;
					
					//269E8 lui        V1,$FFFF8804                      V1=88040000     
					//269EC lw         V1,$FFFFA1C0(V1)                  *(8803A1C0)     
					//269F0 lui        At,$FFFF8804                      At=88040000     
					//269F4 addi       V1,V1,$1                          V1=FFFFFFFF:8804
					//269F8 jmp        //269AC                                             
					//269FC sw         V1,$FFFFA1C0(At)                  *(8803A1C0)     
					v1=dRam[0x3a1c0/4];
					v1++;
					dRam[0x3a1c0/4]=v1;
					goto Label269ac;
				}
			}
			
			//26a00 lui        At,$FFFF8809                      At=88090000     
			//26a04 sw         A1,$FFFFF4CC(At)                  *(8808F4CC)     
			//26a08 lui        At,$FFFF8809                      At=88090000     
			//26a0C lw         RA,$0(SP)                         *(8808F4E8)     
			//26a10 lw         V1,$4(SP)                         *(8808F4EC)     
			//26a14 sw         A1,$FFFFF3EC(At)                  *(8808F3EC)     
			//26a18 addi       SP,SP,$8                          SP=FFFFFFFF:8808
			dRam[0x8f4cc/4]=a1;
			ra=dRam[((sp+0x00)&0x7fffff)/4];
			v1=dRam[((sp+0x04)&0x7fffff)/4];
			dRam[0x8f3ec/4]=a1;
			sp+=8;
			
Label26a1c:
			//26a1C lui        A1,$FFFF8809                      A1=88090000     
			//26a20 lw         A1,$FFFFF4C8(A1)                  *(8808F4C8)     
			//26a24 add        A1,A1,V1                          A1=0:100D0001   
			//26a28 lui        V1,$FFFF8804                      V1=88040000     
			//26a2C lw         V1,$FFFFA184(V1)                  *(8803A184)     
			//26a30 beq        R0,V1 //26a78                                       
			//26a34 slt        At,A1,V1                                          
			a1=dRam[0x8f4c8/4];
			a1+=v1;
			v1=dRam[0x3a184/4];
			if(v1)
			{
				//26a38 bne        R0,At //26a78                                       
				//26a3C lui        At,$FFFF8809                      At=88090000     
				if(a1>=v1)
				{
					//26a40 lui        V1,$FFFF8809                      V1=88090000     
					//26a44 lw         V1,$FFFFF3E8(V1)                  *(8808F3E8)     
					//26a48 sw         R0,$FFFFF3EC(At)                  *(8808F3EC)     
					//26a4C lui        At,$FFFF8809                      At=88090000     
					//26a50 sw         R0,$FFFFF4CC(At)                  *(8808F4CC)     
					v1=dRam[0x8f3e8/4];
					dRam[0x8f3ec/4]=0;
					dRam[0x8F4CC/4]=0;
					
					//26a54 lui        At,$FFFF8809                      At=88090000     
					//26a58 addi       V1,V1,$1                          V1=FFFFFFFF:8809
					//26a5C sw         V1,$FFFFF3E8(At)                  *(8808F3E8)     
					v1++;
					dRam[0x8f3e8/4]=v1;
					
					//26a60 lui        V1,$FFFF8804                      V1=88040000     
					//26a64 lw         V1,$FFFFA1C0(V1)                  *(8803A1C0)     
					//26a68 lui        At,$FFFF8804                      At=88040000     
					//26a6C mov        A1,R0                             A1=0:0          
					//26a70 addi       V1,V1,$1                          V1=FFFFFFFF:8804
					//26a74 sw         V1,$FFFFA1C0(At)                  *(8803A1C0)     
					v1=dRam[0x3a1c0/4];
					a1=0;
					v1++;
					dRam[0x3a1c0/4]=v1;
				}
			}
			//26a78 lui        V1,$FFFF8809                      V1=88090000     
			//26a7C lui        A0,$FFFF8804                      A0=88040000     
			//26a80 lw         V1,$FFFFF3E8(V1)                  *(8808F3E8)     
			//26a84 lw         A0,$FFFFA15C(A0)                  *(8803A15C)     
			//26a88 lui        At,$FFFF8809                      At=88090000     
			//26a8C sw         A1,$FFFFF4C8(At)                  *(8808F4C8)     
			//26a90 slt        At,A0,V1                                          
			//26a94 beq        R0,At //26aA0                                       
			//26a98 lui        At,$FFFF8809                      At=88090000     
			v1=dRam[0x8f3e8/4];
			a0=dRam[0x3a15c/4];
			dRam[0x8f4c8/4]=a1;
			if(a0<v1)
			{
				//26a9C sw         A0,$FFFFF3E8(At)                  *(8808F3E8)     
				dRam[0x8f3e8/4]=a0;
			}
		}
		//26aA0 addi       RA,RA,$FFFFFFFF                   RA=0:4          
		//26aA4 bne        R0,RA //267F8                                       
		//26aA8 addi       V0,V0,$20                         V0=FFFFFFFF:8803
		ra--;
		v0+=0x20;
		if(ra)
			goto Label267f8;
		
		//26aAC lui        V1,$FFFFB000                      V1=B0000000     
		//26aB0 lw         V1,$88(V1)                        *(B0000088)     
		//26aB4 andi       A0,V1,$8000                       A0=0:0          
		//26aB8 beq        R0,A0 26BC8                                       
		//26aBC NOP
		v1=inputs[0];
		v1|=(inputs[1]<<16);
//		if(!(v1&0x8000))	//ie; door NOT open
		if(1)
		{
			//26aC0 lui        A0,$FFFF8809                      A0=88090000     
			//26aC4 lw         A0,$FFFFF4BC(A0)                  *(8808F4BC)     
			//26aC8 lui        At,$FFFF8809                      At=88090000     
			//26aCC sw         V1,$FFFFF4BC(At)                  *(8808F4BC)     
			a0=dRam[0x8f4bc/4];
			dRam[0x8f4bc/4]=v1;
			
			//26aD0 xori       V1,V1,$FFFF                       V1=0:B000FFFF   
			//26aD4 and        V1,V1,A0                          V1=FFFFFFFF:8000
			//26aD8 andi       A0,V1,$2000                       A0=0:0          
			//26aDC beq        R0,A0 26B44                                       
			//26aE0 NOP
			v1^=0xffff;
			v1&=a0;
			a0=v1&0x2000;
			if(a0)		//service button (gives a credit)
			{
				//26aE4 lui        V1,$FFFF8809                      V1=88090000     
				//26aE8 lui        A0,$FFFF8804                      A0=88040000     
				//26aEC lw         V1,$FFFFF3E8(V1)                  *(8808F3E8)     
				//26aF0 lw         A0,$FFFFA15C(A0)                  *(8803A15C)     
				//26aF4 beq        A0,V1 26B44                                       
				//26aF8 NOP
				v1=dRam[0x8f3e8/4];
				a0=dRam[0x3a15c/4];
				if(a0!=v1)
				{
					//26aFC addi       V1,V1,$1                          V1=FFFFFFFF:8809
					//26b00 lui        At,$FFFF8809                      At=88090000     
					//26b04 sw         V1,$FFFFF3E8(At)                  *(8808F3E8)     
					v1++;
					dRam[0x8f3e8/4]=v1;
					
					//26b08 lui        V1,$FFFF8804                      V1=88040000     
					//26b0C lw         V1,$FFFFA1BC(V1)                  *(8803A1BC)     
					//26b10 lui        At,$FFFF8804                      At=88040000     
					//26b14 addi       V1,V1,$1                          V1=FFFFFFFF:8804
					//26b18 sw         V1,$FFFFA1BC(At)                  *(8803A1BC)     
					v1=dRam[0x3a1bc/4];
					v1++;
					dRam[0x3a1bc/4]=v1;
					
					//26b1C lui        V1,$FFFF8804                      V1=88040000     
					//26b20 lw         V1,$FFFFA1C0(V1)                  *(8803A1C0)     
					//26b24 lui        At,$FFFF8804                      At=88040000     
					//26b28 addi       V1,V1,$1                          V1=FFFFFFFF:8804
					//26b2C sw         V1,$FFFFA1C0(At)                  *(8803A1C0)     
					v1=dRam[0x3a1c0/4];
					v1++;
					dRam[0x3a1c0/4]=v1;
					
					//26b30 movi       V1,$1                             V1=0:1          
					//26b34 lui        At,$FFFF8809                      At=88090000     
					//26b38 sw         V1,$FFFFF4B8(At)                  *(8808F4B8)     
					//26b3C lui        At,$FFFF8809                      At=88090000     
					//26b40 sw         V1,$FFFFF4BC(At)                  *(8808F4BC)     
					dRam[0x8f4b8/4]=1;
					dRam[0x8f4bc/4]=1;
				}
			}
			//26b44 lui        V1,$FFFFB000                      V1=B0000000     
			//26b48 lw         V1,$80(V1)                        *(B0000080)     
			//26b4C andi       V1,V1,$1000                       V1=0:0          
			//26b50 bne        R0,V1 //26b64                                       
			//26b54 NOP
			v1=inputs[1];
			if(!(v1&0x1000))	//testmode
			{
				//26b58 movi       V1,$1                             V1=0:1          
				//26b5C lui        At,$FFFF8804                      At=88040000     
				//26b60 sw         V1,$FFFF9AF0(At)                  *(88039AF0)     
				dRam[0x39af0/4]=1;
			}
			
			//26b64 lui        V1,$FFFFB000                      V1=B0000000     
			//26b68 lw         V1,$90(V1)                        *(B0000090)     
			//26b6C andi       A0,V1,$8                          A0=0:0          
			//26b70 bne        R0,A0 //26b84                                       
			//26b74 NOP
			v1=inputs[2];
			a0=v1&0x08;
			if(a0==0) //volume down
			{
				//26b78 movi       V1,$1                             V1=0:1          
				//26b7C lui        At,$FFFF8809                      At=88090000     
				//26b80 sw         V1,$FFFFF3F4(At)                  *(8808F3F4)     
				dRam[0x8f3f4/4]=1;
			}
			
			//26b84 lui        V1,$FFFFB000                      V1=B0000000     
			//26b88 lw         V1,$90(V1)                        *(B0000090)     
			//26b8C andi       A0,V1,$4                          A0=0:0          
			//26b90 bne        R0,A0 //26bA4                                       
			//26b94 NOP
			v1=inputs[2];
			a0=v1&0x04;
			if(a0==0) //volume up
			{
				//26b98 movi       V1,$1                             V1=0:1          
				//26b9C lui        At,$FFFF8809                      At=88090000     
				//26bA0 sw         V1,$FFFFF3F4(At)                  *(8808F3F4)     
				dRam[0x8f3f4/4]=1;
			}
			
			//26bA4 lui        V1,$FFFF8809                      V1=88090000     
			//26bA8 lw         V1,$FFFFF4B8(V1)                  *(8808F4B8)     
			//26bAC beq        R0,V1 //26bC8                                       
			//26bB0 lui        At,$FFFF8809                      At=88090000     
			v1=dRam[0x8f4b8/4];
			if(v1)
			{
				//26bB4 sw         R0,$FFFFF4B8(At)                  *(8808F4B8)     
				//26bB8 lui        At,$FFFF8809                      At=88090000     
				//26bBC sb         V1,$FFFFF3F0(At)                  *(8808F3F0)     
				//26bC0 jal        3E08                                              
				//26bC4 movi       A0,$D7D                           A0=0:D7D        
				dRam[0x8f4b8/4]=0;
				bRam[0x8f3f0]=v1;
				a0=0xd7d;
				hleDSPCommand(0xd7d);
			}
		}
	}
	//26bC8 lui        V1,$FFFF8809                      V1=88090000     
	//26bCC lb         V1,$FFFFF4C5(V1)                  *(8808F4C5)     
	//26bD0 bne        R0,V1 26C44                                       
	//26bD4 NOP
	v1=bRam[0x8f4c5];
	if(v0==0)
	{
		//26bD8 lui        V1,$FFFF8809                      V1=88090000     
		//26bDC lh         V1,$FFFFF4C0(V1)                  *(8808F4C0)     
		//26bE0 lui        A0,$FFFF8809                      A0=88090000     
		//26bE4 lb         A0,$FFFFF4C4(A0)                  *(8808F4C4)     
		//26bE8 beq        R0,V1 26C20                                       
		//26bEC NOP
		v1=wRam[0x8f4c0/2];
		a0=bRam[0x8f4c4];
		if(v1)
		{
			//26bF0 bne        R0,A0 26C20                                       
			//26bF4 NOP
			if(a0==0)
			{
				//26bF8 addi       V1,V1,$FFFFFFFF                   V1=FFFFFFFF:8808
				//26BFC lui        At,$FFFF8809                      At=88090000     
				//26c00 sh         V1,$FFFFF4C0(At)                  *(8808F4C0)     
				v1--;
				wRam[0x8f4c0/2]=v1;
				
				//26c04 movi       A0,$10                            A0=0:10         
				//26c08 lui        At,$FFFF8809                      At=88090000     
				//26c0C sb         A0,$FFFFF4C4(At)                  *(8808F4C4)     
				bRam[0x8f4c4]=0x10;
				
				//26c10 movi       A0,$1                             A0=0:1          
				//26c14 lui        At,$FFFFB000                      At=B0000000     
				//26c18 jmp        //26c44                                             
				//26c1C sw         A0,$B0(At)                        *(B00000B0)     
				hleUpdateScreen(1);
				goto Label26c44;
			}
		}
		//26c20 beq        R0,A0 //26c44                                       
		//26c24 NOP
		if(a0)
		{
			//26c28 addi       A0,A0,$FFFFFFFF                   A0=0:0          
			//26c2C lui        At,$FFFF8809                      At=88090000     
			//26c30 sb         A0,$FFFFF4C4(At)                  *(8808F4C4)     
			//26c34 slti       At,A0,$9                                          
			//26c38 beq        R0,At //26c44                                       
			//26c3C lui        At,$FFFFB000                      At=B0000000     
			a0--;
			bRam[0x8f4c4]=a0;
			if(a0<9)
			{
				//26c40 sw         R0,$B0(At)                        *(B00000B0)     
				hleUpdateScreen(0);
			}
		}
	}
Label26c44:
	//26c44 lui        V1,$FFFF8809                      V1=88090000     
	//26c48 lb         V1,$FFFFF4C4(V1)                  *(8808F4C4)     
	//26c4C bne        R0,V1 //26cC0                                       
	//26c50 NOP
	v1=bRam[0x8f4c4];
	if(v1==0)
	{
		//26c54 lui        V1,$FFFF8809                      V1=88090000     
		//26c58 lh         V1,$FFFFF4C2(V1)                  *(8808F4C2)     
		//26c5C lui        A0,$FFFF8809                      A0=88090000     
		//26c60 lb         A0,$FFFFF4C5(A0)                  *(8808F4C5)     
		//26c64 beq        R0,V1 //26c9C                                       
		//26c68 NOP
		v1=wRam[0x8f4c2/2];
		a0=bRam[0x8f4c5];
		if(v1)
		{
			//26c6C bne        R0,A0 //26c9C                                       
			//26c70 NOP
			if(a0==0)
			{
				//26c74 addi       V1,V1,$FFFFFFFF                   V1=FFFFFFFF:8808
				//26c78 lui        At,$FFFF8809                      At=88090000     
				//26c7C sh         V1,$FFFFF4C2(At)                  *(8808F4C2)     
				//26c80 movi       A0,$10                            A0=0:10         
				//26c84 lui        At,$FFFF8809                      At=88090000     
				//26c88 sb         A0,$FFFFF4C5(At)                  *(8808F4C5)     
				//26c8C movi       A0,$2                             A0=0:2          
				//26c90 lui        At,$FFFFB000                      At=B0000000     
				//26c94 jmp        //26cC0                                             
				//26c98 sw         A0,$B0(At)                        *(B00000B0)     
				v1--;
				wRam[0x8f4c2/2]=v1;
				bRam[0x8f4c5]=0x10;
				hleUpdateScreen(2);
				goto Label26cc0;
			}
		}
		//26c9C beq        R0,A0 //26cC0                                       
		//26cA0 NOP
		if(a0)
		{
			//26cA4 addi       A0,A0,$FFFFFFFF                   A0=0:1          
			//26cA8 lui        At,$FFFF8809                      At=88090000     
			//26cAC sb         A0,$FFFFF4C5(At)                  *(8808F4C5)     
			//26cB0 slti       At,A0,$9                                          
			//26cB4 beq        R0,At //26cC0                                       
			//26cB8 lui        At,$FFFFB000                      At=B0000000     
			a0--;
			bRam[0x8f4c5]=a0;
			if(a0<9)
			{
				//26cBC sw         R0,$B0(At)                        *(B00000B0)     
				hleUpdateScreen(0);
			}
		}
	}
	
Label26cc0:
	
	//26cC0 addi       SP,SP,$40                         SP=FFFFFFFF:8808
	//26cC4 ld         A1,$FFFFFFD0(SP)                  *(8808F500)     
	//26cC8 ld         At,$0(SP)                         *(8808F530)     
	//26cCC ld         V0,$FFFFFFF8(SP)                  *(8808F528)     
	//26cD0 mthi       R0                                                
	//26cD4 ld         A1,$FFFFFFC8(SP)                  *(8808F4F8)     
	//26cD8 ld         V1,$FFFFFFF0(SP)                  *(8808F520)     
	//26cDC ld         A0,$FFFFFFE8(SP)                  *(8808F518)     
	//26cE0 mtlo       R0                                                
	//26cE4 ld         A1,$FFFFFFE0(SP)                  *(8808F510)     
	//26cE8 ld         RA,$FFFFFFD8(SP)                  *(8808F508)     
	//26cEC ld         SP,$1F0(R0)                       *(1F0)          
	return;
}

void hleUpdateScreen(WORD page)
{
 
}

void hleDSPCommand(WORD command)
{
	WORD v1;
	BYTE *bRam=(BYTE *)m->rdRam;

	//3d08 slti       At,A0,$FA0                                        
	//3d0C bne        R0,At //3d24                                        
	//3d10 NOP
	if(command>=0xfa0)
	{
		//3d14 lui        V1,$FFFF8808                      V1=88080000     
		//3d18 lbu        V1,$622B(V1)                      *(8808622B)     
		//3d1C bne        R0,V1 3F5C                                        
		//3d20 NOP
		v1=bRam[0x8622b];
		if(v1)
			return;
	}
	//3d24 lui        V1,$FFFF8809                      V1=88090000     
	//3d28 lbu        V1,$FFFFF5E2(V1)                  *(8808F5E2)     
	//3d2C bne        R0,V1 3F5C                                        
	//3d30 NOP
	v1=bRam[0x8f5e2];
	if(v1)
		return;

	//3d34 lui        V1,$FFFF8808                      V1=88080000     
	//3d38 lbu        V1,$6244(V1)                      *(88086244)     
	//3d3C bne        R0,V1 3F5C                                        
	//3d40 NOP
	v1=bRam[0x86244];
	if(v1)
		return;

	//3d44 mf0        V1,T4            TAGHI            V1=0:0          
	//3d48 NOP
	//3d4C ori        V1,V1,$1                          V1=0:1          
	//3d50 xori       V1,V1,$1                          V1=0:0          
	//3d54 mt0        T4,V1            TAGHI            T4=FFFFFFFF:8801
	//3d58 NOP
	//3d5C NOP
	r->CPR0[2*CAUSE] |= 0x1;
	r->CPR0[2*CAUSE] ^= 0x1;

	//3d60 lui        At,$16                            At=160000       
	//3d64 ori        At,At,$E360                       At=0:16E360     
	//3d68 lui        V1,$FFFFB000                      V1=B0000000     
	//3d6C lb         V1,$90(V1)                        *(B0000090)     
	//3d70 andi       V1,V1,$2                          V1=0:0          
	//3d74 bne        R0,V1 //3d8C                                        
	//3d78 addi       At,At,$FFFFFFFF                   At=0:16E35F     
	//3d7C bne        R0,At //3d68                                        
	//3d80 NOP
	//3d84 jmp        3F64                                              
	//3d88 NOP
	//delay

	//3d8C srl        V1,A0,$8                          V1=0:82         
	//3d90 lui        At,$FFFFB000                      At=B0000000     
	//3d94 sb         V1,$98(At)                        *(B0000098)     
	//3d98 movi       V1,$1                             V1=0:1          
	//3d9C lui        At,$FFFFB000                      At=B0000000     
	//3dA0 sb         V1,$90(At)                        *(B0000090)     
	//3dA4 movi       V1,$578                           V1=0:578        
	//3dA8 addi       V1,V1,$FFFFFFFF                   V1=0:577        
	//3dAC bne        R0,V1 //3dA8                                        
	//3dB0 NOP
	//would send high byte

	//3dB4 movi       V1,$3                             V1=0:3          
	//3dB8 lui        At,$FFFFB000                      At=B0000000     
	//3dBC sb         V1,$90(At)                        *(B0000090)     
	//3dC0 movi       V1,$C8                            V1=0:C8         
	//3dC4 addi       V1,V1,$FFFFFFFF                   V1=0:C7         
	//3dC8 bne        R0,V1 //3dC4                                        
	//3dCC NOP
	//delay

	//3dD0 lui        At,$16                            At=160000       
	//3dD4 ori        At,At,$E360                       At=0:16E360     
	//3dD8 lui        V1,$FFFFB000                      V1=B0000000     
	//3dDC lb         V1,$90(V1)                        *(B0000090)     
	//3dE0 andi       V1,V1,$2                          V1=0:0          
	//3dE4 bne        R0,V1 //3dFC                                        
	//3dE8 addi       At,At,$FFFFFFFF                   At=0:16E35F     
	//3dEC bne        R0,At 3ED8                                        
	//3EF0 NOP
	//3EF4 jmp        3F64                                              
	//3EF8 NOP
	//huge delay

	//3EFC movi       V1,$C8                            V1=0:C8         
	//3F00 addi       V1,V1,$FFFFFFFF                   V1=0:C7         
	//3F04 bne        R0,V1 //3F00                                        
	//3F08 NOP
	//delay

	//3F0C lui        At,$FFFFB000                      At=B0000000     
	//3F10 sb         A0,$98(At)                        *(B0000098)     
	//send low byte

	//3F14 movi       V1,$1                             V1=0:1          
	//3F18 lui        At,$FFFFB000                      At=B0000000     
	//3F1C sb         V1,$90(At)                        *(B0000090)     
	//3F20 movi       V1,$578                           V1=0:578        
	//3F24 addi       V1,V1,$FFFFFFFF                   V1=0:577        
	//3F28 bne        R0,V1 //3F24                                        
	//3F2C NOP
	//3F30 movi       V1,$3                             V1=0:3          
	//3F34 lui        At,$FFFFB000                      At=B0000000     
	//3F38 sb         V1,$90(At)                        *(B0000090)     
	//3F3C movi       V1,$C8                            V1=0:C8         
	//3F40 addi       V1,V1,$FFFFFFFF                   V1=0:C7         
	//3F44 bne        R0,V1 //3F40                                        
	//3F48 NOP
	//delay

	//3F4C mf0        V1,T4            TAGHI            V1=0:0          
	//3F50 NOP
	//3F54 ori        V1,V1,$1                          V1=0:1          
	//3F58 mt0        T4,V1            TAGHI            T4=FFFFFFFF:8801
	//3F5C jr         RA                                                
	//3F60 NOP
	r->CPR0[2*CAUSE] |= 0x1;
	dspPeek=(command>>8)&0xff;
	hleDSPIntService();
	dspPeek=command&0xff;
	hleDSPIntService();
}

void hleSendDSPCommand()
{
	//a0, which is register 4
	hleDSPCommand(r->GPR[4*2]&0xffff);
}

void hleFMV1()
{
	svcOutputDebugString("hleFMV1()",20);
	
	//32818 addi       SP,SP,$FFFFFFF8                   SP=0:10         
	//3281C sd         RA,$0(SP)                         *(10)           
	short *s0=(short *)hleMakeP(r->GPR[S0*2]);
	short *src=s0;
	short *dst=s0;
	dst+=0x40;

	BYTE i = 0;

	for(i=0;i<7;i++)
	{
		hleFMVSub1(src,dst);
		src++;
		dst+=0x08;
	}

	src=s0;
	dst=s0;
	src+=0x40;

	for(i=0;i<7;i++)
	{
		hleFMVSub1(src,dst);
		src++;
		dst+=0x08;
	}
}


void hleFMVSub1(short *src,short *dst)
{
	int t1,t2,t3,t4,t5,t6,t7,s3;
	int at,s5,t8,s6,s7,s4,t9,s8;

	svcOutputDebugString("hleFMVSub1()",20);
	
	t1=*src;
	src+=16;
	t2=*src;
	src+=16;
	t3=*src;
	src+=16;
	t4=*src;
	src+=16;
	t5=*src;
	src+=16;
	t6=*src;
	src+=16;
	t7=*src;
	src+=16;
	s3=*src;
/*
	t1=r->GPR[T1*2];
	t2=r->GPR[T2*2];
	t3=r->GPR[T3*2];
	t4=r->GPR[T4*2];
	t5=r->GPR[T5*2];
	t6=r->GPR[T6*2];
	t7=r->GPR[T7*2];
	s3=r->GPR[S3*2];
*/

	//32330 sll        At,T1,$3                          At=0:0          
	//32334 sub        T1,At,T1                          T1=0:0          
	//32338 sll        At,At,$1                          At=0:0          
	//3233C add eq     T1,At                             T1=0:0          
//	t1=t1*23;
	at=t1<<3;
	t1=at-t1;
	at<<=1;
	t1+=at;

	//32340 sll        At,T2,$3                          At=0:4019C600   
	//32344 sll        T2,T2,$1                          T2=0:10067180   
	//32348 sub        At,At,T2                          At=0:30135480   
	//3234C sll        T2,T2,$3                          T2=FFFFFFFF:8033
//	at=t2*6;
//	t2=t2*16;
	at=t2<<3;
	t2<<=1;
	at-=t2;
	t2<<=3;

	//32350 add        S5,T1,T2                          S5=FFFFFFFF:8033
	//32354 sub        T8,T1,T2                          T8=0:7FCC7400   
	//32358 sll        T2,T2,$1                          T2=0:671800     
	//3235C add        S6,T1,At                          S6=0:30135480   
	//32360 sub        S7,T1,At                          S7=FFFFFFFF:CFEC
	//32364 add        S4,T1,T2                          S4=0:671800     
	//32368 sub        T9,T1,T2                          T9=FFFFFFFF:FF98
	//3236C sub        At,T2,At                          At=FFFFFFFF:D053
	//32370 add        T2,T1,At                          T2=FFFFFFFF:D053
	//32374 beq        R0,T3 //323B0                                       
	//32378 sub        T1,T1,At                          T1=0:2FAC3C80   
	s5=t1+t2;
	t8=t1-t2;
	t2<<=1;
	s6=t1+at;
	s7=t1-at;
	s4=t1+t2;
	t9=t1-t2;
	at=t2-at;
	t2=t1+at;
	t1=t1-at;
	if(t3)
	{
		//3237C sll        At,T3,$1                          At=0:3C         
		//32380 add        At,T3,At                          At=0:5A         
		//32384 sll        T3,T3,$5                          T3=0:3C0        
		//32388 sub        T3,T3,At                          T3=0:366        
		//3238C sll        At,At,$2                          At=0:168        
		//32390 add eq     S4,T3                             S4=0:671B66     
		//32394 add eq     T2,At                             T2=FFFFFFFF:D053
		//32398 sub        S5,S5,At                          S5=FFFFFFFF:8033
		//3239C sub        S6,S6,T3                          S6=0:3013511A   
		//323A0 sub        S7,S7,T3                          S7=FFFFFFFF:CFEC
		//323A4 sub        T8,T8,At                          T8=0:7FCC7298   
		//323A8 add eq     T1,At                             T1=0:2FAC3DE8   
		//323AC add eq     T9,T3                             T9=FFFFFFFF:FF98
//		at=t3*12;
//		t3=t3*29;
		at=t3<<1;
		at+=t3;
		t3<<=5;
		t3-=at;
		at<<=2;

		s4+=t3;
		t2+=at;
		s5-=at;
		s6-=t3;
		s7-=t3;
		t8-=at;
		t1+=at;
		t9+=t3;
	}
	//323B0 beq        R0,T4 //323EC                                       
	if(t4)
	{
		//323B4 sll        At,T4,$1                          At=0:3C         
		//323B8 sll        T4,T4,$3                          T4=0:F0         
		//323BC sub        At,T4,At                          At=0:B4         
		//323C0 sll        T4,T4,$1                          T4=0:1E0        
//		t4=t4*16;
//		at=t4*6;
		at=t4<<1;
		t4<<=3;
		at=t4-at;
		t4<<=1;

		//323C4 sub        S6,S6,T4                          S6=0:30134F3A   
		//323C8 add eq     S7,T4                             S7=FFFFFFFF:CFEC
		//323CC sll        T4,T4,$1                          T4=0:3C0        
		//323D0 sub        S5,S5,T4                          S5=FFFFFFFF:8033
		s6-=t4;
		s7+=t4;
		t4<<=1;
		s5-=t4;

		//323D4 add eq     T8,T4                             T8=0:7FCC7658   
		//323D8 sub        T4,T4,At                          T4=0:30C        
		//323DC sub        T2,T2,At                          T2=FFFFFFFF:D053
		//323E0 add eq     T1,At                             T1=0:2FAC3E9C   
		//323E4 add eq     S4,T4                             S4=0:671E72     
		//323E8 sub        T9,T9,T4                          T9=FFFFFFFF:FF98
		t8+=t4;
		t4-=at;
		t2-=at;
		t1+=at;
		s4+=t4;
		t9-=t4;
	}
	//323EC beq        R0,T5 //3241C                                       
	if(t5)
	{
		//323F0 sll        At,T5,$4                          At=0:0          
		//323F4 sll        T5,T5,$3                          T5=0:0          
		//323F8 add eq     At,T5                             At=0:0          
		at=t5<<4;
		t5<<=3;
		at+=t5;

		//323FC add eq     S4,At                             S4=0:671E72     
		//32400 sub        T2,T2,At                          T2=FFFFFFFF:D053
		//32404 sub        S5,S5,At                          S5=FFFFFFFF:8033
		//32408 add eq     S6,At                             S6=0:30134F3A   
		//3240C add eq     S7,At                             S7=FFFFFFFF:CFEC
		//32410 sub        T8,T8,At                          T8=0:7FCC7658   
		//32414 sub        T1,T1,At                          T1=0:2FAC3E9C   
		//32418 add eq     T9,At                             T9=FFFFFFFF:FF98
		s4+=at;
		t2-=at;
		s5-=at;
		s6+=at;
		s7+=at;
		t8-=at;
		t1-=at;
		t9+=at;
	}
	//3241C beq        R0,T6 //32450                                       
	if(t6)
	{
		//32420 sll        At,T6,$4                          At=0:0          
		//32424 add eq     S4,At                             S4=0:671E72     
		//32428 sub        T9,T9,At                          T9=FFFFFFFF:FF98
		//3242C sll        At,T6,$2                          At=0:0          
		//32430 sll        T6,T6,$5                          T6=0:0          
		at=t6<<4;
		s4+=at;
		t9-=at;
		at=t6<<2;
		t6<<=5;

		//32434 add eq     S5,At                             S5=FFFFFFFF:8033
		//32438 sub        T8,T8,At                          T8=0:7FCC7658   
		//3243C sub        At,T6,At                          At=0:0          
		//32440 sub        T2,T2,T6                          T2=FFFFFFFF:D053
		//32444 add eq     T1,T6                             T1=0:2FAC3E9C   
		//32448 add eq     S6,At                             S6=0:30134F3A   
		//3244C sub        S7,S7,At                          S7=FFFFFFFF:CFEC
		s5+=at;
		t8-=at;
		at=t6-at;
		t2-=t6;
		t1+=t6;
		s6+=at;
		s7-=at;

	}
	//32450 beq        R0,T7 //32484                                       
	if(t7)
	{
		//32454 sll        At,T7,$4                          At=0:0          
		//32458 sll        T7,T7,$1                          T7=0:0          
		//3245C sub        At,At,T7                          At=0:0          
		//32460 sll        T7,At,$1                          T7=0:0          
		//32464 add eq     S4,At                             S4=0:671E72     
		at=t7<<4;
		t7<<=1;
		at-=t7;
		t7=at<<1;
		s4+=at;

		//32468 sub        T2,T2,T7                          T2=FFFFFFFF:D053
		//3246C add eq     S5,T7                             S5=FFFFFFFF:8033
		//32470 sub        S6,S6,At                          S6=0:30134F3A   
		//32474 sub        S7,S7,At                          S7=FFFFFFFF:CFEC
		//32478 add eq     T8,T7                             T8=0:7FCC7658   
		//3247C sub        T1,T1,T7                          T1=0:2FAC3E9C   
		//32480 add eq     T9,At                             T9=FFFFFFFF:FF98
		t2-=t7;
		s5+=t7;
		s6-=at;
		s7-=at;
		t8+=t7;
		t1-=t7;
		t9+=at;
	}
	//32484 beq        R0,S3 //324B8                                       
	if(s3)
	{
		//32488 sll        At,S3,$4                          At=FFFFFFFF:8040
		//3248C sub        T2,T2,At                          T2=0:5012CC14   
		//32490 add eq     T1,At                             T1=FFFFFFFF:AFED
		//32494 sll        At,S3,$2                          At=0:20103E08   
		//32498 sll        S3,S3,$5                          S3=0:81F040     
		//3249C add eq     S4,At                             S4=0:20775C7A   
		at=s3<<4;
		t2-=at;
		t1+=at;
		at=s3<<2;
		s3<<=5;
		s4+=at;

		//324A0 sub        T9,T9,At                          T9=FFFFFFFF:DF88
		//324A4 sub        At,S3,At                          At=FFFFFFFF:E071
		//324A8 sub        S6,S6,S3                          S6=0:2F915EFA   
		//324AC add eq     S7,S3                             S7=FFFFFFFF:D06E
		//324B0 add eq     S5,At                             S5=0:60A53910   
		//324B4 sub        T8,T8,At                          T8=FFFFFFFF:9F5A
		t9-=at;
		at=s3-at;
		s6-=s3;
		s7+=s3;
		s5+=at;
		t8-=at;
	}
	//324B8 sra        S4,S4,$6                          S4=0:81DD71     
	//324BC sra        T2,T2,$6                          T2=0:1404B30    
	//324C0 sra        S5,S5,$6                          S5=0:18294E4    
	//324C4 sra        S6,S6,$6                          S6=0:BE457B     
	//324C8 sra        S7,S7,$6                          S7=FFFFFFFF:FF41
	//324CC sra        T8,T8,$6                          T8=FFFFFFFF:FE7D
	//324D0 sra        T1,T1,$6                          T1=FFFFFFFF:FEBF
	//324D4 jr         RA                                                
	//324D8 sra        T9,T9,$6                          T9=FFFFFFFF:FF7E
	s4>>=6;
	t2>>=6;
	s5>>=6;
	s6>>=6;
	s7>>=6;
	t8>>=6;
	t1>>=6;
	t9>>=6;

	*(dst++)=s4;
	*(dst++)=t2;
	*(dst++)=s5;
	*(dst++)=s6;
	*(dst++)=s7;
	*(dst++)=t8;
	*(dst++)=t1;
	*(dst++)=t9;
/*
	r->GPR[S4*2]=s4;
	r->GPR[T2*2]=t2;
	r->GPR[S5*2]=s5;
	r->GPR[S6*2]=s6;
	r->GPR[S7*2]=s7;
	r->GPR[T8*2]=t8;
	r->GPR[T1*2]=t1;
	r->GPR[T9*2]=t9;
*/
}

void hleFMVSub2Patch()
{
	DWORD v0,s1;
	int s2;
	QWORD s0;
	
	svcOutputDebugString("hleFMVSub2Patch()",20);

	v0=r->GPR[V0*2];
	s0=*(QWORD *)&r->GPR[S0*2];
	s1=r->GPR[S1*2];
	s2=r->GPR[S2*2];

	hleFMVSub2(&v0,&s0,&s1,&s2);

	r->GPR[V0*2]=v0;
	*(QWORD *)&r->GPR[S0*2]=s0;
	r->GPR[S1*2]=s1;
	r->GPR[S2*2]=s2;
}

void hleFMVSub2(DWORD *v0,QWORD *s0,DWORD *s1,int *s2)
{
	DWORD a1;
	int a0;
	BYTE *src1;
	src1=(BYTE *)hleMakeP((*v0));


	while(1)
	{
		//3228C lb         A0,$0(V0)                         *(FF)           
		//32290 addi       V0,V0,$1                          V0=0:100        
		//32294 bgtz       A0 //322B4                                          
		//32298 andi       A1,S0,$1                    
		a0=(int)(short)(char)*(src1++);
		(*v0)++;
		a1=(DWORD)(*s0)&1;
		if(a0<=0)
		{
			//3229C beq        R0,A0 //322E0                                       
			//322A0 andi       A0,A0,$7F                         A0=0:8          
			if(a0==0)
				return;
			a0&=0x7f;

			//322A4 sll        A0,A0,$8                          A0=0:800        
			//322A8 lbu        At,$0(V0)                         *(100)          
			//322AC add eq     A0,At                             A0=0:7C8        
			//322B0 addi       V0,V0,$1                          V0=0:101        
			a0<<=8;
			a0+=*(src1++);
			(*v0)++;
		}

		//322B4 addi       S2,S2,$1                          S2=FFFFFFFF:FFFF
		//322B8 blez       S2 //322D4                                          
		//322BC dsrl       S0,S0,$1                                          
		(*s2)++;
		(*s0)>>=1;
		if((*s2)>0)
		{
			//322C0 ldl        S0,$7(S1)                         *(10D)          
			//322C4 ldr        S0,$0(S1)                         *(106)          
			//322C8 addi       S1,S1,$7                          S1=0:10D        
			//322CC dsrlv      S0,S0,S2                                          
			//322D0 addi       S2,S2,$FFFFFFC8                   S2=FFFFFFFF:FFFF
			(*s0)=*(QWORD *)hleMakeP((*s1));
			(*s1)+=7;
			(*s0)>>=(*s2);
			(*s2)-=56;
		}
		//322D4 bne        R0,A1 //3228C                                       
		//322D8 beq        R0,A1 //3228C                                       
		//322DC add eq     V0,A0                             V0=0:8C9        
		if(a1==0)
		{
			(*v0)+=a0;
			src1+=a0;
		}
	}
	//322E0 jr         RA                                                
	//322E4 NOP
}


void hleFMV2()
{
	DWORD t1,t2,a3,a2,at,t3,t0;
	QWORD s0;
	DWORD v0,s1;
	int a1,s2;
	BYTE i = 0;

	svcOutputDebugString("hleFMV2",20);
	
//	hleFMVDelay=true;

	//31c58 addi       SP,SP,$FFFFFFF8                   SP=0:0          
	//31c5C sd         RA,$0(SP)                         *(0)            

	//31c60 movi       T1,$0                             T1=0:0          
	//31c64 lui        T2,$FFFF8803                      T2=88030000     
	//31c68 addi       T2,T2,$38C0                       T2=FFFFFFFF:8803
	t1=0;
	t3=r->GPR[T3*2];
	a3=r->GPR[A3*2];
	t0=r->GPR[T0*2];
	DWORD *tmp=(DWORD *)hleMakeP(a3);
	t2=0x880338C0;

	//31c6C sd         R0,$0(A3)                                         
	//31c70 sd         R0,$8(A3)                                         
	//31c74 sd         R0,$20(A3)                                        
	//31c78 sd         R0,$28(A3)                                        
	//31c7C sd         R0,$40(A3)                                        
	//31c80 sd         R0,$48(A3)                                        
	//31c84 sd         R0,$60(A3)                                        
	//31c88 sd         R0,$68(A3)                                        
	//31c8C sd         R0,$80(A3)                                        
	//31c90 sd         R0,$88(A3)                                        
	//31cA8 sd         R0,$A0(A3)                                        
	//31cA4 sd         R0,$A8(A3)                                        
	//31c94 sd         R0,$C0(A3)                                        
	//31c98 sd         R0,$C8(A3)                                        
	//31c9C sd         R0,$E0(A3)                                        
	//31cA0 sd         R0,$E8(A3)                                        
	for(int i=0;i<8;i++)
	{
		*(tmp++)=0;
		*(tmp++)=0;
		*(tmp++)=0;
		*(tmp++)=0;
		tmp+=4;
	}

	v0=r->GPR[V0*2];
	a2=r->GPR[A2*2];
	s0=*(QWORD *)&r->GPR[S0*2];
	s1=r->GPR[S1*2];
	s2=(int)r->GPR[S2*2];

	//31cAC jal        3228C                                             
	//31cB0 lw         V0,$0(T0)       
	v0=*(DWORD *)hleMakeP(t0);
	hleFMVSub2(&v0,&s0,&s1,&s2);

	//31cB4 lwl        A1,$1(V0)                                         
	//31cB8 lwr        A1,$FFFFFFFE(V0)                                  
	a1=*(int *)hleMakeP(v0-2);

	//31cBC sra        A1,A1,$10                                         
	//31cC0 lbu        At,$0(A2)                                         
	a1>>=0x10;
	at=*(BYTE *)hleMakeP(a2);

	//31cC4 mult       At,A1                                             
	//31cC8 mflo       A1                                                
	//31cCC add eq     A1,T3                                             
	//31cD0 sh         A1,$0(A3)                                         
	a1=a1*(int)at;
	a1+=t3;
	*(WORD *)hleMakeP(a3)=a1&0xffff;

	while(1)
	{
		//31cD4 jal        3228C                                             
		//31cD8 lw         V0,$4(T0)                                         
		v0=*(DWORD *)hleMakeP(t0+4);
		hleFMVSub2(&v0,&s0,&s1,&s2);

		//31cDC lwl        A1,$1(V0)                                         
		//31cE0 lwr        A1,$FFFFFFFE(V0)                                  
		//31cE4 sra        A1,A1,$10                                         
		//31cE8 beq        R0,A1 31D1C                                       
		a1=*(int *)hleMakeP(v0-2);
		a1>>=0x10;
		if(a1==0) 
			break;

		//31cEC andi       V0,A1,$3F                                         
		//31cF0 add eq     A2,V0                                             
		//31cF4 lbu        At,$0(A2)                                         
		//31cF8 sra        A1,A1,$6 
		v0=a1&0x3f;
		a2+=v0;
		at=*(BYTE *)hleMakeP(a2);
		a1>>=6;

		//31cFC mult       At,A1                                             
		//31D00 add eq     T2,V0                                             
		//31D04 lbu        At,$0(T2)                         *(880338C0)     
		//31D08 add eq     T1,V0                                             
		//31D0C add eq     At,A3                                             
		//31D10 mflo       A1                                                
		//31D14 jmp        31CD4                                             
		//31D18 sh         A1,$0(At)                                         
		a1=a1*(int)at;
		t2+=v0;
		at=*(BYTE *)hleMakeP(t2);
		t1+=v0;
		at+=a3;
		*(WORD *)hleMakeP(at)=a1&0xffff;
	}
	//31D1C NOP

	short *src=(short *)hleMakeP(a3);
	short *dst=(short *)hleMakeP(a3);
	dst+=(0x100)/2;

	for(i=0;i<8;i++)
	{
		hleFMVSub1(src,dst);
		src++;
		dst+=0x10;
	}

	src=(short *)hleMakeP(a3);
	dst=(short *)hleMakeP(a3);
	src+=(0x100/2);

	for(i=0;i<8;i++)
	{
		hleFMVSub1(src,dst);
		src++;
		dst+=0x10;
	}

	r->GPR[A2*2]=a2;
	r->GPR[V0*2]=v0;
	r->GPR[S1*2]=s1;
	r->GPR[S2*2]=s2;
	*(QWORD *)&r->GPR[S0*2]=s0;

}

void hleTri()
{
	static WORD tnum=0;
	static DWORD tp[8]={0,0,0,0,0,0,0,0};
	short x1,x2,x3,y1,y2,y3,u1,u2,u3,v1,v2,v3;

	short color;
	BYTE *dst=(BYTE *)hleMakeP(r->GPR[V0*2]);
	WORD *src=(WORD *)hleMakeP(r->GPR[GP*2]);
	x1=r->GPR[A0*2];
	y1=r->GPR[A1*2];
	x2=r->GPR[A2*2];
	y2=r->GPR[A3*2];
	x3=r->GPR[T0*2];
	y3=r->GPR[T1*2];
	u1=r->GPR[T2*2];
	v1=r->GPR[T3*2];
	u2=r->GPR[T4*2];
	v2=r->GPR[T5*2];
	u3=r->GPR[T6*2];
	v3=r->GPR[T7*2];
	if(tnum<7)
	{
		bool save=false;
		for(int i=0;i<(tnum+1);i++)
		{
			if(tp[i]==0)
			{
				tp[i]=(unsigned long)src;
				tnum++;
				save=true;
				break;
			}
			else if(tp[i]==(unsigned long)src)
				break;
		}
/* TODO:FIX		
		if(save)
		{
			char buf[64*64*3];
			char *dst=buf;
			char r,g,b;
			WORD pixel;
			CFile *tmpFile=(CFile *)new CFile();
			char filename[64];
			sprintf(filename,"text%d.raw",tnum-1);
			tmpFile->Open(filename,CFile::modeReadWrite|CFile::modeCreate);
			src+=4;
			for(int c=0;c<64*64;c++)
			{
				pixel=*(src++);
				r=(pixel&0x7c00)>>7;
				g=(pixel&0x03e0)>>2;
				b=(pixel&0x001f)<<3;
				*(dst++)=b;
				*(dst++)=g;
				*(dst++)=r;
			}
			tmpFile->Write(buf,64*64*3);
			tmpFile->Close();
			delete tmpFile;
		}
*/		
	}

	color=(x1|y1)&0x7ffff;

	if((x1>0)&&(x1<320))
	{
		if((y1>0)&&(y1<240))
		{
			if((x2>0)&&(x2<320))
			{
				if((y2>0)&&(y2<240))
				{
					if((x3>0)&&(x3<320))
					{
						if((y3>0)&&(y3<240))
						{
//							hleFillTriangle(dst,x1,y1,x2,y2,x3,y3,color);
							hleTextureTriangle(dst,x1,y1,x2,y2,x3,y3,u1,v1,u2,v2,u3,v3,(BYTE *)src);
						}
					}
				}
			}
		}
	}
}

void hleFillTriangle(BYTE *dst,short x1,short y1,short x2,short y2,short x3,short y3,WORD color)
{
	if(y1<=y2)
	{
		if(y2<=y3) //correct
		{
//			hleDrawTriangle(dst,x1,y1,x2,y2,x3,y3,color);
		}
		else if(y1<y3)
		{
//			hleDrawTriangle(dst,x1,y1,x3,y3,x2,y2,color);
		}
		else
		{
//			hleDrawTriangle(dst,x3,y3,x1,y1,x2,y2,color);
		}
	}
	else if(y2<=y3)	//y2>y1, so y2,y1,y3 or y2,y3,y1
	{
		if(y3<=y1)
		{
//			hleDrawTriangle(dst,x2,y2,x3,y3,x1,y1,color);
		}
		else
		{
//			hleDrawTriangle(dst,x2,y2,x1,y1,x3,y3,color);
		}
	}
	else //y2>y1 && y3>y2
	{
//		hleDrawTriangle(dst,x3,y3,x2,y2,x1,y1,color);
	}
}

void hleTextureTriangle(BYTE *dst,short x1,short y1,short x2,short y2,short x3,short y3,
						short u1,short v1,short u2,short v2, short u3, short v3,BYTE *src)
{
	if(y1<=y2)
	{
		if(y2<=y3) //correct
		{
			hleDrawTriangle(dst,x1,y1,x2,y2,x3,y3,u1,v1,u2,v2,u3,v3,src);
		}
		else if(y1<y3)
		{
			hleDrawTriangle(dst,x1,y1,x3,y3,x2,y2,u1,v1,u3,v3,u2,v2,src);
		}
		else
		{
			hleDrawTriangle(dst,x3,y3,x1,y1,x2,y2,u3,v3,u1,v1,u2,v2,src);
		}
	}
	else if(y2<=y3)	//y2>y1, so y2,y1,y3 or y2,y3,y1
	{
		if(y3<=y1)
		{
			hleDrawTriangle(dst,x2,y2,x3,y3,x1,y1,u2,v2,u3,v3,u1,v1,src);
		}
		else
		{
			hleDrawTriangle(dst,x2,y2,x1,y1,x3,y3,u2,v2,u1,v1,u3,v3,src);
		}
	}
	else //y2>y1 && y3>y2
	{
		hleDrawTriangle(dst,x3,y3,x2,y2,x1,y1,u3,v3,u2,v2,u1,v1,src);
	}
}

void hleDrawTriangle(BYTE *dst,short x1,short y1,short x2,short y2,short x3,short y3,
						short u1,short v1,short u2,short v2, short u3, short v3,BYTE *src)
{
	short irl;	//initial row length
	float dx1,dx2,dx3,dx4;
	float du1,dv1,du2,dv2,du3,dv3,du4,dv4;
	WORD p1,p2;

//	theApp.LogMessage("%d,%d %d,%d %d,%d",u1,v1,u2,v2,u3,v3);
	if(y1==y2) //flat top
	{
		p1=0;
		p2=y3-y2;
		dx1=0.f;
		dx2=0.f;
		if(x2<x1)
		{
			irl=x1-x2;
			dx3=(float)(x3-x2)/(float)p2;
			dx4=(float)(x3-x1)/(float)p2;
			x1=x2;
		}
		else
		{
			irl=x2-x1;
			dx3=(float)(x3-x1)/(float)p2;
			dx4=(float)(x3-x2)/(float)p2;
		}
	}
	else
	{
		irl=1;
		p1=y2-y1;
		if(y2==y3)	//flat bottom
		{
			p2=0;
			if(x2<x3)
			{
				dx1=(float)(x2-x1)/(float)p1;
				dx2=(float)(x3-x1)/(float)p1;
			}
			else
			{
				dx1=(float)(x3-x1)/(float)p1;
				dx2=(float)(x2-x1)/(float)p1;
			}
			dx3=0.f;
			dx4=0.f;
		}
		else
		{
			p2=y3-y2;
			if(x2<x3)
			{
				dx1=(float)(x2-x1)/(float)p1;
				dx2=(float)(x3-x1)/(float)(p1+p2);
				dx3=(float)(x3-x2)/(float)p2;
				dx4=dx2;
			}
			else
			{
				dx2=(float)(x2-x1)/(float)p1;
				dx1=(float)(x3-x1)/(float)(p1+p2);
				dx4=(float)(x3-x2)/(float)p2;
				dx3=dx1;
			}
		}
	}

	float fx1,fx2;
	fx1=(float)x1;
	x2=(x1+irl);
	fx2=(float)x2;
	dst+=((y1*640)+(x1*2));
	short ox=x1;
	du1=(u2-u1)/(x2-x1)*dx1;
	du2=(u3-u1)/(x3-x1)*dx2;
	while(p1)
	{
		hleLine((WORD *)dst,x1,x2,u1,u2,(WORD *)src);
		fx1+=dx1;
		fx2+=dx2;
		x1=(short)(fx1+.5f);
		x2=(short)(fx2+.5f);
		p1--;
		dst+=640;
		dst+=((x1-ox)*2);
		ox=x1;
	}
	while(p2)
	{
		hleLine((WORD *)dst,x1,x2,u1,u2,(WORD *)src);
		fx1+=dx3;
		fx2+=dx4;
		x1=(short)(fx1+.5f);
		x2=(short)(fx2+.5f);
		p2--;
		dst+=640;
		dst+=((x1-ox)*2);
		ox=x1;
	}
}

void hleLine(WORD *dst,short x1,short x2,short u1,short u2,WORD *src)
{
	short count;
	count=x2-x1;
	WORD color=0x7a55;
	if(count>0)
	{
		while(count)
		{
			*(dst++)=color;
			count--;
		}
			*(dst++)=color;
	}
	else
	{
		while(count)
		{
			*(dst--)=color;
			count++;
		}
			*(dst--)=color;
	}
}

void hleClearRows()
{
	DWORD rows=r->GPR[A1*2];

	//18c34 addi       SP,SP,$FFFFFFE8                   SP=0:120        
	//18c38 slti       At,A1,$F0                                         
	//18c3C sw         RA,$0(SP)                         *(120)          
	//18c40 sw         V0,$4(SP)                         *(124)          
	//18c44 sw         V1,$8(SP)                         *(128)          
	//18c48 sw         A0,$C(SP)                         *(12C)          
	//18c4C bne        R0,At //18c58                                       
	//18c50 sw         A1,$10(SP)                        *(130)          
	if(rows>=0xf0)
	{
		//18c54 movi       A1,$EF                            A1=0:EF         
		rows=0xef;
	}
	//18c58 lui        V0,$FFFF8804                      V0=88040000     
	//18c5C sll        V1,A1,$6                          V1=0:3BC0       
	//18c60 sll        A0,A1,$4                          A0=0:EF0        
	//18c64 lw         V0,$DC8(V0)                       *(88040DC8)     
	//18c68 add        V1,V1,A0                          V1=0:4AB0       
	//18c6C addi       V1,V1,$FFFFFFFF                   V1=0:4AAF       
	DWORD *dst=(DWORD *)hleMakeP(*(DWORD *)&m->rdRam[0x40dc8]);
	DWORD count=rows*640;
	memset(dst,0,count);

	//18c70 sd         R0,$0(V0)                         *(88040000)     
	//18c74 bne        R0,V1 //18c6C                                       
	//18c78 addi       V0,V0,$8                          V0=FFFFFFFF:8804
	//18c7C lw         RA,$0(SP)                         *(120)          
	//18c80 lw         V0,$4(SP)                         *(124)          
	//18c84 lw         V1,$8(SP)                         *(128)          
	//18c88 lw         A0,$C(SP)                         *(12C)          
	//18c8C lw         A1,$10(SP)                        *(130)          
	//18c90 jr         RA                                                
	//18c94 addi       SP,SP,$18                         SP=0:138        
}

void hleSkySub2()
{
	DWORD v0,v1,*pv1,*pv0,*pa0,*pa1;
	int a0,a1,a2,a3,t1,t2,t0,t3,t4,t5,t6;
	int s2;


	v1=r->GPR[V1*2];
	v0=r->GPR[V0*2];
	a0=r->GPR[A0*2];
	a1=r->GPR[A1*2];
	a2=r->GPR[A2*2];
	a3=r->GPR[A3*2];
	t0=r->GPR[T0*2];
	t4=r->GPR[T4*2];
	t5=r->GPR[T5*2];

	//18c98 addi       SP,SP,$FFFFFFF0                   SP=0:128        
	//18c9C sw         RA,$0(SP)                         *(128)          
	//18cA0 sw         T2,$4(SP)                         *(12C)          
	//18cA4 sw         T3,$8(SP)                         *(130)          
	//18cA8 bgez       A1 //18cE0                                          
	//18cAC sw         T4,$C(SP)                         *(134)          
	if(a1<0)
	{
		//18cB0 add        T2,A1,A3                          T2=0:100CCEF    
		//18cB4 blez       T2 //18dC8                                          
		//18cB8 NOP
		t2=a1+a3;
		if(t2<=0) return;

		//18cBC sub        T2,R0,A1                          T2=0:7F77       
		//18cC0 sub        A3,A3,T2                          A3=0:1004C89    
		//18cC4 sll        T2,T2,$1                          T2=0:FEEE       
		//18cC8 addi       T3,A2,$4                          T3=FFFFFFFF:FFFF
		//18cCC mult       T2,T3                                             
		//18cD0 mov        A1,R0                             A1=0:8066       
		//18cD4 mflo       T2                                                
		//18cD8 add        V1,V1,T2                          V1=0:1499D      
		//18cDC NOP
		t2=-a1;
		a3-=t2;
		t2<<=1;
		t3=a2+4;
		t2*=t3;
		a1=0;
		v1+=t2;
	}

	//18cE0 slti       At,A1,$F0                                         
	//18cE4 beq        R0,At //18dC8                                       
	//18cE8 NOP
	if(a1>=0xf0)
		return;

	//18cEC add        T2,A1,A3                          T2=0:100CCEF    
	//18cF0 addi       T2,T2,$FFFFFF10                   T2=0:100CBFF    
	//18cF4 blez       T2 //18d00                                          
	//18cF8 NOP
	t2=a1+a3;
	t2-=240;
	if(t2>0)
	{
		//18cFC sub        A3,A3,T2                          A3=FFFFFFFF:FFFF
		a3-=t2;
	}

	//18d00 sll        T0,A1,$9                          T0=0:100CC00    
	//18d04 sll        A1,A1,$7                          A1=0:403300     
	//18d08 add        A1,A1,T0                          A1=0:140FF00    
	//18d0C add        V0,V0,A1                          V0=FFFFFFFF:8944
	//18d10 bgez       A0 //18d24                                          
	//18d14 mov        A1,R0                             A1=0:8066       
	v0+=(a1*640);
	a1=0;
	if(a0<0)
	{
		//18d18 add        A0,A0,A2                          A0=FFFFFFFF:FFFF
		//18d1C bltz       A0 //18d18                                          
		//18d20 NOP
		while(a0<0)
			a0+=a2;
	}

	//18d24 slt        At,A0,A2                                          
	//18d28 bne        R0,At //18d40                                       
	//18d2C NOP
	if(a0>=a2)
	{
		//18d30 sub        A0,A0,A2                          A0=0:EF0        
		//18d34 slt        At,A0,A2                                          
		//18d38 beq        R0,At //18d30                                       
		//18d3C NOP
		while(a0>=a2)
			a0-=a2;
	}

	//18d40 addi       T0,A2,$FFFFFEC4                   T0=FFFFFFFF:FFFF
	//18d44 slt        At,T0,A0                                          
	//18d48 bne        R0,At //18dE0                                       
	//18d4C NOP
	t0=a2-316;
	if(t0>=a0)
	{
		//18d50 addi       A2,A2,$4                          A2=FFFFFFFF:FFFF
		//18d54 mult       A2,A1                                             
		//18d58 sll        A0,A0,$1                          A0=0:1DE0       
		//18d5C add        V1,V1,A0                          V1=0:1677D      
		a2+=4;
		t0=a2*a1;
		a0<<=1;
		v1+=a0;

		//18d60 addi       A2,A2,$FFFFFEC0                   A2=FFFFFFFF:FFFF
		//18d64 sll        A2,A2,$1                          A2=FFFFFFFF:FFFE
		//18d68 movi       T2,$14                            T2=0:14         
		//18d6C mflo       T0                                                
		//18d70 sll        T0,T0,$1                          T0=FFFFFFFF:FFFE
		//18d74 add        V1,V1,T0                          V1=0:6617       
		a2-=320;
		a2<<=1;
		t0<<=1;
		v1+=t0;

		pv0=(DWORD *)hleMakeP(v0);
		pv1=(DWORD *)hleMakeP(v1);
		while(a3)
		{
			DWORD count=0x14*(0x20/4);
			while(count)
			{
				*(pv0++)=*(pv1++);
				count--;
			}
			a3--;
			pv1+=(a2/4);
		}
		//18dC8 lw         RA,$0(SP)                         *(128)          
		//18dCC lw         T2,$4(SP)                         *(12C)          
		//18dD0 lw         T3,$8(SP)                         *(130)          
		//18dD4 lw         T4,$C(SP)                         *(134)          
		//18dD8 jr         RA                                                
		//18dDC addi       SP,SP,$10                         SP=0:138        
		return;
	}
	else
	{
		//18dE0 sub        T0,A2,A0                          T0=FFFFFFFF:FFFE
		//18dE4 addi       A2,A2,$4                          A2=FFFFFFFF:FFFE
		//18dE8 mult       A2,A1                                             
		//18dEC sra        T1,T0,$2                          T1=FFFFFFFF:FFFF
		t0=a2-a0;
		a2+=4;
		a1=a1*a2;
		t1=t0>>2;

		//18dF0 addi       T1,T1,$1                          T1=FFFFFFFF:FFFF
		//18dF4 movi       T2,$50                            T2=0:50         
		//18dF8 sll        T5,T1,$2                          T5=FFFFFFFF:FFFE
		//18dFC sub        T2,T2,T1                          T2=0:40AE       
		//18e00 sll        T4,T2,$2                          T4=0:102B8      
		t1+=1;
		t2=0x50;
		t5=t1<<2;
		t2-=t1;
		t4=t2<<2;

		//18e04 sub        T3,A2,T5                          T3=0:16         
		//18e08 sub        T4,A2,T4                          T4=FFFFFFFF:FFFD
		//18e0C sll        A0,A0,$1                          A0=0:26         
		//18e10 mflo       A1                                                
		//18e14 sll        A1,A1,$1                          A1=0:100CC      
		t3=a2-t5;
		t4=a2-t4;
		a0<<=1;
		a1<<=1;
		
		//18e18 add        V1,V1,A1                          V1=0:659D       
		//18e1C sub        A1,T5,T0                          A1=0:1          
		//18e20 mov        S2,A1                             S2=0:1          
		//18e24 sll        A1,A1,$1                          A1=0:2          
		//18e28 sll        T3,T3,$1                          T3=0:2C         
		v1+=a1;
		a1=t5-t0;
		s2=a1;
		a1<<=1;
		t3<<=1;

		//18e2C sll        T4,T4,$1                          T4=FFFFFFFF:FFFB
		//18e30 add        A0,A0,V1                          A0=0:65C3       
		//18e34 beq        R0,T2 //18eA4                                       
		//18e38 add        A1,A1,V1                          A1=0:659F       
		t4<<=1;
		a0+=v1;
		a1+=v1;
		if(t2)
		{
			pa0=(DWORD *)hleMakeP(a0);
			pa1=(DWORD *)hleMakeP(a1);
			pv0=(DWORD *)hleMakeP(v0);
			while(a3)
			{
				//18e3C mov        T6,T1                             T6=FFFFFFFF:FFFF
				t6=t1;
				pa0++;
				while(t6)
				{
					//18e40 ldl        A2,$F(A0)                         *(65D2)         
					//18e44 ldr        A2,$8(A0)                         *(65CB)         
					//18e48 addi       T6,T6,$FFFFFFFF                   T6=FFFFFFFF:FFFF
					//18e4C addi       V0,V0,$8                          V0=FFFFFFFF:8944
					//18e50 addi       A0,A0,$8                          A0=0:65CB       
					//18e54 bne        R0,T6 //18e40                                       
					//18e58 sd         A2,$FFFFFFF8(V0)                  *(8944FF28)     
					t6--;
					*(pv0++)=*(pa0++);
					*(pv0++)=*(pa0++);
				}
				pa0--;
				//18e5C add        A0,A0,T3                          A0=0:65F7       
				//18e60 mov        T6,T2                             T6=0:40AE       
				pa0+=(t3/4);
				t6=t2;
				pa1++;
				while(t6)
				{
					//18e64 ldl        A2,$F(A1)                         *(65AE)         
					//18e68 ldr        A2,$8(A1)                         *(65A7)         
					//18e6C addi       T6,T6,$FFFFFFFF                   T6=0:40AD       
					//18e70 addi       V0,V0,$8                          V0=FFFFFFFF:8944
					//18e74 addi       A1,A1,$8                          A1=0:65A7       
					//18e78 bne        R0,T6 //18e64                                       
					//18e7C sd         A2,$FFFFFFF8(V0)                  *(8944FF30)     
					t6--;
					*(pv0++)=*(pa1++);
					*(pv0++)=*(pa1++);
				}
				pa1--;
				//18e80 addi       A3,A3,$FFFFFFFF                   A3=FFFFFFFF:FFFF
				//18e84 bne        R0,A3 //18e3C                                       
				//18e88 add        A1,A1,T4                          A1=FFFFFFFF:FFFC
				a3--;
				pa1+=(t4/4);
			}
			//18e8C lw         RA,$0(SP)                         *(138)          
			//18e90 lw         T2,$4(SP)                         *(13C)          
			//18e94 lw         T3,$8(SP)                         *(140)          
			//18e98 lw         T4,$C(SP)                         *(144)          
			//18e9C jr         RA                                                
			//18eA0 addi       SP,SP,$10                         SP=0:148        
			return;
		}
		else
		{
			pa0=(DWORD *)hleMakeP(a0);
			pv0=(DWORD *)hleMakeP(v0);
			while(a3)
			{
				//18eA4 mov        T6,T1                             T6=FFFFFFFF:FFFF
				pa0++;
				t6=t1;
				while(t6)
				{
					//18eA8 ldl        A2,$F(A0)                         *(6606)         
					//18eAC ldr        A2,$8(A0)                         *(65FF)         
					//18eB0 addi       T6,T6,$FFFFFFFF                   T6=FFFFFFFF:FFFF
					//18eB4 addi       V0,V0,$8                          V0=FFFFFFFF:8944
					//18eB8 addi       A0,A0,$8                          A0=0:65FF       
					//18eBC bne        R0,T6 //18eA8                                       
					//18eC0 sd         A2,$FFFFFFF8(V0)                  *(8944FF38)     
					*(pv0++)=*(pa0++);
					*(pv0++)=*(pa0++);
					t6--;
				}
				pa0--;
				//18eC4 addi       A3,A3,$FFFFFFFF                   A3=FFFFFFFF:FFFF
				//18eC8 bne        R0,A3 //18eA4                                       
				//18eCC add        A0,A0,T3                          A0=0:662B       
				pa0+=(t3/4);
				a3--;
			}
			//18eD0 lw         RA,$0(SP)                         *(148)          
			//18eD4 lw         T2,$4(SP)                         *(14C)          
			//18eD8 lw         T3,$8(SP)                         *(150)          
			//18eDC lw         T4,$C(SP)                         *(154)          
			//18eE0 jr         RA                                                
			//18eE4 addi       SP,SP,$10                         SP=0:158        
		}
	}

}

void hleKeyInput()
{
	DWORD t0=r->GPR[T0*2];	//pattern to fill (16bits)
	DWORD v1=m->aiReg[0x98];
	DWORD a3=r->GPR[A3*2];
	BYTE *pa3=(BYTE *)hleMakeP(a3);
	BYTE a2;

//	2A56C and        V1,V1,T0                          V1=0:0          
//	2A570 sb         R0,$0(A3)                         *(887FF2A1)     
//	2A574 bne        R0,V1 2A588                                       
//	2A578 lb         A2,$1(A3)                         *(887FF2A2)     
	v1=v1&t0;
	*(pa3)=0;
	a2=*(pa3+1);
	if(v1==0)
	{
		//	2A57C movi       A1,$1                             A1=0:1          
		//	2A580 sb         A1,$1(A3)                         *(887FF2A2)     
		//	2A584 movi       A2,$0                             A2=0:0          
		*(pa3+1)=1;
		a2=0;
	}
	//	2A588 movi       At,$1                             At=0:1          
	//	2A58C bne        At,A2 2A59C                                       
	//	2A590 NOP
	if(a2==1)
	{
		//	2A594 sb         A2,$0(A3)                         *(887FF2A1)     
		//	2A598 sb         R0,$1(A3)                         *(887FF2A2)     
		*(pa3)=a2;
		*(pa3+1)=0;
	}
//	2A59C jr         RA                                                
}

void hleFillRows()
{
	//18bB8 sll        A2,T1,$10                         A2=FFFFFFFF:A466
	//18bBC or         T1,T1,A2                          T1=FFFFFFFF:EDF7
	//18bC0 addi       SP,SP,$FFFFFFF8                   SP=0:130        
	//18bC4 dsll32     A2,T1,$20                                         
	//18bC8 sw         RA,$0(SP)                         *(130)          
	//18bCC bgez       A1 //18bD8                                          
	//18bD0 or         T1,T1,A2                          T1=FFFFFFFF:EDF7
	DWORD a2;
	DWORD a1=r->GPR[A1*2];	//rows to fill
	DWORD t1=r->GPR[T1*2];	//pattern to fill (16bits)

	a2=t1<<0x10;
	t1|=a2;
	if(a1<0)
	{
		//18bD4 mov        A1,R0                             A1=0:8066       
		a1=0;
	}

	//18bD8 movi       A2,$F0                            A2=0:F0         
	//18bDC slt        At,A1,A2                                          
	//18bE0 beq        R0,At //18c24                                       
	//18bE4 NOP
	if(a1>0xf0)
		return;

	//18bE8 lui        V0,$FFFF8804                      V0=88040000     
	//18bEC lw         V0,$DC8(V0)                       *(88040DC8)     
	DWORD *dst=(DWORD *)hleMakeP(*(DWORD *)&m->rdRam[0x40dc8]);

	//18bF0 sll        A3,A1,$9                          A3=0:100CC00    
	//18bF4 sll        T0,A1,$7                          T0=0:403300     
	//18bF8 add        T0,T0,A3                          T0=0:140FF00    
	//18bFC sub        A2,A2,A1                          A2=FFFFFFFF:FFFF
	//18c00 add        V0,V0,T0                          V0=FFFFFFFF:8944
	dst+=(a1*160);
	a2=0xf0-a1;
	while(a2)
	{
		//18c04 movi       T0,$50                            T0=0:50         
		//18c08 addi       T0,T0,$FFFFFFFF                   T0=0:4F         
		//18c0C sd         T1,$0(V0)                         *(8944FF00)     
		//18c10 bne        R0,T0 //18c08                                       
		//18c14 addi       V0,V0,$8                          V0=FFFFFFFF:8944
		WORD count=160;
		while(count)
		{
			*(dst++)=t1;
			count--;
		}
		//18c18 addi       A2,A2,$FFFFFFFF                   A2=FFFFFFFF:FFFF
		//18c1C bne        R0,A2 //18c04                                       
		//18c20 NOP
		a2--;
	}


	//18c24 lw         RA,$0(SP)                         *(130)          
	//18c28 addi       SP,SP,$8                          SP=0:138        
	//18c2C jr         RA                                                
	//18c30 NOP
}

void hleSky()
{
	//1882C lui        A1,$FFFF8809                      A1=88090000     
	//18830 lw         A1,$FFFFDE5C(A1)                  *(8808DE5C)     
	//18834 movi       A2,$11E                           A2=0:11E        
	//18838 lui        At,$FFFF8803                      At=88030000     
	//1883C div        A1,A2                                             
	//18840 lui        A2,$FFFF8809                      A2=88090000     
	//18844 lh         A2,$FFFFE03C(A2)                  *(8808E03C)     
	//18848 lui        T3,$FFFF8809                      T3=88090000     
	//1884C lw         T3,$FFFFDE4C(T3)                  *(8808DE4C)     
	//18850 sb         R0,$59B8(At)                      *(880359B8)     
	//18854 sra        A2,A2,$1                          A2=FFFFFFFF:C404
	//18858 addi       SP,SP,$FFFFFFF8                   SP=0:128        
	//1885C lui        At,$FFFF8809                      At=88090000     
	//18860 sw         RA,$0(SP)                         *(128)          
	//18864 mflo       A1                                                
	//18868 sub        T2,R0,A1                          T2=0:77F78066   
	//1886C add        T2,T2,A2                          T2=0:3BFC0066   
	//18870 sw         T2,$FFFFE00C(At)                  *(8808E00C)     
	//18874 lw         T4,$0(T3)                         *(88090000)     
	//18878 blez       T4 18ACC                                          
	//1887C NOP
	//18880 addi       T3,T3,$4                          T3=FFFFFFFF:8809

	//18884 lw         T5,$0(T3)                         *(88090004)     
	//18888 lw         T6,$0(T5)                         *(8802FFB0)     
	//1888C bne        R0,T6 //1891C                                       
	//18890 NOP
	//18894 lui        T7,$FFFF8803                      T7=88030000     
	//18898 lw         T7,$59EC(T7)                      *(880359EC)     
	//1889C lw         V1,$14(T5)                        *(8802FFC4)     
	//188A0 lw         A1,$4(T5)                         *(8802FFB4)     
	//188A4 lw         T7,$0(T7)                         *(88030000)     
	//188A8 lui        A0,$FFFF8809                      A0=88090000     
	//188AC lw         A2,$C(T5)                         *(8802FFBC)     
	//188B0 add        V1,V1,T7                          V1=0:100C0000   
	//188B4 lw         T7,$10(T5)                        *(8802FFC0)     
	//188B8 lw         A3,$8(T5)                         *(8802FFB8)     
	//188BC lw         A0,$FFFFDE58(A0)                  *(8808DE58)     
	//188C0 slti       At,T7,$191                                        
	//188C4 beq        R0,At //188D8                                       
	//188C8 add        A1,T2,A1                          A1=FFFFFFFF:C405
	//188CC lui        S0,$FFFF8809                      S0=88090000     
	//188D0 lw         S0,$FFFFE008(S0)                  *(8808E008)     
	//188D4 add        A0,A0,S0                          A0=0:10120000   
	//188D8 div        A0,T7                                             
	//188DC lui        S4,$FFFF8803                      S4=88030000     
	//188E0 lb         S4,$59B8(S4)                      *(880359B8)     
	//188E4 mflo       A0                                                
	//188E8 bne        R0,S4 //18908                                       
	//188EC NOP
	//188F0 movi       S4,$1                             S4=0:1          
	//188F4 lui        At,$FFFF8803                      At=88030000     
	//188F8 blez       A1 //18908                                          
	//188FC sb         S4,$59B8(At)                      *(880359B8)     
	//18900 jal        //18c34                                             
	//18904 NOP
	//18908 lui        V0,$FFFF8804                      V0=88040000     
	//1890C jal        //18c98                                             
	//18910 lw         V0,$DC8(V0)                       *(88040DC8)     
	//18914 jmp        //1892C                                             
	//18918 NOP
	//1891C lw         A1,$4(T5)                         *(8802FFB4)     
	//18920 lw         T1,$8(T5)                         *(8802FFB8)     
	//18924 jal        18BB8                                             
	//18928 add        A1,A1,T2                          A1=0:100CC      

	//1892C addi       T4,T4,$FFFFFFFF                   T4=FFFFFFFF:DFF5
	//18930 bne        R0,T4 //18884                                       
	//18934 addi       T3,T3,$4                          T3=FFFFFFFF:8809
	//18938 lw         RA,$0(SP)                         *(128)          
	//1893C addi       SP,SP,$8                          SP=0:130        
	//18940 jr         RA                                                
	//18944 NOP
}


void hleISR2()
{
	DWORD sp=0x887FF100;
	DWORD a0,a1,v0,v1,ra;
	BYTE *bRam=(BYTE *)m->rdRam;
	WORD *wRam=(WORD *)m->rdRam;
	DWORD *dRam=(DWORD *)m->rdRam;
	
	
	r->CPR0[2*13]|=0x400;
	r->CPR0[2*13]^=0x400;
	r->CPR0[2*13]|=0x800;
	r->CPR0[2*13]^=0x800;
	//26754 mf0        V1,T1                                             
	//26758 NOP
	//2675C lui        A0,$C                             A0=C0000        
	//26760 ori        A0,A0,$B735                       A0=0:CB735      
	//26764 add        V1,V1,A0                                          
	//26768 mt0        T3,V1                                             
	r->CompareCount+=0xcb735;
	r->NextIntCount=r->CompareCount;
	
	//2676C NOP
	//26770 lui        V0,$FFFF8809                      V0=88090000     
	//26774 lbu        V0,$FFFFF4D1(V0)                  *(8808F4D1)     
	//26778 addi       V0,V0,$1                          V0=FFFFFFFF:8809
	//2677C slti       At,V0,$3C                                         
	//26780 bne        R0,At 2678C                                       
	v0=bRam[0x7FF12C];
	v0++;
	if(v0>=0x3c)
	{
		//26784 NOP
		//26788 mov        V0,R0                             V0=0:0          
		v0=0;
	}
	//2678C lui        At,$FFFF8809                      At=88090000     
	//26790 bne        R0,V0 267C0                                       
	//26794 sb         V0,$FFFFF4D1(At)                  *(8808F4D1)     
	bRam[0x7FF12C]=v0;
	if(v0==0)
	{
		//26798 lui        V0,$FFFF8804                      V0=88040000     
		//2679C lwu        V0,$FFFFA1EC(V0)                  *(8803A1EC)     
		//267A0 lui        At,$FFFF8804                      At=88040000     
		//267A4 addi       V0,V0,$1                          V0=FFFFFFFF:8804
		//267A8 sw         V0,$FFFFA1EC(At)                  *(8803A1EC)     
		v0=dRam[0x3659C/4];
		v0++;
		dRam[0x3659C/4]=v0;
		
		//267AC lui        V0,$FFFF8809                      V0=88090000     
		//267B0 lwu        V0,$FFFFF3D8(V0)                  *(8808F3D8)     
		//267B4 lui        At,$FFFF8809                      At=88090000     
		//267B8 addi       V0,V0,$1                          V0=FFFFFFFF:8809
		//267BC sw         V0,$FFFFF3D8(At)                  *(8808F3D8)     
		v0=dRam[0x7FF130/4];
		v0++;
		dRam[0x7FF130/4]=v0;
	}
	
	//267C0 lui        V1,$FFFF8808                      V1=88080000     
	//267C4 lb         V1,$6244(V1)                      *(88086244)     
	//267C8 beq        R0,V1 267DC                                       
	//267CC NOP
	BYTE tb=bRam[0x7F5710];
	if(tb)
	{
		//267D0 addi       V1,V1,$FFFFFFFF                   V1=FFFFFFFF:8807
		//267D4 lui        At,$FFFF8808                      At=88080000     
		//267D8 sb         V1,$6244(At)                      *(88086244)     
		tb--;
		bRam[0x7F5710]=tb;
	}
	
	//267DC lui        V1,$FFFF8809                      V1=88090000     
	//267E0 lb         V1,$FFFFF4D0(V1)                  *(8808F4D0)     
	//267E4 bne        R0,V1 26BC8                                       
	//267E8 NOP
	tb=bRam[0x7FF120];
	if(tb==0)
	{
		//267EC lui        V0,$FFFF8804                      V0=88040000     
		//267F0 addi       V0,V0,$FFFFC4C0                   V0=FFFFFFFF:8803
		//267F4 movi       RA,$5                             RA=0:5          
		v0=0x3C4C0;
		ra=5;
		
Label267f8:
		//267F8 lw         A0,$C(V0)                         *(8803C4CC)     
		//267FC lw         A1,$1C(V0)                        *(8803C4DC)     
		//26800 lw         V1,$8(V0)                         *(8803C4C8)     
		//26804 lw         A0,$0(A0)                         *(CB735)        
		//26808 nor        A0,A0,R0                          A0=FFFFFFFF:FFF3
		//2680C and        A0,A0,A1                                          
		//26810 beq        R0,A0 //2681C                                       
		//26814 NOP
		a0=dRam[(v0+0x0c)/4];
		a1=dRam[(v0+0x1c)/4];
		v1=dRam[(v0+0x08)/4];
		a0=dRam[(a0&0x7fffff)/4];
		a0=~a0;
		a0=a0&a1;
		if(a0)
		{
			//26818 movi       A0,$1                             A0=0:1          
			a0=1;
		}
		
		//2681C sll        V1,V1,$1                          V1=0:10120000   
		//26820 or         V1,V1,A0                          V1=0:10120001   
		//26824 andi       A0,V1,$3FFF                       A0=0:1          
		//26828 movi       At,$FFF                           At=0:FFF        
		//2682C mov        A1,R0                             A1=0:0          
		//26830 bne        At,A0 //2683C                                       
		//26834 sw         V1,$8(V0)                         *(8803C4C8)     
		v1<<=1;
		v1=v1|a0;
		a0=v1&0x3fff;
		a1=0;
		dRam[(v0+0x08)/4]=v1;
		if(a0==0xfff)
		{
			//26838 sw         A0,$4(V0)                         *(8803C4C4)     
			dRam[(v0+0x04)/4]=a0;
		}
		
		//2683C andi       A0,V1,$1F                         A0=0:1          
		//26840 movi       At,$10                            At=0:10         
		//26844 bne        At,A0 //2686C                                       
		//26848 NOP
		a0=v1&0x1f;
		if(a0==0x10)
		{
			//2684C lw         A0,$4(V0)                         *(8803C4C4)     
			//26850 bne        R0,A0 //2686C                                       
			//26854 NOP
			a0=dRam[(v0+0x04)/4];
			if(a0==0)
			{
				//26858 lw         A0,$0(V0)                         *(8803C4C0)     
				//2685C beq        R0,A0 //2686C                                       
				//26860 NOP
				a0=dRam[(v0+0x00)/4];
				if(a0)
				{
					//26864 movi       A1,$1                             A1=0:1          
					//26868 sw         R0,$0(V0)                         *(8803C4C0)     
					dRam[(v0+0x00)/4]=0;
					a1=1;
				}
			}
		}
		
		//2686C lw         V1,$8(V0)                         *(8803C4C8)     
		//26870 movi       At,$1                             At=0:1          
		//26874 andi       V1,V1,$F                          V1=0:1          
		//26878 bne        At,V1 //26884                                       
		//2687C NOP
		v1=dRam[(v0+0x08)/4];
		v1&=0xf;
		if(v1==0x1)
		{
			//26880 sw         V1,$0(V0)                         *(8803C4C0)     
			dRam[(v0+0x00)/4]=v1;
		}
		
		//26884 bne        R0,V1 26894                                       
		//26888 NOP
		if(v1==0)
		{
			//2688C sw         R0,$4(V0)                         *(8803C4C4)     
			//26890 sw         R0,$0(V0)                         *(8803C4C0)     
			dRam[(v0+0x04)/4]=0;
			dRam[(v0+0x00)/4]=0;
		}
		
		//26894 beq        R0,A1 26AA0                                       
		//26898 NOP
		if(a1)
		{
			//2689C lui        V1,$FFFF8804                      V1=88040000     
			//268A0 lw         V1,$FFFFA16C(V1)                  *(8803A16C)     
			//268A4 beq        R0,V1 26904                                       
			//268A8 NOP
			v1=dRam[0x3651C/4];
			if(v1)
			{
				//268AC lui        V1,$FFFF8809                      V1=88090000     
				//268B0 lb         V1,$FFFFF4D2(V1)                  *(8808F4D2)     
				//268B4 beq        R0,V1 268E4                                       
				//268B8 movi       At,$1                             At=0:1          
				v1=bRam[0x7FF134];
				a1=1;
				if(v1)
				{
					//268BC bne        At,RA 268E4                                       
					//268C0 NOP
					if(ra==1)
					{
						//268C4 lw         V1,$14(V0)                        *(8803C4D4)     
						//268C8 lui        A0,$FFFF8809                      A0=88090000     
						//268CC lh         A0,$FFFFF4C2(A0)                  *(8808F4C2)     
						//268D0 lw         V1,$0(V1)                         *(88090000)     
						//268D4 lui        At,$FFFF8809                      At=88090000     
						//268D8 add        A0,A0,V1                          A0=0:10120000   
						//268DC jmp        26948                                             
						//268E0 sh         A0,$FFFFF4C2(At)                  *(8808F4C2)     
						v1=dRam[(v0+0x14)/4];
						a0=wRam[0x7FF114/2];
						v1=dRam[(v1&0x7fffff)];
						a0+=v1;
						wRam[0x7FF114/2]=a0;
						goto Label26948;
					}
				}
				//268E4 lw         V1,$14(V0)                        *(8803C4D4)     
				//268E8 lui        A0,$FFFF8809                      A0=88090000     
				//268EC lh         A0,$FFFFF4C0(A0)                  *(8808F4C0)     
				//268F0 lw         V1,$0(V1)                         *(88090000)     
				//268F4 lui        At,$FFFF8809                      At=88090000     
				//268F8 add        A0,A0,V1                          A0=0:10120000   
				//268FC jmp        26948                                             
				//26900 sh         A0,$FFFFF4C0(At)                  *(8808F4C0)     
				v1=dRam[(v0+0x14)/4];
				a0=wRam[0x7FF114/2];
				v1=dRam[(v1&0x7fffff)];
				a0+=v1;
				wRam[0x7FF114/2]=a0;
				goto Label26948;
			}
			//26904 lui        V1,$FFFF8809                      V1=88090000     
			//26908 lb         V1,$FFFFF4D2(V1)                  *(8808F4D2)     
			//2690C beq        R0,V1 26934                                       
			//26910 movi       At,$1                             At=0:1          
			v1=bRam[0x8f4d2];
			a1=1;
			if(v1)
			{
				//26914 bne        At,RA 26934                                       
				//26918 NOP
				if(ra==1)
				{
					//2691C lui        V1,$FFFF8809                      V1=88090000     
					//26920 lh         V1,$FFFFF4C2(V1)                  *(8808F4C2)     
					//26924 lui        At,$FFFF8809                      At=88090000     
					//26928 addi       V1,V1,$1                          V1=FFFFFFFF:8809
					//2692C jmp        //26948                                             
					//26930 sh         V1,$FFFFF4C2(At)                  *(8808F4C2)     
					v1=wRam[0x7FF114/2];
					v1=dRam[(v1&0x7fffff)];
					v1++;
					wRam[0x7FF114/2]=v1;
					goto Label26948;
				}
			}
			//26934 lui        V1,$FFFF8809                      V1=88090000     
			//26938 lh         V1,$FFFFF4C0(V1)                  *(8808F4C0)     
			//2693C lui        At,$FFFF8809                      At=88090000     
			//26940 addi       V1,V1,$1                          V1=FFFFFFFF:8809
			//26944 sh         V1,$FFFFF4C0(At)                  *(8808F4C0)     
			v1=wRam[0x7FF112/2];
			v1=dRam[(v1&0x7fffff)];
			v1++;
			wRam[0x7FF112/2]=v1;
Label26948:
			//26948 lw         V1,$18(V0)                        *(8803C4D8)     
			//2694C lui        At,$FFFF8809                      At=88090000     
			//26950 lw         A0,$0(V1)                         *(88090001)     
			//26954 addi       A0,A0,$1                          A0=0:10120001   
			//26958 sw         A0,$0(V1)                         *(88090001)     
			v1=dRam[(v0+0x18)/4];
			a0=dRam[(v1&0x7fffff)/4];
			a0++;
			dRam[(v1&0x7fffff)/4]=a0;
			
			//2695C lw         V1,$10(V0)                        *(8803C4D0)     
			//26960 lui        A0,$FFFF8804                      A0=88040000     
			//26964 lw         A0,$FFFFA1A0(A0)                  *(8803A1A0)     
			//26968 lw         V1,$0(V1)                         *(88090001)     
			//2696C sw         A1,$FFFFF4B8(At)                  *(8808F4B8)     
			v1=dRam[(v0+0x18)/4];
			a0=dRam[0x36550/4];
			v1=dRam[(v1&0x7fffff)/4];
			dRam[0x7FF108/4]=a1;
			
			//26970 lui        A1,$FFFF8809                      A1=88090000     
			//26974 lw         A1,$FFFFF4CC(A1)                  *(8808F4CC)     
			//26978 add        A1,A1,V1                          A1=0:10120001   
			//2697C slt        At,A1,A0                                          
			//26980 beq        R0,At //26990                                       
			//26984 lui        At,$FFFF8809                      At=88090000     
			a1=dRam[0x7FF11C];
			a1+=v1;
			if(a1<a0)
			{
				//26988 jmp        26A1C                                             
				//2698C sw         A1,$FFFFF4CC(At)                  *(8808F4CC)     
				dRam[0x7FF11C]=a1;
				goto Label26a1c;
			}
			
			//26990 lui        A0,$FFFF8804                      A0=88040000     
			//26994 lw         A0,$FFFFA170(A0)                  *(8803A170)     
			//26998 beq        R0,A0 26AA0                                       
			//2699C NOP
			a0=dRam[0x36520];
			if(a0)
			{
				//269A0 addi       SP,SP,$FFFFFFF8                   SP=FFFFFFFF:8808
				//269A4 sw         RA,$0(SP)                         *(8808F4E8)     
				//269A8 sw         V1,$4(SP)                         *(8808F4EC)     
				sp-=8;
				dRam[((sp+0x00)&0x7fffff)/4]=ra;
				dRam[((sp+0x04)&0x7fffff)/4]=v1;
				
Label269ac:
				//269AC mov        V1,A1                             V1=0:10120001   
				//269B0 sub        V1,V1,A0                          V1=FFFFFFFF:880E
				//269B4 bltz       V1 26A00                                          
				//269B8 NOP
				v1=a1;
				v1-=a0;
				if((int)v1>=0)
				{
					//269BC lui        V1,$FFFF8809                      V1=88090000     
					//269C0 lw         V1,$FFFFF3E8(V1)                  *(8808F3E8)     
					//269C4 lui        At,$FFFF8809                      At=88090000     
					//269C8 sub        A1,A1,A0                          A1=FFFFFFFF:880E
					//269CC addi       V1,V1,$1                          V1=FFFFFFFF:8809
					//269D0 sw         V1,$FFFFF3E8(At)                  *(8808F3E8)     
					v1=dRam[0x7FF124/4];
					a1-=a0;
					v1++;
					dRam[0x7FF124/4]=v1;
					
					//269D4 lui        V1,$FFFF8804                      V1=88040000     
					//269D8 lw         V1,$FFFFA1B8(V1)                  *(8803A1B8)     
					//269DC lui        At,$FFFF8804                      At=88040000     
					//269E0 addi       V1,V1,$1                          V1=FFFFFFFF:8804
					//269E4 sw         V1,$FFFFA1B8(At)                  *(8803A1B8)     
					v1=dRam[0x36568/4];
					v1++;
					dRam[0x36568/4]=v1;
					
					//269E8 lui        V1,$FFFF8804                      V1=88040000     
					//269EC lw         V1,$FFFFA1C0(V1)                  *(8803A1C0)     
					//269F0 lui        At,$FFFF8804                      At=88040000     
					//269F4 addi       V1,V1,$1                          V1=FFFFFFFF:8804
					//269F8 jmp        //269AC                                             
					//269FC sw         V1,$FFFFA1C0(At)                  *(8803A1C0)     
					v1=dRam[0x36570/4];
					v1++;
					dRam[0x36570/4]=v1;
					goto Label269ac;
				}
			}
			
			//26a00 lui        At,$FFFF8809                      At=88090000     
			//26a04 sw         A1,$FFFFF4CC(At)                  *(8808F4CC)     
			//26a08 lui        At,$FFFF8809                      At=88090000     
			//26a0C lw         RA,$0(SP)                         *(8808F4E8)     
			//26a10 lw         V1,$4(SP)                         *(8808F4EC)     
			//26a14 sw         A1,$FFFFF3EC(At)                  *(8808F3EC)     
			//26a18 addi       SP,SP,$8                          SP=FFFFFFFF:8808
			dRam[0x7FF11C/4]=a1;
			ra=dRam[((sp+0x00)&0x7fffff)/4];
			v1=dRam[((sp+0x04)&0x7fffff)/4];
			dRam[0x7FF11C/4]=a1;
			sp+=8;
			
Label26a1c:
			//26a1C lui        A1,$FFFF8809                      A1=88090000     
			//26a20 lw         A1,$FFFFF4C8(A1)                  *(8808F4C8)     
			//26a24 add        A1,A1,V1                          A1=0:100D0001   
			//26a28 lui        V1,$FFFF8804                      V1=88040000     
			//26a2C lw         V1,$FFFFA184(V1)                  *(8803A184)     
			//26a30 beq        R0,V1 //26a78                                       
			//26a34 slt        At,A1,V1                                          
			a1=dRam[0x7FF118/4];
			a1+=v1;
			v1=dRam[0x36534/4];
			if(v1)
			{
				//26a38 bne        R0,At //26a78                                       
				//26a3C lui        At,$FFFF8809                      At=88090000     
				if(a1>=v1)
				{
					//26a40 lui        V1,$FFFF8809                      V1=88090000     
					//26a44 lw         V1,$FFFFF3E8(V1)                  *(8808F3E8)     
					//26a48 sw         R0,$FFFFF3EC(At)                  *(8808F3EC)     
					//26a4C lui        At,$FFFF8809                      At=88090000     
					//26a50 sw         R0,$FFFFF4CC(At)                  *(8808F4CC)     
					v1=dRam[0x7FF124/4];
					dRam[0x7FF128/4]=0;
					dRam[0x7FF11C/4]=0;
					
					//26a54 lui        At,$FFFF8809                      At=88090000     
					//26a58 addi       V1,V1,$1                          V1=FFFFFFFF:8809
					//26a5C sw         V1,$FFFFF3E8(At)                  *(8808F3E8)     
					v1++;
					dRam[0x7FF124/4]=v1;
					
					//26a60 lui        V1,$FFFF8804                      V1=88040000     
					//26a64 lw         V1,$FFFFA1C0(V1)                  *(8803A1C0)     
					//26a68 lui        At,$FFFF8804                      At=88040000     
					//26a6C mov        A1,R0                             A1=0:0          
					//26a70 addi       V1,V1,$1                          V1=FFFFFFFF:8804
					//26a74 sw         V1,$FFFFA1C0(At)                  *(8803A1C0)     
					v1=dRam[0x36570/4];
					a1=0;
					v1++;
					dRam[0x36570/4]=v1;
				}
			}
			//26a78 lui        V1,$FFFF8809                      V1=88090000     
			//26a7C lui        A0,$FFFF8804                      A0=88040000     
			//26a80 lw         V1,$FFFFF3E8(V1)                  *(8808F3E8)     
			//26a84 lw         A0,$FFFFA15C(A0)                  *(8803A15C)     
			//26a88 lui        At,$FFFF8809                      At=88090000     
			//26a8C sw         A1,$FFFFF4C8(At)                  *(8808F4C8)     
			//26a90 slt        At,A0,V1                                          
			//26a94 beq        R0,At //26aA0                                       
			//26a98 lui        At,$FFFF8809                      At=88090000     
			v1=dRam[0x7FF124/4];
			a0=dRam[0x3650C/4];
			dRam[0x7FF118/4]=a1;
			if(a0<v1)
			{
				//26a9C sw         A0,$FFFFF3E8(At)                  *(8808F3E8)     
				dRam[0x7FF124/4]=a0;
			}
		}
		//26aA0 addi       RA,RA,$FFFFFFFF                   RA=0:4          
		//26aA4 bne        R0,RA //267F8                                       
		//26aA8 addi       V0,V0,$20                         V0=FFFFFFFF:8803
		ra--;
		v0+=0x20;
		if(ra)
			goto Label267f8;
		
		//26aAC lui        V1,$FFFFB000                      V1=B0000000     
		//26aB0 lw         V1,$88(V1)                        *(B0000088)     
		//26aB4 andi       A0,V1,$8000                       A0=0:0          
		//26aB8 beq        R0,A0 26BC8                                       
		//26aBC NOP
		v1=inputs[0];
		v1|=(inputs[1]<<16);
//		if(!(v1&0x8000))	//ie; door NOT open
		if(1)
		{
			//26aC0 lui        A0,$FFFF8809                      A0=88090000     
			//26aC4 lw         A0,$FFFFF4BC(A0)                  *(8808F4BC)     
			//26aC8 lui        At,$FFFF8809                      At=88090000     
			//26aCC sw         V1,$FFFFF4BC(At)                  *(8808F4BC)     
			a0=dRam[0x7FF10C/4];
			dRam[0x7FF10C/4]=v1;
			
			//26aD0 xori       V1,V1,$FFFF                       V1=0:B000FFFF   
			//26aD4 and        V1,V1,A0                          V1=FFFFFFFF:8000
			//26aD8 andi       A0,V1,$2000                       A0=0:0          
			//26aDC beq        R0,A0 26B44                                       
			//26aE0 NOP
			v1^=0xffff;
			v1&=a0;
			a0=v1&0x2000;
			if(a0)		//service button (gives a credit)
			{
				//26aE4 lui        V1,$FFFF8809                      V1=88090000     
				//26aE8 lui        A0,$FFFF8804                      A0=88040000     
				//26aEC lw         V1,$FFFFF3E8(V1)                  *(8808F3E8)     
				//26aF0 lw         A0,$FFFFA15C(A0)                  *(8803A15C)     
				//26aF4 beq        A0,V1 26B44                                       
				//26aF8 NOP
				v1=dRam[0x7FF124/4];
				a0=dRam[0x3650C/4];
				if(a0!=v1)
				{
					//26aFC addi       V1,V1,$1                          V1=FFFFFFFF:8809
					//26b00 lui        At,$FFFF8809                      At=88090000     
					//26b04 sw         V1,$FFFFF3E8(At)                  *(8808F3E8)     
					v1++;
					dRam[0x7FF124/4]=v1;
					
					//26b08 lui        V1,$FFFF8804                      V1=88040000     
					//26b0C lw         V1,$FFFFA1BC(V1)                  *(8803A1BC)     
					//26b10 lui        At,$FFFF8804                      At=88040000     
					//26b14 addi       V1,V1,$1                          V1=FFFFFFFF:8804
					//26b18 sw         V1,$FFFFA1BC(At)                  *(8803A1BC)     
					v1=dRam[0x3656C/4];
					v1++;
					dRam[0x3656C/4]=v1;
					
					//26b1C lui        V1,$FFFF8804                      V1=88040000     
					//26b20 lw         V1,$FFFFA1C0(V1)                  *(8803A1C0)     
					//26b24 lui        At,$FFFF8804                      At=88040000     
					//26b28 addi       V1,V1,$1                          V1=FFFFFFFF:8804
					//26b2C sw         V1,$FFFFA1C0(At)                  *(8803A1C0)     
					v1=dRam[0x36570/4];
					v1++;
					dRam[0x36570/4]=v1;
					
					//26b30 movi       V1,$1                             V1=0:1          
					//26b34 lui        At,$FFFF8809                      At=88090000     
					//26b38 sw         V1,$FFFFF4B8(At)                  *(8808F4B8)     
					//26b3C lui        At,$FFFF8809                      At=88090000     
					//26b40 sw         V1,$FFFFF4BC(At)                  *(8808F4BC)     
					dRam[0x7FF108/4]=1;
					dRam[0x7FF10c/4]=1;
				}
			}
			//26b44 lui        V1,$FFFFB000                      V1=B0000000     
			//26b48 lw         V1,$80(V1)                        *(B0000080)     
			//26b4C andi       V1,V1,$1000                       V1=0:0          
			//26b50 bne        R0,V1 //26b64                                       
			//26b54 NOP
			v1=inputs[1];
			if(!(v1&0x1000))	//testmode
			{
				//26b58 movi       V1,$1                             V1=0:1          
				//26b5C lui        At,$FFFF8804                      At=88040000     
				//26b60 sw         V1,$FFFF9AF0(At)                  *(88039AF0)     
				dRam[0x7FECBC/4]=1;
			}
			
			//26b64 lui        V1,$FFFFB000                      V1=B0000000     
			//26b68 lw         V1,$90(V1)                        *(B0000090)     
			//26b6C andi       A0,V1,$8                          A0=0:0          
			//26b70 bne        R0,A0 //26b84                                       
			//26b74 NOP
			v1=inputs[2];
			a0=v1&0x08;
			if(a0==0) //volume down
			{
				//26b78 movi       V1,$1                             V1=0:1          
				//26b7C lui        At,$FFFF8809                      At=88090000     
				//26b80 sw         V1,$FFFFF3F4(At)                  *(8808F3F4)     
//				dRam[0x8f3f4/4]=1;
				bRam[0x7FF135]=1;
			}
			
			//26b84 lui        V1,$FFFFB000                      V1=B0000000     
			//26b88 lw         V1,$90(V1)                        *(B0000090)     
			//26b8C andi       A0,V1,$4                          A0=0:0          
			//26b90 bne        R0,A0 //26bA4                                       
			//26b94 NOP
			v1=inputs[2];
			a0=v1&0x04;
			if(a0==0) //volume up
			{
				//26b98 movi       V1,$1                             V1=0:1          
				//26b9C lui        At,$FFFF8809                      At=88090000     
				//26bA0 sw         V1,$FFFFF3F4(At)                  *(8808F3F4)     
//				dRam[0x8f3f4/4]=1;
				bRam[0x7FF135]=1;
			}
			
			//26bA4 lui        V1,$FFFF8809                      V1=88090000     
			//26bA8 lw         V1,$FFFFF4B8(V1)                  *(8808F4B8)     
			//26bAC beq        R0,V1 //26bC8                                       
			//26bB0 lui        At,$FFFF8809                      At=88090000     
			v1=dRam[0x7FF108/4];
			if(v1)
			{
				//26bB4 sw         R0,$FFFFF4B8(At)                  *(8808F4B8)     
				//26bB8 lui        At,$FFFF8809                      At=88090000     
				//26bBC sb         V1,$FFFFF3F0(At)                  *(8808F3F0)     
				//26bC0 jal        3E08                                              
				//26bC4 movi       A0,$D7D                           A0=0:D7D        
				dRam[0x7FF108/4]=0;
				bRam[0x7FF110]=v1;
//				a0=0xd7d;
				a0=0x1f8;
				hleDSPCommand(0x1f8);
			}
		}
	}
	//26bC8 lui        V1,$FFFF8809                      V1=88090000     
	//26bCC lb         V1,$FFFFF4C5(V1)                  *(8808F4C5)     
	//26bD0 bne        R0,V1 26C44                                       
	//26bD4 NOP
	v1=bRam[0x7FF117];
	if(v0==0)
	{
		//26bD8 lui        V1,$FFFF8809                      V1=88090000     
		//26bDC lh         V1,$FFFFF4C0(V1)                  *(8808F4C0)     
		//26bE0 lui        A0,$FFFF8809                      A0=88090000     
		//26bE4 lb         A0,$FFFFF4C4(A0)                  *(8808F4C4)     
		//26bE8 beq        R0,V1 26C20                                       
		//26bEC NOP
		v1=wRam[0x7FF112/2];
		a0=bRam[0x7FF116];
		if(v1)
		{
			//26bF0 bne        R0,A0 26C20                                       
			//26bF4 NOP
			if(a0==0)
			{
				//26bF8 addi       V1,V1,$FFFFFFFF                   V1=FFFFFFFF:8808
				//26BFC lui        At,$FFFF8809                      At=88090000     
				//26c00 sh         V1,$FFFFF4C0(At)                  *(8808F4C0)     
				v1--;
				wRam[0x7FF112/2]=v1;
				
				//26c04 movi       A0,$10                            A0=0:10         
				//26c08 lui        At,$FFFF8809                      At=88090000     
				//26c0C sb         A0,$FFFFF4C4(At)                  *(8808F4C4)     
				bRam[0x7FF116]=0x10;
				
				//26c10 movi       A0,$1                             A0=0:1          
				//26c14 lui        At,$FFFFB000                      At=B0000000     
				//26c18 jmp        //26c44                                             
				//26c1C sw         A0,$B0(At)                        *(B00000B0)     
				hleUpdateScreen(1);
				goto Label26c44;
			}
		}
		//26c20 beq        R0,A0 //26c44                                       
		//26c24 NOP
		if(a0)
		{
			//26c28 addi       A0,A0,$FFFFFFFF                   A0=0:0          
			//26c2C lui        At,$FFFF8809                      At=88090000     
			//26c30 sb         A0,$FFFFF4C4(At)                  *(8808F4C4)     
			//26c34 slti       At,A0,$9                                          
			//26c38 beq        R0,At //26c44                                       
			//26c3C lui        At,$FFFFB000                      At=B0000000     
			a0--;
			bRam[0x7FF116]=a0;
			if(a0<9)
			{
				//26c40 sw         R0,$B0(At)                        *(B00000B0)     
				hleUpdateScreen(0);
			}
		}
	}
Label26c44:
	//26c44 lui        V1,$FFFF8809                      V1=88090000     
	//26c48 lb         V1,$FFFFF4C4(V1)                  *(8808F4C4)     
	//26c4C bne        R0,V1 //26cC0                                       
	//26c50 NOP
	v1=bRam[0x7FF116];
	if(v1==0)
	{
		//26c54 lui        V1,$FFFF8809                      V1=88090000     
		//26c58 lh         V1,$FFFFF4C2(V1)                  *(8808F4C2)     
		//26c5C lui        A0,$FFFF8809                      A0=88090000     
		//26c60 lb         A0,$FFFFF4C5(A0)                  *(8808F4C5)     
		//26c64 beq        R0,V1 //26c9C                                       
		//26c68 NOP
		v1=wRam[0x7FF114/2];
		a0=bRam[0x7FF117];
		if(v1)
		{
			//26c6C bne        R0,A0 //26c9C                                       
			//26c70 NOP
			if(a0==0)
			{
				//26c74 addi       V1,V1,$FFFFFFFF                   V1=FFFFFFFF:8808
				//26c78 lui        At,$FFFF8809                      At=88090000     
				//26c7C sh         V1,$FFFFF4C2(At)                  *(8808F4C2)     
				//26c80 movi       A0,$10                            A0=0:10         
				//26c84 lui        At,$FFFF8809                      At=88090000     
				//26c88 sb         A0,$FFFFF4C5(At)                  *(8808F4C5)     
				//26c8C movi       A0,$2                             A0=0:2          
				//26c90 lui        At,$FFFFB000                      At=B0000000     
				//26c94 jmp        //26cC0                                             
				//26c98 sw         A0,$B0(At)                        *(B00000B0)     
				v1--;
				wRam[0x7FF114/2]=v1;
				bRam[0x7FF117]=0x10;
				hleUpdateScreen(2);
				goto Label26cc0;
			}
		}
		//26c9C beq        R0,A0 //26cC0                                       
		//26cA0 NOP
		if(a0)
		{
			//26cA4 addi       A0,A0,$FFFFFFFF                   A0=0:1          
			//26cA8 lui        At,$FFFF8809                      At=88090000     
			//26cAC sb         A0,$FFFFF4C5(At)                  *(8808F4C5)     
			//26cB0 slti       At,A0,$9                                          
			//26cB4 beq        R0,At //26cC0                                       
			//26cB8 lui        At,$FFFFB000                      At=B0000000     
			a0--;
			bRam[0x7FF117]=a0;
			if(a0<9)
			{
				//26cBC sw         R0,$B0(At)                        *(B00000B0)     
				hleUpdateScreen(0);
			}
		}
	}
	
Label26cc0:
	
	//26cC0 addi       SP,SP,$40                         SP=FFFFFFFF:8808
	//26cC4 ld         A1,$FFFFFFD0(SP)                  *(8808F500)     
	//26cC8 ld         At,$0(SP)                         *(8808F530)     
	//26cCC ld         V0,$FFFFFFF8(SP)                  *(8808F528)     
	//26cD0 mthi       R0                                                
	//26cD4 ld         A1,$FFFFFFC8(SP)                  *(8808F4F8)     
	//26cD8 ld         V1,$FFFFFFF0(SP)                  *(8808F520)     
	//26cDC ld         A0,$FFFFFFE8(SP)                  *(8808F518)     
	//26cE0 mtlo       R0                                                
	//26cE4 ld         A1,$FFFFFFE0(SP)                  *(8808F510)     
	//26cE8 ld         RA,$FFFFFFD8(SP)                  *(8808F508)     
	//26cEC ld         SP,$1F0(R0)                       *(1F0)          
	return;
}
