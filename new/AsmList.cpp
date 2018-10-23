// AsmList.cpp : implementation file
//

#include "stdafx.h"
#include "ki.h"
#include "AsmList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAsmList

CAsmList::CAsmList()
{
	m_Font=0;
}

CAsmList::~CAsmList()
{
	if(m_Font)
		DeleteObject(m_Font);
}


BEGIN_MESSAGE_MAP(CAsmList, CListBox)
	//{{AFX_MSG_MAP(CAsmList)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsmList message handlers

void CAsmList::InitDCStuff(HDC hDC)
{

	int nHeight = -MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72);
 	m_Font=CreateFont(nHeight,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		FF_MODERN,"Courier New");
}

void CAsmList::SubDraw(HDC hDC,char *buf,RECT *where)
{
	SIZE size;
	char buf2[256];
	char *src=buf;
	char *dst=buf2;

	SetTextColor(hDC,0x7f);
	SetBkColor(hDC,0xffffff);
	while((*src)&&((*src)!=' '))
	{
		*(dst++)=*(src++);
	}
	*(dst++)=0;
	DrawTextEx(hDC,buf2,strlen(buf2),where,DT_LEFT,0);
	GetTextExtentPoint32(hDC,buf2,strlen(buf2),&size);
	where->left+=size.cx;
	SetTextColor(hDC,0x00);
	SetBkColor(hDC,0xffffff);
	if(*src)
	{
		dst=buf2;
		while((*src)&&((*src)!='$'))
		{
			*(dst++)=*(src++);
		}
		*(dst++)=0;
		DrawTextEx(hDC,buf2,strlen(buf2),where,DT_LEFT,0);
		GetTextExtentPoint32(hDC,buf2,strlen(buf2),&size);
		where->left+=size.cx;
		SetTextColor(hDC,0x007f00);
		SetBkColor(hDC,0xffffff);
		if(*src)
		{
			bool done=false;
			dst=buf2;
			while(!done)
			{
				switch(*src)
				{
				default:
				case 0:
					{
						done=true;
						break;
					}
				case '$':
				case 0x30:
				case 0x31:
				case 0x32:
				case 0x33:
				case 0x34:
				case 0x35:
				case 0x36:
				case 0x37:
				case 0x38:
				case 0x39:
				case 0x41:
				case 0x42:
				case 0x43:
				case 0x44:
				case 0x45:
				case 0x46:
				case 0x47:
					{
						*(dst++)=*(src++);
						break;
					}
				}
			}
			*(dst++)=0;
			DrawTextEx(hDC,buf2,strlen(buf2),where,DT_LEFT,0);
			GetTextExtentPoint32(hDC,buf2,strlen(buf2),&size);
			where->left+=size.cx;
			SetTextColor(hDC,0x00);
			SetBkColor(hDC,0xffffff);
			if(*src)
			{
				dst=buf2;
				while((*src)&&((*src)!='$'))
				{
					*(dst++)=*(src++);
				}
				*(dst++)=0;
				DrawTextEx(hDC,buf2,strlen(buf2),where,DT_LEFT,0);
				GetTextExtentPoint32(hDC,buf2,strlen(buf2),&size);
			}
		}
	}
}

void CAsmList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	HDC hDC=lpDrawItemStruct->hDC;

	switch(lpDrawItemStruct->itemAction)
	{
	case ODA_DRAWENTIRE:
		{
			if(m_Font==0)
				InitDCStuff(hDC);
			HFONT oldFont=(HFONT) SelectObject(hDC,m_Font);
			if(lpDrawItemStruct->itemID>=0)
			{
				char buf[256];
				GetText(lpDrawItemStruct->itemID,buf);
				switch((lpDrawItemStruct->itemData&0xff0000)>>16)
				{
				case 0:	// normal
					{
						SubDraw(hDC,buf,&lpDrawItemStruct->rcItem);
						return;
						SetTextColor(hDC,0x0);
						SetBkColor(hDC,0xffffff);
						break;
					}
				case 1:	// breakpoint
					{
						SetTextColor(hDC,0x0000ff);
						SetBkColor(hDC,0xffffff);
						break;
					}
				case 2:	// normal, active (selected)
					{
						SetTextColor(hDC,0x0ffffff);
						SetBkColor(hDC,0xff0000);
						break;
					}
				case 3: // breakpoint, active (selected)
					{
						SetTextColor(hDC,0x0ff00ff);
						SetBkColor(hDC,0xff0000);
						break;
					}
				}
				DrawTextEx(hDC,buf,strlen(buf),&lpDrawItemStruct->rcItem,DT_LEFT,0);
			}
			SelectObject(hDC,oldFont);
			break;
		}
	case ODA_FOCUS:
		{
			break;
		}
	case ODA_SELECT:
		{
			break;
		}
	}

}
