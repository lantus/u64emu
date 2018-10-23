// AdvSettings.cpp : implementation file
//

#include "stdafx.h"
#include "ki.h"
#include "AdvSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvSettings dialog


CAdvSettings::CAdvSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvSettings)
	m_FrameRate = 0;
	m_FullScreen = FALSE;
	m_NoHLE = FALSE;
	m_ScanLines = FALSE;
	//}}AFX_DATA_INIT
}


void CAdvSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvSettings)
	DDX_Control(pDX, IDC_ScreenResolution, m_ScreenResolution);
	DDX_Text(pDX, IDC_FrameRate, m_FrameRate);
	DDV_MinMaxInt(pDX, m_FrameRate, 40, 75);
	DDX_Check(pDX, IDC_FullScreen, m_FullScreen);
	DDX_Check(pDX, IDC_FullHLE, m_NoHLE);
	DDX_Check(pDX, IDC_EmulateScanlines, m_ScanLines);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvSettings, CDialog)
	//{{AFX_MSG_MAP(CAdvSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvSettings message handlers


void CAdvSettings::OnCancel() 
{
	CDialog::OnCancel();
}

void CAdvSettings::OnOK() 
{
	UpdateData(TRUE);
	m_ScreenRes=m_ScreenResolution.GetCurSel();
	if(m_ScreenRes>4) m_ScreenRes=0;
	CDialog::OnOK();
}

BOOL CAdvSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_ScreenResolution.SetCurSel(m_ScreenRes);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
