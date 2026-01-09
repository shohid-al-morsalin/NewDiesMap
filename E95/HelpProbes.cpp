#include "stdafx.h"
#include "e95.h"

#include "413App.h"
// #include "Global413.h"
// #include "MParam.h"

#include "HelpProbes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelpProbes dialog

CHelpProbes::CHelpProbes(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpProbes::IDD, pParent) {
	//{{AFX_DATA_INIT(CHelpProbes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CHelpProbes::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelpProbes)
	DDX_Control(pDX, IDC_LIST1, m_cList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHelpProbes, CDialog)
	//{{AFX_MSG_MAP(CHelpProbes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpProbes message handlers

BOOL CHelpProbes::OnInitDialog() {
	CDialog::OnInitDialog();

	char* dwCjName[] =
	{
		"#", "Description"
	};
	int nSize[] = { 32, 360 };
	LV_COLUMN nListColumn;
	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_cList.InsertColumn(i, &nListColumn);
	}
	m_cList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	if (p413App->Global.Confg.bEchoMod) {
		//SetItem(MParam::ECHO1PRB, "Thickness top probe (TTOP)"); // Commented MORSALIN//01012025
		//SetItem(MParam::ECHO2PRB, "Thickness bottom probe (TBOT)"); // Commented MORSALIN//01012025
		//SetItem(MParam::ECHOTT1PRB, "Thickness dual probe (TT)"); // Commented MORSALIN//01012025
	}
	if (0/*p413App->Global.Confg.bEchoMicroscopeMod*/) {
		SetItem(MParam::ECHO1AFPRB, "Thickness top probe with auto focus (TAFTOP)");
		//SetItem(MParam::ECHO2AFPRB, "Thickness bottom probe with auto focus (TAFBOT)");
		SetItem(MParam::ECHOTTAFPRB, "Thickness dual probe with auto focus (TTAF)");
	}
	if (p413App->Global.Confg.bThinFilmMod) {
		SetItem(MParam::ODSTIPRB, "Thin silicon multi points (TSIM)");
		SetItem(MParam::TFILMSDIPRB, "Thin film position 2 (TSI2)"); // Not use at the moment [5/13/2013 user]
		SetItem(MParam::TFILMPRB, "Thin silicon single point (TSI1)");
		SetItem(MParam::TFILMSPMV, "Thin Film (Single Point, Multi Value) (TFM)");
	}
	if (p413App->Global.Confg.bThinFilmMicroscopeMod) {
		SetItem(MParam::TFILMFASTPRB, "Thin Film One Step Pat. Match. (Single Point) (TF1)");
	}
	if (p413App->Global.Confg.bTrimMod) {
		// [  // Commented MORSALIN//01012025
		/*SetItem(MParam::NTRIMPROBE1, "Trim width narrow method 1 (NTW1)");
		SetItem(MParam::WTRIMPRB1, "Trim width wide method 1 (WTW1)");
		SetItem(MParam::NTRIMPROBE2, "Trim width narrow method 2 (NTW2)");
		SetItem(MParam::WTRIMPRB2, "Trim width wide method 2 (WTW2)");
		SetItem(MParam::NTRIMPROBE3, "Trim width narrow method 3 (NTW3)");
		SetItem(MParam::WTRIMPRB3, "Trim width wide method 3(WTW3)");
		SetItem(MParam::TRIMWHPROB, "Trim width and height combined (TWH)");
		SetItem(MParam::STEPHEIGHTCOMPRB, "Step Height for monitor wafer (SPH)");*/
		// ]
		//SetItem(MParam::STEPHEIGHTTHKPRB, "Step Height (thickness) (SHT)");
		//SetItem(MParam::STEPHEIGHTTHKBPRB, "Step Height Bottom Probe (thickness) (SHTB)");
		//SetItem(MParam::STEPHEIGHTDISTPRB, "Trim Height (distance)"); // Commented MORSALIN//01012025
		SetItem(MParam::BEVELPRB, "Bevel width (BVW)");
	}
	if (0) {
		SetItem(MParam::BUMPHEIGHTTPRB, "Bump Height Top Probe (generic)");
		//SetItem(MParam::BUMPHEIGHTBPRB, "Bump Height Bottom Probe (generic)");
		SetItem(MParam::TRENCHTOPPRB, "Trench depth using Top Probe (generic)");
		//SetItem(MParam::TRENCHBOTPRB, "Trench depth using Bottom Probe (generic)");
	}
	if (p413App->Global.bEnableIntensity) {
		//SetItem(MParam::INTENSI1PRB, "Intensity top probe"); // Commented MORSALIN//01012025
		//SetItem(MParam::INTENSI2PRB, "Intensity bottom probe"); // Commented MORSALIN//01012025
	}

	if (p413App->Global.bEnablePosition) {
		//SetItem(MParam::PPOS1PRB, "Position top probe"); // Commented MORSALIN//01012025
		//SetItem(MParam::PPOS2PRB, "Position bottom probe"); // Commented MORSALIN//01012025
	}

	if (p413App->Global.bEnableMaths) {
		//SetItem(MParam::MATHPRB, "Maths"); // Commented MORSALIN//01012025
	}

	if (p413App->Global.bEnableBow && p413App->Global.bEnableWarp) {
		SetItem(MParam::BOWWARP1, "Bow & warp top probe (BWT)");
		SetItem(MParam::BOWWARP2, "Bow & warp bottom probe (BWB)");
	}

	// [ 01182024 ZHIMING
	if (p413App->Global.Confg.bConfocalMod) {
		SetItem(MParam::CONFOCAL_TOP, "Confocal Top Probe Measurement (CTOP)");
		SetItem(MParam::CONFOCAL_STIR, "Confocal Site Total Indicator Reading (CSTIR)"); //24102024/MORSALIN
		//SetItem(MParam::CONFOCAL_TIR, "Confocal Total Indicator Reading (CTIR)"); //24102024/MORSALIN // Commented MORSALIN//01012025
		SetItem(MParam::CONFOCAL_LTV, "Confocal Local Thickness Variation (CLTV)"); //24102024/MORSALIN
		SetItem(MParam::CONFOCAL_ROUGHNESS, "Confocal Roughness (CROU)"); //12102024

		//SetItem(MParam::CONFOCAL_LTV_STIR, "Confocal LTV & STIR (LTV-STIR)"); //24102024/MORSALIN  // Commented MORSALIN//01012025
		

		if (p413App->Global.bDualConfocal) {
			SetItem(MParam::CONFOCAL_BOT, "Confocal Bottom Probe Measurement (CBOT)");
			SetItem(MParam::CONFOCAL_TOTAL, "Confocal Dual Probe Measurement (CTT)"); // Tushar  //20231227
			SetItem(MParam::CONFOCAL_DUALWARP, "Confocal Dual Probe Bow & Warp (CDW)"); // Tushar  //23102024

		}
		if (p413App->Global.bEnableWarp) {
			SetItem(MParam::CONFOCAL_WARP, "Confocal Warp probe (CW)"); //20221108
		}
	}
	// ]

	//SetItem(MParam::POFPRB, "Pass or Fail");
	//SetItem(MParam::BESTVALUEPRB, "Best Value");
	//[ 03202024 ----> Mahedi Kamal {for adding rough probe}
	if (p413App->Global.Confg.bRoughness) {
		//SetItem(MParam::ROUGHPRB, "Roughness"); // ARIF COMMENTED
	}
	//]
	// //[ 08132024 ----> Mahedi Kamal 
	if (p413App->Global.Confg.bTaiko) {
		SetItem(MParam::RINGPRB, "Ring Width(Taiko)");
	}
	//]
	//SetItem(MParam::SHAPEFAC1PRB, "Shape Factor top probe");
	//SetItem(MParam::SHAPEFAC2PRB, "Shape Factor bottom probe");
	//SetItem(MParam::RADIALMULTITPRB, "Radial multi points from the top");
	//SetItem(MParam::RADIALMULTIBPRB, "Radial multi points from the bottom");
	//SetItem(MParam::STRESSPRB, "Stress measurement from the top");

	if (p413App->Global.Confg.bPatMatchMod) {
		if (p413App->Global.Confg.bEchoMicroscopeMod) {
			// [  // Commented MORSALIN//01012025
			//SetItem(MParam::ECMARK1, "Echo probe position marker 1 (MKE1)"); 
			//SetItem(MParam::ECMARK2, "Echo probe position marker 2 (MKE2)");			//20220691/yukchiu / command out from original for use MK2
			// ]
		}
		if (p413App->Global.Confg.bThinFilmMicroscopeMod) {
			SetItem(MParam::TFMARK1, "Thin film probe position marker 1 (MKT1)");
			SetItem(MParam::TFMARK2, "Thin film probe position marker 2 (MKT2)");			//20220519/yukchiu / command out from original for use MK2
		}
	}
	// [6/14/2022 ZHIMING]
	//SetItem(MParam::BEVEL_TF_PRB, "Bevel width using thin film probe(BVWTF)");
	// ]
	SetItem(MParam::DISPPRB, "Display only");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CHelpProbes::SetItem(short Prb, CString Desc) {
	CString str;
	str.Format("%d", Prb);
	int idx = m_cList.InsertItem(m_cList.GetItemCount(), str);
	m_cList.SetItemText(idx, 1, Desc);
}