// SetupProp92Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "SetupProp92Dlg.h"
#include "afxdialogex.h"
#include "DiceMap.h"

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

bool ComputeAffineTransform(const std::vector<WaferPoint>& measured, const std::vector<WaferPoint>& ideal, double M[2][3])
{
	if (measured.size() < 3 || ideal.size() < 3)
		return false;

	int N = measured.size();

	double A[6][6] = { 0 };
	double B[6] = { 0 };

	for (int i = 0; i < N; i++)
	{
		double x = measured[i].x;
		double y = measured[i].y;
		double X = ideal[i].x;
		double Y = ideal[i].y;

		// Equation for X
		A[0][0] += x * x;  A[0][1] += x * y;  A[0][2] += x;
		A[1][0] += x * y;  A[1][1] += y * y;  A[1][2] += y;
		A[2][0] += x;    A[2][1] += y;    A[2][2] += 1;

		B[0] += x * X;
		B[1] += y * X;
		B[2] += X;

		// Equation for Y
		A[3][3] += x * x;  A[3][4] += x * y;  A[3][5] += x;
		A[4][3] += x * y;  A[4][4] += y * y;  A[4][5] += y;
		A[5][3] += x;    A[5][4] += y;    A[5][5] += 1;

		B[3] += x * Y;
		B[4] += y * Y;
		B[5] += Y;
	}

	// Solve manually (since matrix is small)
	double det =
		A[0][0] * (A[1][1] * A[2][2] - A[1][2] * A[2][1]) -
		A[0][1] * (A[1][0] * A[2][2] - A[1][2] * A[2][0]) +
		A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);

	if (fabs(det) < 1e-9)
		return false;

	double invDet = 1.0 / det;

	// Solve X part
	M[0][0] = (B[0] * (A[1][1] * A[2][2] - A[1][2] * A[2][1])
		- A[0][1] * (B[1] * A[2][2] - A[1][2] * B[2])
		+ A[0][2] * (B[1] * A[2][1] - A[1][1] * B[2])) * invDet;

	M[0][1] = (A[0][0] * (B[1] * A[2][2] - A[1][2] * B[2])
		- B[0] * (A[1][0] * A[2][2] - A[1][2] * A[2][0])
		+ A[0][2] * (A[1][0] * B[2] - B[1] * A[2][0])) * invDet;

	M[0][2] = (A[0][0] * (A[1][1] * B[2] - B[1] * A[2][1])
		- A[0][1] * (A[1][0] * B[2] - B[1] * A[2][0])
		+ B[0] * (A[1][0] * A[2][1] - A[1][1] * A[2][0])) * invDet;

	// Solve Y part
	det =
		A[3][3] * (A[4][4] * A[5][5] - A[4][5] * A[5][4]) -
		A[3][4] * (A[4][3] * A[5][5] - A[4][5] * A[5][3]) +
		A[3][5] * (A[4][3] * A[5][4] - A[4][4] * A[5][3]);

	invDet = 1.0 / det;

	M[1][0] = (B[3] * (A[4][4] * A[5][5] - A[4][5] * A[5][4])
		- A[3][4] * (B[4] * A[5][5] - A[4][5] * B[5])
		+ A[3][5] * (B[4] * A[5][4] - A[4][4] * B[5])) * invDet;

	M[1][1] = (A[3][3] * (B[4] * A[5][5] - A[4][5] * B[5])
		- B[3] * (A[4][3] * A[5][5] - A[4][5] * A[5][3])
		+ A[3][5] * (A[4][3] * B[5] - B[4] * A[5][3])) * invDet;

	M[1][2] = (A[3][3] * (A[4][4] * B[5] - B[4] * A[5][4])
		- A[3][4] * (A[4][3] * B[5] - B[4] * A[5][3])
		+ B[3] * (A[4][3] * A[5][4] - A[4][4] * A[5][3])) * invDet;

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

	std::vector<WaferPoint> ideal =
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
	};

	double M[2][3];

	if (!ComputeAffineTransform(measured, ideal, M))
	{
		//std::cout << "Transform failed\n";
		return;
	}

	WaferPoint die = { 120.0, 130.0 };
	WaferPoint corrected = ApplyTransform(die, M);
	// ]

}

// Access 27th die corner coordinate: m_diceMap.m_DieList[27].corners[0].x;

