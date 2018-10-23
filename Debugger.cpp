// Debugger.cpp : implementation file
//

#include "stdafx.h"
#include "ki.h"
#include "Debugger.h"

#include "iMain.h"
#include "dynaCompiler.h"
#include "dasmMain.h"
#include "adsp2100.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern volatile WORD NewTask;
extern "C"
{
extern unsigned int dasm2100(char *buffer, unsigned int op);
}

CDebugger *m_Debugger;
/////////////////////////////////////////////////////////////////////////////
// CDebugger dialog


CDebugger::CDebugger(CWnd* pParent /*=NULL*/)
	: CDialog(CDebugger::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDebugger)
	//}}AFX_DATA_INIT
	Create(IDD_Debugger,pParent);
	m_InsText=NULL;
	m_RegSet=0;
	m_Debugger=this;
	dspNumBPoints=0;
}


CDebugger::~CDebugger()
{
	dasmClose();
}


void CDebugger::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDebugger)
	DDX_Control(pDX, IDC_AsmListing, m_AsmList);
	DDX_Control(pDX, IDC_RegList, m_RegList);
	DDX_Control(pDX, IDC_Registers, m_RegSelect);
	DDX_Control(pDX, IDC_DSPListing, m_DSPList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDebugger, CDialog)
	//{{AFX_MSG_MAP(CDebugger)
	ON_LBN_SELCHANGE(IDC_AsmListing, OnSelchangeAsmListing)
	ON_LBN_DBLCLK(IDC_AsmListing, OnDblclkAsmListing)
	ON_BN_CLICKED(IDC_Break, OnBreak)
	ON_BN_CLICKED(IDC_BreakDSP, OnBreakDSP)
	ON_BN_CLICKED(IDC_BreakPoints, OnBreakPoints)
	ON_BN_CLICKED(IDC_BreakPointsDSP, OnBreakPointsDSP)
	ON_LBN_SELCHANGE(IDC_DSPListing, OnSelchangeDSPListing)
	ON_LBN_DBLCLK(IDC_DSPListing, OnDblclkDSPListing)
	ON_BN_CLICKED(IDC_Go, OnGo)
	ON_BN_CLICKED(IDC_GoDSP, OnGoDSP)
	ON_BN_CLICKED(IDC_Step, OnStep)
	ON_BN_CLICKED(IDC_StepDSP, OnStepDSP)
	ON_BN_CLICKED(IDC_StepInto, OnStepInto)
	ON_BN_CLICKED(IDC_StepIntoDSP, OnStepIntoDSP)
	ON_NOTIFY(TCN_SELCHANGE, IDC_Registers, OnSelchangeRegisters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDebugger message handlers

void CDebugger::OnSelchangeAsmListing() 
{
}

void CDebugger::OnDblclkAsmListing() 
{
	int sel=m_AsmList.GetCurSel();
	int index=sel; //-m_AsmList.GetTopIndex();
	char text[256];
	m_AsmList.GetText(sel,text);
	DWORD data=m_AsmList.GetItemData(index);
	if(data&0x10000)
	{
		data&=~0x10000;
		m_AsmList.SetItemData(index,data);
		RemoveBP(m_InsStart+(index*4));
	}
	else
	{
		data|=0x10000;
		m_AsmList.SetItemData(index,data);
		AddBP(m_InsStart+(index*4));
	}
	m_AsmList.Invalidate(false);
}

void CDebugger::AddBP(DWORD address)
{
	dynaBPoint[dynaNumBPoints]=address;
	dynaNumBPoints++;
	dynaInvalidate(address,4);
}

void CDebugger::RemoveBP(DWORD address)
{
	if(dynaNumBPoints==0)
		return;
	for(int i=0;i<dynaNumBPoints;i++)
	{
		if(dynaBPoint[i]==address)
		{
			for(int j=i;j<(dynaNumBPoints-1);j++)
			{
				dynaBPoint[j]=dynaBPoint[j+1];
			}
			break;
		}
	}
	dynaNumBPoints--;
	dynaInvalidate(address,4);
}

void CDebugger::AddBPDSP(DWORD address)
{
	dspBPoint[dspNumBPoints]=address;
	dspNumBPoints++;
}

void CDebugger::RemoveBPDSP(DWORD address)
{
	if(dspNumBPoints==0)
		return;
	for(int i=0;i<dspNumBPoints;i++)
	{
		if(dspBPoint[i]==address)
		{
			for(int j=i;j<(dspNumBPoints-1);j++)
			{
				dspBPoint[j]=dspBPoint[j+1];
			}
			break;
		}
	}
	dspNumBPoints--;
}

bool CDebugger::CheckBPDSP(DWORD address)
{
	if(dspNumBPoints==0)
		return false;
	for(int i=0;i<dspNumBPoints;i++)
	{
		if(dspBPoint[i]==((address&0x7ffff)*4))
		{
			return true;
		}
	}
	return false;
}


bool CDebugger::CheckBP(DWORD address)
{
	if(dynaNumBPoints==0)
		return false;
	for(int i=0;i<dynaNumBPoints;i++)
	{
		if(dynaBPoint[i]==(address&0x7ffff))
		{
			return true;
		}
	}
	return false;
}

void CDebugger::OnBreak() 
{
	NewTask=DEBUG_BREAK;
	DWORD StopTime=timeGetTime()+2000;
	while(NewTask!=DEBUG_STEPMODE)
	{
		if(timeGetTime()>StopTime)
			return;
	}

	DWORD start=r->PC&0x7ffff;
	if(start>0x200) start-=0x200;
	else start=0;
	DisAsmRange(start,start+(4*200));
	InitRegisters();
}

void CDebugger::OnGo() 
{
	if(NewTask==DEBUG_STEPMODE)
		NewTask=DEBUG_FASTSTEP;
	else
		NewTask=NORMAL_GAME;
	m_EmuObject->SetFocus();
}

void CDebugger::OnStep() 
{
	DWORD data=0;
	NewTask=DEBUG_SINGLESTEP;
	
	DWORD StopTime=timeGetTime()+2000;
	while(NewTask!=DEBUG_STEPMODE)
	{
		Sleep(10);
		if(timeGetTime()>StopTime)
			return;
	}
	CWnd *tmp=GetDlgItem(IDC_PC);
	char buf[128];
	sprintf(buf,"%X",r->PC);
	tmp->SetWindowText(buf);

	int index=((r->PC&0x7ffff)-m_InsStart)/4;
	if((index<0)||(index>(m_AsmList.GetCount()-2)))
	{
		DWORD start;
		if((r->PC&0xf0000000)!=0x80000000)
			start=r->PC&0x7ffff;
		else
			start=r->PC&0x7fffff;
		DisAsmRange(start,start+(4*2000));
		m_InsStart=start;
		InitRegisters();
		index=((r->PC&0x7ffff)-m_InsStart)/4;
	}
	else
	{
		data=m_AsmList.GetItemData(m_LastCursor);
		data&=~0x20000;
		m_AsmList.SetItemData(m_LastCursor,data);
		data=m_AsmList.GetItemData(index);
	}
	int top,bottom;
	top=m_AsmList.GetTopIndex();
	bottom=top+35;
	if(index<top)
		m_AsmList.SetTopIndex(index);
	if(index>bottom)
		m_AsmList.SetTopIndex(index-15);
	data|=0x20000;
	m_LastCursor=index;
	m_AsmList.SetItemData(m_LastCursor,data);
	m_AsmList.Invalidate(false);
	UpdateCPURegisters();
}

void CDebugger::OnStepInto() 
{
	DisAsmRange(0x1b69c,0x20000);
}

void CDebugger::OnBreakPoints() 
{
}

void CDebugger::OnBreakDSP() 
{
}

void CDebugger::OnBreakPointsDSP() 
{
	DisAsmRangeDSP(0,0x3000);
}

void CDebugger::OnSelchangeDSPListing() 
{
}

void CDebugger::OnDblclkDSPListing() 
{
	int sel=m_DSPList.GetCurSel();
	int index=sel; //-m_DSPList.GetTopIndex();
	char text[256];
	m_DSPList.GetText(sel,text);
	DWORD data=m_DSPList.GetItemData(index);
	if(data&0x10000)
	{
		data&=~0x10000;
		m_DSPList.SetItemData(index,data);
		RemoveBPDSP(m_InsStartDSP+(index*4));
	}
	else
	{
		data|=0x10000;
		m_DSPList.SetItemData(index,data);
		AddBPDSP(m_InsStartDSP+(index*4));
	}
	m_DSPList.Invalidate(false);
}

void CDebugger::OnGoDSP() 
{
	DspTask=NORMAL_GAME;
}

void CDebugger::OnStepDSP() 
{
	DWORD data=0;

	if(DspTask==DEBUG_STEPMODE)
	{
		adsp2100_execute(1);
	}
	else
	{
		DspTask=DEBUG_SINGLESTEP;
		
		DWORD StopTime=timeGetTime()+2000;
		while(DspTask!=DEBUG_STEPMODE)
		{
			if(timeGetTime()>StopTime)
				return;
		}
	}


//	int index=adsp2100_get_pc()-m_InsStartDSP;
	int	index=((adsp2100_get_pc()&0x7ffff)-m_InsStartDSP);
	char buf[128];
	CWnd *tmp=GetDlgItem(IDC_PC);
	sprintf(buf,"%X",index+m_InsStartDSP);
	tmp->SetWindowText(buf);

	if((index<0)||(index>(m_DSPList.GetCount()-2)))
	{
		DWORD start;
		if((adsp2100_get_pc()&0x7fffff)>20)
			start=(adsp2100_get_pc()&0x7fffff)-20;
		else
			start=0;
		DisAsmRangeDSP(start,start+0x3000);
		m_InsStartDSP=start;
		index=((adsp2100_get_pc()&0x7ffff)-m_InsStartDSP);
	}
	else
	{
		data=m_DSPList.GetItemData(m_LastCursorDSP);
		data&=~0x20000;
		m_DSPList.SetItemData(m_LastCursorDSP,data);
		data=m_DSPList.GetItemData(index);
	}
	int top,bottom;
	top=m_DSPList.GetTopIndex();
	bottom=top+35;
	if(index<top)
		m_DSPList.SetTopIndex(index);
	if(index>bottom)
		m_DSPList.SetTopIndex(index-15);
	data|=0x20000;
	m_LastCursorDSP=index;
	m_DSPList.SetItemData(m_LastCursorDSP,data);
	m_DSPList.Invalidate(false);
	UpdateDSPRegisters();
}

void CDebugger::OnStepIntoDSP() 
{
}

BOOL CDebugger::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;
	TabCtrlItem.pszText = "CPU - GPR";
	m_RegSelect.InsertItem( 0, &TabCtrlItem );
	TabCtrlItem.pszText = "CPU - CPR0";
	m_RegSelect.InsertItem( 1, &TabCtrlItem );
	TabCtrlItem.pszText = "CPU - CCR0";
	m_RegSelect.InsertItem( 2, &TabCtrlItem );
	TabCtrlItem.pszText = "DSP - GPR";
	m_RegSelect.InsertItem( 3, &TabCtrlItem );
	
	
	return TRUE; 
}

void CDebugger::Init(CEmuObject *emuobj)
{
	m_EmuObject=emuobj;
	DWORD start=0x0;
	DisAsmRange(start,start+(4*2000));
	InitRegisters();
	dasmNumLookups=0;

	DisAsmRangeDSP(start,start+0x3000);

}

void CDebugger::DisAsmRange(DWORD start,DWORD end)
{
	dasmReset();
	SafeFree(m_InsText);
	m_AsmList.ResetContent();
	if((r->PC&0xff000000)==0x88000000)
		m_InsText=dasmDo((DWORD *)&m->rdRam[start],start,end);
	else
		m_InsText=dasmDo((DWORD *)&rom->Image[start],start,end);
	DWORD num=(end-start)/4;
	char *src=m_InsText;
	m_InsStart=start;
	m_InsEnd=end;
	CFile *t=(CFile *)new CFile();
	t->Open("tmp.asm",CFile::modeReadWrite|CFile::modeCreate);

	DWORD data;
	for(DWORD i=0;i<num;i++)
	{
		data=i;
		if(CheckBP(start+(i*4)))
		{
			data|=0x10000;
		}
		m_AsmList.AddString(src);
		m_AsmList.SetItemData(i,data);
		t->Write(src,strlen(src));
		t->Write("\r\n",2);
		src+=strlen(src)+1;
	}
	t->Close();
	delete t;
}

void CDebugger::DisAsmRangeDSP(DWORD start,DWORD end)
{
	m_DSPList.ResetContent();

	DWORD num=(end-start)/4;
	DWORD *src=(DWORD *)&m->dspPMem[start*4];

	m_InsStartDSP=start;
	m_InsEndDSP=end;
//	CFile *t=(CFile *)new CFile();
//	t->Open("tmp.asm",CFile::modeReadWrite|CFile::modeCreate);

	DWORD data,b1,b2;
	char buf[256],text[256];
	for(DWORD i=0;i<num;i++)
	{
		data=i;
		b1=*(src++);
//		b2=((b1&0xff)<<16)|((b1&0xff00)<<0)|((b1&0xff0000)>>16);
		dasm2100(buf,b1);

		if(CheckBPDSP(start+(i*4)))
		{
			data|=0x10000;
		}

		sprintf(text,"%04X %s",i+m_InsStartDSP,buf);
		m_DSPList.AddString(text);
		m_DSPList.SetItemData(i,data);
//		t->Write(src,strlen(src));
//		t->Write("\r\n",2);
	}
//	t->Close();
//	delete t;
}

void CDebugger::InitRegisters()
{
	m_RegList.ResetContent();
	switch(m_RegSet)
	{
	case 0:
		{
			InitCPURegisters();
			break;
		}
	case 1:
		{
			InitCPR0Registers();
			break;
		}
	case 2:
		{
			InitCCR0Registers();
			break;
		}
	case 3:
		{
			InitDSPRegisters();
			break;
		}
	}
}

void CDebugger::InitCPURegisters()
{
	int reg=0;
	for(int i=0;i<32;i++)
	{
		char tmp[256];
		strcpy(tmp,"");
		m_CPUReg[reg]=*(sQWORD *)&r->GPR[reg*2];
		sprintf(tmp,"%s %08X:%08X  ",dasmRegName[reg],(DWORD)((QWORD)m_CPUReg[reg]>>32),(DWORD)m_CPUReg[reg]);
		reg++;
		m_RegList.AddString(tmp);
	}
}

void CDebugger::InitCPR0Registers()
{
	int reg=0;
	for(int i=0;i<32;i++)
	{
		char tmp[256];
		strcpy(tmp,"");
		for(int j=0;j<1;j++)
		{
			char entry[32];
			m_CPR0Reg[reg]=r->CPR0[2*reg];
			sprintf(entry,"%s %02X:%08X  ",dasmCRegName[reg],(DWORD)(m_CPR0Reg[reg]>>32),(DWORD)m_CPR0Reg[reg]);
			reg++;
			strcat(tmp,entry);
		}
		m_RegList.AddString(tmp);
	}
}

void CDebugger::InitCCR0Registers()
{
	int reg=0;
	for(int i=0;i<16;i++)
	{
		char tmp[256];
		strcpy(tmp,"");
		for(int j=0;j<2;j++)
		{
			char entry[32];
			m_CCR0Reg[reg]=r->CCR0[2*reg];
			sprintf(entry,"%02d %02X:%08X  ",reg,(DWORD)(m_CCR0Reg[reg]>>32),(DWORD)m_CCR0Reg[reg]);
			reg++;
			strcat(tmp,entry);
		}
		m_RegList.AddString(tmp);
	}
}

void CDebugger::UpdateDSPRegisters()
{
	m_RegList.ResetContent();
	InitDSPRegisters();
}

void CDebugger::InitDSPRegisters()
{
	char buf[256];

	sprintf(buf,"AX0 %X", adsp2100_get_reg(ADSP2100_AX0));
	m_RegList.AddString(buf);
	sprintf(buf,"AX1 %X", adsp2100_get_reg(ADSP2100_AX1));
	m_RegList.AddString(buf);
	sprintf(buf,"AY0 %X", adsp2100_get_reg(ADSP2100_AY0));
	m_RegList.AddString(buf);
	sprintf(buf,"AY1 %X", adsp2100_get_reg(ADSP2100_AY1));
	m_RegList.AddString(buf);
	sprintf(buf,"AR %X", adsp2100_get_reg(ADSP2100_AR));
	m_RegList.AddString(buf);
	sprintf(buf,"AF %X", adsp2100_get_reg(ADSP2100_AF));
	m_RegList.AddString(buf);
	sprintf(buf,"MX0 %X", adsp2100_get_reg(ADSP2100_MX0));
	m_RegList.AddString(buf);
	sprintf(buf,"MX1 %X", adsp2100_get_reg(ADSP2100_MX1));
	m_RegList.AddString(buf);
	sprintf(buf,"MY0 %X", adsp2100_get_reg(ADSP2100_MY0));
	m_RegList.AddString(buf);
	sprintf(buf,"MY1 %X", adsp2100_get_reg(ADSP2100_MY1));
	m_RegList.AddString(buf);
	sprintf(buf,"MR0 %X", adsp2100_get_reg(ADSP2100_MR0));
	m_RegList.AddString(buf);
	sprintf(buf,"MR1 %X", adsp2100_get_reg(ADSP2100_MR1));
	m_RegList.AddString(buf);
	sprintf(buf,"MR2 %X", adsp2100_get_reg(ADSP2100_MR2));
	m_RegList.AddString(buf);
	sprintf(buf,"MF %X", adsp2100_get_reg(ADSP2100_MF));
	m_RegList.AddString(buf);
	sprintf(buf,"SI %X", adsp2100_get_reg(ADSP2100_SI));
	m_RegList.AddString(buf);
	sprintf(buf,"SE %X", adsp2100_get_reg(ADSP2100_SE));
	m_RegList.AddString(buf);
	sprintf(buf,"SB %X", adsp2100_get_reg(ADSP2100_SB));
	m_RegList.AddString(buf);
	sprintf(buf,"SR0 %X", adsp2100_get_reg(ADSP2100_SR0));
	m_RegList.AddString(buf);
	sprintf(buf,"SR1 %X", adsp2100_get_reg(ADSP2100_SR1));
	m_RegList.AddString(buf);
	sprintf(buf,"I0 %X", adsp2100_get_reg(ADSP2100_I0));
	m_RegList.AddString(buf);
	sprintf(buf,"I1 %X", adsp2100_get_reg(ADSP2100_I1));
	m_RegList.AddString(buf);
	sprintf(buf,"I2 %X", adsp2100_get_reg(ADSP2100_I2));
	m_RegList.AddString(buf);
	sprintf(buf,"I3 %X", adsp2100_get_reg(ADSP2100_I3));
	m_RegList.AddString(buf);
	sprintf(buf,"I4 %X", adsp2100_get_reg(ADSP2100_I4));
	m_RegList.AddString(buf);
	sprintf(buf,"I5 %X", adsp2100_get_reg(ADSP2100_I5));
	m_RegList.AddString(buf);
	sprintf(buf,"I6 %X", adsp2100_get_reg(ADSP2100_I6));
	m_RegList.AddString(buf);
	sprintf(buf,"I7 %X", adsp2100_get_reg(ADSP2100_I7));
	m_RegList.AddString(buf);
	sprintf(buf,"L0 %X", adsp2100_get_reg(ADSP2100_L0));
	m_RegList.AddString(buf);
	sprintf(buf,"L1 %X", adsp2100_get_reg(ADSP2100_L1));
	m_RegList.AddString(buf);
	sprintf(buf,"L2 %X", adsp2100_get_reg(ADSP2100_L2));
	m_RegList.AddString(buf);
	sprintf(buf,"L3 %X", adsp2100_get_reg(ADSP2100_L3));
	m_RegList.AddString(buf);
	sprintf(buf,"L4 %X", adsp2100_get_reg(ADSP2100_L4));
	m_RegList.AddString(buf);
	sprintf(buf,"L5 %X", adsp2100_get_reg(ADSP2100_L5));
	m_RegList.AddString(buf);
	sprintf(buf,"L6 %X", adsp2100_get_reg(ADSP2100_L6));
	m_RegList.AddString(buf);
	sprintf(buf,"L7 %X", adsp2100_get_reg(ADSP2100_L7));
	m_RegList.AddString(buf);
	sprintf(buf,"M0 %X", adsp2100_get_reg(ADSP2100_M0));
	m_RegList.AddString(buf);
	sprintf(buf,"M1 %X", adsp2100_get_reg(ADSP2100_M1));
	m_RegList.AddString(buf);
	sprintf(buf,"M2 %X", adsp2100_get_reg(ADSP2100_M2));
	m_RegList.AddString(buf);
	sprintf(buf,"M3 %X", adsp2100_get_reg(ADSP2100_M3));
	m_RegList.AddString(buf);
	sprintf(buf,"M4 %X", adsp2100_get_reg(ADSP2100_M4));
	m_RegList.AddString(buf);
	sprintf(buf,"M5 %X", adsp2100_get_reg(ADSP2100_M5));
	m_RegList.AddString(buf);
	sprintf(buf,"M6 %X", adsp2100_get_reg(ADSP2100_M6));
	m_RegList.AddString(buf);
	sprintf(buf,"M7 %X", adsp2100_get_reg(ADSP2100_M7));
	m_RegList.AddString(buf);
	sprintf(buf,"CTR %X", adsp2100_get_reg(ADSP2100_CNTR));
	m_RegList.AddString(buf);
}

void CDebugger::UpdateCPURegisters()
{
	int reg=0;
	int index=0;
	DWORD data;
	for(int i=0;i<32;i++)
	{
		char tmp[256];
		strcpy(tmp,"");
		bool UpdateNeeded=false;
		if(m_CPUReg[reg]!=*(sQWORD *)&r->GPR[reg*2])
		{
			UpdateNeeded=true;
			char entry[32];
			m_CPUReg[reg]=*(sQWORD *)&r->GPR[reg*2];
			UpdateNeeded=true;
			sprintf(tmp,"%s %08X:%08X  ",dasmRegName[reg],(DWORD)((QWORD)m_CPUReg[reg]>>32),(DWORD)m_CPUReg[reg]);
		}
		reg++;
		data=0;
		if(UpdateNeeded)
		{
			m_RegList.DeleteString(index);
			m_RegList.InsertString(index,tmp);
			data|=0x10000;
		}
		m_RegList.SetItemData(index,data);
		index++;
	}
//	UpdateData(TRUE);
	m_RegList.Invalidate(false);
}

void CDebugger::OnOK() 
{
	CDialog::OnOK();
}

void CDebugger::Break(DWORD address)
{
	DisAsmRange(address,address+(4*4000));
	InitRegisters();
}

void CDebugger::BreakDSP(DWORD address)
{
	DisAsmRangeDSP(address,address+(4*200));

	int index=adsp2100_get_pc();
	char buf[128];
	CWnd *tmp=GetDlgItem(IDC_PC);
	sprintf(buf,"%X",index);
	tmp->SetWindowText(buf);

	int top,bottom;
	top=m_DSPList.GetTopIndex();
	bottom=top+35;
	if(index<top)
		m_DSPList.SetTopIndex(index);
	if(index>bottom)
		m_DSPList.SetTopIndex(index-15);
	DWORD data=m_DSPList.GetItemData(index);
	data|=0x20000;
	m_LastCursorDSP=index;
	m_DSPList.SetItemData(m_LastCursorDSP,data);
	m_DSPList.Invalidate(false);
	UpdateDSPRegisters();
}

void CDebugger::OnSelchangeRegisters(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_RegSet=m_RegSelect.GetCurSel();
	InitRegisters();
	*pResult = 0;
}
