// DIPSettings.cpp : implementation file
//

#include "stdafx.h"
#include "ki.h"
#include "DIPSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDIPSettings dialog


CDIPSettings::CDIPSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDIPSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDIPSettings)
	m_DIP1SW1 = _T("");
	m_DIP1SW2 = _T("");
	m_DIP1SW3 = _T("");
	m_DIP1SW4 = _T("");
	m_DIP1SW5 = _T("");
	m_DIP1SW6 = _T("");
	m_DIP1SW7 = _T("");
	m_DIP1SW8 = _T("");
	m_DIP2SW1 = _T("");
	m_DIP2SW2 = _T("");
	m_DIP2SW3 = _T("");
	m_DIP2SW4 = _T("");
	m_DIP2SW5 = _T("");
	m_DIP2SW6 = _T("");
	m_DIP2SW7 = _T("");
	m_DIP2SW8 = _T("");
	//}}AFX_DATA_INIT
}


void CDIPSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDIPSettings)
	DDX_Text(pDX, IDC_DIP1_Text1, m_DIP1SW1);
	DDX_Text(pDX, IDC_DIP1_Text2, m_DIP1SW2);
	DDX_Text(pDX, IDC_DIP1_Text3, m_DIP1SW3);
	DDX_Text(pDX, IDC_DIP1_Text4, m_DIP1SW4);
	DDX_Text(pDX, IDC_DIP1_Text5, m_DIP1SW5);
	DDX_Text(pDX, IDC_DIP1_Text6, m_DIP1SW6);
	DDX_Text(pDX, IDC_DIP1_Text7, m_DIP1SW7);
	DDX_Text(pDX, IDC_DIP1_Text8, m_DIP1SW8);
	DDX_Text(pDX, IDC_DIP2_Text1, m_DIP2SW1);
	DDX_Text(pDX, IDC_DIP2_Text2, m_DIP2SW2);
	DDX_Text(pDX, IDC_DIP2_Text3, m_DIP2SW3);
	DDX_Text(pDX, IDC_DIP2_Text4, m_DIP2SW4);
	DDX_Text(pDX, IDC_DIP2_Text5, m_DIP2SW5);
	DDX_Text(pDX, IDC_DIP2_Text6, m_DIP2SW6);
	DDX_Text(pDX, IDC_DIP2_Text7, m_DIP2SW7);
	DDX_Text(pDX, IDC_DIP2_Text8, m_DIP2SW8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDIPSettings, CDialog)
	//{{AFX_MSG_MAP(CDIPSettings)
	ON_BN_CLICKED(IDC_DIP1_SW1, OnDip1Sw1)
	ON_BN_CLICKED(IDC_DIP1_SW2, OnDip1Sw2)
	ON_BN_CLICKED(IDC_DIP1_SW3, OnDip1Sw3)
	ON_BN_CLICKED(IDC_DIP1_SW4, OnDip1Sw4)
	ON_BN_CLICKED(IDC_DIP1_SW5, OnDip1Sw5)
	ON_BN_CLICKED(IDC_DIP1_SW6, OnDip1Sw6)
	ON_BN_CLICKED(IDC_DIP1_SW7, OnDip1Sw7)
	ON_BN_CLICKED(IDC_DIP1_SW8, OnDip1Sw8)
	ON_BN_CLICKED(IDC_DIP2_SW1, OnDip2Sw1)
	ON_BN_CLICKED(IDC_DIP2_SW2, OnDip2Sw2)
	ON_BN_CLICKED(IDC_DIP2_SW3, OnDip2Sw3)
	ON_BN_CLICKED(IDC_DIP2_SW4, OnDip2Sw4)
	ON_BN_CLICKED(IDC_DIP2_SW5, OnDip2Sw5)
	ON_BN_CLICKED(IDC_DIP2_SW6, OnDip2Sw6)
	ON_BN_CLICKED(IDC_DIP2_SW7, OnDip2Sw7)
	ON_BN_CLICKED(IDC_DIP2_SW8, OnDip2Sw8)
	ON_BN_CLICKED(IDC_Default, OnDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDIPSettings message handlers

void CDIPSettings::OnOK() 
{
	CDialog::OnOK();
}

void CDIPSettings::OnCancel() 
{
	CDialog::OnCancel();
}


void CDIPSettings::OnDefault() 
{
	m_DIP1=0x0;
	m_DIP2=0x0e;
	UnPackSettings();
}

void CDIPSettings::DIPSWDesc()
{
	switch(m_DIP1&0x3)
	{
	case 0:
		{
			m_DIP1SW1="High Blood";
			m_DIP1SW2="";
			break;
		}
	case 1:
		{
			m_DIP1SW1="Medium Blood";
			m_DIP1SW2="";
			break;
		}
	case 2:
		{
			m_DIP1SW1="Low Blood";
			m_DIP1SW2="";
			break;
		}
	case 3:
		{
			m_DIP1SW1="No Blood";
			m_DIP1SW2="";
			break;
		}
	}
	if(m_DIP1&4)
		m_DIP1SW3="No Attract Sounds";
	else
		m_DIP1SW3="Attract Sounds";

	if(m_DIP1&8)
		m_DIP1SW4="No Finishing Moves";
	else
		m_DIP1SW4="Finishing Moves";

	if(m_DIP1&16)
		m_DIP1SW5="No Display Warnings";
	else
		m_DIP1SW5="Display Warnings";

	if(m_DIP1&32)
		m_DIP1SW6="White Blood";
	else
		m_DIP1SW6="Red Blood";

	m_DIP1SW7="Unused";
	m_DIP1SW8="Unused";


	switch((m_DIP2>>4)&3)
	{
	case 0:
		{
			m_DIP2SW5="USA";
			break;
		}
	case 1:
		{
			m_DIP2SW5="Germany";
			break;
		}
	case 2:
		{
			m_DIP2SW5="France";
			break;
		}
	case 3:
		{
			m_DIP2SW5="USA";
			break;
		}
	}
	if(m_DIP2&1)
		m_DIP2SW1="Disk Coinage";
	else
		m_DIP2SW1="Dip Switch Coinage";
	switch((m_DIP2>>1)&7)
	{
	case 7:
		{
			m_DIP2SW2="Free Play";
			m_DIP2SW3="";
			m_DIP2SW4="";
			break;
		}
	default:
		{
			m_DIP2SW2="On for Free Play";
			m_DIP2SW3="On for Free Play";
			m_DIP2SW4="On for Free Play";
		}
	}
	if(m_DIP2&64)
		m_DIP2SW7="Two Counters";
	else
		m_DIP2SW7="One Counter";

	if(m_DIP2&128)
		m_DIP2SW8="Test Mode";
	else
		m_DIP2SW8="Play Mode";
	theApp.m_DIPS=(WORD)(m_DIP1|(m_DIP2<<8));
	UpdateData(false);
}

void CDIPSettings::OnDip1Sw1() 
{
	m_DIP1^=0x01;
	DIPSWDesc();
}

void CDIPSettings::OnDip1Sw2() 
{
	m_DIP1^=0x02;
	DIPSWDesc();
}

void CDIPSettings::OnDip1Sw3() 
{
	m_DIP1^=0x04;
	DIPSWDesc();
}

void CDIPSettings::OnDip1Sw4() 
{
	m_DIP1^=0x08;
	DIPSWDesc();
}

void CDIPSettings::OnDip1Sw5() 
{
	m_DIP1^=0x10;
	DIPSWDesc();
}

void CDIPSettings::OnDip1Sw6() 
{
	m_DIP1^=0x20;
	DIPSWDesc();
}

void CDIPSettings::OnDip1Sw7() 
{
	m_DIP1^=0x40;
	DIPSWDesc();
}

void CDIPSettings::OnDip1Sw8() 
{
	m_DIP1^=0x80;
	DIPSWDesc();
}


void CDIPSettings::OnDip2Sw1() 
{
	m_DIP2^=0x01;
	DIPSWDesc();
}

void CDIPSettings::OnDip2Sw2() 
{
	m_DIP2^=0x02;
	DIPSWDesc();
}

void CDIPSettings::OnDip2Sw3() 
{
	m_DIP2^=0x04;
	DIPSWDesc();
}

void CDIPSettings::OnDip2Sw4() 
{
	m_DIP2^=0x08;
	DIPSWDesc();
}

void CDIPSettings::OnDip2Sw5() 
{
	m_DIP2^=0x10;
	DIPSWDesc();
}

void CDIPSettings::OnDip2Sw6() 
{
	m_DIP2^=0x20;
	DIPSWDesc();
}

void CDIPSettings::OnDip2Sw7() 
{
	m_DIP2^=0x40;
	DIPSWDesc();
}

void CDIPSettings::OnDip2Sw8() 
{
	m_DIP2^=0x80;
	DIPSWDesc();
}

void CDIPSettings::UnPackSettings()
{
	if(m_DIP1&0x1) CheckDlgButton(IDC_DIP1_SW1,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP1_SW1,BST_UNCHECKED);
	if(m_DIP1&0x2) CheckDlgButton(IDC_DIP1_SW2,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP1_SW2,BST_UNCHECKED);
	if(m_DIP1&0x4) CheckDlgButton(IDC_DIP1_SW3,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP1_SW3,BST_UNCHECKED);
	if(m_DIP1&0x8) CheckDlgButton(IDC_DIP1_SW4,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP1_SW4,BST_UNCHECKED);
	if(m_DIP1&0x10) CheckDlgButton(IDC_DIP1_SW5,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP1_SW5,BST_UNCHECKED);
	if(m_DIP1&0x20) CheckDlgButton(IDC_DIP1_SW6,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP1_SW6,BST_UNCHECKED);
	if(m_DIP1&0x40) CheckDlgButton(IDC_DIP1_SW7,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP1_SW7,BST_UNCHECKED);
	if(m_DIP1&0x80) CheckDlgButton(IDC_DIP1_SW8,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP1_SW8,BST_UNCHECKED);

	if(m_DIP2&0x1) CheckDlgButton(IDC_DIP2_SW1,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP2_SW1,BST_UNCHECKED);
	if(m_DIP2&0x2) CheckDlgButton(IDC_DIP2_SW2,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP2_SW2,BST_UNCHECKED);
	if(m_DIP2&0x4) CheckDlgButton(IDC_DIP2_SW3,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP2_SW3,BST_UNCHECKED);
	if(m_DIP2&0x8) CheckDlgButton(IDC_DIP2_SW4,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP2_SW4,BST_UNCHECKED);
	if(m_DIP2&0x10) CheckDlgButton(IDC_DIP2_SW5,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP2_SW5,BST_UNCHECKED);
	if(m_DIP2&0x20) CheckDlgButton(IDC_DIP2_SW6,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP2_SW6,BST_UNCHECKED);
	if(m_DIP2&0x40) CheckDlgButton(IDC_DIP2_SW7,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP2_SW7,BST_UNCHECKED);
	if(m_DIP2&0x80) CheckDlgButton(IDC_DIP2_SW8,BST_CHECKED);
	else  CheckDlgButton(IDC_DIP2_SW8,BST_UNCHECKED);

	DIPSWDesc();
	UpdateData(false);
}

BOOL CDIPSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UnPackSettings();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
