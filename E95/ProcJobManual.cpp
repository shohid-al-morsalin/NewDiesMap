#include "stdafx.h"
#include "strSOp.h"
#include "e95.h"

#include "128/GEM/ObjectsContainer.h"
#include "413/MotorS2.h"
#include "413App.h"
#include "AttachDelete.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "InfoPanelMain.h"
#include "ManualAlgnDlg.h"
#include "SelScanNumDlg.h"
#include "SRC/DOSUtil.h"
#include "SRC/LogInDataNew.h"
#include "SYS/CUMMsg.h"
#include "SYS/Tool.h"

#include "Recipe413.h"
#include "MPoint.h"
#include "Data.h"

#include "ProcJobManual.h"

#ifdef _DEBUG

#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAXLOGITEMS	2048

/////////////////////////////////////////////////////////////////////////////
// CProcJobManual dialog

CProcJobManual::CProcJobManual(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CProcJobManual::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobManual)
	CarrierID = _T("");
	//}}AFX_DATA_INIT
	ID = 0; 	// ID is zero offset
	m_Parent = NULL;

	bAssign = FALSE;
}

CProcJobManual::~CProcJobManual() {}

void CProcJobManual::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobManual)
	DDX_Control(pDX, IDC_RECIPE9, m_R9);
	DDX_Control(pDX, IDC_RECIPE8, m_R8);
	DDX_Control(pDX, IDC_RECIPE7, m_R7);
	DDX_Control(pDX, IDC_RECIPE6, m_R6);
	DDX_Control(pDX, IDC_RECIPE5, m_R5);
	DDX_Control(pDX, IDC_RECIPE4, m_R4);
	DDX_Control(pDX, IDC_RECIPE3, m_R3);
	DDX_Control(pDX, IDC_RECIPE20, m_R20);
	DDX_Control(pDX, IDC_RECIPE2, m_R2);
	DDX_Control(pDX, IDC_RECIPE19, m_R19);
	DDX_Control(pDX, IDC_RECIPE18, m_R18);
	DDX_Control(pDX, IDC_RECIPE17, m_R17);
	DDX_Control(pDX, IDC_RECIPE16, m_R16);
	DDX_Control(pDX, IDC_RECIPE15, m_R15);
	DDX_Control(pDX, IDC_RECIPE14, m_R14);
	DDX_Control(pDX, IDC_RECIPE13, m_R13);
	DDX_Control(pDX, IDC_RECIPE12, m_R12);
	DDX_Control(pDX, IDC_RECIPE11, m_R11);
	DDX_Control(pDX, IDC_RECIPE10, m_R10);
	DDX_Control(pDX, IDC_RECIPE1, m_R1);
	DDX_Control(pDX, IDC_RECIPE21, m_R21);
	DDX_Control(pDX, IDC_RECIPE22, m_R22);
	DDX_Control(pDX, IDC_RECIPE23, m_R23);
	DDX_Control(pDX, IDC_RECIPE24, m_R24);
	DDX_Control(pDX, IDC_RECIPE25, m_R25);
	DDX_Control(pDX, IDC_RECIPE26, m_R26);
	DDX_Control(pDX, IDC_RECIPE27, m_R27);
	DDX_Control(pDX, IDC_RECIPE28, m_R28);
	DDX_Control(pDX, IDC_RECIPE29, m_R29);
	DDX_Control(pDX, IDC_RECIPE30, m_R30);
	DDX_Control(pDX, IDC_RECIPE31, m_R31);
	DDX_Control(pDX, IDC_RECIPE32, m_R32);
	DDX_Control(pDX, IDC_RECIPE33, m_R33);
	DDX_Control(pDX, IDC_RECIPE34, m_R34);
	DDX_Control(pDX, IDC_RECIPE35, m_R35);
	DDX_Control(pDX, IDC_RECIPE36, m_R36);
	DDX_Control(pDX, IDC_ASSIGN, m_cAssign);
	DDX_Text(pDX, IDC_CARRIERID, CarrierID);
	DDX_Text(pDX, IDC_WAFERID, m_WaferId);
	DDX_Text(pDX, IDC_OPERATOR, m_Operator);
	DDX_Text(pDX, IDC_LOTID, m_LotID);
	DDX_Text(pDX, IDC_WAFER_REPEATS, p413App->Global.WaferRepeat);
	DDX_Text(pDX, IDC_CASSETTE_REPEATS, p413App->Global.CassetteRepeat);
	DDX_Text(pDX, IDC_WAFERSIZE, m_WaferSize); // 20251014_Mohir
	DDX_Text(pDX, IDC_STATION, m_Station); // 20251014_Mohir
	DDX_Text(pDX, IDC_WAFERTYPE, m_WaferType); // 20251014_Mohir


	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcJobManual, CResizableDialog)
	//{{AFX_MSG_MAP(CProcJobManual)
	ON_BN_CLICKED(IDC_ASSIGN, OnAssign)
	ON_BN_CLICKED(IDC_RECIPE1, OnRecipe1)
	ON_BN_CLICKED(IDC_RECIPE2, OnRecipe2)
	ON_BN_CLICKED(IDC_RECIPE3, OnRecipe3)
	ON_BN_CLICKED(IDC_RECIPE4, OnRecipe4)
	ON_BN_CLICKED(IDC_RECIPE5, OnRecipe5)
	ON_BN_CLICKED(IDC_RECIPE6, OnRecipe6)
	ON_BN_CLICKED(IDC_RECIPE7, OnRecipe7)
	ON_BN_CLICKED(IDC_RECIPE8, OnRecipe8)
	ON_BN_CLICKED(IDC_RECIPE9, OnRecipe9)
	ON_BN_CLICKED(IDC_RECIPE10, OnRecipe10)
	ON_BN_CLICKED(IDC_RECIPE11, OnRecipe11)
	ON_BN_CLICKED(IDC_RECIPE12, OnRecipe12)
	ON_BN_CLICKED(IDC_RECIPE13, OnRecipe13)
	ON_BN_CLICKED(IDC_RECIPE14, OnRecipe14)
	ON_BN_CLICKED(IDC_RECIPE15, OnRecipe15)
	ON_BN_CLICKED(IDC_RECIPE16, OnRecipe16)
	ON_BN_CLICKED(IDC_RECIPE17, OnRecipe17)
	ON_BN_CLICKED(IDC_RECIPE18, OnRecipe18)
	ON_BN_CLICKED(IDC_RECIPE19, OnRecipe19)
	ON_BN_CLICKED(IDC_RECIPE20, OnRecipe20)
	ON_BN_CLICKED(IDC_LOADWAFER, OnLoadwafer)
	ON_BN_CLICKED(IDC_RECIPE21, OnRecipe21)
	ON_BN_CLICKED(IDC_RECIPE22, OnRecipe22)
	ON_BN_CLICKED(IDC_RECIPE23, OnRecipe23)
	ON_BN_CLICKED(IDC_RECIPE24, OnRecipe24)
	ON_BN_CLICKED(IDC_RECIPE25, OnRecipe25)
	ON_BN_CLICKED(IDC_RECIPE26, OnRecipe26)
	ON_BN_CLICKED(IDC_RECIPE27, OnRecipe27)
	ON_BN_CLICKED(IDC_RECIPE28, OnRecipe28)
	ON_BN_CLICKED(IDC_RECIPE29, OnRecipe29)
	ON_BN_CLICKED(IDC_RECIPE30, OnRecipe30)
	ON_BN_CLICKED(IDC_RUNRECIPE, OnRunrecipe)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RECIPE31, OnRecipe31)
	ON_BN_CLICKED(IDC_RECIPE32, OnRecipe32)
	ON_BN_CLICKED(IDC_RECIPE33, OnRecipe33)
	ON_BN_CLICKED(IDC_RECIPE34, OnRecipe34)
	ON_BN_CLICKED(IDC_RECIPE35, OnRecipe35)
	ON_BN_CLICKED(IDC_RECIPE36, OnRecipe36)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcJobManual message handlers

long CProcJobManual::OnTabSelected(WPARAM wP, LPARAM lP) {
	if (!bTabSelected) {
		if (pUDev) {
			CString msg;
			msg.Format("Jobs\n%s", (char*)lP);
			pUDev->SetCurrentView(msg);
		}

		bTabSelected = TRUE;
		p413App->Global.bAborting = p413App->Global.bStop = FALSE;
	}

	return 0;
}

long CProcJobManual::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
	}
	bTabSelected = FALSE;

	return 0;
}

BOOL CProcJobManual::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_LOADWAFER, TOP_RIGHT);
	AddAnchor(IDC_RUNRECIPE, TOP_RIGHT);
	AddAnchor(IDC_ASSIGN, BOTTOM_RIGHT);

	m_R1.SetWindowText(DosUtil.GetRecipeButtonStr(1));
	m_R2.SetWindowText(DosUtil.GetRecipeButtonStr(2));
	m_R3.SetWindowText(DosUtil.GetRecipeButtonStr(3));
	m_R4.SetWindowText(DosUtil.GetRecipeButtonStr(4));
	m_R5.SetWindowText(DosUtil.GetRecipeButtonStr(5));
	m_R6.SetWindowText(DosUtil.GetRecipeButtonStr(6));
	m_R7.SetWindowText(DosUtil.GetRecipeButtonStr(7));
	m_R8.SetWindowText(DosUtil.GetRecipeButtonStr(8));
	m_R9.SetWindowText(DosUtil.GetRecipeButtonStr(9));
	m_R10.SetWindowText(DosUtil.GetRecipeButtonStr(10));
	m_R11.SetWindowText(DosUtil.GetRecipeButtonStr(11));
	m_R12.SetWindowText(DosUtil.GetRecipeButtonStr(12));
	m_R13.SetWindowText(DosUtil.GetRecipeButtonStr(13));
	m_R14.SetWindowText(DosUtil.GetRecipeButtonStr(14));
	m_R15.SetWindowText(DosUtil.GetRecipeButtonStr(15));
	m_R16.SetWindowText(DosUtil.GetRecipeButtonStr(16));
	m_R17.SetWindowText(DosUtil.GetRecipeButtonStr(17));
	m_R18.SetWindowText(DosUtil.GetRecipeButtonStr(18));
	m_R19.SetWindowText(DosUtil.GetRecipeButtonStr(19));
	m_R20.SetWindowText(DosUtil.GetRecipeButtonStr(20));
	m_R21.SetWindowText(DosUtil.GetRecipeButtonStr(21));
	m_R22.SetWindowText(DosUtil.GetRecipeButtonStr(22));
	m_R23.SetWindowText(DosUtil.GetRecipeButtonStr(23));
	m_R24.SetWindowText(DosUtil.GetRecipeButtonStr(24));
	m_R25.SetWindowText(DosUtil.GetRecipeButtonStr(25));
	m_R26.SetWindowText(DosUtil.GetRecipeButtonStr(26));
	m_R27.SetWindowText(DosUtil.GetRecipeButtonStr(27));
	m_R28.SetWindowText(DosUtil.GetRecipeButtonStr(28));
	m_R29.SetWindowText(DosUtil.GetRecipeButtonStr(29));
	m_R30.SetWindowText(DosUtil.GetRecipeButtonStr(30));
	m_R31.SetWindowText(DosUtil.GetRecipeButtonStr(31));
	m_R32.SetWindowText(DosUtil.GetRecipeButtonStr(32));
	m_R33.SetWindowText(DosUtil.GetRecipeButtonStr(33));
	m_R34.SetWindowText(DosUtil.GetRecipeButtonStr(34));
	m_R35.SetWindowText(DosUtil.GetRecipeButtonStr(35));
	m_R36.SetWindowText(DosUtil.GetRecipeButtonStr(36));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

CInfoPanelMain* CProcJobManual::GetParentWnd() {
	return m_Parent;
}

void CProcJobManual::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "PJManual");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CProcJobManual::DestroyWindow() {
	return CResizableDialog::DestroyWindow();
}

void CProcJobManual::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

void CProcJobManual::OnAssign() {
	if (bAssign) {
		bAssign = FALSE;
		m_cAssign.SetWindowText("Assign");
	}
	else {
		short AuthenLevel;
		if (!pGDev->AuthenticateUser(AuthenLevel)) {
			return;
		}
		if (AuthenLevel < CLogInDataNew::RUNRCP) {
			return;
		}
		bAssign = TRUE;
		m_cAssign.SetWindowText("Execute");
	}
}

void CProcJobManual::OnRecipe1() {
	Run(1);
}

void CProcJobManual::OnRecipe2() {
	Run(2);
}

void CProcJobManual::OnRecipe3() {
	Run(3);
}

void CProcJobManual::OnRecipe4() {
	Run(4);
}

void CProcJobManual::OnRecipe5() {
	Run(5);
}

void CProcJobManual::OnRecipe6() {
	Run(6);
}

void CProcJobManual::OnRecipe7() {
	Run(7);
}

void CProcJobManual::OnRecipe8() {
	Run(8);
}

void CProcJobManual::OnRecipe9() {
	Run(9);
}

void CProcJobManual::OnRecipe10() {
	Run(10);
}

void CProcJobManual::OnRecipe11() {
	Run(11);
}

void CProcJobManual::OnRecipe12() {
	Run(12);
}

void CProcJobManual::OnRecipe13() {
	Run(13);
}

void CProcJobManual::OnRecipe14() {
	Run(14);
}

void CProcJobManual::OnRecipe15() {
	Run(15);
}

void CProcJobManual::OnRecipe16() {
	Run(16);
}

void CProcJobManual::OnRecipe17() {
	Run(17);
}

void CProcJobManual::OnRecipe18() {
	Run(18);
}

void CProcJobManual::OnRecipe19() {
	Run(19);
}

void CProcJobManual::OnRecipe20() {
	Run(20);
}

BOOL CProcJobManual::Run(short RcpNo) {
	if (bAssign) {
		CAttachDelete dlg;
		if (dlg.DoModal() == IDOK) {
			return AssignRecipe(RcpNo);
		}
		else {
			if (dlg.option == 1) {
				return DeleteRecipe(RcpNo);
			}
		}
	}
	else {
		return ExecuteRecipe(RcpNo);
	}
	return FALSE;
}

BOOL CProcJobManual::AssignRecipe(short RcpNo) {
	CFileDialog dlg(TRUE, _T("RCP"), NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("Recipe files (*.RCP)|*.RCP|"));
	dlg.m_ofn.lpstrInitialDir = DosUtil.GetRecipeDir();
	if (dlg.DoModal() == IDOK) {
		CString filename = "";
		filename = dlg.GetPathName();
		DosUtil.ExtractFilename(filename);
		switch (RcpNo) {
		case 1:
			m_R1.SetWindowText(filename);
			break;
		case 2:
			m_R2.SetWindowText(filename);
			break;
		case 3:
			m_R3.SetWindowText(filename);
			break;
		case 4:
			m_R4.SetWindowText(filename);
			break;
		case 5:
			m_R5.SetWindowText(filename);
			break;
		case 6:
			m_R6.SetWindowText(filename);
			break;
		case 7:
			m_R7.SetWindowText(filename);
			break;
		case 8:
			m_R8.SetWindowText(filename);
			break;
		case 9:
			m_R9.SetWindowText(filename);
			break;
		case 10:
			m_R10.SetWindowText(filename);
			break;
		case 11:
			m_R11.SetWindowText(filename);
			break;
		case 12:
			m_R12.SetWindowText(filename);
			break;
		case 13:
			m_R13.SetWindowText(filename);
			break;
		case 14:
			m_R14.SetWindowText(filename);
			break;
		case 15:
			m_R15.SetWindowText(filename);
			break;
		case 16:
			m_R16.SetWindowText(filename);
			break;
		case 17:
			m_R17.SetWindowText(filename);
			break;
		case 18:
			m_R18.SetWindowText(filename);
			break;
		case 19:
			m_R19.SetWindowText(filename);
			break;
		case 20:
			m_R20.SetWindowText(filename);
			break;
		case 21:
			m_R21.SetWindowText(filename);
			break;
		case 22:
			m_R22.SetWindowText(filename);
			break;
		case 23:
			m_R23.SetWindowText(filename);
			break;
		case 24:
			m_R24.SetWindowText(filename);
			break;
		case 25:
			m_R25.SetWindowText(filename);
			break;
		case 26:
			m_R26.SetWindowText(filename);
			break;
		case 27:
			m_R27.SetWindowText(filename);
			break;
		case 28:
			m_R28.SetWindowText(filename);
			break;
		case 29:
			m_R29.SetWindowText(filename);
			break;
		case 30:
			m_R30.SetWindowText(filename);
			break;
		case 31:
			m_R31.SetWindowText(filename);
			break;
		case 32:
			m_R32.SetWindowText(filename);
			break;
		case 33:
			m_R33.SetWindowText(filename);
			break;
		case 34:
			m_R34.SetWindowText(filename);
			break;
		case 35:
			m_R35.SetWindowText(filename);
			break;
		case 36:
			m_R36.SetWindowText(filename);
			break;
		}
		DosUtil.SetRecipeButtonStr(RcpNo, filename);
	}

	return TRUE;
}

BOOL CProcJobManual::DeleteRecipe(short RcpNo) {
	CString filename = "";
	switch (RcpNo) {
	case 1:
		m_R1.SetWindowText(filename);
		break;
	case 2:
		m_R2.SetWindowText(filename);
		break;
	case 3:
		m_R3.SetWindowText(filename);
		break;
	case 4:
		m_R4.SetWindowText(filename);
		break;
	case 5:
		m_R5.SetWindowText(filename);
		break;
	case 6:
		m_R6.SetWindowText(filename);
		break;
	case 7:
		m_R7.SetWindowText(filename);
		break;
	case 8:
		m_R8.SetWindowText(filename);
		break;
	case 9:
		m_R9.SetWindowText(filename);
		break;
	case 10:
		m_R10.SetWindowText(filename);
		break;
	case 11:
		m_R11.SetWindowText(filename);
		break;
	case 12:
		m_R12.SetWindowText(filename);
		break;
	case 13:
		m_R13.SetWindowText(filename);
		break;
	case 14:
		m_R14.SetWindowText(filename);
		break;
	case 15:
		m_R15.SetWindowText(filename);
		break;
	case 16:
		m_R16.SetWindowText(filename);
		break;
	case 17:
		m_R17.SetWindowText(filename);
		break;
	case 18:
		m_R18.SetWindowText(filename);
		break;
	case 19:
		m_R19.SetWindowText(filename);
		break;
	case 20:
		m_R20.SetWindowText(filename);
		break;
	case 21:
		m_R21.SetWindowText(filename);
		break;
	case 22:
		m_R22.SetWindowText(filename);
		break;
	case 23:
		m_R23.SetWindowText(filename);
		break;
	case 24:
		m_R24.SetWindowText(filename);
		break;
	case 25:
		m_R25.SetWindowText(filename);
		break;
	case 26:
		m_R26.SetWindowText(filename);
		break;
	case 27:
		m_R27.SetWindowText(filename);
		break;
	case 28:
		m_R28.SetWindowText(filename);
		break;
	case 29:
		m_R29.SetWindowText(filename);
		break;
	case 30:
		m_R30.SetWindowText(filename);
		break;
	case 31:
		m_R31.SetWindowText(filename);
		break;
	case 32:
		m_R32.SetWindowText(filename);
		break;
	case 33:
		m_R33.SetWindowText(filename);
		break;
	case 34:
		m_R34.SetWindowText(filename);
		break;
	case 35:
		m_R35.SetWindowText(filename);
		break;
	case 36:
		m_R36.SetWindowText(filename);
		break;
	}
	DosUtil.SetRecipeButtonStr(RcpNo, filename);

	return TRUE;
}

BOOL CProcJobManual::ExecuteRecipe(short RcpNo) {
	return ExecuteRecipe(GetRecipeName(RcpNo));
}

BOOL CProcJobManual::ExecuteRecipe(CString RcpName) {
	UpdateData(TRUE);

	CString msg;
	msg.Format("Execute recipe: %s", RcpName);
	Log(msg);

	p413App->Global.bAborting = FALSE;

	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	CNCas* pCas = &pInfo->Cas;

	pInfo->Cas.winfo.Clear();

	strscpy(pCas->Recipe413.RcpeName, RCPNAMELEN, RcpName + ".RCP");
	if (!p413App->LoadRecipe(&pCas->Recipe413, 1)) {
		Log("Could not load recipe", 2);
		return FALSE;
	}
	msg.Format("Recipe %s loaded", pCas->Recipe413.RcpeName);
	Log(msg);

	// Preload first scan file if stress measurement is specified [3/30/2013 Yuen]
	for (int i = 0; i < MAXMEPASET; i++) {
		if (pCas->Recipe413.IsStress(i)) {
			CSelScanNumDlg dlg;
			dlg.m_CarrierID = CarrierID;
			dlg.m_WaferID = m_WaferId;
			//dlg.m_WaferType = m_WaferType;

			if (dlg.DoModal() == IDOK) {
				pCas->Recipe413.ScanNo = dlg.ScanNum;
				m_WaferId = dlg.m_WaferID;
				m_WaferId = dlg.m_WaferID;
 				CarrierID = dlg.m_CarrierID;
				CCasMgr CasMgr;
				if (CasMgr.OpenCassette(dlg.filename)) {
					if (CasMgr.pCas) {
						*pCas = *CasMgr.pCas;
					}
				}
				break;
			}
			else {
				return FALSE;
			}
		}
	}

	// [02072024 ZHIMING
	if (p413App->Global.Confg.bEchoMicroscopeMod == FALSE &&
		p413App->Global.Confg.bThinFilmMicroscopeMod == FALSE) {
		p413App->ResetEchoSamplingProperty(&pCas->Recipe413, NULL);
		if (pCas->Recipe413.IsPatternMatching()) {
			CManualAlgnDlg dlg;
			dlg.m_Operator = m_Operator;
			dlg.m_WaferID = m_WaferId;
			dlg.m_CarrierID = CarrierID;
			dlg.m_LotID = m_LotID;
			//[[20251022_Mohir
			dlg.m_WaferType = m_WaferType;
			dlg.m_WaferSize = m_WaferSize;
			dlg.m_Station = m_Station;
			//]]
			dlg.pRcp = &pInfo->Cas.Recipe413;
			if (dlg.DoModal() != IDOK) {
				return FALSE;
			}
			m_Operator = dlg.m_Operator;
			m_WaferId = dlg.m_WaferID;
			CarrierID = dlg.m_CarrierID;
			m_LotID = dlg.m_LotID;
			//[20251022
			m_WaferSize = dlg.m_WaferSize;
			m_WaferType = dlg.m_WaferType;
			m_Station = dlg.m_Station;
			//]
			UpdateData(FALSE);
		}
	}
	// ]

	// Loading baseline information [11/2/2011 Administrator]
	if (pCas->Recipe413.IsEchoBowWarpMeasurement()) {
		pCas->Recipe413.AddBaseline();
	}

	pCas->JobInfo.WaferSize = m_WaferSize; // 20251014_Mohir
	pCas->JobInfo.WaferType = m_WaferType; // 20251014_Mohir
	pCas->JobInfo.Station = m_Station; // 20251014_Mohir

	pCas->JobInfo.batchname = m_LotID;
	pCas->JobInfo.operatorname = m_Operator;

	if (CarrierID.GetLength() < 1) {
		m_CarrierID = CTime::GetCurrentTime().Format("%y%m%d%H%M%S");
	}
	else {
		m_CarrierID = CarrierID;
	}

	msg.Format("Carrier ID: %s", m_CarrierID);
	Log(msg);

	pCas->JobInfo.CarrierID = m_CarrierID;

	CObjectsContainer* pGEMObjs = pGDev->pGEMObjs;
	pGEMObjs->SetCOAccessingStatus(m_CarrierID, CCarrierObj::INACCESS);
	pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDNOTREAD);
	pGEMObjs->SetCOSlotmapStatus(m_CarrierID, ID + 1, "", CCarrierObj::SMNOTREAD);

	strscpy(pGEMObjs->Cms[ID].CaObj.SlotMaP, MAXCAPACITYLEN, "1111111111111111111111111");

	SetContentMap();

	CWaferInfo* p = pInfo->Cas.winfo.FindWafer(1);
	if (!p) {
		p = new CWaferInfo;
	}
	if (p) {
		p->bSelected = TRUE;
		p->wafernum = 1;
		pInfo->Cas.winfo.Add(p, FALSE);
		p->WaferID = m_WaferId;
		p->LotID = m_LotID;
		//[[20251022_Mohir
		p->WaferType = m_WaferType;
		p->WaferSize = m_WaferSize;
		p->Station = m_Station;
		//]]
		p->RecipeName = pCas->Recipe413.RcpeName;
	}
	else {
		return FALSE;
	}

	pGEMObjs->SetCOIDStatus(m_CarrierID, ID + 1, CCarrierObj::IDVERIFICATIONOK);
	pGEMObjs->SetCOSlotmapStatus(m_CarrierID, ID + 1, pGEMObjs->Cms[ID].CaObj.SlotMaP, CCarrierObj::SMVERIFICATIONOK);

	Tool.SetCasStateQueued(ID + 1);

	if (m_Parent) {
		ShowWindow(SW_HIDE);
		int type = 3;
		m_Parent->PostMessage(UM_TO_PROCJOBPROCESSING, ID, type);
	}

	return TRUE;
}

CString CProcJobManual::GetRecipeName(int RcpNo) {
	CString name = "";
	switch (RcpNo) {
	case 1:
		m_R1.GetWindowText(name);
		break;
	case 2:
		m_R2.GetWindowText(name);
		break;
	case 3:
		m_R3.GetWindowText(name);
		break;
	case 4:
		m_R4.GetWindowText(name);
		break;
	case 5:
		m_R5.GetWindowText(name);
		break;
	case 6:
		m_R6.GetWindowText(name);
		break;
	case 7:
		m_R7.GetWindowText(name);
		break;
	case 8:
		m_R8.GetWindowText(name);
		break;
	case 9:
		m_R9.GetWindowText(name);
		break;
	case 10:
		m_R10.GetWindowText(name);
		break;
	case 11:
		m_R11.GetWindowText(name);
		break;
	case 12:
		m_R12.GetWindowText(name);
		break;
	case 13:
		m_R13.GetWindowText(name);
		break;
	case 14:
		m_R14.GetWindowText(name);
		break;
	case 15:
		m_R15.GetWindowText(name);
		break;
	case 16:
		m_R16.GetWindowText(name);
		break;
	case 17:
		m_R17.GetWindowText(name);
		break;
	case 18:
		m_R18.GetWindowText(name);
		break;
	case 19:
		m_R19.GetWindowText(name);
		break;
	case 20:
		m_R20.GetWindowText(name);
		break;
	case 21:
		m_R21.GetWindowText(name);
		break;
	case 22:
		m_R22.GetWindowText(name);
		break;
	case 23:
		m_R23.GetWindowText(name);
		break;
	case 24:
		m_R24.GetWindowText(name);
		break;
	case 25:
		m_R25.GetWindowText(name);
		break;
	case 26:
		m_R26.GetWindowText(name);
		break;
	case 27:
		m_R27.GetWindowText(name);
		break;
	case 28:
		m_R28.GetWindowText(name);
		break;
	case 29:
		m_R29.GetWindowText(name);
		break;
	case 30:
		m_R30.GetWindowText(name);
		break;
	case 31:
		m_R31.GetWindowText(name);
		break;
	case 32:
		m_R32.GetWindowText(name);
		break;
	case 33:
		m_R33.GetWindowText(name);
		break;
	case 34:
		m_R34.GetWindowText(name);
		break;
	case 35:
		m_R35.GetWindowText(name);
		break;
	case 36:
		m_R36.GetWindowText(name);
		break;
	}

	return name;
}

void CProcJobManual::OnLoadwafer() {
	// Move stage to load position [8/13/2011 FSMT]
	if (!p413App->GotoXYS(p413App->Global.LoadPos.PX, p413App->Global.LoadPos.PY, CCamBase::ECHOCAM)) // 20140223 SCOTT ADD : CHECK RETURN VALUE
	{
		p413App->StageMoveError();
	}

	// Switch off vacuum [8/13/2011 FSMT]
	if (p413App->pMtr) p413App->pMtr->SetChuckVacuum(FALSE);
}

void CProcJobManual::OnRecipe21() {
	Run(21);
}

void CProcJobManual::OnRecipe22() {
	Run(22);
}

void CProcJobManual::OnRecipe23() {
	Run(23);
}

void CProcJobManual::OnRecipe24() {
	Run(24);
}

void CProcJobManual::OnRecipe25() {
	Run(25);
}

void CProcJobManual::OnRecipe26() {
	Run(26);
}

void CProcJobManual::OnRecipe27() {
	Run(27);
}

void CProcJobManual::OnRecipe28() {
	Run(28);
}

void CProcJobManual::OnRecipe29() {
	Run(29);
}

void CProcJobManual::OnRecipe30() {
	Run(30);
}

void CProcJobManual::OnRunrecipe() {
	CFileDialog dlg(TRUE,
		"RCP",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Recipe file|*.RCP|");

	CString filename = "";
	dlg.m_ofn.lpstrInitialDir = DosUtil.GetRecipeDir();

	if (dlg.DoModal() == IDOK) {
		filename = dlg.GetPathName();
		DosUtil.ExtractFilename(filename);
		ExecuteRecipe(filename);
	}
}

void CProcJobManual::OnRecipe31() {
	Run(31);
}

void CProcJobManual::OnRecipe32() {
	Run(32);
}

void CProcJobManual::OnRecipe33() {
	Run(33);
}

void CProcJobManual::OnRecipe34() {
	Run(34);
}

void CProcJobManual::OnRecipe35() {
	Run(35);
}

void CProcJobManual::OnRecipe36() {
	Run(36);
}
