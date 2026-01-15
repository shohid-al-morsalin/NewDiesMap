// SetupProp92Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "SetupProp92Dlg.h"
#include "afxdialogex.h"
#include "DiceMap.h"
#include "413App.h"

#include <string>

// CSetupProp92Dlg dialog



CSetupProp92Dlg::CSetupProp92Dlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_SETUPPROP92, pParent)
{

}

CSetupProp92Dlg::~CSetupProp92Dlg()
{
}

void CSetupProp92Dlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DICEMAP, m_diceMap);
	DDX_Control(pDX, IDC_CHECK_EXCLUDE_DIE, m_maskCheck);

	DDX_Control(pDX, IDC_UL, m_btnUL);
	DDX_Control(pDX, IDC_LL, m_btnLL);
	DDX_Control(pDX, IDC_LR, m_btnLR);
}


BEGIN_MESSAGE_MAP(CSetupProp92Dlg, CResizableDialog)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_DIEMAP, &CSetupProp92Dlg::OnBnClickedButtonUpdateDiemap)
	ON_BN_CLICKED(IDC_CHECK_EXCLUDE_DIE, &CSetupProp92Dlg::OnBnClickedCheckExcludeDie)
	ON_BN_CLICKED(IDC_UL, &CSetupProp92Dlg::OnBnClickedUL)
	ON_BN_CLICKED(IDC_LL, &CSetupProp92Dlg::OnBnClickedLL)
	ON_BN_CLICKED(IDC_LR, &CSetupProp92Dlg::OnBnClickedLR)
	ON_BN_CLICKED(IDC_CAL_PITCH, &CSetupProp92Dlg::OnBnClickedCalPitch)
	ON_BN_CLICKED(IDC_PRECONSTRUCT_DIE, &CSetupProp92Dlg::OnBnClickedPreconstructDie)
	ON_BN_CLICKED(IDC_CREATE_DIE, &CSetupProp92Dlg::OnBnClickedCreateDie)
END_MESSAGE_MAP()


// CSetupProp92Dlg message handlers

BOOL CSetupProp92Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();


	m_dWaferDiameter = 100;
	m_dGapX = 5.0;
	m_dGapY = 5.0;
	//PointD pTL = { 32.58,121.255 };
	//PointD pBL = { 32.24, 88.965 };
	//PointD pBR = { 56.629, 88.699 };
	pTL = { 2.5 , 12.5 };
	pBL = { 2.5 , 2.5 };
	pBR = { 12.5 , 2.5 };

	//PointD pTL = { -2 , 4 };
	//PointD pBL = { -4 , 2 };
	//PointD pBR = { -2 , 0 };
	InitEditControls();

	m_diceMap.SetWaferParams(m_dWaferDiameter, m_dGapX, m_dGapY);
	//// 2. Set Reference Die Points (TL, BL, BR)
	//PointD pTL = { 10, 40 }; 
	//PointD pBL = { 10, 10 };
	//PointD pBR = { 70, 10 };
	m_diceMap.SetReferenceDie(pTL, pBL, pBR);
	m_diceMap.SetShowPartialDies(true);

	SetEditVal(IDC_EDIT10, m_diceMap.m_angle);
	p413App->Global.SelectedDies.clear(); // 01092026_1 MORSALIN

	return TRUE;
}


void CSetupProp92Dlg::RecipeToLocal() {

}

void CSetupProp92Dlg::LocalToRecipe() {

}


void CSetupProp92Dlg::InitEditControls()
{
	// Set Single Variables
	SetEditVal(IDC_EDIT2, m_dWaferDiameter);
	SetEditVal(IDC_EDIT8, m_dGapX);
	SetEditVal(IDC_EDIT9, m_dGapY);
	//SetEditVal(IDC_EDIT10, m_diceMap.m_angle);

	// Set Point Top-Left
	SetEditVal(IDC_EDIT1, pTL.x);
	SetEditVal(IDC_EDIT5, pTL.y);

	// Set Point Bottom-Left
	SetEditVal(IDC_EDIT3, pBL.x);
	SetEditVal(IDC_EDIT6, pBL.y);

	// Set Point Bottom-Right
	SetEditVal(IDC_EDIT4, pBR.x);
	SetEditVal(IDC_EDIT7, pBR.y);


}


void CSetupProp92Dlg::SetEditVal(int nID, double val)
{
	CString str;
	// Format to 3 decimal places (change .3 to whatever you need)
	str.Format(_T("%.3f"), val);
	SetDlgItemText(nID, str);
}


void CSetupProp92Dlg::ReadData()
{
	// --- Read Single Variables ---
	m_dWaferDiameter = GetDoubleFromEdit(IDC_EDIT2);
	m_dGapX = GetDoubleFromEdit(IDC_EDIT8);
	m_dGapY = GetDoubleFromEdit(IDC_EDIT9);
	//m_diceMap.m_angle = GetDoubleFromEdit(IDC_EDIT10);

	// --- Read PointD: Top Left ---
	pTL.x = GetDoubleFromEdit(IDC_EDIT1);
	pTL.y = GetDoubleFromEdit(IDC_EDIT5);

	// --- Read PointD: Bottom Left ---
	pBL.x = GetDoubleFromEdit(IDC_EDIT3);
	pBL.y = GetDoubleFromEdit(IDC_EDIT6);

	// --- Read PointD: Bottom Right ---
	pBR.x = GetDoubleFromEdit(IDC_EDIT4);
	pBR.y = GetDoubleFromEdit(IDC_EDIT7);
}


void CSetupProp92Dlg::OnBnClickedButtonUpdateDiemap()
{
	p413App->Global.SelectedDies.clear();

	// UpdateData(TRUE) reads from Screen to Variables
	if (UpdateData(TRUE))
	{
		// Data is now successfully stored in m_dWaferDiameter, pTL.x, etc.
		//AfxMessageBox(_T("Data Read Successfully"));

		ReadData();
		m_diceMap.SetWaferParams(m_dWaferDiameter, m_dGapX, m_dGapY);
		m_diceMap.SetReferenceDie(pTL, pBL, pBR);

		SetEditVal(IDC_EDIT10, m_diceMap.m_angle);

	}
	else
	{
		AfxMessageBox(_T("Invalid Data Entered"));
	}
}


// specific helper to read a double from an ID
double CSetupProp92Dlg::GetDoubleFromEdit(int nID)
{
	CString strText;
	GetDlgItemText(nID, strText);
	return _ttof(strText); // _ttof converts string to double
}


void CSetupProp92Dlg::OnBnClickedCheckExcludeDie()
{
	int state = m_maskCheck.GetCheck();

	if (state == BST_CHECKED) {
		m_diceMap.SetShowPartialDies(false);
		// It is checked (1)
	}
	else if (state == BST_UNCHECKED) {
		m_diceMap.SetShowPartialDies(true);
		// It is unchecked (0)
	}
	else if (state == BST_INDETERMINATE) {
		// Only used for 3-state checkboxes (2)
	}
}

// [ 01092026_1 MORSALIN

void CSetupProp92Dlg::OnBnClickedUL()
{
	// Need to collect from stage later
	ULx = -2.5;
	ULy = 2.5;
	CString st; st.Format("%.2lf, %.2lf", ULx, ULy);
	m_btnUL.SetWindowText(_T(st));
}

void CSetupProp92Dlg::OnBnClickedLL()
{
	// Need to collect from stage later
	LLx = -2.5;
	LLy = -2.5;
	CString st; st.Format("%.2lf, %.2lf", LLx, LLy);
	m_btnLL.SetWindowText(_T(st));
}

void CSetupProp92Dlg::OnBnClickedLR()
{
	// Need to collect from stage later
	LRx = 2.5;
	LRy = -2.5;
	CString st; st.Format("%.2lf, %.2lf", LRx, LRy);
	m_btnLR.SetWindowText(_T(st));
}

double CalPitchX(double LLx, double LLy, double LRx, double LRy)
{
	return std::sqrt(
		(LLx - LRx) * (LLx - LRx) +
		(LLy - LRy) * (LLy - LRy)
	);
}
double CalPitchY(double LLx, double LLy, double ULx, double ULy)
{
	return std::sqrt(
		(LLx - ULx) * (LLx - ULx) +
		(LLy - ULy) * (LLy - ULy)
	);
}

void CSetupProp92Dlg::OnBnClickedCalPitch()
{
	double PitchX = CalPitchX(LLx, LLy, LRx, LRy);
	double PitchY = CalPitchY(LLx, LLy, ULx, ULy);
	
	SetEditVal(IDC_EDIT8, PitchX);
	SetEditVal(IDC_EDIT9, PitchY);
}

void CSetupProp92Dlg::OnBnClickedPreconstructDie()
{
	p413App->Global.SelectedDies.clear();

	// Calculate 4th corner.
	
	// Create die.
	
	double PitchX = CalPitchX(LLx, LLy, LRx, LRy);
	double PitchY = CalPitchY(LLx, LLy, ULx, ULy);
	SetEditVal(IDC_EDIT8, PitchX);
	SetEditVal(IDC_EDIT9, PitchY);
	if (UpdateData(TRUE))
	{
		// Data is now successfully stored in m_dWaferDiameter, pTL.x, etc.
		//AfxMessageBox(_T("Data Read Successfully"));

		ReadData();
		m_diceMap.SetWaferParams(m_dWaferDiameter, PitchX, PitchY);
		m_diceMap.SetReferenceDie(pTL, pBL, pBR);
		SetEditVal(IDC_EDIT10, m_diceMap.m_angle);
	}
	else {
		AfxMessageBox(_T("Invalid Data Entered"));
	}
}
// ]


// [ 01092026 MORSALIN
struct WaferPoint
{
	double x;
	double y;
};

bool ComputeSimilarityTransform(
	const std::vector<WaferPoint>& measured,
	const std::vector<WaferPoint>& ideal,
	double M[2][3])
{
	if (measured.size() < 2 || measured.size() != ideal.size())
		return false;

	int N = measured.size();

	double mx = 0, my = 0;
	double ix = 0, iy = 0;

	for (int i = 0; i < N; ++i)
	{
		mx += measured[i].x;
		my += measured[i].y;
		ix += ideal[i].x;
		iy += ideal[i].y;
	}

	mx /= N; my /= N;
	ix /= N; iy /= N;

	double num = 0;
	double den = 0;

	for (int i = 0; i < N; ++i)
	{
		double xm = measured[i].x - mx;
		double ym = measured[i].y - my;

		double xi = ideal[i].x - ix;
		double yi = ideal[i].y - iy;

		num += xm * yi - ym * xi;
		den += xm * xi + ym * yi;
	}

	double theta = atan2(num, den);

	double c = cos(theta);
	double s = sin(theta);

	M[0][0] = c;
	M[0][1] = -s;
	M[1][0] = s;
	M[1][1] = c;

	M[0][2] = ix - (c * mx - s * my);
	M[1][2] = iy - (s * mx + c * my);

	return true;
}

WaferPoint ApplyTransform(const WaferPoint& p, double M[2][3])
{
	WaferPoint out;
	out.x = M[0][0] * p.x + M[0][1] * p.y + M[0][2];
	out.y = M[1][0] * p.x + M[1][1] * p.y + M[1][2];
	return out;
}
// ]

void CSetupProp92Dlg::OnBnClickedCreateDie()
{
	m_diceMap.m_DieList[27].corners[0].x;

	// [ 01092026 MORSALIN

	std::vector<WaferPoint> ideal, measured;
	// Stage will goto the all 4 marking positions and create ideal, measured vector automatically using PR.
	if (p413App->Global.SelectedDies.size() != 4) {
		AfxMessageBox("Please select 4 dies in random position.");
		return;
	}
	else {
		

		uint64_t  id;
		for (int i = 0; i < 4; i++) {
			auto it = p413App->Global.SelectedDies.begin();
			std::advance(it, i);
			id = *it;
			double x = m_diceMap.m_DieList[id-1].corners[3].x;  // corrently it's considering LL of die. (index 3)
			double y = m_diceMap.m_DieList[id-1].corners[3].y;

			// For simulation I am set random data to check the calculation
			if (i == 0) x = 90.0, y = 0.0;
			else if (i == 1) x = 0.0, y = 90.0;
			else if (i == 2) x = -90.0, y = 0.0;
			else if (i == 3) x = 0.0, y = -90.0;

			ideal.push_back({ x, y });
		}
	}
	

	/*std::vector<WaferPoint> measured = {
		{10, 20}, {200, 25}, {15, 220}, {210, 215}
	};

	std::vector<WaferPoint> ideal = {
		{0, 0}, {200, 0}, {0, 200}, {200, 200}
	};*/

	/*std::vector<WaferPoint> measured = {
		{110, 0}, {-90, 0}, {10, 100}, {10, -100}
	};

	std::vector<WaferPoint> ideal = {
		{100, 0}, {-100, 0}, {0, 100}, {0, -100}
	};*/

	/*std::vector<WaferPoint> ideal =
	{
		{ 0.0,   0.0 },
		{ 200.0, 0.0 },
		{ 0.0,   200.0 },
		{ 200.0, 200.0 }
	};
	std::vector<WaferPoint> measured =
	{
		{ 0.00,    0.00    },
		{ 196.96, -34.73  },
		{ 34.73,  196.96  },
		{ 231.69, 162.23  }
	};*/

	//std::vector<WaferPoint> ideal =
	//{
	//	{  90.0,   0.0 },
	//	{   0.0,  90.0 },
	//	{ -90.0,   0.0 },
	//	{   0.0, -90.0 }
	//};

	measured =
	{
		{  90.63, -15.63 },
		{  17.63,  88.63 },
		{ -86.63,  15.63 },
		{ -13.63, -88.63 }
	};

	double M[2][3];

	if (!ComputeSimilarityTransform(measured, ideal, M))
	{
		//std::cout << "Transform failed\n";
		return;
	}

	//WaferPoint die = { 90.63, -15.63 };
	WaferPoint die = { -13.63, -88.63 };
	WaferPoint corrected = ApplyTransform(die, M);
	// ]

}

// Access 27th die corner coordinate: m_diceMap.m_DieList[27].corners[0].x;

