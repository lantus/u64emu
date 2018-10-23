// CaptureMap.cpp : implementation file
//

#include "stdafx.h"
#include "ki.h"
#include "CaptureMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaptureMap dialog


CCaptureMap::CCaptureMap(CWnd* pParent /*=NULL*/)
	: CDialog(CCaptureMap::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCaptureMap)
	m_MapText = _T("");
	m_ModifyText = _T("");
	//}}AFX_DATA_INIT
}


void CCaptureMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaptureMap)
	DDX_Text(pDX, IDC_MapText, m_MapText);
	DDX_Text(pDX, IDC_ModifyText, m_ModifyText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaptureMap, CDialog)
	//{{AFX_MSG_MAP(CCaptureMap)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaptureMap message handlers

void CCaptureMap::OnOK() 
{
	CDialog::OnOK();
}

void CCaptureMap::OnTimer(UINT nIDEvent) 
{
	char Text[255];
	WORD Device;
	WORD Index;
	DWORD results;
	results=m_InputDevice->Scan(0,&Device,&Index,Text);
	if(results!=0xffffffff)
	{
		m_MapText=Text;
		m_LastMap=results;
		UpdateData(false);
	}
}

BOOL CCaptureMap::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetTimer(1002,100,NULL);
	m_InputDevice=(mmDirectInputDevice *)new mmDirectInputDevice();
	m_InputDevice->Create(DISCL_FOREGROUND,this->m_hWnd);
	m_InputDevice->Open();
	m_ModifyText=m_KeyText;
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCaptureMap::DestroyWindow() 
{
	KillTimer(1002);
	m_InputDevice->Close();
	delete m_InputDevice;
	return CDialog::DestroyWindow();
}
