#ifndef GLOBAL1_H
#define GLOBAL1_H
 
#include <stdint.h>
#include <switch.h>
 
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

typedef void *HANDLE;
typedef long HRESULT;
 
typedef long intptr_t;
typedef unsigned long uintptr_t;
 
typedef unsigned char						UINT8;
typedef signed char 						INT8;
typedef unsigned short						UINT16;
typedef signed short						INT16;
typedef unsigned long						UINT32;
typedef signed long							INT32;
typedef long long							INT64;
typedef unsigned long long					UINT64; 
 
typedef UINT8 BYTE;
typedef UINT16 WORD;
typedef UINT32 DWORD;
 
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

#define QWORD       UINT64
#define sQWORD      INT64  
#define sDWORD      INT32
#define HWORD       unsigned int16
#define sHWORD      signed int16
#define sBYTE		signed char
#define BIT         unsigned char        
#define BOOL		int

#define SafeFree(ptr) if(ptr!=NULL) { free(ptr); ptr=NULL; }
#define SafeDelete(ptr) if(ptr!=NULL) { delete ptr; ptr=NULL; }
#define SafeRelease(ptr) if(ptr!=NULL) { ptr->Release();  }
#define SafeReleaseNoCheck(ptr) if(ptr!=NULL) { ptr->Release(); }

#define ToUpper(string) { char *dst=string; char *src=string; while(*src) { *(dst++)=toupper(*(src++)); } }

#endif
