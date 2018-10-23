#ifndef GLOBAL1_H
#define GLOBAL1_H

#define DX7

#include "mmsystem.h"
//#include "mmio.h"
#include "ddraw.h"
#include "d3d.h"
#include "d3dcaps.h"
#include "d3dtypes.h"
#include "dsound.h"
#include "d3dmacs.h"
#include "dinput.h"


#define _PI  3.14159265359f
#define _PI2  6.28318530718f
//#define DO_ALL_MATH

#undef RELEASE
#ifdef __cplusplus
#define RELEASE(x) if (x != NULL) {x->Release(); x = NULL;}
#else
#define RELEASE(x) if (x != NULL) {x->lpVtbl->Release(x); x = NULL;}
#endif


typedef struct _tgaHeader{
    BYTE IDLength;
    BYTE CMapType;
    BYTE ImgType;
    BYTE CMapStartLo;
    BYTE CMapStartHi;
    BYTE CMapLengthLo;
    BYTE CMapLengthHi;
    BYTE CMapDepth;
    BYTE XOffSetLo;
    BYTE XOffSetHi;
    BYTE YOffSetLo;
    BYTE YOffSetHi;
    BYTE WidthLo;
    BYTE WidthHi;
    BYTE HeightLo;
    BYTE HeightHi;
    BYTE PixelDepth;
    BYTE ImageDescriptor;
} TgaHeader;

/* Definitions for image types. */
#define TGA_NULL	0
#define TGA_CMAP	1
#define TGA_TRUE	2
#define TGA_MONO	3
#define TGA_CMAP_RLE	9
#define TGA_TRUE_RLE	10
#define TGA_MONO_RLE	11



#define HACK_1(w) ((float)w-.5f)/(float)w;
#define HACK_0(w) 0.5f/(float)w;

#define QWORD      unsigned __int64   
#define sQWORD      signed __int64    
#define sDWORD      signed long
#define HWORD      unsigned __int16
#define sHWORD      signed __int16
#define sBYTE		signed char
#define BIT        unsigned char        


#define SafeFree(ptr) if(ptr!=NULL) { free(ptr); ptr=NULL; }
#define SafeDelete(ptr) if(ptr!=NULL) { delete ptr; ptr=NULL; }
#define SafeRelease(ptr) if(ptr!=NULL) { ptr->Release();  }
#define SafeReleaseNoCheck(ptr) if(ptr!=NULL) { ptr->Release(); }

#define ToUpper(string) { char *dst=string; char *src=string; while(*src) { *(dst++)=toupper(*(src++)); } }

#endif
