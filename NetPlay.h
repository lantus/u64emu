#ifndef NetPlayH
#define NetPlayH

#include "dplay.h"
#include "dplobby.h"



HRESULT NetPlayCreateAddress(VOID **ppAddress,DWORD *pdwAddressSize);
bool NetPlayCreate();
void NetPlayDestroy();
char *ErrorToString(HRESULT errornum);







#endif
