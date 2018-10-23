// ControlConfig.cpp : implementation file
//

#include "stdafx.h"
#include "ki.h"
#include "ControlConfig.h"
#include "mmDirectInputDevice.h"
#include "CaptureMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlConfig dialog


CControlConfig::CControlConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CControlConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControlConfig)
	//}}AFX_DATA_INIT
}


void CControlConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlConfig)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControlConfig, CDialog)
	//{{AFX_MSG_MAP(CControlConfig)
	ON_LBN_SELCHANGE(IDC_Controls, OnSelchangeControls)
	ON_WM_CHAR()
	ON_LBN_DBLCLK(IDC_Controls, OnDblclkControls)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlConfig message handlers

void CControlConfig::OnOK() 
{
	CFile *tmpFile;
	tmpFile=(CFile *)new CFile();
	bool results=tmpFile->Open("U64KeyMap.dat",CFile::modeReadWrite);
	if(results)
	{
		tmpFile->Write(m_Map,sizeof(DWORD)*22);
		tmpFile->Close();
	}
	SafeDelete(tmpFile);
	m_InputDevice->Close();
	delete m_InputDevice;
	CDialog::OnOK();
}

void CControlConfig::OnCancel() 
{
	m_InputDevice->Close();
	delete m_InputDevice;
	CDialog::OnCancel();
}

void CControlConfig::UpdateMap(int index)
{
	CListBox *tmpListMap=(CListBox *)GetDlgItem(IDC_ControlsMap);
	if(index>=0)
	{
		if(m_Map[index]<255)
		{
			char text[255];
			m_InputDevice->KeyToText(m_Map[index],text);
			tmpListMap->DeleteString(index);
			tmpListMap->InsertString(index,text);
			tmpListMap->SetCurSel(index);
			return;
		}
		else
		{
			char text[255];
			DWORD map=m_Map[index];
			int device=map/256;
			device--;
			map=map%256;
			if(map<64)
			{
				sprintf(text,"Joy %d Button %d",device,map);
				tmpListMap->DeleteString(index);
				tmpListMap->InsertString(index,text);
				tmpListMap->SetCurSel(index);
				return;
			}
			if(map<80)
			{
				int pov=(map-64)/4;
				int dir=(map-64)%4;
				switch(dir)
				{
				case 0:
					{
						sprintf(text,"Joy %d POV %d UP",device,pov);
						break;
					}
				case 1:
					{
						sprintf(text,"Joy %d POV %d LEFT",device,pov);
						break;
					}
				case 2:
					{
						sprintf(text,"Joy %d POV %d DOWN",device,pov);
						break;
					}
				case 3:
					{
						sprintf(text,"Joy %d POV %d RIGHT",device,pov);
						break;
					}
				}
				tmpListMap->DeleteString(index);
				tmpListMap->InsertString(index,text);
				tmpListMap->SetCurSel(index);
				return;
			}
			switch(map-80)
			{
			case 0:
				{
					sprintf(text,"Joy %d X+ Axis",device);
					break;
				}
			case 1:
				{
					sprintf(text,"Joy %d X- Axis",device);
					break;
				}
			case 2:
				{
					sprintf(text,"Joy %d Y+ Axis",device);
					break;
				}
			case 3:
				{
					sprintf(text,"Joy %d Y- Axis",device);
					break;
				}
			case 4:
				{
					sprintf(text,"Joy %d Z+ Axis",device);
					break;
				}
			case 5:
				{
					sprintf(text,"Joy %d Z- Axis",device);
					break;
				}
			}
			tmpListMap->DeleteString(index);
			tmpListMap->InsertString(index,text);
			tmpListMap->SetCurSel(index);
			return;
		}
	}
}

void CControlConfig::OnSelchangeControls() 
{
	char Text[255];
	CListBox *tmpList=(CListBox *)GetDlgItem(IDC_Controls);
	int index=tmpList->GetCurSel();
	UpdateMap(index);
}

BOOL CControlConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CListBox *tmpList=(CListBox *)GetDlgItem(IDC_Controls);
	tmpList->AddString("Player2 B1");
	tmpList->AddString("Player2 B2");
	tmpList->AddString("Player2 B3");
	tmpList->AddString("Player2 B4");
	tmpList->AddString("Player2 B5");
	tmpList->AddString("Player2 B6");
	tmpList->AddString("Player2 UP");
	tmpList->AddString("Player2 DOWN");
	tmpList->AddString("Player2 LEFT");
	tmpList->AddString("Player2 RIGHT");
	tmpList->AddString("Player2 Start");

	tmpList->AddString("Player1 B1");
	tmpList->AddString("Player1 B2");
	tmpList->AddString("Player1 B3");
	tmpList->AddString("Player1 B4");
	tmpList->AddString("Player1 B5");
	tmpList->AddString("Player1 B6");
	tmpList->AddString("Player1 UP");
	tmpList->AddString("Player1 DOWN");
	tmpList->AddString("Player1 LEFT");
	tmpList->AddString("Player1 RIGHT");
	tmpList->AddString("Player1 Start");
	
	tmpList->SetCurSel(0);
	m_InputDevice=(mmDirectInputDevice *)new mmDirectInputDevice();
	m_InputDevice->Create(DISCL_FOREGROUND,this->m_hWnd);
	m_InputDevice->Open();

	CFile *tmpFile=(CFile *)new CFile();
	bool results=tmpFile->Open("U64KeyMap.dat",CFile::modeReadWrite);
	if(!results)
	{
		tmpFile->Open("U64KeyMap.dat",CFile::modeReadWrite|CFile::modeCreate);
		m_Map[0]=DIK_U;
		m_Map[1]=DIK_I;
		m_Map[2]=DIK_O;
		m_Map[3]=DIK_J;
		m_Map[4]=DIK_K;
		m_Map[5]=DIK_L;
		m_Map[6]=DIK_UP;
		m_Map[7]=DIK_DOWN;
		m_Map[8]=DIK_LEFT;
		m_Map[9]=DIK_RIGHT;
		m_Map[10]=DIK_Y;

		m_Map[11]=DIK_W;
		m_Map[12]=DIK_E;
		m_Map[13]=DIK_R;
		m_Map[14]=DIK_S;
		m_Map[15]=DIK_D;
		m_Map[16]=DIK_F;
		m_Map[17]=DIK_HOME;
		m_Map[18]=DIK_END;
		m_Map[19]=DIK_DELETE;
		m_Map[20]=DIK_NEXT;
		m_Map[21]=DIK_Q;
		tmpFile->Write(m_Map,sizeof(DWORD)*22);
		tmpFile->Close();
	}
	else
	{
		tmpFile->Read(m_Map,sizeof(DWORD)*22);
		tmpFile->Close();
	}
	for(int i=0;i<22;i++)
	{
		UpdateMap(i);
	}
	m_LastSelect=0;
	UpdateMap(0);
	SafeDelete(tmpFile);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CControlConfig::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
}

void CControlConfig::OnDblclkControls() 
{
	m_InputDevice->Close();
	delete m_InputDevice;

	CListBox *tmpList=(CListBox *)GetDlgItem(IDC_Controls);
	int index=tmpList->GetCurSel();

	CCaptureMap *tmpMap=(CCaptureMap *)new CCaptureMap();
	char text[256];
	tmpList->GetText(index,text);
	strcpy(tmpMap->m_KeyText,text);

	if(tmpMap->DoModal()==IDOK)
	{
		m_Map[index]=tmpMap->m_LastMap;
		UpdateMap(index);
	}
	delete tmpMap;

	m_InputDevice=(mmDirectInputDevice *)new mmDirectInputDevice();
	m_InputDevice->Create(DISCL_FOREGROUND,this->m_hWnd);
	m_InputDevice->Open();
}
