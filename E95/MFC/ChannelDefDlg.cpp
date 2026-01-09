// ChannelDefDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "HeatView.h"
//#include "PlotView.h"
#include "ChannelDefDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChannelDefDlg dialog


CChannelDefDlg::CChannelDefDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChannelDefDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChannelDefDlg)
	//}}AFX_DATA_INIT

	for (int i = 0; i < 36; i++) 
	{
		VisMask[i+1] = FALSE;
	}
}


void CChannelDefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChannelDefDlg)
	DDX_Control(pDX, IDC_CHANNEL36, m_ch36);
	DDX_Control(pDX, IDC_CHANNEL35, m_ch35);
	DDX_Control(pDX, IDC_CHANNEL34, m_ch34);
	DDX_Control(pDX, IDC_CHANNEL33, m_ch33);
	DDX_Control(pDX, IDC_CHANNEL9, m_ch9);
	DDX_Control(pDX, IDC_CHANNEL8, m_ch8);
	DDX_Control(pDX, IDC_CHANNEL7, m_ch7);
	DDX_Control(pDX, IDC_CHANNEL6, m_ch6);
	DDX_Control(pDX, IDC_CHANNEL5, m_ch5);
	DDX_Control(pDX, IDC_CHANNEL4, m_ch4);
	DDX_Control(pDX, IDC_CHANNEL32, m_ch32);
	DDX_Control(pDX, IDC_CHANNEL31, m_ch31);
	DDX_Control(pDX, IDC_CHANNEL30, m_ch30);
	DDX_Control(pDX, IDC_CHANNEL3, m_ch3);
	DDX_Control(pDX, IDC_CHANNEL29, m_ch29);
	DDX_Control(pDX, IDC_CHANNEL28, m_ch28);
	DDX_Control(pDX, IDC_CHANNEL27, m_ch27);
	DDX_Control(pDX, IDC_CHANNEL26, m_ch26);
	DDX_Control(pDX, IDC_CHANNEL25, m_ch25);
	DDX_Control(pDX, IDC_CHANNEL24, m_ch24);
	DDX_Control(pDX, IDC_CHANNEL23, m_ch23);
	DDX_Control(pDX, IDC_CHANNEL22, m_ch22);
	DDX_Control(pDX, IDC_CHANNEL21, m_ch21);
	DDX_Control(pDX, IDC_CHANNEL20, m_ch20);
	DDX_Control(pDX, IDC_CHANNEL2, m_ch2);
	DDX_Control(pDX, IDC_CHANNEL19, m_ch19);
	DDX_Control(pDX, IDC_CHANNEL18, m_ch18);
	DDX_Control(pDX, IDC_CHANNEL17, m_ch17);
	DDX_Control(pDX, IDC_CHANNEL16, m_ch16);
	DDX_Control(pDX, IDC_CHANNEL15, m_ch15);
	DDX_Control(pDX, IDC_CHANNEL14, m_ch14);
	DDX_Control(pDX, IDC_CHANNEL13, m_ch13);
	DDX_Control(pDX, IDC_CHANNEL12, m_ch12);
	DDX_Control(pDX, IDC_CHANNEL11, m_ch11);
	DDX_Control(pDX, IDC_CHANNEL10, m_ch10);
	DDX_Control(pDX, IDC_CHANNEL1, m_ch1);
	//}}AFX_DATA_MAP

	DDX_Check(pDX, IDC_CHANNEL1, VisMask[1]);
	DDX_Check(pDX, IDC_CHANNEL2, VisMask[2]);
	DDX_Check(pDX, IDC_CHANNEL3, VisMask[3]);
	DDX_Check(pDX, IDC_CHANNEL4, VisMask[4]);
	DDX_Check(pDX, IDC_CHANNEL5, VisMask[5]);
	DDX_Check(pDX, IDC_CHANNEL6, VisMask[6]);
	DDX_Check(pDX, IDC_CHANNEL7, VisMask[7]);
	DDX_Check(pDX, IDC_CHANNEL8, VisMask[8]);
	DDX_Check(pDX, IDC_CHANNEL9, VisMask[9]);
	DDX_Check(pDX, IDC_CHANNEL10, VisMask[10]);
	DDX_Check(pDX, IDC_CHANNEL11, VisMask[11]);
	DDX_Check(pDX, IDC_CHANNEL12, VisMask[12]);
	DDX_Check(pDX, IDC_CHANNEL13, VisMask[13]);
	DDX_Check(pDX, IDC_CHANNEL14, VisMask[14]);
	DDX_Check(pDX, IDC_CHANNEL15, VisMask[15]);
	DDX_Check(pDX, IDC_CHANNEL16, VisMask[16]);
	DDX_Check(pDX, IDC_CHANNEL17, VisMask[17]);
	DDX_Check(pDX, IDC_CHANNEL18, VisMask[18]);
	DDX_Check(pDX, IDC_CHANNEL19, VisMask[19]);
	DDX_Check(pDX, IDC_CHANNEL20, VisMask[20]);
	DDX_Check(pDX, IDC_CHANNEL21, VisMask[21]);
	DDX_Check(pDX, IDC_CHANNEL22, VisMask[22]);
	DDX_Check(pDX, IDC_CHANNEL23, VisMask[23]);
	DDX_Check(pDX, IDC_CHANNEL24, VisMask[24]);
	DDX_Check(pDX, IDC_CHANNEL25, VisMask[25]);
	DDX_Check(pDX, IDC_CHANNEL26, VisMask[26]);
	DDX_Check(pDX, IDC_CHANNEL27, VisMask[27]);
	DDX_Check(pDX, IDC_CHANNEL28, VisMask[28]);
	DDX_Check(pDX, IDC_CHANNEL29, VisMask[29]);
	DDX_Check(pDX, IDC_CHANNEL30, VisMask[30]);
	DDX_Check(pDX, IDC_CHANNEL31, VisMask[31]);
	DDX_Check(pDX, IDC_CHANNEL32, VisMask[32]);
	DDX_Check(pDX, IDC_CHANNEL33, VisMask[33]);
	DDX_Check(pDX, IDC_CHANNEL34, VisMask[34]);
	DDX_Check(pDX, IDC_CHANNEL35, VisMask[35]);
	DDX_Check(pDX, IDC_CHANNEL36, VisMask[36]);
}


BEGIN_MESSAGE_MAP(CChannelDefDlg, CDialog)
	//{{AFX_MSG_MAP(CChannelDefDlg)
	ON_BN_CLICKED(IDC_CHANNEL1, OnChannel1)
	ON_BN_CLICKED(IDC_CHANNEL2, OnChannel2)
	ON_BN_CLICKED(IDC_CHANNEL3, OnChannel3)
	ON_BN_CLICKED(IDC_CHANNEL4, OnChannel4)
	ON_BN_CLICKED(IDC_CHANNEL5, OnChannel5)
	ON_BN_CLICKED(IDC_CHANNEL6, OnChannel6)
	ON_BN_CLICKED(IDC_CHANNEL7, OnChannel7)
	ON_BN_CLICKED(IDC_CHANNEL8, OnChannel8)
	ON_BN_CLICKED(IDC_CHANNEL9, OnChannel9)
	ON_BN_CLICKED(IDC_CHANNEL10, OnChannel10)
	ON_BN_CLICKED(IDC_CHANNEL11, OnChannel11)
	ON_BN_CLICKED(IDC_CHANNEL12, OnChannel12)
	ON_BN_CLICKED(IDC_CHANNEL13, OnChannel13)
	ON_BN_CLICKED(IDC_CHANNEL14, OnChannel14)
	ON_BN_CLICKED(IDC_CHANNEL15, OnChannel15)
	ON_BN_CLICKED(IDC_CHANNEL16, OnChannel16)
	ON_BN_CLICKED(IDC_CHANNEL17, OnChannel17)
	ON_BN_CLICKED(IDC_CHANNEL18, OnChannel18)
	ON_BN_CLICKED(IDC_CHANNEL19, OnChannel19)
	ON_BN_CLICKED(IDC_CHANNEL20, OnChannel20)
	ON_BN_CLICKED(IDC_CHANNEL21, OnChannel21)
	ON_BN_CLICKED(IDC_CHANNEL22, OnChannel22)
	ON_BN_CLICKED(IDC_CHANNEL23, OnChannel23)
	ON_BN_CLICKED(IDC_CHANNEL24, OnChannel24)
	ON_BN_CLICKED(IDC_CHANNEL25, OnChannel25)
	ON_BN_CLICKED(IDC_CHANNEL26, OnChannel26)
	ON_BN_CLICKED(IDC_CHANNEL27, OnChannel27)
	ON_BN_CLICKED(IDC_CHANNEL28, OnChannel28)
	ON_BN_CLICKED(IDC_CHANNEL29, OnChannel29)
	ON_BN_CLICKED(IDC_CHANNEL30, OnChannel30)
	ON_BN_CLICKED(IDC_CHANNEL31, OnChannel31)
	ON_BN_CLICKED(IDC_CHANNEL32, OnChannel32)
	ON_BN_CLICKED(IDC_CHANNEL33, OnChannel33)
	ON_BN_CLICKED(IDC_CHANNEL34, OnChannel34)
	ON_BN_CLICKED(IDC_CHANNEL35, OnChannel35)
	ON_BN_CLICKED(IDC_CHANNEL36, OnChannel36)
	ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
	ON_BN_CLICKED(IDC_CLEARTREND, OnCleartrend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChannelDefDlg message handlers

void CChannelDefDlg::OnChannel1() 
{
	UpdateSlot(1);
}

void CChannelDefDlg::OnChannel10() 
{
	UpdateSlot(10);
}

void CChannelDefDlg::OnChannel11() 
{
	UpdateSlot(11);
}

void CChannelDefDlg::OnChannel12() 
{
	UpdateSlot(12);
}

void CChannelDefDlg::OnChannel13() 
{
	UpdateSlot(13);
}

void CChannelDefDlg::OnChannel14() 
{
	UpdateSlot(14);
}

void CChannelDefDlg::OnChannel15() 
{
	UpdateSlot(15);
}

void CChannelDefDlg::OnChannel16() 
{
	UpdateSlot(16);
}

void CChannelDefDlg::OnChannel17() 
{
	UpdateSlot(17);
}

void CChannelDefDlg::OnChannel18() 
{
	UpdateSlot(18);
}

void CChannelDefDlg::OnChannel19() 
{
	UpdateSlot(19);
}

void CChannelDefDlg::OnChannel2() 
{
	UpdateSlot(2);
}

void CChannelDefDlg::OnChannel20() 
{
	UpdateSlot(20);
}

void CChannelDefDlg::OnChannel21() 
{
	UpdateSlot(21);
}

void CChannelDefDlg::OnChannel22() 
{
	UpdateSlot(22);
}

void CChannelDefDlg::OnChannel23() 
{
	UpdateSlot(23);
}

void CChannelDefDlg::OnChannel24() 
{
	UpdateSlot(24);
}

void CChannelDefDlg::OnChannel25() 
{
	UpdateSlot(25);
}

void CChannelDefDlg::OnChannel26() 
{
	UpdateSlot(26);
}

void CChannelDefDlg::OnChannel27() 
{
	UpdateSlot(27);
}

void CChannelDefDlg::OnChannel28() 
{
	UpdateSlot(28);
}

void CChannelDefDlg::OnChannel29() 
{
	UpdateSlot(29);
}

void CChannelDefDlg::OnChannel3() 
{
	UpdateSlot(3);
}

void CChannelDefDlg::OnChannel30() 
{
	UpdateSlot(30);
}

void CChannelDefDlg::OnChannel31() 
{
	UpdateSlot(31);
}

void CChannelDefDlg::OnChannel32() 
{
	UpdateSlot(32);
}

void CChannelDefDlg::OnChannel4() 
{
	UpdateSlot(4);
}

void CChannelDefDlg::OnChannel5() 
{
	UpdateSlot(5);
}

void CChannelDefDlg::OnChannel6() 
{
	UpdateSlot(6);
}

void CChannelDefDlg::OnChannel7() 
{
	UpdateSlot(7);
}

void CChannelDefDlg::OnChannel8() 
{
	UpdateSlot(8);
}

void CChannelDefDlg::OnChannel9() 
{
	UpdateSlot(9);
}

void CChannelDefDlg::OnChannel33() 
{
	UpdateSlot(33);
}

void CChannelDefDlg::OnChannel34() 
{
	UpdateSlot(34);
}

void CChannelDefDlg::OnChannel35() 
{
	UpdateSlot(35);
}

void CChannelDefDlg::OnChannel36() 
{
	UpdateSlot(36);
}

BOOL CChannelDefDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	pBtnList[0] = &m_ch1;
	pBtnList[1] = &m_ch2;
	pBtnList[2] = &m_ch3;
	pBtnList[3] = &m_ch4;
	pBtnList[4] = &m_ch5;
	pBtnList[5] = &m_ch6;
	pBtnList[6] = &m_ch7;
	pBtnList[7] = &m_ch8;
	pBtnList[8] = &m_ch9;
	pBtnList[9] = &m_ch10;
	pBtnList[10] = &m_ch11;
	pBtnList[11] = &m_ch12;
	pBtnList[12] = &m_ch13;
	pBtnList[13] = &m_ch14;
	pBtnList[14] = &m_ch15;
	pBtnList[15] = &m_ch16;
	pBtnList[16] = &m_ch17;
	pBtnList[17] = &m_ch18;
	pBtnList[18] = &m_ch19;
	pBtnList[19] = &m_ch20;
	pBtnList[20] = &m_ch21;
	pBtnList[21] = &m_ch22;
	pBtnList[22] = &m_ch23;
	pBtnList[23] = &m_ch24;
	pBtnList[24] = &m_ch25;
	pBtnList[25] = &m_ch26;
	pBtnList[26] = &m_ch27;
	pBtnList[27] = &m_ch28;
	pBtnList[28] = &m_ch29;
	pBtnList[29] = &m_ch30;
	pBtnList[30] = &m_ch31;
	pBtnList[31] = &m_ch32;
	pBtnList[32] = &m_ch33;
	pBtnList[33] = &m_ch34;
	pBtnList[34] = &m_ch35;
	pBtnList[35] = &m_ch36;
	
	SetWindowText("Select Lines to display");
	for(int Index =0;Index < int(NameList.size());Index++)
	{
		pBtnList[Index]->SetWindowText(NameList[Index]);
//		VisMask[Index+1] = TRUE;
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChannelDefDlg::OnOK() 
{
	CDialog::OnOK();
}

void CChannelDefDlg::UpdateSlot(short Slot)
{
	VisMask[Slot] = !VisMask[Slot];
}

void CChannelDefDlg::OnClearall() 
{
	for (int i = 1; i <= 32; i++) 
	{
		ClearSlot(i);
	}
}

void CChannelDefDlg::ClearSlot(short Slot)
{
	if (VisMask[Slot]) 
	{
		VisMask[Slot] = FALSE; 
		pBtnList[Slot-1]->SetCheck(FALSE);
	}
}

void CChannelDefDlg::OnCleartrend() 
{
	for (int Index = 0; Index < 32; Index++) 
	{
		pBtnList[Index]->SetWindowText("");
		VisMask[Index+1] = FALSE;
	}
	UpdateData(FALSE);
}

void CChannelDefDlg::GetSelNames(std::vector<CString> &Names)
{
	Names.clear();
	for(int Index =1;Index < 37;Index++)
	{
		if ((VisMask[Index]) && (int(NameList.size()) > Index-1)) 
		{
			Names.push_back(NameList[Index-1]);
		}
	}
}
