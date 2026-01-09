// SetupProp16Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "SetupProp16Dlg.h"
#include "413App.h"

#include "XTabCtrl.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"
#include "Data.h"
#include "MPoint.h"
#include "afxcoll.h"
#include "..\413\MotorS2.h" //08142024
#include "..\SYS\CUMMsg.h"
#include "413/DMCMotorBase.h"
#include "413/EchoScope/EchoProbeBase.h"
#include "ProbeSwitching.h"

// CSetupProp16Dlg dialog -- Added By Mahedi Kamal - 08092024

CSetupProp16Dlg::CSetupProp16Dlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(CSetupProp16Dlg::IDD, pParent)
{
	g = NULL;
	pRcp = NULL;
	m_cVideo.Name = "Setup16";
}

CSetupProp16Dlg::~CSetupProp16Dlg()
{
}

void CSetupProp16Dlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap); // waferView
	DDX_Control(pDX, IDC_LIST1, m_cPoint); // data point view
	DDX_Control(pDX, IDC_CAMERAVIEW, m_cVideo);
	DDX_Control(pDX, IDC_EDIT_TRW_RADIUS, m_radius);
	DDX_Control(pDX, IDC_EDIT_TRW_ANGLE, m_angle);
	DDX_Control(pDX, IDC_EDIT_TRW_SETZVAL, m_ZPos);
	DDX_Control(pDX, IDC_EDIT_FROM_ANGLE, m_fromAngle);
	DDX_Control(pDX, IDC_EDIT_TO_ANGLE, m_toAngle);
	DDX_Control(pDX, IDC_EDIT_TRW_DIS_FR_CEN, m_distanceFromCenter);
	DDX_Control(pDX, IDC_EDIT_TRW_OFFSET_ANGLE, m_offsetAngle);
	DDX_Control(pDX, IDC_EDIT_TRW_WAFER_RADIUS, m_waferRadius);
	DDX_Control(pDX, IDC_MESSAGE, m_cList);
	DDX_Control(pDX, IDC_TRW_MIN_WIDTH, m_minWidth);
	DDX_Control(pDX, IDC_TRW_MAX_WIDTH, m_maxWidth);
	DDX_Control(pDX, IDC_TRW_REPEAT, m_Repeat);
	DDX_Control(pDX, IDC_TRW_STEP_DISTANCE, m_StepDistance);
	DDX_Control(pDX, IDC_COMB_TRW_STYPE, m_comSType);
	DDX_Control(pDX, IDC_EDIT_TRW_NUMOF_POINT, m_numOfPOints);
	DDX_Control(pDX, IDC_TRW_REMEAS, m_RemeasBtn);
	DDX_Control(pDX, IDC_IMGVIEW, m_cImgWnd);
	DDX_Control(pDX, IDC_MOUSE_MSG, m_cMouseMessage);
	DDX_Control(pDX, IDC_TRW_RI, m_RI);
	DDX_Control(pDX, IDC_TRW_EXPOSURE, m_Exposure);
}


BEGIN_MESSAGE_MAP(CSetupProp16Dlg, CResizableDialog)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
	ON_MESSAGE(IDC_ADDALGNPOINT, OnAddalgnpoint)
	ON_BN_CLICKED(IDC_TAIKO_RECIPE, &CSetupProp16Dlg::OnBnClickedTaikoRecipe)
	ON_BN_CLICKED(IDC_MOTIONCONTROL2, &CSetupProp16Dlg::OnBnClickedMotioncontrol2)
	ON_BN_CLICKED(IDC_CAMERASETTING_TAIKO, &CSetupProp16Dlg::OnBnClickedCamerasettingTaiko)
	ON_BN_CLICKED(IDC_BTN_TRW_GO_XY, &CSetupProp16Dlg::OnBnClickedBtnTrwGoXy)
	ON_BN_CLICKED(IDC_BTN_TRW_SETZ, &CSetupProp16Dlg::OnBnClickedBtnTrwSetz)
	ON_BN_CLICKED(IDC_MEASURE_TAIKO, &CSetupProp16Dlg::OnBnClickedMeasureTaiko)
	ON_BN_CLICKED(IDC_TRW_REMEAS, &CSetupProp16Dlg::OnBnClickedTrwRemeas)
	ON_CBN_SELCHANGE(IDC_COMB_TRW_STYPE, &CSetupProp16Dlg::OnCbnSelchangeCombTrwStype)
	ON_MESSAGE(UM_MOUSE_MOVE, OnMoMove)
	ON_BN_CLICKED(IDC_EXPORT_DATA, &CSetupProp16Dlg::OnBnClickedExportData)
END_MESSAGE_MAP()

void CSetupProp16Dlg::RecipeToLocal() {
	if (pRcp) {
		pRcp->UpdateControl(m_cPoint);
		SetValue();
		m_cWaferMap.Redraw();
	}
	UpdateData(FALSE);
}

void CSetupProp16Dlg::Renumber() {
	pRcp->Renumber();
	pRcp->UpdateControl(m_cPoint);
	m_cWaferMap.Redraw();
}

long CSetupProp16Dlg::OnAddalgnpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

BOOL CSetupProp16Dlg::DestroyWindow() {
	StopAllCamera(&m_cVideo);
	return CResizableDialog::DestroyWindow();
}

long CSetupProp16Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		if (!m_cVideo.IsReserved()) {
			p413App->StopCamera(&m_cVideo, NULL);
			m_cVideo.pWnd = NULL;
		}
		bTabSelected = FALSE;
	}
	return 0;
}

long CSetupProp16Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	/* default recipe to show on wafermap*/
	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	if (!bTabSelected) {
		m_cWaferMap.pRcp = pRcp;
		RecipeToLocal();

		if (!m_cVideo.IsReserved()) {
			p413App->StartCamera(&m_cVideo, CCamBase::ECHOCAM, &m_cList, pRcp);
			m_pVideo = &m_cVideo;
			m_cVideo.SetRecipe(pRcp);
			m_cVideo.bShowCrossHair = FALSE;

			m_cVideo.pWnd = this;
			m_cVideo.bSetRedXHair = FALSE;
		}

		bTabSelected = TRUE;
	}

	return 0;
}

long CSetupProp16Dlg::OnMoMove(WPARAM wP, LPARAM lP) {
	m_cMouseMessage.SetWindowText(MoMove(wP, lP));
	return 0;
}

void CSetupProp16Dlg::SetValue() {
	CString temp;
	temp.Format("%.02f", pRcp->TRWMeasProp.radius);
	m_radius.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.angle);
	m_angle.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.ZPos);
	m_ZPos.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.fromAngle);
	m_fromAngle.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.toAngle);
	m_toAngle.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.distanceFromCenter);
	m_distanceFromCenter.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.offsetAngle);
	m_offsetAngle.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.waferRadius);
	m_waferRadius.SetWindowText(temp);
	temp.Format("%d", pRcp->TRWMeasProp.npoints);
	m_numOfPOints.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.fMinWidth);
	m_minWidth.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.fMaxWidth);
	m_maxWidth.SetWindowText(temp);
	temp.Format("%d", pRcp->TRWMeasProp.nRepeat);
	m_Repeat.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.fStepDistance);
	m_StepDistance.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.RI);
	m_RI.SetWindowText(temp);
	temp.Format("%.02f", pRcp->TRWMeasProp.Exposure);
	m_Exposure.SetWindowText(temp);
	m_comSType.SetCurSel(pRcp->TRWMeasProp.eSearchType);
	m_RemeasBtn.SetCheck(pRcp->TRWMeasProp.bSearchAround);
}

bool CSetupProp16Dlg::UpdateAndValidate() {
	CString temp;
	m_radius.GetWindowText(temp);
	double tmpRadius = static_cast<double>(atof(temp));
	if (!(tmpRadius >= 1 && tmpRadius <= 150)) {
		AfxMessageBox("Input Radius Between 1 To 150 mm", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.radius = tmpRadius;
	}
	m_angle.GetWindowText(temp);
	double tmpAngle = static_cast<double>(atof(temp));
	if(!(tmpAngle >= 0 && tmpAngle <= 360)) {
		AfxMessageBox("Input Angle Between 0 To 360 Degree", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.angle = tmpAngle;
	}
	m_fromAngle.GetWindowText(temp);
	double tmpFromAngle = static_cast<double>(atof(temp));
	if (!(tmpFromAngle >= 0 && tmpFromAngle <= 360)) {
		AfxMessageBox("Input Angle Between 0 To 360 Degree", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.fromAngle = tmpFromAngle;
	}
	m_toAngle.GetWindowText(temp);
	double tmpToAngle = static_cast<double>(atof(temp));
	if (!(tmpToAngle >= 0 && tmpToAngle <= 360)) {
		AfxMessageBox("Input Angle Between 0 To 360 Degree", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.toAngle = tmpToAngle;
	}
	m_distanceFromCenter.GetWindowText(temp);
	double tmpDisFromCenter = static_cast<double>(atof(temp));
	if (!(tmpDisFromCenter >= 1 && tmpDisFromCenter <= 151)) {
		AfxMessageBox("Input Distance Between 1 To 151 Degree", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.distanceFromCenter = tmpDisFromCenter;
	}
	m_offsetAngle.GetWindowText(temp);
	double tmpOffsetAngle = static_cast<double>(atof(temp));
	if (!(tmpOffsetAngle >= -360 && tmpOffsetAngle <= 360)) {
		AfxMessageBox("Input Angle Between -360 To 360 Degree", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.offsetAngle = tmpOffsetAngle;
	}
	m_waferRadius.GetWindowText(temp);
	double tmpWaferRadius = static_cast<double>(atof(temp));
	if (!(tmpWaferRadius >= 1 && tmpWaferRadius <= 150)) {
		AfxMessageBox("Input Wafer Radius Between 1 To 150 mm", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.waferRadius = tmpWaferRadius;
	}
	m_numOfPOints.GetWindowText(temp);
	int tmpNumOfPoints = atoi(temp);
	if (!(tmpNumOfPoints >= 1 && tmpNumOfPoints <= 100)) {
		AfxMessageBox("Input Num Of Points Between 1 To 100", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.npoints = tmpNumOfPoints;
	}
	m_minWidth.GetWindowText(temp);
	float tmpminWidth = atof(temp);
	if (!(tmpminWidth >= 0 && tmpminWidth <= 200)) {
		AfxMessageBox("Input Min Width Points Between 0 To 200 mm", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.fMinWidth = tmpminWidth;
	}
	m_maxWidth.GetWindowText(temp);
	float tmpmaxWidth = atof(temp);
	if (!(tmpmaxWidth >= 0 && tmpmaxWidth <= 200)) {
		AfxMessageBox("Input Max Width Points Between 0 To 200 mm", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.fMaxWidth = tmpmaxWidth;
	}
	m_Repeat.GetWindowText(temp);
	int tmpRepeat = atoi(temp);
	if (!(tmpRepeat >= 0 && tmpRepeat <= 100)) {
		AfxMessageBox("Input Repeat Between 0 To 100", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.nRepeat = tmpRepeat;
	}
	m_StepDistance.GetWindowText(temp);
	float tmpStepDis = atof(temp);
	if (!(tmpStepDis >= 0 && tmpStepDis <= 200)) {
		AfxMessageBox("Input Step Distance Between 0 To 200", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.fStepDistance = tmpStepDis;
	}
	m_RI.GetWindowText(temp);
	float tmpRI = atof(temp);
	if (!(tmpRI >= 1 && tmpRI <= 100)) {
		AfxMessageBox("Input RI Between 1 To 100", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.RI = tmpRI;
	}
	m_Exposure.GetWindowText(temp);
	float tmpExposure = atof(temp);
	if (!(tmpExposure >= 1 && tmpExposure <= 100)) {
		AfxMessageBox("Input Exposure Between 1 To 100", MB_ICONINFORMATION);
		return false;
	}
	else {
		pRcp->TRWMeasProp.Exposure = tmpExposure;
	}
	return true;
}

BOOL CSetupProp16Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	//[ setting position
	AddAnchor(IDC_MOTIONCONTROL2, TOP_RIGHT);
	AddAnchor(IDC_TAIKO_RECIPE, TOP_RIGHT);
	AddAnchor(IDC_MEASURE_TAIKO, TOP_RIGHT);
	AddAnchor(IDC_CAMERASETTING_TAIKO, TOP_RIGHT);
	AddAnchor(IDC_EXPORT_DATA, TOP_RIGHT);
	//]

	g = &p413App->Global;
	pRcp = &g->RcpSetup;
	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE;
	m_cWaferMap.pParent = this;

	SetupColumnPos();
	SetupColumnMessage();

	UpdateData(FALSE);
	return TRUE;
}

// CSetupProp16Dlg message handlers

void CSetupProp16Dlg::SetupColumnPos() {
	char* dwCjName[] =
	{
		"#", "Position", "Me", "Pat", "  Ring Width[mm]  ", "  Bevel Width[mm]  ", "  Thickness[mm]  ", "  Step Height[mm]  "
	};
	int nSize[] = { 32, 160, 32, 32, 120, 120, 120, 120 };
	LV_COLUMN nListColumn;
	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_cPoint.InsertColumn(i, &nListColumn);
	}
	m_cPoint.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
}

void CSetupProp16Dlg::SetupColumnMessage() {
	int idx = 0;

	// Insert a column. This override is the most convenient.
	m_cList.InsertColumn(idx, _T("Time       "), LVCFMT_LEFT);
	m_cList.SetColumnWidth(idx++, 96);

	// The other InsertColumn() override requires an initialized
	// LVCOLUMN structure.
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT;
	col.fmt = LVCFMT_LEFT;
	col.pszText = _T("Message");
	m_cList.InsertColumn(idx, &col);
	m_cList.SetColumnWidth(idx++, LVSCW_AUTOSIZE_USEHEADER);
}

void CSetupProp16Dlg::OnBnClickedTaikoRecipe()
{
	if(!UpdateAndValidate()) return;
	RecipeIO();
}


void CSetupProp16Dlg::OnBnClickedMotioncontrol2()
{
	ShowMotionControl(this);
}


void CSetupProp16Dlg::OnBnClickedCamerasettingTaiko()
{
	// TODO: Add your control notification handler code here
	p413App->CameraSetting(m_cVideo);
}

void CSetupProp16Dlg::OnBnClickedBtnTrwGoXy()
{
	// TODO: Add your control notification handler code here
	if(!UpdateAndValidate()) return;
	float r = static_cast<float>(pRcp->TRWMeasProp.radius);
	float a = static_cast<float>(pRcp->TRWMeasProp.angle);
	float x, y;
	g->Polar2Cartesian(r, a, x, y);
	if (p413App->pMtr) p413App->pMtr->GotoXY(x, y);
}


void CSetupProp16Dlg::OnBnClickedBtnTrwSetz()
{
	// TODO: Add your control notification handler code here
	if (!UpdateAndValidate()) return;
	pRcp->TRWMeasProp.ZPos = g->LocEchoTopZ;
	SetValue();
}

void CSetupProp16Dlg::OnBnClickedTrwRemeas()
{
	// TODO: Add your control notification handler code here
	if (!UpdateAndValidate()) return;
	pRcp->TRWMeasProp.bSearchAround = ((pRcp->TRWMeasProp.bSearchAround) ? FALSE : TRUE);
}

void CSetupProp16Dlg::OnCbnSelchangeCombTrwStype()
{
	// TODO: Add your control notification handler code here
	if (!UpdateAndValidate()) return;
	int nIndex = m_comSType.GetCurSel();
	switch (nIndex) {
	case  SearchType::LINEAR:
		pRcp->TRWMeasProp.eSearchType = pRcp->TRWMeasProp.LINEAR;
		break;
	case SearchType::CIRCULAR:
		pRcp->TRWMeasProp.eSearchType = pRcp->TRWMeasProp.CIRCULAR;
		break;
	}
}

void CSetupProp16Dlg::OnBnClickedMeasureTaiko()
{
	// TODO: Add your control notification handler code here
	if (!UpdateAndValidate()) return;
	p413App->pPSwitch->MoveToECHOprb();
	m_cVideo.SetExposure(pRcp->TRWMeasProp.Exposure + g->BaseExpoEcho);
	Sleep(100);
	p413App->PreMeasureRingWidth(pRcp, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList);
	g->TaikoPointCount = 0;
	CMPoint* p = new CMPoint();
	p->MeSet = 1;
	//g->Polar2Cartesian(pRcp->TRWMeasProp.radius, pRcp->TRWMeasProp.angle, p->Co.x, p->Co.y);
	//p413App->pDMC->Run();
	p413App->pEcho->bSignal = TRUE;
	g->bAborting = FALSE;
	g->bStop = FALSE;
	short code[MAXMEPASET * MaXPA];
	for (int i = 1; i <= 4; i++) {
		p413App->MeasureOnePointRingWidth(pRcp, p, g->ActiveCamera, &m_cVideo, &m_cImgWnd, &m_cList, code);
		UpdateReport(i-1);
	}
	if (p) {
		delete p;
		p = NULL;
	}
	//p413App->pDMC->Stop();
	if (!p413App->pMtr->GotoXY(0, 0)) { CString msg = _T("Goto Origin FAILED"); p413App->Message(&m_cList, msg, 2); };
	p413App->pEcho->bSignal = FALSE;
	p413App->PostMeasureRingWidth(&m_cList);

	SYSTEMTIME st;
	GetSystemTime(&st);
	CString time;
	time.Format(_T("%02d_%02d_%02d_%02d_%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
	CString ResultPath = DosUtil.GetResultDir() + pRcp->GetRecipeName() + _T("_") + time + _T(".TXT");
	ExportData(ResultPath);
	ResultPath = "";
	ResultPath.Format(_T("Data Exported At %s Directory"), DosUtil.GetResultDir());
	AfxMessageBox(ResultPath, MB_ICONINFORMATION);
}

void CSetupProp16Dlg::UpdateReport(int idx) {
	int col = 4;
	CString str;
	str.Format(_T("%.6f"), pRcp->TRWMeasProp.ringWidth);
	m_cPoint.SetItemText(idx, col++, str);
	str.Format(_T("%.6f"), pRcp->TRWMeasProp.bevelWidth);
	m_cPoint.SetItemText(idx, col++, str);
	str.Format(_T("%.6f"), pRcp->TRWMeasProp.RingThk);
	m_cPoint.SetItemText(idx, col++, str);
	str.Format(_T("%.6f"), pRcp->TRWMeasProp.RingStepHeight);
	m_cPoint.SetItemText(idx, col, str);
}

void CSetupProp16Dlg::OnBnClickedExportData()
{
	CFileDialog dlg(FALSE, _T("TXT"), NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR, _T("TEXT File|*.TXT|"));

	CString filename = _T("");
	CString ResDir = DosUtil.GetResultDir();
	dlg.m_ofn.lpstrInitialDir = ResDir;

	if (dlg.DoModal() == IDOK) {
		filename = dlg.GetPathName();
		ExportData(filename);
	}
}

void CSetupProp16Dlg::ExportData(CString ResultPath) {
	int NumCol = 8;
	CString item;
	FILE* pFile;

	pFile = _tfopen(ResultPath, _T("w+"));
	if (pFile != nullptr) {
		_ftprintf(pFile, _T("#,Position,Me,PatRing,Width[mm],Bevel Width[mm],Thickness[mm],Step Height[mm]\n"));
		for (int row = 0; m_cPoint.GetItemCount(); ++row) {
			item = m_cPoint.GetItemText(row, 0);
			if (item.GetLength() == 0) break;

			for (int col = 0; col < NumCol; ++col) {
				item = m_cPoint.GetItemText(row, col);
				_ftprintf(pFile, _T("%s"), (LPCTSTR)item);

				if (col < NumCol - 1) {
					_ftprintf(pFile, _T(","));
				}
			}

			_ftprintf(pFile, _T("\n"));
		}

		fclose(pFile);
	}
}
