
#include "stdafx.h"
#include "math.h"
#include "ki.h"

#include "iMain.h"

#include "iCPU.h"			// Core 4600 emulation routines
#include "iMemory.h"		// Memory emulation routines
#include "iRom.h"			// Rom (cart) emulation routines

WORD EndianChangeMode;
CFile *iRomFile;
WORD iRomNumPages;
#define ROM_PAGE_SIZE 0x20000  //128K
BYTE *iRomPageMap;


void iRomConstruct()
{
	rom=(N64RomStruct *)malloc(sizeof(N64RomStruct));
	rom->Image=NULL;
	iRomNumPages=0;
	iRomPageMap=NULL;
	iRomFile=NULL;
}

void iRomDestruct()
{
	if(iRomFile)
	{
		iRomFile->Close();
		SafeDelete(iRomFile);
	}
	SafeFree(rom->Image);
	SafeFree(rom);
	SafeFree(iRomPageMap);
}


int iRomReadImage(char *filename)
{   
	CFile *in;
    BYTE    endian_1;
    BYTE    endian_2;
    BYTE    endian_3;
    BYTE    endian_4;
   

	iRomFile=(CFile *)new CFile();
	in=iRomFile;
	BOOL result=in->Open(filename,CFile::modeRead);
	if(!result)
	{
		theApp.ErrorMessage("Failed to open rom image file %s",filename);
		delete in;
		return(-1);
	}
	rom->Length=in->GetLength();
	rom->Length/=ROM_PAGE_SIZE;
	rom->Length++;
	rom->Length*=ROM_PAGE_SIZE;
	rom->PrgCodeLength=rom->Length-0x1000;
	rom->Image=(BYTE *)malloc((size_t)rom->Length);
	if(rom->Image==NULL)
	{
		theApp.ErrorMessage("Malloc error for RomImage - length %d",rom->Length);
		in->Close();
		delete in;
		return(-1);
	}
	rom->Header=rom->Image;
	rom->BootCode=rom->Image+0x40;
	rom->PrgCode=(DWORD *)(rom->Image + 0x1000);


/*
	in->Read(&endian_1,1);
	in->Read(&endian_2,1);
	in->Read(&endian_3,1);
	in->Read(&endian_4,1);
	if( (endian_1==0x37)&&(endian_2==0x80) )
	{
		EndianChangeMode=0;
	}
	else if( (endian_2==0x37)&&(endian_1==0x80) )
	{
		EndianChangeMode=1;
	}
	else if( (endian_3==0x37)&&(endian_4==0x80) )
	{
		EndianChangeMode=2;
	}
	else
	{
		in->Close();
		delete in;
		theApp.ErrorMessage("File %s is not a valid N64 file",filename);
		return(-1);
	}
*/
	EndianChangeMode=2;

	in->Seek(0,CFile::begin);
	int length=in->Read(rom->Image,rom->Length);
	if(EndianChangeMode!=2)
		iRomChangeRomEndian(EndianChangeMode,rom->Length);
//	in->Close();
//	delete in;
/*
	if(length!=rom->Length)
	{
		in->Close();
		delete in;
		theApp.ErrorMessage("Read Error for rom image: Read %d and wanted %d",length,rom->Length);
        return(-1);
	}


  iRomSetupPageMap();


	rom->Info.Validation           = *(WORD *)(rom->Header + (0x00 ^ 0x02));
    rom->Info.Compression          = *(BYTE *) (rom->Header + (0x02 ^ 0x03));
    rom->Info.Unknown1             = *(BYTE *) (rom->Header + (0x03 ^ 0x03));
    rom->Info.Clockrate            = *(DWORD *) (rom->Header + 0x04);
    rom->Info.ProgramCounter       = *(DWORD *) (rom->Header + 0x08);
    rom->Info.Release              = *(DWORD *) (rom->Header + 0x0c);
    rom->Info.Crc1                 = *(DWORD *) (rom->Header + 0x10);
    rom->Info.Crc2                 = *(DWORD *) (rom->Header + 0x14);
    rom->Info.Unknown2             = *(DWORD *)(rom->Header + 0x18);
    rom->Info.Unknown2a             = *(DWORD *)(rom->Header + 0x1c);
    for(int i=0; i<20; i++)
        rom->Info.Name[i^0x03] = rom->Header[i+0x20];
	if(strncmp((char *)rom->Info.Name,"Wave Race 64 SE   ",18)==0)
	{
		theApp.m_EmuObj->m_IsWaveraceSE=true;
	}
    rom->Info.Unknown3             = *(BYTE *) (rom->Header + (0x34 ^ 0x03));
    rom->Info.Unknown4             = *(BYTE *) (rom->Header + (0x35 ^ 0x03));
    rom->Info.Unknown5             = *(BYTE *) (rom->Header + (0x36 ^ 0x03));
    rom->Info.Unknown6             = *(BYTE *) (rom->Header + (0x37 ^ 0x03));
    rom->Info.Unknown7             = *(BYTE *) (rom->Header + (0x38 ^ 0x03));
    rom->Info.Unknown8             = *(BYTE *) (rom->Header + (0x39 ^ 0x03));
    rom->Info.Unknown9             = *(BYTE *) (rom->Header + (0x3a ^ 0x03));
    rom->Info.ManufacturerId       = *(BYTE *) (rom->Header + (0x3b ^ 0x03));
    rom->Info.CartridgeId          = *(WORD *)(rom->Header + (0x3c ^ 0x03));
    rom->Info.CountryCode          = *(BYTE *) (rom->Header + (0x3e ^ 0x03));
    rom->Info.Unknown10            = *(BYTE *) (rom->Header + (0x3f ^ 0x03));
    rom->PrgCodeBaseOrig = rom->Info.ProgramCounter;
    rom->PrgCodeBase = rom->PrgCodeBaseOrig & 0x1fffffff;
*/   
	if(iRomFile)
	{
		iRomFile->Close();
		SafeDelete(iRomFile);
	}

	return(0);
} 


int iRomReadHeader(char *filename)
{   
	CFile *in;
    BYTE    endian_1;
    BYTE    endian_2;
    BYTE    endian_3;
    BYTE    endian_4;
   

	in=(CFile *)new CFile();
	BOOL result=in->Open(filename,CFile::modeRead);
	if(!result)
	{
		delete in;
		return(-1);
	}
	rom->Length=in->GetLength();
	rom->PrgCodeLength=rom->Length-0x1000;
	rom->Image=(unsigned char *)malloc(0x40);
	rom->Header=rom->Image;
	rom->BootCode=rom->Image+0x40;
	rom->PrgCode=(DWORD *)(rom->Image + 0x1000);

	WORD EndianChangeMode;

	in->Read(&endian_1,1);
	in->Read(&endian_2,1);
	in->Read(&endian_3,1);
	in->Read(&endian_4,1);
	if( (endian_1==0x37)&&(endian_2==0x80) )
	{
		EndianChangeMode=0;
	}
	else if( (endian_2==0x37)&&(endian_1==0x80) )
	{
		EndianChangeMode=1;
	}
	else if( (endian_3==0x37)&&(endian_4==0x80) )
	{
		EndianChangeMode=2;
	}
	else
	{
		in->Close();
		delete in;
		return(-2);
	}

	in->Seek(0,CFile::begin);
	int length=in->Read(rom->Image,0x40);
	if(length!=0x40)
	{
		in->Close();
		delete in;
        return(-1);
	}
	in->Close();
	delete in;
   

	if(EndianChangeMode!=2)
		iRomChangeRomEndian(EndianChangeMode,0x40);

	rom->Info.Validation           = *(WORD *)(rom->Header + (0x00 ^ 0x02));
    rom->Info.Compression          = *(BYTE *) (rom->Header + (0x02 ^ 0x03));
    rom->Info.Unknown1             = *(BYTE *) (rom->Header + (0x03 ^ 0x03));
    rom->Info.Clockrate            = *(DWORD *) (rom->Header + 0x04);
    rom->Info.ProgramCounter       = *(DWORD *) (rom->Header + 0x08);
    rom->Info.Release              = *(DWORD *) (rom->Header + 0x0c);
    rom->Info.Crc1                 = *(DWORD *) (rom->Header + 0x10);
    rom->Info.Crc2                 = *(DWORD *) (rom->Header + 0x14);
    rom->Info.Unknown2             = *(DWORD *)(rom->Header + 0x18);
    rom->Info.Unknown2a             = *(DWORD *)(rom->Header + 0x1c);
    for(int i=0; i<20; i++)
        rom->Info.Name[i^0x03] = rom->Header[i+0x20];
    rom->Info.Unknown3             = *(BYTE *) (rom->Header + (0x34 ^ 0x03));
    rom->Info.Unknown4             = *(BYTE *) (rom->Header + (0x35 ^ 0x03));
    rom->Info.Unknown5             = *(BYTE *) (rom->Header + (0x36 ^ 0x03));
    rom->Info.Unknown6             = *(BYTE *) (rom->Header + (0x37 ^ 0x03));
    rom->Info.Unknown7             = *(BYTE *) (rom->Header + (0x38 ^ 0x03));
    rom->Info.Unknown8             = *(BYTE *) (rom->Header + (0x39 ^ 0x03));
    rom->Info.Unknown9             = *(BYTE *) (rom->Header + (0x3a ^ 0x03));
    rom->Info.ManufacturerId       = *(BYTE *) (rom->Header + (0x3b ^ 0x03));
    rom->Info.CartridgeId          = *(WORD *)(rom->Header + (0x3c ^ 0x03));
    rom->Info.CountryCode          = *(BYTE *) (rom->Header + (0x3e ^ 0x03));
    rom->Info.Unknown10            = *(BYTE *) (rom->Header + (0x3f ^ 0x03));
    rom->PrgCodeBaseOrig = rom->Info.ProgramCounter;
    rom->PrgCodeBase = rom->PrgCodeBaseOrig & 0x1fffffff;

	return(0);
} 

   
void iRomChangeRomEndian(WORD Mode,DWORD Length)
{
    DWORD     i;
    char    change_endian[4];



    for(i=0; i<Length; i+=4)
    {
        change_endian[0] = rom->Image[i+0];
        change_endian[1] = rom->Image[i+1];
        change_endian[2] = rom->Image[i+2];
        change_endian[3] = rom->Image[i+3];
		if(Mode==0)
		{
			rom->Image[i+0] = change_endian[2];
			rom->Image[i+1] = change_endian[3];
			rom->Image[i+2] = change_endian[0];
			rom->Image[i+3] = change_endian[1];
		}
		else
		{
			rom->Image[i+0] = change_endian[3];
			rom->Image[i+1] = change_endian[2];
			rom->Image[i+2] = change_endian[1];
			rom->Image[i+3] = change_endian[0];
		}
	}
} 
   
void iRomChangeRomEndianEx(WORD Mode,DWORD Length,DWORD Offset)
{
    DWORD     i;
    char    change_endian[4];



    for(i=0; i<Length; i+=4)
    {
        change_endian[0] = rom->Image[i+0+Offset];
        change_endian[1] = rom->Image[i+1+Offset];
        change_endian[2] = rom->Image[i+2+Offset];
        change_endian[3] = rom->Image[i+3+Offset];
		if(Mode==0)
		{
			rom->Image[i+0+Offset] = change_endian[2];
			rom->Image[i+1+Offset] = change_endian[3];
			rom->Image[i+2+Offset] = change_endian[0];
			rom->Image[i+3+Offset] = change_endian[1];
		}
		else
		{
			rom->Image[i+0+Offset] = change_endian[3];
			rom->Image[i+1+Offset] = change_endian[2];
			rom->Image[i+2+Offset] = change_endian[1];
			rom->Image[i+3+Offset] = change_endian[0];
		}
	}
} 

void iRomSetupPageMap()
{
	iRomNumPages=rom->Length/ROM_PAGE_SIZE;	
	iRomPageMap=(BYTE *)malloc(iRomNumPages*1);
	memset(iRomPageMap,0,iRomNumPages);
	for(DWORD p=0;p<10;p++)
		iRomReadPage(p);
}

void iRomReadPage(WORD PageNum)
{
	return;

	if(iRomPageMap[PageNum]) return;
	if(PageNum>iRomNumPages) return;
	theApp.LogMessage("Reading page %d",PageNum);

	DWORD Offset=PageNum*ROM_PAGE_SIZE;
	iRomFile->Seek(Offset,CFile::begin);
	iRomFile->Read(rom->Image+Offset,ROM_PAGE_SIZE);
	iRomPageMap[PageNum]=1;
	if(EndianChangeMode!=2)
		iRomChangeRomEndianEx(EndianChangeMode,ROM_PAGE_SIZE,Offset);
}

void iRomMapCheck(DWORD Offset,DWORD Length)
{
	return;

	DWORD PageNum=Offset/ROM_PAGE_SIZE;
	DWORD NumPages=(Length/ROM_PAGE_SIZE)+2;
	for(DWORD page=PageNum;page<PageNum+NumPages;page++)
		iRomReadPage(page);
}
