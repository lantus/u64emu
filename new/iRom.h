#ifndef iRom_h
#define iRom_h

extern void iRomConstruct();
extern void iRomDestruct();
extern int iRomReadImage(char *filename);
extern int iRomReadHeader(char *filename);
extern void iRomChangeRomEndian(WORD Mode,DWORD Length);
extern void iRomMapCheck(DWORD Offset,DWORD Length);
extern void iRomReadPage(WORD PageNum);
extern void iRomSetupPageMap();
extern void iRomChangeRomEndianEx(WORD Mode,DWORD Length,DWORD Offset);

#endif
