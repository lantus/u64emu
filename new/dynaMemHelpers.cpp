
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "iMain.h"

#include "iCPU.h"			// Core 4300 emulation routines
#include "dynaCompiler.h"
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines
#include "iATA.h"			// KI ATA-2 hard disk emulation

extern DWORD cheat;

void helperLb(DWORD address,DWORD op0)
{
	if(op0==0)
		iMemPhysReadAddr(address);
	else
		*(sQWORD *)&r->GPR[op0*2]=(sQWORD) (sDWORD ) (short) *(char *)iMemPhysReadAddr(address);
}

void helperLbU(DWORD address,DWORD op0)
{
	if(op0==0)
		iMemPhysReadAddr(address);
	else
		*(sQWORD *)&r->GPR[op0*2]=(QWORD) (DWORD ) (WORD) *(BYTE *)iMemPhysReadAddr(address);
}

void helperLh(DWORD address,DWORD op0)
{
	if(op0==0)
		iMemPhysReadAddr(address);
	else
		*(sQWORD *)&r->GPR[op0*2]=(sQWORD)(sDWORD) *(short *)iMemPhysReadAddr(address);
}

void helperLhU(DWORD address,DWORD op0)
{
	if(op0==0)
		iMemPhysReadAddr(address);
	else
		*(QWORD *)&r->GPR[op0*2]=(QWORD)(DWORD)*(WORD *)iMemPhysReadAddr(address);
}

void helperLw(DWORD address,DWORD op0)
{
	if(op0==0)
		iMemPhysReadAddr(address);
	else
		*(sQWORD *)&r->GPR[op0*2]=(sQWORD) *(sDWORD *)iMemPhysReadAddr(address);
}

void helperLwU(DWORD address,DWORD op0)
{
	if(op0==0)
		iMemPhysReadAddr(address);
	else
		*(sQWORD *)&r->GPR[op0*2]=(QWORD) *(DWORD *)iMemPhysReadAddr(address);
}

void helperLd(DWORD address,DWORD op0)
{
	if(op0==0)
		iMemPhysReadAddr(address);
	else
		*(sQWORD *)&r->GPR[op0*2]=*(sQWORD *)iMemPhysReadAddr(address);
}

void helperSb(DWORD address,DWORD op0)
{
//	cheat=*(BYTE *)&r->GPR[op0*2];
	*(BYTE *)iMemPhysWriteAddr(address)=*(BYTE *)&r->GPR[op0*2];
	if(iMemToDo & 0x8000)
		iATAUpdate();
}

void helperSh(DWORD address,DWORD op0)
{
	*(WORD *)iMemPhysWriteAddr(address)=*(WORD *)&r->GPR[op0*2];
	if(iMemToDo & 0x8000)
		iATAUpdate();
}

void helperSw(DWORD address,DWORD op0)
{
	*(DWORD *)iMemPhysWriteAddr(address)=*(DWORD *)&r->GPR[op0*2];
	if(iMemToDo & 0x8000)
		iATAUpdate();
}

void helperSd(DWORD address,DWORD op0)
{
	*(QWORD *)iMemPhysWriteAddr(address)=*(QWORD *)&r->GPR[op0*2];
	if(iMemToDo & 0x8000)
		iATAUpdate();
}

void helperLwr(DWORD address,DWORD op0)
{
	DWORD    offset;
	DWORD   data;
	
	offset = (DWORD)address;
	
	data=iMemReadDWord(offset & 0xfffffffc);

	switch(3-(offset % 4))
	{
	case 0:
		*(sQWORD *)&r->GPR[op0*2]  = (*(sQWORD *)&r->GPR[op0*2] & 0xffffff00) | (data >> 24);
		break;
		
	case 1:
		*(sQWORD *)&r->GPR[op0*2]  = (*(sQWORD *)&r->GPR[op0*2] & 0xffff0000) | (data >> 16);
		break;
		
	case 2:
		*(sQWORD *)&r->GPR[op0*2]  = (*(sQWORD *)&r->GPR[op0*2] & 0xff000000) | (data >> 8);
		break;
		
	case 3:
		*(sQWORD *)&r->GPR[op0*2]  = (sQWORD)(sDWORD)data;
		break;
	}
}

void helperLwl(DWORD address,DWORD op0)
{
	DWORD    offset;
	DWORD   data;
	
	offset = (DWORD)address;
	
	data=iMemReadDWord(offset & 0xfffffffc);

	switch(3-(offset % 4))
	{
	case 0:
		*(sQWORD *)&r->GPR[op0*2] = (sQWORD)(sDWORD)data;
		break;
		
	case 1:
		*(sQWORD *)&r->GPR[op0*2]  = (sQWORD)(sDWORD)((r->GPR[op0*2] & 0x000000ff) | (data << 8));
		break;
		
	case 2:
		*(sQWORD *)&r->GPR[op0*2]  = (sQWORD)(sDWORD)((r->GPR[op0*2] & 0x0000ffff) | (data << 16));
		break;
		
	case 3:
		*(sQWORD *)&r->GPR[op0*2]  = (sQWORD)(sDWORD)((r->GPR[op0*2] & 0x00ffffff) | (data << 24));
		break;
	}
}

void helperLdr(DWORD address,DWORD op0)
{
	DWORD    offset;
	QWORD   data;
	
	offset = (DWORD)address;
	
	data=iMemReadQWord(offset & 0xfffffff8);

	switch(7-(offset % 8))
	{
	case 7:
		*(QWORD *)&r->GPR[op0*2] = data;
		break;
		
	case 6:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xff00000000000000) | (data >> 8);
		break;
		
	case 5:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffff000000000000) | (data >> 16);
		break;
		
	case 4:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffffff0000000000) | (data >> 24);
		break;
		
	case 3:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffffffff00000000) | (data >> 32);
		break;
		
	case 2:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffffffffff000000) | (data >> 40);
		break;
		
	case 1:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffffffffffff0000) | (data >> 48);
		break;
		
	case 0:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0xffffffffffffff00) | (data >> 56);
		break;
		
	}
}

void helperLdl(DWORD address,DWORD op0)
{
	DWORD    offset;
	QWORD   data;
	
	offset = (DWORD)address;
	
	data=iMemReadQWord(offset & 0xfffffff8);

	switch(7-(offset % 8))
	{
	case 0:
		*(QWORD *)&r->GPR[op0*2] = data;
		break;
		
	case 1:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x00000000000000ff) | (data << 8);
		break;
		
	case 2:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x000000000000ffff) | (data << 16);
		break;
		
	case 3:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x0000000000ffffff) | (data << 24);
		break;
		
	case 4:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x00000000ffffffff) | (data << 32);
		break;
		
	case 5:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x000000ffffffffff) | (data << 40);
		break;
		
	case 6:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x0000ffffffffffff) | (data << 48);
		break;
		
	case 7:
		*(QWORD *)&r->GPR[op0*2]  = (*(QWORD *)&r->GPR[op0*2] & 0x00ffffffffffffff) | (data << 56);
		break;
		
	} 
}


void helperSwr(DWORD offset,DWORD op0)
{
	DWORD  data;
	DWORD  old_data;

	old_data=iMemReadDWord((DWORD)(offset & 0xfffffffc));
	
	switch(3-(offset % 4))
	{
	case 0:
		data = (old_data & 0x00ffffff) | ((DWORD)r->GPR[op0*2] << 24);
		break;
		
	case 1:
		data = (old_data & 0x0000ffff) | ((DWORD)r->GPR[op0*2] << 16);
		break;
		
	case 2:
		data = (old_data & 0x000000ff) | ((DWORD)r->GPR[op0*2] << 8);
		break;
		
	case 3:
		data = (DWORD)r->GPR[op0*2];
		break;
		
	default:
		data = 0;   /* to make compiler happy */
	}
	
	iMemWriteDWord(data, (DWORD)(offset & 0xfffffffc));
}


void helperSwl(DWORD offset,DWORD op0)
{
	DWORD  data;
	DWORD  old_data;
	
	old_data = iMemReadDWord((DWORD)(offset & 0xfffffffc));
	
	switch(3-(offset % 4))
	{
	case 0:
		data = (DWORD)r->GPR[op0*2];
		break;
		
	case 1:
		data = (old_data & 0xff000000) | ((DWORD)r->GPR[op0*2] >> 8);
		break;
		
	case 2:
		data = (old_data & 0xffff0000) | ((DWORD)r->GPR[op0*2] >> 16);
		break;
		
	case 3:
		data = (old_data & 0xffffff00) | ((DWORD)r->GPR[op0*2] >> 24);
		break;
		
	default:
		data = 0;   /* to make compiler happy */
	}
	
	iMemWriteDWord(data, (DWORD)(offset & 0xfffffffc));
}


void helperSdl(DWORD offset,DWORD op0)
{

	QWORD   data;

	data=iMemReadQWord((DWORD)(offset & 0xfffffff8));
	
	switch(7-(offset % 8))
	{
	case 0:
		iMemWriteQWord(*(QWORD *)&r->GPR[op0*2], offset & 0xfffffff8);
		break;
		
	case 1:
		iMemWriteQWord((data & 0xff00000000000000) | (*(QWORD *)&r->GPR[op0*2] >> 8), offset & 0xfffffff8);
		break;
		
	case 2:
		iMemWriteQWord((data & 0xffff000000000000) | (*(QWORD *)&r->GPR[op0*2] >> 16), offset & 0xfffffff8);
		break;
		
	case 3:
		iMemWriteQWord((data & 0xffffff0000000000) | (*(QWORD *)&r->GPR[op0*2] >> 24), offset & 0xfffffff8);
		break;
		
	case 4:
		iMemWriteQWord((data & 0xffffffff00000000) | (*(QWORD *)&r->GPR[op0*2] >> 32), offset & 0xfffffff8);
		break;
		
	case 5:
		iMemWriteQWord((data & 0xffffffffff000000) | (*(QWORD *)&r->GPR[op0*2] >> 40), offset & 0xfffffff8);
		break;
		
	case 6:
		iMemWriteQWord((data & 0xffffffffffff0000) | (*(QWORD *)&r->GPR[op0*2] >> 48), offset & 0xfffffff8);
		break;
		
	case 7:
		iMemWriteQWord((data & 0xffffffffffffff00) | (*(QWORD *)&r->GPR[op0*2] >> 56), offset & 0xfffffff8);
		break;
		
	} /* switch(offset % 8) */
}

void helperSdr(DWORD offset,DWORD op0)
{
	QWORD   data;

	data=iMemReadQWord((DWORD)(offset & 0xfffffff8));
	
	switch(7-(offset % 8))
	{
	case 7:
		iMemWriteQWord(*(QWORD *)&r->GPR[op0*2], offset & 0xfffffff8);
		break;
		
	case 6:
		iMemWriteQWord((data & 0x00000000000000ff) | (*(QWORD *)&r->GPR[op0*2] << 8), offset & 0xfffffff8);
		break;
		
	case 5:
		iMemWriteQWord((data & 0x000000000000ffff) | (*(QWORD *)&r->GPR[op0*2] << 16), offset & 0xfffffff8);
		break;
		
	case 4:
		iMemWriteQWord((data & 0x0000000000ffffff) | (*(QWORD *)&r->GPR[op0*2] << 24), offset & 0xfffffff8);
		break;
		
	case 3:
		iMemWriteQWord((data & 0x00000000ffffffff) | (*(QWORD *)&r->GPR[op0*2] << 32), offset & 0xfffffff8);
		break;
		
	case 2:
		iMemWriteQWord((data & 0x000000ffffffffff) | (*(QWORD *)&r->GPR[op0*2] << 40), offset & 0xfffffff8);
		break;
		
	case 1:
		iMemWriteQWord((data & 0x0000ffffffffffff) | (*(QWORD *)&r->GPR[op0*2] << 48), offset & 0xfffffff8);
		break;
		
	case 0:
		iMemWriteQWord((data & 0x00ffffffffffffff) | (*(QWORD *)&r->GPR[op0*2] << 56), offset & 0xfffffff8);
		break;
		
	} /* switch(offset % 8) */
}

void helperSc(DWORD offset,DWORD op0)
{
	r->Llbit=1;
	if(r->Llbit)
	{
		iMemWriteDWord((DWORD)r->GPR[op0*2],offset);
        
	} /* if(r->Llbit) */
	
	*(sQWORD *)&r->GPR[op0*2] = (sQWORD)(sBYTE)r->Llbit;
}

void helperScd(DWORD offset,DWORD op0)
{
	r->Llbit=1;
	if(r->Llbit)
	{
		iMemWriteQWord(*(QWORD *)&r->GPR[op0*2],offset);
        
	} /* if(r->Llbit) */
	
	*(sQWORD *)&r->GPR[op0*2] = (sQWORD)(sBYTE)r->Llbit;
}

void helperLl(DWORD offset,DWORD op0)
{
	*(sQWORD *)&r->GPR[op0*2]=(sQWORD) *(sDWORD *)iMemPhysReadAddr(offset);
	*(sQWORD *)&r->CPR0[2*LLADDR] = (sQWORD)(sDWORD)offset;
	r->Llbit = 1;
}

void helperLld(DWORD offset,DWORD op0)
{
	*(sQWORD *)&r->GPR[op0*2]=* (sQWORD*)iMemPhysReadAddr(offset);
	*(sQWORD *)&r->CPR0[2*LLADDR] = (sQWORD)(sDWORD)offset;
	r->Llbit = 1;
}
