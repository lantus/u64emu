#ifndef iATA_H
#define iATA_H

void iATAConstruct();
void iATADestruct();
bool iATAOpen();
void iATAClose();
void iATAUpdate();
void iATAInitParams();
void iATAReadSectors();
void iATAWriteSectors();
void iATADriveIdentify();
BYTE *iATADataRead();



#endif
