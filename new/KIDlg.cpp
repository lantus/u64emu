// KIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ki.h"
#include "KIDlg.h"
#include "global.h"
#include "ControlConfig.h"
#include "NetPlay.h"

#include "dasmMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKIDlg dialog

CKIDlg::CKIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKIDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKIDlg)
	m_DIP1SW1Text = _T("");
	m_DIP1SW2Text = _T("");
	m_DIP1SW3Text = _T("");
	m_DIP1SW4Text = _T("");
	m_DIP1SW5Text = _T("");
	m_DIP1SW6Text = _T("");
	m_DIP1SW7Text = _T("");
	m_DIP1SW8Text = _T("");
	m_DIP2SW1Text = _T("");
	m_DIP2SW2Text = _T("");
	m_DIP2SW3Text = _T("");
	m_DIP2SW4Text = _T("");
	m_DIP2SW5Text = _T("");
	m_DIP2SW6Text = _T("");
	m_DIP2SW7Text = _T("");
	m_DIP2SW8Text = _T("");
	m_FullScreen = FALSE;
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
	m_LockVSYNC = FALSE;
	m_ScanLines = FALSE;
	m_Skip = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_File=NULL;
	m_Image=NULL;

}

void CKIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKIDlg)
	DDX_Control(pDX, IDC_SetName, m_SetName);
	DDX_Text(pDX, IDC_DIP1_Text1, m_DIP1SW1Text);
	DDX_Text(pDX, IDC_DIP1_Text2, m_DIP1SW2Text);
	DDX_Text(pDX, IDC_DIP1_Text3, m_DIP1SW3Text);
	DDX_Text(pDX, IDC_DIP1_Text4, m_DIP1SW4Text);
	DDX_Text(pDX, IDC_DIP1_Text5, m_DIP1SW5Text);
	DDX_Text(pDX, IDC_DIP1_Text6, m_DIP1SW6Text);
	DDX_Text(pDX, IDC_DIP1_Text7, m_DIP1SW7Text);
	DDX_Text(pDX, IDC_DIP1_Text8, m_DIP1SW8Text);
	DDX_Text(pDX, IDC_DIP2_Text1, m_DIP2SW1Text);
	DDX_Text(pDX, IDC_DIP2_Text2, m_DIP2SW2Text);
	DDX_Text(pDX, IDC_DIP2_Text3, m_DIP2SW3Text);
	DDX_Text(pDX, IDC_DIP2_Text4, m_DIP2SW4Text);
	DDX_Text(pDX, IDC_DIP2_Text5, m_DIP2SW5Text);
	DDX_Text(pDX, IDC_DIP2_Text6, m_DIP2SW6Text);
	DDX_Text(pDX, IDC_DIP2_Text7, m_DIP2SW7Text);
	DDX_Text(pDX, IDC_DIP2_Text8, m_DIP2SW8Text);
	DDX_Check(pDX, IDC_FullScreen, m_FullScreen);
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
	DDX_Check(pDX, IDC_LockVSYNC, m_LockVSYNC);
	DDX_Check(pDX, IDC_ScanLines, m_ScanLines);
	DDX_Check(pDX, IDC_Skip, m_Skip);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKIDlg, CDialog)
	//{{AFX_MSG_MAP(CKIDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DoIt, OnDoIt)
	ON_BN_CLICKED(IDC_Browse1, OnBrowse1)
	ON_BN_CLICKED(IDC_Browse2, OnBrowse2)
	ON_BN_CLICKED(IDC_Browse3, OnBrowse3)
	ON_BN_CLICKED(IDC_Browse4, OnBrowse4)
	ON_BN_CLICKED(IDC_Browse5, OnBrowse5)
	ON_BN_CLICKED(IDC_Browse6, OnBrowse6)
	ON_BN_CLICKED(IDC_Browse7, OnBrowse7)
	ON_BN_CLICKED(IDC_Browse8, OnBrowse8)
	ON_BN_CLICKED(IDC_Browse9, OnBrowse9)
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
	ON_BN_CLICKED(IDC_FullScreen, OnFullScreen)
	ON_BN_CLICKED(IDC_HDBrowse, OnHDBrowse)
	ON_BN_CLICKED(IDC_Res1, OnRes1)
	ON_BN_CLICKED(IDC_Res2, OnRes2)
	ON_BN_CLICKED(IDC_Res3, OnRes3)
	ON_BN_CLICKED(IDC_Res4, OnRes4)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_Default, OnDefault)
	ON_BN_CLICKED(IDC_Input, OnInput)
	ON_BN_CLICKED(IDC_ScanLines, OnScanLines)
	ON_CBN_SELENDOK(IDC_SetName, OnSelendokSetName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKIDlg message handlers

BOOL CKIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	InitSettings();
	m_SetName.SetCurSel(gRomSet-1);
	if(theApp.m_Auto)
		PostMessage(WM_COMMAND,IDC_DoIt,0);

//	NetPlayCreate();
//	NetPlayDestroy();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKIDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKIDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CKIDlg::OnDoIt() 
{
	m_EmuObj=(CEmuObject *) new CEmuObject(AfxGetMainWnd());
	char filename[256];
	theApp.m_UCode=0;
	theApp.m_BootCode=0x3f;

//	UnPackSettings();
	VerifySettings();
	theApp.m_DIPS=m_DIP1|(m_DIP2<<8);
//	m_LockVSYNC=true;
	if(m_ScanLines)
	{
		m_ScreenRes=1;
		m_FullScreen=true;
	}
	theApp.m_FullScreen=m_FullScreen;
	theApp.m_ScreenRes=m_ScreenRes;
	theApp.m_LockOn=m_LockVSYNC;
	theApp.m_ScanLines=m_ScanLines;
	theApp.m_Skip=m_Skip;

	char *tmp=m_HDImage.GetBuffer(255);
	if(!DoesExists(tmp))
	{
		MessageBox("HD Image not found","Error Loading",MB_OK);
		return;
	}
	strcpy(theApp.m_HDImage,tmp);

	tmp=m_Rom2.GetBuffer(255);
	if(!DoesExists(tmp))
	{
		MessageBox("Audio Sample 1 Rom not found","Error Loading",MB_OK);
		return;
	}
	strcpy(theApp.m_ARom1,tmp);
	tmp=m_Rom3.GetBuffer(255);
	if(!DoesExists(tmp))
	{
		MessageBox("Audio Sample 2 Rom not found","Error Loading",MB_OK);
		return;
	}
	strcpy(theApp.m_ARom2,tmp);
	tmp=m_Rom4.GetBuffer(255);
	if(!DoesExists(tmp))
	{
		MessageBox("Audio Sample 3 Rom not found","Error Loading",MB_OK);
		return;
	}
	strcpy(theApp.m_ARom3,tmp);
	tmp=m_Rom5.GetBuffer(255);
	if(!DoesExists(tmp))
	{
		MessageBox("Audio Sample 4 Rom not found","Error Loading",MB_OK);
		return;
	}
	strcpy(theApp.m_ARom4,tmp);
	tmp=m_Rom6.GetBuffer(255);
	if(!DoesExists(tmp))
	{
		MessageBox("Audio Sample 5 Rom not found","Error Loading",MB_OK);
		return;
	}
	strcpy(theApp.m_ARom5,tmp);
	tmp=m_Rom7.GetBuffer(255);
	if(!DoesExists(tmp))
	{
		MessageBox("Audio Sample 6 Rom not found","Error Loading",MB_OK);
		return;
	}
	strcpy(theApp.m_ARom6,tmp);
	tmp=m_Rom8.GetBuffer(255);
	if(!DoesExists(tmp))
	{
		MessageBox("Audio Sample 7 Rom not found","Error Loading",MB_OK);
		return;
	}
	strcpy(theApp.m_ARom7,tmp);
	tmp=m_Rom9.GetBuffer(255);
	if(!DoesExists(tmp))
	{
		MessageBox("Audio Sample 8 Rom not found","Error Loading",MB_OK);
		return;
	}
	strcpy(theApp.m_ARom8,tmp);

	tmp=m_Rom1.GetBuffer(255);
	if(!DoesExists(tmp))
	{
		MessageBox("Main Rom not found","Error Loading",MB_OK);
		return;
	}


	HRESULT result=m_EmuObj->Init();
	if(result!=DD_OK)
	{
		SafeDelete(m_EmuObj);
	}
	else
	{
		m_Debug=true;
		m_Debug=false;
		m_EmuObj->ShowWindow(SW_SHOW);
		m_EmuObj->m_Debug=m_Debug;
		m_EmuObj->Emulate(tmp);
		theApp.m_UCode=1;
		if(m_Debug)
		{
			m_Debugger=(CDebugger *)new CDebugger();
			m_Debugger->ShowWindow(SW_SHOW);
			m_Debugger->Init(m_EmuObj);
		}
	}
	
}

void CKIDlg::OnBrowse1() 
{
	BrowseFile(&m_Rom1,"*.");
	UpdateData(false);
}

void CKIDlg::OnBrowse2() 
{
	BrowseFile(&m_Rom2,"*.");
	UpdateData(false);
}

void CKIDlg::OnBrowse3() 
{
	BrowseFile(&m_Rom3,"*.");
	UpdateData(false);
}

void CKIDlg::OnBrowse4() 
{
	BrowseFile(&m_Rom4,"*.");
	UpdateData(false);
}

void CKIDlg::OnBrowse5() 
{
	BrowseFile(&m_Rom5,"*.");
	UpdateData(false);
}

void CKIDlg::OnBrowse6() 
{
	BrowseFile(&m_Rom6,"*.");
	UpdateData(false);
}

void CKIDlg::OnBrowse7() 
{
	BrowseFile(&m_Rom7,"*.");
	UpdateData(false);
}

void CKIDlg::OnBrowse8() 
{
	BrowseFile(&m_Rom8,"*.");
	UpdateData(false);
}

void CKIDlg::OnBrowse9() 
{
	BrowseFile(&m_Rom9,"*.");
	UpdateData(false);
}

void CKIDlg::OnDip1Sw1() 
{
	m_DIP1^=0x01;
	DIPSWDesc();
}

void CKIDlg::OnDip1Sw2() 
{
	m_DIP1^=0x02;
	DIPSWDesc();
}

void CKIDlg::OnDip1Sw3() 
{
	m_DIP1^=0x04;
	DIPSWDesc();
}

void CKIDlg::OnDip1Sw4() 
{
	m_DIP1^=0x08;
	DIPSWDesc();
}

void CKIDlg::OnDip1Sw5() 
{
	m_DIP1^=0x10;
	DIPSWDesc();
}

void CKIDlg::OnDip1Sw6() 
{
	m_DIP1^=0x20;
	DIPSWDesc();
}

void CKIDlg::OnDip1Sw7() 
{
	m_DIP1^=0x40;
	DIPSWDesc();
}

void CKIDlg::OnDip1Sw8() 
{
	m_DIP1^=0x80;
	DIPSWDesc();
}


void CKIDlg::OnDip2Sw1() 
{
	m_DIP2^=0x01;
	DIPSWDesc();
}

void CKIDlg::OnDip2Sw2() 
{
	m_DIP2^=0x02;
	DIPSWDesc();
}

void CKIDlg::OnDip2Sw3() 
{
	m_DIP2^=0x04;
	DIPSWDesc();
}

void CKIDlg::OnDip2Sw4() 
{
	m_DIP2^=0x08;
	DIPSWDesc();
}

void CKIDlg::OnDip2Sw5() 
{
	m_DIP2^=0x10;
	DIPSWDesc();
}

void CKIDlg::OnDip2Sw6() 
{
	m_DIP2^=0x20;
	DIPSWDesc();
}

void CKIDlg::OnDip2Sw7() 
{
	m_DIP2^=0x40;
	DIPSWDesc();
}

void CKIDlg::OnDip2Sw8() 
{
	m_DIP2^=0x80;
	DIPSWDesc();
}


void CKIDlg::OnFullScreen() 
{
	m_FullScreen^=1;	
}

void CKIDlg::OnHDBrowse() 
{
	BrowseFile(&m_HDImage,"*.");
	UpdateData(false);
}

void CKIDlg::OnRes1() 
{
	m_ScreenRes=0;
}

void CKIDlg::OnRes2() 
{
	m_ScreenRes=1;
}

void CKIDlg::OnRes3() 
{
	m_ScreenRes=2;
}

void CKIDlg::OnRes4() 
{
	m_ScreenRes=3;
}

void CKIDlg::ReadSettings()
{
	if(gRomSet==KI1)
	{
		m_FullScreen=theApp.GetProfileInt("Options","FullScreen",0);
		m_ScreenRes=theApp.GetProfileInt("Options","ScreenRes",1);
		m_LastPath=theApp.GetProfileString("Options","LastPath","");
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
		m_DIP1=theApp.GetProfileInt("DIP1","DIP 1 Binary",0x0);
		m_DIP2=theApp.GetProfileInt("DIP2","DIP 2 Binary",0x0e);
		m_NeedInit=theApp.GetProfileInt("Options","Re-Init",1);
		m_LockVSYNC=theApp.GetProfileInt("Options","LockVSYNC",1);
		m_ScanLines=theApp.GetProfileInt("Options","ScanLines",1);
		m_Skip=theApp.GetProfileInt("Options","FrameSkip",0);
		gAllowHLE=0;
	}
	else
	{
		m_FullScreen=theApp.GetProfileInt("KI2_Options","FullScreen",0);
		m_ScreenRes=theApp.GetProfileInt("KI2_Options","ScreenRes",1);
		m_LastPath=theApp.GetProfileString("KI2_Options","LastPath","");
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
		m_DIP1=theApp.GetProfileInt("KI2_DIP1","DIP 1 Binary",0x0);
		m_DIP2=theApp.GetProfileInt("KI2_DIP2","DIP 2 Binary",0x0e);
		m_NeedInit=theApp.GetProfileInt("KI2_Options","Re-Init",1);
		m_LockVSYNC=theApp.GetProfileInt("KI2_Options","LockVSYNC",1);
		m_ScanLines=theApp.GetProfileInt("KI2_Options","ScanLines",1);
		m_Skip=theApp.GetProfileInt("KI2_Options","FrameSkip",0);
		gAllowHLE=0;
	}
}

void CKIDlg::InitSettings()
{
	gRomSet=theApp.GetProfileInt("Options","RomSet",2);
	ReadSettings();
	UnPackSettings();
}

bool CKIDlg::DoesExists(char *filename)
{
	return(true);
	CFile *tmp=(CFile *)new CFile();

	bool results=tmp->Open(filename,CFile::modeRead);
	if(results)
	{
		tmp->Close();
	}
	delete tmp;
	return(results);
}

void CKIDlg::PackSettings()
{
	UpdateData(true);
}

void CKIDlg::UnPackSettings()
{
	switch(m_ScreenRes)
	{
	case 0:
		{
			CheckDlgButton(IDC_Res1,BST_CHECKED);
			break;
		}
	case 1:
		{
			CheckDlgButton(IDC_Res2,BST_CHECKED);
			break;
		}
	case 2:
		{
			CheckDlgButton(IDC_Res3,BST_CHECKED);
			break;
		}
	case 3:
		{
			CheckDlgButton(IDC_Res4,BST_CHECKED);
			break;
		}
	}
	if(m_FullScreen) CheckDlgButton(IDC_FullScreen,BST_CHECKED);

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

bool CKIDlg::VerifySettings()
{
	UpdateData(true);
	PackSettings();
	WriteSettings();
	return(true);
}

void CAboutDlg::OnOK() 
{
}

bool CKIDlg::BrowseFile(CString *filename,char *ext)
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

void CKIDlg::OnOK() 
{
	PackSettings();
	WriteSettings();
	CDialog::OnOK();
}

void CKIDlg::DIPSWDesc()
{
	switch(m_DIP1&0x3)
	{
	case 0:
		{
			m_DIP1SW1Text="High Blood";
			m_DIP1SW2Text="";
			break;
		}
	case 1:
		{
			m_DIP1SW1Text="Medium Blood";
			m_DIP1SW2Text="";
			break;
		}
	case 2:
		{
			m_DIP1SW1Text="Low Blood";
			m_DIP1SW2Text="";
			break;
		}
	case 3:
		{
			m_DIP1SW1Text="No Blood";
			m_DIP1SW2Text="";
			break;
		}
	}
	if(m_DIP1&4)
		m_DIP1SW3Text="No Attract Sounds";
	else
		m_DIP1SW3Text="Attract Sounds";

	if(m_DIP1&8)
		m_DIP1SW4Text="No Finishing Moves";
	else
		m_DIP1SW4Text="Finishing Moves";

	if(m_DIP1&16)
		m_DIP1SW5Text="No Display Warnings";
	else
		m_DIP1SW5Text="Display Warnings";

	if(m_DIP1&32)
		m_DIP1SW6Text="White Blood";
	else
		m_DIP1SW6Text="Red Blood";

	m_DIP1SW7Text="Unused";
	m_DIP1SW8Text="Unused";


	switch((m_DIP2>>4)&3)
	{
	case 0:
		{
			m_DIP2SW5Text="USA";
			break;
		}
	case 1:
		{
			m_DIP2SW5Text="Germany";
			break;
		}
	case 2:
		{
			m_DIP2SW5Text="France";
			break;
		}
	case 3:
		{
			m_DIP2SW5Text="USA";
			break;
		}
	}
	if(m_DIP2&1)
		m_DIP2SW1Text="Disk Coinage";
	else
		m_DIP2SW1Text="Dip Switch Coinage";
	switch((m_DIP2>>1)&7)
	{
	case 7:
		{
			m_DIP2SW2Text="Free Play";
			m_DIP2SW3Text="";
			m_DIP2SW4Text="";
			break;
		}
	default:
		{
			m_DIP2SW2Text="On for Free Play";
			m_DIP2SW3Text="On for Free Play";
			m_DIP2SW4Text="On for Free Play";
		}
	}
	if(m_DIP2&64)
		m_DIP2SW7Text="Two Counters";
	else
		m_DIP2SW7Text="One Counter";

	if(m_DIP2&128)
		m_DIP2SW8Text="Test Mode";
	else
		m_DIP2SW8Text="Play Mode";
	theApp.m_DIPS=m_DIP1|(m_DIP2<<8);
	UpdateData(false);
}

void CKIDlg::WriteSettings()
{
	if(gRomSet==KI1)
	{
		theApp.WriteProfileInt("Options","RomSet",1);
		theApp.WriteProfileInt("Options","FullScreen",m_FullScreen);
		theApp.WriteProfileInt("Options","ScreenRes",m_ScreenRes);
		theApp.WriteProfileString("Roms","U98",m_Rom1);
		theApp.WriteProfileString("Roms","U10",m_Rom2);
		theApp.WriteProfileString("Roms","U11",m_Rom3);
		theApp.WriteProfileString("Roms","U12",m_Rom4);
		theApp.WriteProfileString("Roms","U13",m_Rom5);
		theApp.WriteProfileString("Roms","U33",m_Rom6);
		theApp.WriteProfileString("Roms","U34",m_Rom7);
		theApp.WriteProfileString("Roms","U35",m_Rom8);
		theApp.WriteProfileString("Roms","U36",m_Rom9);
		theApp.WriteProfileString("HardDisk","HD Image",m_HDImage);
		theApp.WriteProfileInt("DIP1","DIP 1 Binary",m_DIP1);
		theApp.WriteProfileInt("DIP2","DIP 2 Binary",m_DIP2);
		m_NeedInit=0;
		theApp.WriteProfileInt("Options","Re-Init",m_NeedInit);
		theApp.WriteProfileInt("Options","LockVSYNC",m_LockVSYNC);
		theApp.WriteProfileInt("Options","ScanLines",m_ScanLines);
		theApp.WriteProfileInt("Options","FrameSkip",m_Skip);
	}
	else
	{
		theApp.WriteProfileInt("Options","RomSet",2);
		theApp.WriteProfileInt("KI2_Options","FullScreen",m_FullScreen);
		theApp.WriteProfileInt("KI2_Options","ScreenRes",m_ScreenRes);
		theApp.WriteProfileString("KI2_Roms","U98",m_Rom1);
		theApp.WriteProfileString("KI2_Roms","U10",m_Rom2);
		theApp.WriteProfileString("KI2_Roms","U11",m_Rom3);
		theApp.WriteProfileString("KI2_Roms","U12",m_Rom4);
		theApp.WriteProfileString("KI2_Roms","U13",m_Rom5);
		theApp.WriteProfileString("KI2_Roms","U33",m_Rom6);
		theApp.WriteProfileString("KI2_Roms","U34",m_Rom7);
		theApp.WriteProfileString("KI2_Roms","U35",m_Rom8);
		theApp.WriteProfileString("KI2_Roms","U36",m_Rom9);
		theApp.WriteProfileString("KI2_HardDisk","HD Image",m_HDImage);
		theApp.WriteProfileInt("KI2_DIP1","DIP 1 Binary",m_DIP1);
		theApp.WriteProfileInt("KI2_DIP2","DIP 2 Binary",m_DIP2);
		m_NeedInit=0;
		theApp.WriteProfileInt("KI2_Options","Re-Init",m_NeedInit);
		theApp.WriteProfileInt("KI2_Options","LockVSYNC",m_LockVSYNC);
		theApp.WriteProfileInt("KI2_Options","ScanLines",m_ScanLines);
		theApp.WriteProfileInt("KI2_Options","FrameSkip",m_Skip);
	}
}

void CKIDlg::OnClose() 
{
	PackSettings();
	WriteSettings();
	CDialog::OnClose();
}

void CKIDlg::OnDefault() 
{
	m_DIP1=0x0;
	m_DIP2=0x0e;
	UnPackSettings();
}

void CKIDlg::OnInput() 
{
	CControlConfig *tmpConfig=(CControlConfig *)new CControlConfig();
	tmpConfig->DoModal();
}

void CKIDlg::OnScanLines() 
{
}

void CKIDlg::OnSelendokSetName() 
{
	VerifySettings();
	int index=m_SetName.GetCurSel();
	gRomSet=index+1;
	ReadSettings();
	UpdateData(false);
}
