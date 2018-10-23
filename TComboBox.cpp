// TComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "ki.h"
#include "TComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TComboBox

TComboBox::TComboBox()
{
	m_Font=0;
	m_BackBrush=0;
	m_BackColor=0x2f2f2f;
}

TComboBox::~TComboBox()
{
	if(m_Font)
		DeleteObject(m_Font);
	delete m_BackBrush;
}


void TComboBox::InitDCStuff(HDC hDC)
{

	int nHeight = -MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72);
 	m_Font=CreateFont(nHeight,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		FF_MODERN,"Courier New");
}


BEGIN_MESSAGE_MAP(TComboBox, CComboBox)
	//{{AFX_MSG_MAP(TComboBox)
	ON_WM_ERASEBKGND()
	ON_WM_DRAWCLIPBOARD()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TComboBox message handlers

void TComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
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
				GetLBText(lpDrawItemStruct->itemID,buf);
				SetTextColor(hDC,0x0);
				SetBkMode(hDC,TRANSPARENT);
				DrawTextEx(hDC,buf,strlen(buf),&lpDrawItemStruct->rcItem,DT_LEFT,0);
			}
			SelectObject(hDC,oldFont);
			break;
		}
	case ODA_FOCUS:
	case ODA_SELECT:
		{
			break;
		}
	default:
		{
			break;
		}
	}

}

BOOL TComboBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
//	return CComboBox::OnEraseBkgnd(pDC);
	return true;
}

void TComboBox::OnDrawClipboard() 
{
//	CComboBox::OnDrawClipboard();
	
	// TODO: Add your message handler code here
	
}
