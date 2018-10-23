#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "iMain.h"
#include "iCPU.h"			// Core 4600 emulation routines
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines

//#pragma optimize("",off)
#define _SHIFTR(v, s, w)	\
    ((unsigned int)(((unsigned int)(v) >> (s)) & ((0x01 << (w)) - 1)))

#define VERBOSE_TLBn

void iOpLdl()
{
	DWORD    offset;
	QWORD   data;
	
	offset = (DWORD)(r->GPR[MAKE_RS*2] + MAKE_I);
	
	data=iMemReadQWord(offset & 0xfffffff8);
	
	switch(7-(offset % 8))
	{
	case 0:
		*(QWORD *)&r->GPR[MAKE_RT*2] = data;
		break;
		
	case 1:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0x00000000000000ff) | (data << 8);
		break;
		
	case 2:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0x000000000000ffff) | (data << 16);
		break;
		
	case 3:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0x0000000000ffffff) | (data << 24);
		break;
		
	case 4:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0x00000000ffffffff) | (data << 32);
		break;
		
	case 5:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0x000000ffffffffff) | (data << 40);
		break;
		
	case 6:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0x0000ffffffffffff) | (data << 48);
		break;
		
	case 7:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0x00ffffffffffffff) | (data << 56);
		break;
		
	} 
}

void iOpLdr()
{
	DWORD    offset;
	QWORD   data;

	offset = (DWORD) (r->GPR[MAKE_RS*2] + MAKE_I);
	
	data=iMemReadQWord(offset & 0xfffffff8);
	
	switch(7-(offset % 8))
	{
	case 7:
		*(QWORD *)&r->GPR[MAKE_RT*2] = data;
		break;
		
	case 6:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0xff00000000000000) | (data >> 8);
		break;
		
	case 5:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0xffff000000000000) | (data >> 16);
		break;
		
	case 4:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0xffffff0000000000) | (data >> 24);
		break;
		
	case 3:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0xffffffff00000000) | (data >> 32);
		break;
		
	case 2:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0xffffffffff000000) | (data >> 40);
		break;
		
	case 1:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0xffffffffffff0000) | (data >> 48);
		break;
		
	case 0:
		*(QWORD *)&r->GPR[MAKE_RT*2]  = (*(QWORD *)&r->GPR[MAKE_RT*2] & 0xffffffffffffff00) | (data >> 56);
		break;
		
	} /* switch(offset % 8) */
}

void iOpLb()
{
	*(sQWORD *)&r->GPR[MAKE_RT*2] = (sQWORD)(sDWORD)(short)(sBYTE)iMemReadByte((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpLh()
{
	*(sQWORD *)&r->GPR[MAKE_RT*2] = (sQWORD)(sDWORD)(short)iMemReadWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpLwl()
{
	QWORD offset;
	DWORD  data;
	DWORD rt=MAKE_RT;
	
	offset = (QWORD)(r->GPR[MAKE_RS*2] + MAKE_I);
	
	data = iMemReadDWord((DWORD)(offset & 0xfffffffc));
	
//	switch((offset % 4))
	switch(3-(offset % 4))
	{
	case 0:
		*(sQWORD *)&r->GPR[MAKE_RT*2] = (sQWORD)(sDWORD)data;
		break;
		
	case 1:
		*(sQWORD *)&r->GPR[MAKE_RT*2]  = (sQWORD)(sDWORD)((r->GPR[MAKE_RT*2] & 0x000000ff) | (data << 8));
		break;
		
	case 2:
		*(sQWORD *)&r->GPR[MAKE_RT*2]  = (sQWORD)(sDWORD)((r->GPR[MAKE_RT*2] & 0x0000ffff) | (data << 16));
		break;
		
	case 3:
		*(sQWORD *)&r->GPR[MAKE_RT*2]  = (sQWORD)(sDWORD)((r->GPR[MAKE_RT*2] & 0x00ffffff) | (data << 24));
		break;
	}
}

void iOpLw()
{
	*(sQWORD *)&r->GPR[MAKE_RT*2] =(sQWORD)(sDWORD)iMemReadDWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));

}

void iOpLbu()
{
	*(QWORD *)&r->GPR[MAKE_RT*2] = (QWORD)(DWORD)(WORD)(BYTE)iMemReadByte((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpLhu()
{
	*(QWORD *)&r->GPR[MAKE_RT*2] = (QWORD)(DWORD)(WORD)iMemReadWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpLwr()
{
	QWORD offset;
	DWORD  data;

	offset = (QWORD)(r->GPR[MAKE_RS*2] + MAKE_I);
	
	data = iMemReadDWord((DWORD)(offset & 0xfffffffc));
	
//	switch((offset % 4))
	switch(3-(offset % 4))
	{
	case 0:
		*(sQWORD *)&r->GPR[MAKE_RT*2]  = (*(sQWORD *)&r->GPR[MAKE_RT*2] & 0xffffff00) | (data >> 24);
		break;
		
	case 1:
		*(sQWORD *)&r->GPR[MAKE_RT*2]  = (*(sQWORD *)&r->GPR[MAKE_RT*2] & 0xffff0000) | (data >> 16);
		break;
		
	case 2:
		*(sQWORD *)&r->GPR[MAKE_RT*2]  = (*(sQWORD *)&r->GPR[MAKE_RT*2] & 0xff000000) | (data >> 8);
		break;
		
	case 3:
		*(sQWORD *)&r->GPR[MAKE_RT*2]  = (sQWORD)(sDWORD)data;
		break;
	}
}

void iOpLwu()
{
	*(QWORD *)&r->GPR[MAKE_RT*2] = (QWORD)(DWORD)iMemReadDWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}


void iOpSb()
{
	iMemWriteByte((BYTE)r->GPR[MAKE_RT*2],(DWORD)( r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpSh()
{
	if(r->PC==(0x880011bec))
		iMemWriteWord((WORD)0,(DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
	else
		iMemWriteWord((WORD)r->GPR[MAKE_RT*2],(DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}


void iOpSwl()
{
	QWORD offset;
	DWORD  data;
	DWORD  old_data;
	
	offset = (QWORD)r->GPR[MAKE_RS*2] + MAKE_I;
	
	old_data = iMemReadDWord((DWORD)(offset & 0xfffffffc));
	
//	switch((offset % 4))
	switch(3-(offset % 4))
	{
	case 0:
		data = (DWORD)r->GPR[MAKE_RT*2];
		break;
		
	case 1:
		data = (old_data & 0xff000000) | ((DWORD)r->GPR[MAKE_RT*2] >> 8);
		break;
		
	case 2:
		data = (old_data & 0xffff0000) | ((DWORD)r->GPR[MAKE_RT*2] >> 16);
		break;
		
	case 3:
		data = (old_data & 0xffffff00) | ((DWORD)r->GPR[MAKE_RT*2] >> 24);
		break;
		
	default:
		data = 0;   /* to make compiler happy */
	}
	
	iMemWriteDWord(data, (DWORD)(offset & 0xfffffffc));
}


void iOpSw()
{
	iMemWriteDWord((DWORD)r->GPR[MAKE_RT*2], (DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpSdl()
{
	DWORD    offset;
	QWORD   data;

	offset = (DWORD)(r->GPR[MAKE_RS*2] + MAKE_I);
	
	data=iMemReadQWord((DWORD)(offset & 0xfffffff8));
	
//	switch((offset % 8))
	switch(7-(offset % 8))
	{
	case 0:
		iMemWriteQWord(*(QWORD *)&r->GPR[MAKE_RT*2], offset & 0xfffffff8);
		break;
		
	case 1:
		iMemWriteQWord((data & 0xff00000000000000) | (*(QWORD *)&r->GPR[MAKE_RT*2] >> 8), offset & 0xfffffff8);
		break;
		
	case 2:
		iMemWriteQWord((data & 0xffff000000000000) | (*(QWORD *)&r->GPR[MAKE_RT*2] >> 16), offset & 0xfffffff8);
		break;
		
	case 3:
		iMemWriteQWord((data & 0xffffff0000000000) | (*(QWORD *)&r->GPR[MAKE_RT*2] >> 24), offset & 0xfffffff8);
		break;
		
	case 4:
		iMemWriteQWord((data & 0xffffffff00000000) | (*(QWORD *)&r->GPR[MAKE_RT*2] >> 32), offset & 0xfffffff8);
		break;
		
	case 5:
		iMemWriteQWord((data & 0xffffffffff000000) | (*(QWORD *)&r->GPR[MAKE_RT*2] >> 40), offset & 0xfffffff8);
		break;
		
	case 6:
		iMemWriteQWord((data & 0xffffffffffff0000) | (*(QWORD *)&r->GPR[MAKE_RT*2] >> 48), offset & 0xfffffff8);
		break;
		
	case 7:
		iMemWriteQWord((data & 0xffffffffffffff00) | (*(QWORD *)&r->GPR[MAKE_RT*2] >> 56), offset & 0xfffffff8);
		break;
		
	} /* switch(offset % 8) */
}

void iOpSdr()
{
	DWORD    offset;
	QWORD   data;

	offset = (DWORD)(r->GPR[MAKE_RS*2] + MAKE_I);
	
	data=iMemReadQWord((DWORD)(offset & 0xfffffff8));
	
//	switch((offset % 8))
	switch(7-(offset % 8))
	{
	case 7:
		iMemWriteQWord(*(QWORD *)&r->GPR[MAKE_RT*2], offset & 0xfffffff8);
		break;
		
	case 6:
		iMemWriteQWord((data & 0x00000000000000ff) | (*(QWORD *)&r->GPR[MAKE_RT*2] << 8), offset & 0xfffffff8);
		break;
		
	case 5:
		iMemWriteQWord((data & 0x000000000000ffff) | (*(QWORD *)&r->GPR[MAKE_RT*2] << 16), offset & 0xfffffff8);
		break;
		
	case 4:
		iMemWriteQWord((data & 0x0000000000ffffff) | (*(QWORD *)&r->GPR[MAKE_RT*2] << 24), offset & 0xfffffff8);
		break;
		
	case 3:
		iMemWriteQWord((data & 0x00000000ffffffff) | (*(QWORD *)&r->GPR[MAKE_RT*2] << 32), offset & 0xfffffff8);
		break;
		
	case 2:
		iMemWriteQWord((data & 0x000000ffffffffff) | (*(QWORD *)&r->GPR[MAKE_RT*2] << 40), offset & 0xfffffff8);
		break;
		
	case 1:
		iMemWriteQWord((data & 0x0000ffffffffffff) | (*(QWORD *)&r->GPR[MAKE_RT*2] << 48), offset & 0xfffffff8);
		break;
		
	case 0:
		iMemWriteQWord((data & 0x00ffffffffffffff) | (*(QWORD *)&r->GPR[MAKE_RT*2] << 56), offset & 0xfffffff8);
		break;
		
	} /* switch(offset % 8) */
}

void iOpSwr()
{
	QWORD offset;
	DWORD  data;
	DWORD  old_data;

	offset = (QWORD)r->GPR[MAKE_RS*2] + MAKE_I;
	
	old_data=iMemReadDWord((DWORD)(offset & 0xfffffffc));
	
//	switch((offset % 4))
	switch(3-(offset % 4))
	{
	case 0:
		data = (old_data & 0x00ffffff) | ((DWORD)r->GPR[MAKE_RT*2] << 24);
		break;
		
	case 1:
		data = (old_data & 0x0000ffff) | ((DWORD)r->GPR[MAKE_RT*2] << 16);
		break;
		
	case 2:
		data = (old_data & 0x000000ff) | ((DWORD)r->GPR[MAKE_RT*2] << 8);
		break;
		
	case 3:
		data = (DWORD)r->GPR[MAKE_RT*2];
		break;
		
	default:
		data = 0;   /* to make compiler happy */
	}
	
	iMemWriteDWord(data, (DWORD)(offset & 0xfffffffc));
}

void iOpLl()
{
	*(sQWORD *)&r->GPR[MAKE_RT*2] = (sQWORD) (sDWORD)iMemReadDWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
	
	*(sQWORD *)&r->CPR0[2*LLADDR] = (sQWORD)(sDWORD)(r->GPR[MAKE_RS*2] + MAKE_I);
	
	r->Llbit = 1;
}

void iOpLwc1()
{
	//	*(sQWORD *) &r->FPR[MAKE_FT] = (sQWORD)((sDWORD)iMemReadDWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I)));
	r->FPR[MAKE_FT] = (sDWORD)iMemReadDWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpLwc2()
{
	*(sQWORD *)&r->CPR2[2*MAKE_RT] = (sQWORD)((sDWORD)iMemReadDWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I)));
}

void iOpLld()
{
	*(QWORD *)&r->GPR[MAKE_RT*2]=iMemReadQWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
	
	r->CPR0[2*LLADDR] = (sQWORD)(sDWORD)(r->GPR[MAKE_RS*2] + MAKE_I);
	
	r->Llbit = 1;
}


void iOpLdc1()
{
	QWORD value;
	
	if(r->GPR[MAKE_RS*2]==0x80185938)
		int stop=1;
	value=iMemReadQWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));

	if(r->CPR0[2*STATUS] & 0x04000000)
	{
		r->FPR[MAKE_FT] =(sDWORD)value;
	}
	else
	{
		r->FPR[MAKE_FT+0] = (sDWORD)value;
		r->FPR[MAKE_FT+1] = (sDWORD)(value >> 32);
	}
}

void iOpLdc2()
{
	 *(sQWORD *)&r->CPR2[2*MAKE_RT]=iMemReadQWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpLd()
{
	 *(sQWORD *)&r->GPR[MAKE_RT*2]=iMemReadQWord((DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}


void iOpSc()
{
	r->Llbit=1;
	if(r->Llbit)
	{
		iMemWriteDWord((DWORD)r->GPR[MAKE_RT*2], (DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
        
	} /* if(r->Llbit) */
	
	*(sQWORD *)&r->GPR[MAKE_RT*2] = (sQWORD)(sBYTE)r->Llbit;
}

void iOpSwc1()
{
	iMemWriteDWord((DWORD)r->FPR[MAKE_FT], (DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpSwc2()
{
	iMemWriteDWord((DWORD)r->CPR2[2*MAKE_RT], (DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpScd()
{
	if(r->Llbit)
	{
		iMemWriteQWord(*(QWORD *)&r->GPR[MAKE_RT*2], (DWORD)((r->GPR[MAKE_RS*2] + MAKE_I)));
        
	} /* if(r->Llbit) */
	
	*(sQWORD *)&r->GPR[MAKE_RT*2] = (sQWORD)(sBYTE)r->Llbit;
}

void iOpSdc1()
{
	QWORD value;

/*
	if(r->CPR0[2*STATUS] & 0x04000000)
		value = (sQWORD)(sDWORD)*(float *)&r->FPR[MAKE_FT];
	else
*/
	{
		value = *(QWORD *)&r->FPR[MAKE_FT];
		value = (value <<32 ) | (value >>32);
	}
	iMemWriteQWord(value, (DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpSdc2()
{
	iMemWriteQWord(r->CPR2[2*MAKE_RT*2], (DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}
void iOpSd()
{
	iMemWriteQWord(*(sQWORD *)&r->GPR[MAKE_RT*2], (DWORD)(r->GPR[MAKE_RS*2] + MAKE_I));
}

void iOpTlbr()
{
	QWORD index;

	index = r->CPR0[INDEX*2] & 0x1f;

	r->CPR0[PAGEMASK*2] = r->Tlb[index].hh;
	r->CPR0[ENTRYHI*2]  = (r->Tlb[index].hl & ~r->Tlb[index].hh);
	r->CPR0[ENTRYLO1*2] = (r->Tlb[index].lh | r->Tlb[index].g);
	r->CPR0[ENTRYLO0*2] = (r->Tlb[index].ll | r->Tlb[index].g);
#ifdef VERBOSE_TLB
	theApp.LogMessage("TLBRead %d PageMask:%X EntryHi:%X EntryLo1:%X EntryLo0:%X",index,r->CPR0[PAGEMASK*2],
																				r->CPR0[ENTRYHI*2],
																				r->CPR0[ENTRYLO1*2],
																				r->CPR0[ENTRYLO0*2]);
#endif
}

void iOpTlbwi()
{
	QWORD index;
	QWORD a,b,c,d;


	a=(QWORD)(r->CPR0[2*PAGEMASK]);
	b=(QWORD)(r->CPR0[2*ENTRYHI]);
	c=(QWORD)(r->CPR0[2*ENTRYLO1]);
	d=(QWORD)(r->CPR0[2*ENTRYLO0]);

	index = r->CPR0[2*INDEX];
	

	r->Tlb[index].hh =(DWORD)(r->CPR0[2*PAGEMASK]);
	r->Tlb[index].hl = (DWORD)(r->CPR0[2*ENTRYHI] & ~r->CPR0[2*PAGEMASK]);
	r->Tlb[index].lh = (DWORD)(r->CPR0[2*ENTRYLO1] & 0xfffffffe);
	r->Tlb[index].ll = (DWORD)(r->CPR0[2*ENTRYLO0] & 0xfffffffe);
	r->Tlb[index].g  = (BYTE)(0x01 & r->CPR0[2*ENTRYLO1] & r->CPR0[2*ENTRYLO0]);
#ifdef VERBOSE_TLB
	theApp.LogMessage("TLBWrite %d PageMask:%X EntryHi:%X EntryLo1:%X EntryLo0:%X",index,r->CPR0[PAGEMASK*2],
																				r->CPR0[ENTRYHI*2],
																				r->CPR0[ENTRYLO1*2],
																				r->CPR0[ENTRYLO0*2]);
#endif
	
	DWORD Mask,VirtualPageNum,AddressSpaceID,Region;
	DWORD Dirty1,Valid1,Global1;
	DWORD PageFrame1;
	DWORD Dirty2,Valid2,Global2;
	DWORD PageFrame2;
	Mask=_SHIFTR((DWORD)r->CPR0[2*PAGEMASK],13,12);
	VirtualPageNum=(DWORD)((QWORD)r->CPR0[2*ENTRYHI]>>13)&0x7ffffff;
	AddressSpaceID=(DWORD)r->CPR0[2*ENTRYHI]&0xf;

	PageFrame1=_SHIFTR((DWORD)r->CPR0[2*ENTRYLO1],6,24);
	Dirty1=_SHIFTR((DWORD)r->CPR0[2*ENTRYLO1],2,1);
	Valid1=_SHIFTR((DWORD)r->CPR0[2*ENTRYLO1],1,1);
	Global1=_SHIFTR((DWORD)r->CPR0[2*ENTRYLO1],0,1);
	PageFrame2=_SHIFTR((DWORD)r->CPR0[2*ENTRYLO0],6,24);
	Dirty2=_SHIFTR((DWORD)r->CPR0[2*ENTRYLO0],2,1);
	Valid2=_SHIFTR((DWORD)r->CPR0[2*ENTRYLO0],1,1);
	Global2=_SHIFTR((DWORD)r->CPR0[2*ENTRYLO0],0,1);

#ifdef VERBOSE_TLB
	theApp.LogMessage("TLB Write Indexed %d",index);
	theApp.LogMessage("HH: %X HL:%X LH:%X LL%X G:%X",r->Tlb[index].hh,r->Tlb[index].hl,r->Tlb[index].lh,r->Tlb[index].ll,r->Tlb[index].g);
	theApp.LogMessage("MASK: %X VPN %X  ASID: %X",(DWORD)r->CPR0[2*PAGEMASK],VirtualPageNum,AddressSpaceID);
	theApp.LogMessage("PFN %X V:%X D:%X G:%X",PageFrame1,Valid1,Dirty1,Global1);
	theApp.LogMessage("PFN %X V:%X D:%X G:%X",PageFrame2,Valid2,Dirty2,Global2);
#endif
	if(index==1)
	{
		if(Valid2)
		{
#ifdef VERBOSE_TLB
			theApp.LogMessage("TLB Write Indexed %d",index);
#endif
			iMemTLBActive=true;
		}
		else
		{
		//	m_Memory->m_TLBActive=false;
		}
	}
}

void iOpTlbwr()
{
#ifdef VERBOSE_TLB
	theApp.LogMessage("TLB Write Random");
#endif
}

void iOpTlbp()
{
	int i;
#ifdef VERBOSE_TLB
	theApp.LogMessage("Probing");
#endif
	for(i=0; i<32; i++)
	{
		if
			(
			( (r->Tlb[i].hl & ~0x1fff) == (r->CPR0[2*ENTRYHI] & ~0x1fff) )
			&&
			(
			r->Tlb[i].g
			||
			( (r->Tlb[i].hl & 0xff) == (r->CPR0[2*ENTRYHI] & 0xff) )
			)
			)
		{
#ifdef VERBOSE_TLB
			theApp.LogMessage("Probe find @ %d",i);
#endif
			r->CPR0[2*INDEX] = i;
			break;
		}
		
	} 
}

void iOpLui()	
{
	*(sQWORD *)&r->GPR[2*MAKE_RT]=(sQWORD)(sDWORD)(MAKE_I<<16);

}



#pragma optimize("",on)
