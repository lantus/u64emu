// KIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ki.h"
#include "KIDlg.h"
#include "global.h"
#include "ControlConfig.h"
#include "NetPlay.h"
#include "DIPSettings.h"
#include "AdvSettings.h"
#include "About.h"
#include "RomSet.h"

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
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKIDlg, CDialog)
	//{{AFX_MSG_MAP(CKIDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DoIt, OnDoIt)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_Input, OnInput)
	ON_BN_CLICKED(IDC_DIPSwitches, OnDIPSwitches)
	ON_BN_CLICKED(IDC_Settings, OnSettings)
	ON_BN_CLICKED(IDC_About, OnAbout)
	ON_BN_CLICKED(IDC_RomSet, OnRomSet)
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
	if(theApp.m_Auto)
		PostMessage(WM_COMMAND,IDC_DoIt,0);
	m_Background=(CBitmap *)new CBitmap();
	m_Background->LoadBitmap(IDB_Logo);
	CDC *tmpDC=GetDC();
	m_BackgroundCDC=(CDC *)new CDC();
	m_BackgroundCDC->CreateCompatibleDC(tmpDC);
	m_BackgroundCDC->SelectObject(m_Background);

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
		BITMAP bm;
		CPaintDC dc(this); // device context for painting
		RECT rc;
		m_Background->GetBitmap(&bm);
		GetClientRect(&rc);
		dc.StretchBlt(rc.left,rc.top,rc.right,rc.bottom,m_BackgroundCDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
//		dc.BitBlt(rc.left,rc.top,rc.right,rc.bottom,m_BackgroundCDC,0,0,SRCCOPY);
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

	VerifySettings();
	theApp.m_DIPS=m_DIP1|(m_DIP2<<8);
	m_LockVSYNC=true;
	if(m_ScanLines)
	{
		m_ScreenRes=1;
		m_FullScreen=true;
	}
	theApp.m_FullScreen=m_FullScreen;
	theApp.m_ScreenRes=m_ScreenRes;
	theApp.m_LockOn=m_LockVSYNC;
	theApp.m_ScanLines=m_ScanLines;
	theApp.m_FrameDelay=(1000.f/(float)m_FrameRate+.3f);
	theApp.m_VTraceInc=49999980/m_FrameRate;
	if(gRomSet!=KI2)
		gAllowHLE=!m_NoHLE;

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
		m_FrameRate=theApp.GetProfileInt("Options","FrameRate",60);
		m_NoHLE=theApp.GetProfileInt("Options","NoHLE",1);
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
		m_FrameRate=theApp.GetProfileInt("KI2_Options","FrameRate",60);
		m_NoHLE=theApp.GetProfileInt("KI2_Options","NoHLE",1);
		gAllowHLE=0;
	}
	UpdateData(false);
}

void CKIDlg::InitSettings()
{
	gRomSet=theApp.GetProfileInt("Options","RomSet",2);
	ReadSettings();
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

bool CKIDlg::VerifySettings()
{
	UpdateData(true);
	WriteSettings();
	return(true);
}

void CAboutDlg::OnOK() 
{
}


void CKIDlg::OnOK() 
{
	WriteSettings();
	CDialog::OnOK();
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
		theApp.WriteProfileInt("Options","FrameRate",m_FrameRate);
		theApp.WriteProfileInt("Options","NoHLE",m_NoHLE);
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
		theApp.WriteProfileInt("KI2_Options","FrameRate",m_FrameRate);
		theApp.WriteProfileInt("KI2_Options","NoHLE",m_NoHLE);
	}
}

void CKIDlg::OnClose() 
{
	WriteSettings();
	CDialog::OnClose();
}

void CKIDlg::OnInput() 
{
	CControlConfig *tmpConfig=(CControlConfig *)new CControlConfig();
	tmpConfig->DoModal();
}

void CKIDlg::OnDIPSwitches() 
{
	CDIPSettings *ds=(CDIPSettings *)new CDIPSettings();
	ds->m_DIP1=m_DIP1;
	ds->m_DIP2=m_DIP2;
	HRESULT ret=ds->DoModal();
	if(ret==IDOK)
	{
		m_DIP1=ds->m_DIP1;
		m_DIP2=ds->m_DIP2;
	}
	delete ds;
}

void CKIDlg::OnSettings() 
{
	CAdvSettings *as=(CAdvSettings *)new CAdvSettings();
	as->m_FrameRate=m_FrameRate;
	as->m_FullScreen=m_FullScreen;
	as->m_NoHLE=m_NoHLE;
	as->m_ScanLines=m_ScanLines;
	as->m_ScreenRes=m_ScreenRes;
	HRESULT ret=as->DoModal();
	if(ret==IDOK)
	{
		m_FrameRate=as->m_FrameRate;
		m_FullScreen=as->m_FullScreen;
		m_NoHLE=as->m_NoHLE;
		m_ScanLines=as->m_ScanLines;
		m_ScreenRes=as->m_ScreenRes;
	}
	delete as;
}

void CKIDlg::OnAbout() 
{
	CAbout *a=(CAbout *)new CAbout();
	a->DoModal();
	delete a;
}

BOOL CKIDlg::DestroyWindow() 
{
	delete m_Background;
	return CDialog::DestroyWindow();
}

void CKIDlg::OnRomSet() 
{
	CRomSet *rs=(CRomSet *)new CRomSet();
	rs->m_SetNum=gRomSet;
	rs->m_HDImage=m_HDImage;
	rs->m_Rom1=m_Rom1;
	rs->m_Rom2=m_Rom2;
	rs->m_Rom3=m_Rom3;
	rs->m_Rom4=m_Rom4;
	rs->m_Rom5=m_Rom5;
	rs->m_Rom6=m_Rom6;
	rs->m_Rom7=m_Rom7;
	rs->m_Rom8=m_Rom8;
	rs->m_Rom9=m_Rom9;
	HRESULT ret=rs->DoModal();
	if(ret==IDOK)
	{
		m_HDImage=rs->m_HDImage;
		m_Rom1=rs->m_Rom1;
		m_Rom2=rs->m_Rom2;
		m_Rom3=rs->m_Rom3;
		m_Rom4=rs->m_Rom4;
		m_Rom5=rs->m_Rom5;
		m_Rom6=rs->m_Rom6;
		m_Rom7=rs->m_Rom7;
		m_Rom8=rs->m_Rom8;
		m_Rom9=rs->m_Rom9;
		gRomSet=rs->m_SetNum;
	}
}
