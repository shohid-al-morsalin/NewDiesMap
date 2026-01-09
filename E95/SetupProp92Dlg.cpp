// SetupProp92Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "SetupProp92Dlg.h"
#include "afxdialogex.h"


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
}


BEGIN_MESSAGE_MAP(CSetupProp92Dlg, CResizableDialog)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_DIEMAP, &CSetupProp92Dlg::OnBnClickedButtonUpdateDiemap)
	ON_BN_CLICKED(IDC_CHECK_EXCLUDE_DIE, &CSetupProp92Dlg::OnBnClickedCheckExcludeDie)
END_MESSAGE_MAP()


// CSetupProp92Dlg message handlers

BOOL CSetupProp92Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();


	m_dWaferDiameter = 100;
	m_dGapX = 1.0;
	m_dGapY = 1.0;
	//PointD pTL = { 32.58,121.255 };
	//PointD pBL = { 32.24, 88.965 };
	//PointD pBR = { 56.629, 88.699 };
	pTL = { 0 , 0 };
	pBL = { 0 , -5 };
	pBR = { 5 , -5 };

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
