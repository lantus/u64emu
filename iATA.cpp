
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "iMain.h"
#include "dynaCompiler.h"

#include "iCPU.h"			// Core 4600 emulation routines
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines

#include "iATA.h"			// ATA emulation


FILE *ataFile;
FILE *ataMFile;
DWORD ataHeads;		//num of heads per InitParams command
DWORD ataSectors;	//num of sector per InitParams command

DWORD ataHead;			//cur head
DWORD ataSectorCount;	//sector count
DWORD ataSectorNum;		//cur sector
DWORD ataCylLow;
DWORD ataCylHigh;
WORD *ataCurData;
WORD *ataDataBuffer;
DWORD ataUsed;
DWORD ataBytesHead;
WORD ataTransferMode=0;
DWORD ataTargetLen;
DWORD CatchMe=0;


/*
#ifdef KI2
WORD ataDriveID[6]={0x2020,0x3030,0x5354,0x3931,0x3530};
#else
WORD ataDriveID[6]={0,0x5354,0x3931,0x3530,0x4147,0x2020};
#endif
*/

WORD ataDriveID[6];

#define MIRRORn
//#define VERBOSE

void iATAConstruct()
{
	ataFile=NULL;
	ataDataBuffer=(WORD *)malloc(512*256*2);	//bytes per sector * max sectors per access
	ataCurData=ataDataBuffer;
}

void iATADestruct()
{
	SafeFree(ataDataBuffer);
	SafeDelete(ataFile);

#ifdef MIRROR
	SafeDelete(ataMFile);
#endif
}

bool iATAOpen()
{
	ataFile=fopen(theApp.m_HDImage,"r+b");
	
#ifdef MIRROR
	ataMFile=(CFile *)new CFile();
	ataMFile->Open("mirror.dat",CFile::modeReadWrite|CFile::modeCreate);
	ataFile->Read(ataDataBuffer,1024);
	ataMFile->Write(ataDataBuffer,1024);
#endif

	m->atReg[0x170]=0x48; //we be ready
	m->aiReg[0x170]=0x48;


	switch(gRomSet)
	{
	case KI2:
		{
			ataDriveID[0]=0x2020;
			ataDriveID[1]=0x3030;
			ataDriveID[2]=0x5354;
			ataDriveID[3]=0x3931;
			ataDriveID[4]=0x3530;
			break;
		}
	case KI1:
		{
			ataDriveID[0]=0x0;
			ataDriveID[1]=0x5354;
			ataDriveID[2]=0x3931;
			ataDriveID[3]=0x3530;
			ataDriveID[4]=0x4147;
			ataDriveID[5]=0x2020;
			break;
		}
	}

	if (ataFile)
		return true;
	else
		return false;
}

void iATAClose()
{
	fclose(ataFile);
	 
#ifdef MIRROR
	fclose(ataMFile);
#endif

}


/*
A command has been written, so do whatever we must.... :)

*/

void iATAUpdate()
{
#ifdef VERBOSE
	theApp.LogMessage("ATA DataUsed %d - Command %x",ataUsed,m->atReg[0x138]);
#endif
	ataCurData=ataDataBuffer;
	ataUsed=0;
	switch(m->atReg[0x138])
	{
	case 0xec:
		{
			iATADriveIdentify();
			break;
		}
	case 0x91:	// init drive parameters
		{
			iATAInitParams();
			break;
		}
	case 0x20:	// read sectors
		{
			iATAReadSectors();
			break;
		}
	case 0x30:	// write sectors
		{
			iATAWriteSectors();
			break;
		}
	default:
		{

/*
			for(int Page=0;Page<4;Page++)
			{
				for(int Offset=0;Offset<0x8000;Offset++)
				{
					dynaPageTable[Page].Value[Offset]=0;
				}
			}
*/
#ifdef VERBOSE
			theApp.LogMessage("Unknown ATA command %X at %X A0=%X",(DWORD)m->atReg[0x138],r->PC,(DWORD)r->GPR[8]);
#endif
//			theApp.LogMessage("SP[A1]=%X",*(DWORD*)&m->rdRam[0x8724c]);
			DWORD tmp=*(DWORD*)&m->rdRam[0x8724c];
			tmp+=4;
			*(DWORD*)&m->rdRam[0x8724c]=tmp;
			
			*(DWORD *)&m->atReg[0x138]=0x21;
			r->CPR0[2*CAUSE] = 0x00;
			m->atReg[0x170]=0;
			memcpy(&ataDataBuffer[0x1a],ataDriveID,sizeof(WORD)*6);
			ataUsed=0;
			ataCurData=ataDataBuffer;
//			NewTask=ERROR_BREAK;
			m->aiReg[0x170]=0x48;
			break;
		}
	}
    iMemToDo = 0;
	//whatever we do...say DRVRDY
	m->aiReg[0x170]=0x48;

	*(DWORD *)&m->aiReg[0x138]=0;//m->atReg[0x138];

	//and (we guess) fire IP3
	r->CPR0[2*CAUSE] |= 0x800;
}

void iATADriveIdentify()
{
#ifdef VERBOSE
	theApp.LogMessage("DriveID");
#endif
	if(gRomSet==KI2)
	{
		memcpy(&ataDataBuffer[/*0x14/2*/0x0a],ataDriveID,sizeof(WORD)*6);
	}
	else if(gRomSet==KI1)
	{
		memcpy(&ataDataBuffer[0x1a],ataDriveID,sizeof(WORD)*6);
	}

	ataUsed=0;
	ataCurData=ataDataBuffer;
}

void iATAInitParams()
{
	ataHeads=m->atReg[0x130]+1;
	ataSectors=m->atReg[0x110];
	ataBytesHead=ataSectors*512*419;
	CatchMe++;
//	if(CatchMe==2)
//		NewTask=ERROR_BREAK;
#ifdef VERBOSE
	theApp.LogMessage("ATA InitParams %x heads, %x sectors",ataHeads,ataSectors);
#endif
}

void iATAReadSectors()
{
	ataCurData=ataDataBuffer;
	ataUsed=0;
	ataSectorCount=m->atReg[0x110];
	if(ataSectorCount==0) ataSectorCount=256;

	ataSectorNum=m->atReg[0x118];
	ataCylLow=m->atReg[0x120];
	ataCylHigh=m->atReg[0x128];
	ataHead=m->atReg[0x130];


	DWORD addy=(ataSectorNum-1);
	DWORD cyl=((ataCylHigh<<8)|ataCylLow);
	addy+=(cyl*40*14);
	addy+=(ataHead*40);
	addy*=512;
	addy+=0xd;
	fseek(ataFile, addy, SEEK_SET);
	fread(ataDataBuffer,ataSectorCount*512,1,ataFile);
	ataTransferMode=0;
	ataTargetLen=ataSectorCount*512;

//	if(addy==1024)
//		NewTask=ERROR_BREAK;

#ifdef MIRROR
	ataMFile->Write(ataDataBuffer,ataSectorCount*512);
#endif

#ifdef VERBOSE
	theApp.LogMessage("ATA ReadSectors from %d: Count %d, Head %d, CylHigh %d, CylLow %d, SN %d",addy,ataSectorCount,
		ataHead,ataCylHigh,ataCylLow,ataSectorNum);
	if(addy==3155981)
		CatchMe=1;
#endif
}

void iATAWriteSectors()
{
	ataCurData=ataDataBuffer;
	ataUsed=0;
	ataSectorCount=m->atReg[0x110];
	if(ataSectorCount==0) ataSectorCount=256;

	ataSectorNum=m->atReg[0x118];
	ataCylLow=m->atReg[0x120];
	ataCylHigh=m->atReg[0x128];
	ataHead=m->atReg[0x130];


	ataTargetLen=ataSectorCount*512;
	DWORD addy=(ataSectorNum-1);
	DWORD cyl=((ataCylHigh<<8)|ataCylLow);
	addy+=(cyl*40*14);
	addy+=(ataHead*40);
	addy*=512;
	addy+=0xd;
	fseek(ataFile,addy, SEEK_SET);
//	ataFile->Write(ataDataBuffer,ataSectorCount*512);
	ataTransferMode=1;

#ifdef VERBOSE
	theApp.LogMessage("ATA WriteSectors from %d: Count %d, Head %d, CylHigh %d, CylLow %d, SN %d",addy,ataSectorCount,
		ataHead,ataCylHigh,ataCylLow,ataSectorNum);
#endif
}

BYTE *iATADataRead()
{
	BYTE *add=(BYTE *)ataCurData;
	ataCurData++;
	ataUsed+=2;
	if(ataUsed>=ataTargetLen)
	{
		if(ataUsed==ataTargetLen)
		{
			if(ataTransferMode)
			{
				fwrite(ataDataBuffer,ataSectorCount*512,1,ataFile);
#ifdef VERBOSE
				theApp.LogMessage("ATA WriteData %d",ataTargetLen);
#endif
			}
			else
			{
#ifdef VERBOSE
				theApp.LogMessage("ATA ReadData %d",ataTargetLen);
#endif
			}
		}
		else
		{
			NewTask=DEBUG_BREAK;
		}
		ataCurData=ataDataBuffer;
		ataUsed=0;
	}
/*
	DWORD tmp=*(DWORD *)add;
	if(tmp==0xa87a200a)
		NewTask=DEBUG_BREAK;
	if(tmp==0x20202020)
		NewTask=DEBUG_BREAK;
*/
	return(add);
}