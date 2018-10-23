// RomSet.cpp : implementation file
//

#include "stdafx.h"
#include "ki.h"
#include "RomSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRomSet dialog


CRomSet::CRomSet(CWnd* pParent /*=NULL*/)
	: CDialog(CRomSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRomSet)
	m_HDImage = _T("");
	m_Rom1 = _T("");
	m_Rom2 = _T("");
	m_Rom3 = _T("");
	m_Rom4 = _T("");
	m_Rom5 = _T("");
	m_Rom6 = _T("");
	m_Rom7 = _T("");
	m_Rom8 = _T("");
	m_Rom9 = _T("");
	//}}AFX_DATA_INIT
}


void CRomSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRomSet)
	DDX_Control(pDX, IDC_SetName, m_SetName);
	DDX_Text(pDX, IDC_HDImage, m_HDImage);
	DDX_Text(pDX, IDC_Rom1, m_Rom1);
	DDX_Text(pDX, IDC_Rom2, m_Rom2);
	DDX_Text(pDX, IDC_Rom3, m_Rom3);
	DDX_Text(pDX, IDC_Rom4, m_Rom4);
	DDX_Text(pDX, IDC_Rom5, m_Rom5);
	DDX_Text(pDX, IDC_Rom6, m_Rom6);
	DDX_Text(pDX, IDC_Rom7, m_Rom7);
	DDX_Text(pDX, IDC_Rom8, m_Rom8);
	DDX_Text(pDX, IDC_Rom9, m_Rom9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRomSet, CDialog)
	//{{AFX_MSG_MAP(CRomSet)
	ON_BN_CLICKED(IDC_Browse1, OnBrowse1)
	ON_BN_CLICKED(IDC_Browse2, OnBrowse2)
	ON_BN_CLICKED(IDC_Browse3, OnBrowse3)
	ON_BN_CLICKED(IDC_Browse4, OnBrowse4)
	ON_BN_CLICKED(IDC_Browse5, OnBrowse5)
	ON_BN_CLICKED(IDC_Browse6, OnBrowse6)
	ON_BN_CLICKED(IDC_Browse7, OnBrowse7)
	ON_BN_CLICKED(IDC_Browse8, OnBrowse8)
	ON_BN_CLICKED(IDC_Browse9, OnBrowse9)
	ON_BN_CLICKED(IDC_HDBrowse, OnHDBrowse)
	ON_CBN_SELENDOK(IDC_SetName, OnSelendokSetName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRomSet message handlers


void CRomSet::OnBrowse1() 
{
	BrowseFile(&m_Rom1,"*.");
	UpdateData(false);
}

void CRomSet::OnBrowse2() 
{
	BrowseFile(&m_Rom2,"*.");
	UpdateData(false);
}

void CRomSet::OnBrowse3() 
{
	BrowseFile(&m_Rom3,"*.");
	UpdateData(false);
}

void CRomSet::OnBrowse4() 
{
	BrowseFile(&m_Rom4,"*.");
	UpdateData(false);
}

void CRomSet::OnBrowse5() 
{
	BrowseFile(&m_Rom5,"*.");
	UpdateData(false);
}

void CRomSet::OnBrowse6() 
{
	BrowseFile(&m_Rom6,"*.");
	UpdateData(false);
}

void CRomSet::OnBrowse7() 
{
	BrowseFile(&m_Rom7,"*.");
	UpdateData(false);
}

void CRomSet::OnBrowse8() 
{
	BrowseFile(&m_Rom8,"*.");
	UpdateData(false);
}

void CRomSet::OnBrowse9() 
{
	BrowseFile(&m_Rom9,"*.");
	UpdateData(false);
}

void CRomSet::OnHDBrowse() 
{
	BrowseFile(&m_HDImage,"*.");
	UpdateData(false);
}

bool CRomSet::BrowseFile(CString *filename,char *ext)
{
	CFileDialog *tmp=(CFileDialog *)new CFileDialog(true);
	int result=tmp->DoModal();
	if(result==IDOK)
	{
		CString tmps=tmp->GetPathName();
		*filename=tmps;
//		strcpy(filename,tmps.GetBuffer(256));
		return(true);
	}
	return(false);
}

void CRomSet::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CRomSet::OnOK() 
{
	m_SetNum=m_SetName.GetCurSel()+1;
	CDialog::OnOK();
}

BOOL CRomSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_SetName.SetCurSel(m_SetNum-1);
	UpdateData(FALSE);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRomSet::OnSelendokSetName() 
{
	m_SetNum=m_SetName.GetCurSel()+1;
	if(m_SetNum==KI1)
	{
		m_Rom1=theApp.GetProfileString("Roms","U98","\\u64emu\\roms\\u98-l15d");
		m_Rom2=theApp.GetProfileString("Roms","U10","\\u64emu\\roms\\u10-l1");
		m_Rom3=theApp.GetProfileString("Roms","U11","\\u64emu\\roms\\u11-l1");
		m_Rom4=theApp.GetProfileString("Roms","U12","\\u64emu\\roms\\u12-l1");
		m_Rom5=theApp.GetProfileString("Roms","U13","\\u64emu\\roms\\u13-l1");
		m_Rom6=theApp.GetProfileString("Roms","U33","\\u64emu\\roms\\u33-l1");
		m_Rom7=theApp.GetProfileString("Roms","U34","\\u64emu\\roms\\u34-l1");
		m_Rom8=theApp.GetProfileString("Roms","U35","\\u64emu\\roms\\u35-l1");
		m_Rom9=theApp.GetProfileString("Roms","U36","\\u64emu\\roms\\u36-l1");
		m_HDImage=theApp.GetProfileString("HardDisk","HD Image","\\u64emu\\hdimage\\ki.img");
	}
	else
	{
		m_Rom1=theApp.GetProfileString("KI2_Roms","U98","\\u64emu\\roms\\gamerom");
		m_Rom2=theApp.GetProfileString("KI2_Roms","U10","\\u64emu\\roms\\u10sound");
		m_Rom3=theApp.GetProfileString("KI2_Roms","U11","\\u64emu\\roms\\u11sound");
		m_Rom4=theApp.GetProfileString("KI2_Roms","U12","\\u64emu\\roms\\u12sound");
		m_Rom5=theApp.GetProfileString("KI2_Roms","U13","\\u64emu\\roms\\u13sound");
		m_Rom6=theApp.GetProfileString("KI2_Roms","U33","\\u64emu\\roms\\u33sound");
		m_Rom7=theApp.GetProfileString("KI2_Roms","U34","\\u64emu\\roms\\u34sound");
		m_Rom8=theApp.GetProfileString("KI2_Roms","U35","\\u64emu\\roms\\u35sound");
		m_Rom9=theApp.GetProfileString("KI2_Roms","U36","\\u64emu\\roms\\u36sound");
		m_HDImage=theApp.GetProfileString("KI2_HardDisk","HD Image","\\u64emu\\hdimage\\ki2.img");
	}
	UpdateData(false);
}
