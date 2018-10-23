#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "iMain.h"
#include "iCPU.h"			// Core 4600 emulation routines
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines
#include "hleMain.h"

#define CHECK_BRANCH_IN_BRANCH

#ifdef CHECK_BRANCH_IN_BRANCH
bool logJAL=false;

bool CheckBranchInBranch()
{
	if(r->Delay)
		return(true);
	else
		return(false);
}
#endif

#define BRANCH_CAST *(sDWORD *)&r

void iOpJr()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	r->Delay = DO_DELAY;
	r->PCDelay = (sDWORD) r->GPR[MAKE_RS*2];
}

void iOpJalr()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	r->Delay = DO_DELAY;
	r->PCDelay =(DWORD)r->GPR[MAKE_RS*2];
	if((r->PCDelay&0x7fffff)==0x1EB48)
	{
		logJAL=true;
	}
	if(logJAL)
	{
		theApp.LogMessage("JALR %X\r\n",r->PCDelay);
	}
	r->GPR[MAKE_RD*2] = (sQWORD) (sDWORD) (r->PC+LINK_OFFSET);
}


void iOpJ()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(MAKE_T==r->PC-4)
	{
		r->ICount=r->NextIntCount;
	}
    r->Delay = DO_DELAY;
    r->PCDelay = MAKE_T;
}

void iOpJal()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
 
#endif
 
}

void iOpBeq()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(BRANCH_CAST->GPR[MAKE_RS*2] == BRANCH_CAST->GPR[MAKE_RT*2])
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
		if(r->PCDelay==r->PC-4)
		{
			r->ICount=r->NextIntCount;
		}
	}
}

void iOpBne()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif
	DWORD test=MAKE_O;
	if((test&0xfffff)==0x2d3d8)
	{
		return;
	}

	if(BRANCH_CAST->GPR[MAKE_RS*2] != BRANCH_CAST->GPR[MAKE_RT*2])
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
		if(r->PCDelay==r->PC-4)
		{
			r->ICount=r->NextIntCount;
		}
	}
}

void iOpBlez()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(BRANCH_CAST->GPR[MAKE_RS*2] <=0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
	return;
}

void iOpBgtz()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(BRANCH_CAST->GPR[MAKE_RS*2] >0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
	return;
}


void iOpBeql()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(BRANCH_CAST->GPR[MAKE_RS*2] == BRANCH_CAST->GPR[MAKE_RT*2])
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
	else
	{
		r->PC += 4;
	}
	return;
}

void iOpBnel()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(BRANCH_CAST->GPR[MAKE_RS*2] != BRANCH_CAST->GPR[MAKE_RT*2])
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
	else
	{
		r->PC += 4;
	}

}

void iOpBlezl()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(BRANCH_CAST->GPR[MAKE_RS*2] <=0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
	else
	{
		r->PC += 4;
	}

}

void iOpBgtzl()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(BRANCH_CAST->GPR[MAKE_RS*2] >0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
	else
	{
		r->PC += 4;
	}
}

void iOpBltz()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(BRANCH_CAST->GPR[MAKE_RS*2] < 0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
}

void iOpBgez()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(BRANCH_CAST->GPR[MAKE_RS*2] >= 0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
}

void iOpBltzl()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(BRANCH_CAST->GPR[MAKE_RS*2] < 0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
	else
	{
		r->PC += 4;
	}
}

void iOpBgezl()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	if(BRANCH_CAST->GPR[MAKE_RS*2] >= 0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
	else
	{
		r->PC += 4;
	}
}

void iOpBltzal()
{
 #ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

   r->GPR[31*2] =  (sDWORD) (r->PC+LINK_OFFSET);
	
	if(BRANCH_CAST->GPR[MAKE_RS*2] < 0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
}

void iOpBgezal()
{
 #ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

	DWORD Address=MAKE_O;
	if((Address&0x3fffff)==0x30d10)
	{
		//hleScrollBlock(MAKE_RS);
		r->PC+=8;
		return;
	}

   r->GPR[31*2] = (sDWORD) (r->PC+LINK_OFFSET);
	
	if(BRANCH_CAST->GPR[MAKE_RS*2] >= 0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
	
}

void iOpBltzall()
{
 #ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

   r->GPR[31*2] = (sDWORD) (r->PC+LINK_OFFSET);
	
	if(BRANCH_CAST->GPR[MAKE_RS*2] < 0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
	else
	{
		r->PC += 4;
	}
}

void iOpBgezall()
{
 #ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

   r->GPR[31*2] = (sDWORD) (r->PC+LINK_OFFSET);
	
	if(BRANCH_CAST->GPR[MAKE_RS*2] >= 0)
	{
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
	}
	else
	{
		r->PC += 4;
	}
}

void iOpEret()
{
	if(r->CPR0[STATUS*2] & 0x0004)                   /* if(ERL exception)           */
	{
		r->PC = (DWORD)(r->CPR0[2*ERROREPC] - 4);             /*    PC = ErrorEPC            */
		r->CPR0[2*STATUS] &= 0xfffffffffffffffbU;   /*    STATUS -= ERL            */
	}
	else                                              /* else ("normal" exception)   */
	{
		r->PC = (DWORD)(r->CPR0[2*EPC] );                  /*    PC = EPC                 */
		r->CPR0[2*STATUS] &= 0xfffffffffffffffdU;   /*    STATUS -= EXL            */
	}

	r->PC|=0x88000000;
	r->Delay = NO_DELAY;
	r->Llbit = 0;
//	r->Lo=r->Break;
//	r->Hi=r->LastPC;

} 

void iOpBcf()
{
#ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

    if(!(r->CCR1[2*31] & 0x00800000))
    {
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
    }           
}

void iOpBct()
{
 #ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

   if(r->CCR1[2*31] & 0x00800000)
    {
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
    }           
}

void iOpBcfl()
{
 #ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

   if(!(r->CCR1[2*31] & 0x00800000))
    {
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
    }
    else
	{
		r->PC += 4;
	}
}

void iOpBctl()
{
 #ifdef CHECK_BRANCH_IN_BRANCH
	if(CheckBranchInBranch()) return;
#endif

   if(r->CCR1[2*31] & 0x00800000)
    {
		r->Delay = DO_DELAY;
		r->PCDelay = MAKE_O;
    }
    else
	{
		r->PC += 4;
	}
}
