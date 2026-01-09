// KeyPad.cpp : implementation file
//

#include "stdafx.h"
#include "KeyPad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyPad dialog

CKeyPad::CKeyPad(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyPad::IDD, pParent) {
	//{{AFX_DATA_INIT(CKeyPad)
	//}}AFX_DATA_INIT
	m_Value = _T("");
}

void CKeyPad::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyPad)
	DDX_Control(pDX, IDC_VALUE, m_cValue);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_VALUE, m_Value);
}

BEGIN_MESSAGE_MAP(CKeyPad, CDialog)
	//{{AFX_MSG_MAP(CKeyPad)
	ON_BN_CLICKED(IDC_BUT0, OnBut0)
	ON_BN_CLICKED(IDC_BUT1, OnBut1)
	ON_BN_CLICKED(IDC_BUT2, OnBut2)
	ON_BN_CLICKED(IDC_BUT3, OnBut3)
	ON_BN_CLICKED(IDC_BUT4, OnBut4)
	ON_BN_CLICKED(IDC_BUT5, OnBut5)
	ON_BN_CLICKED(IDC_BUT6, OnBut6)
	ON_BN_CLICKED(IDC_BUT7, OnBut7)
	ON_BN_CLICKED(IDC_BUT8, OnBut8)
	ON_BN_CLICKED(IDC_BUT9, OnBut9)
	ON_BN_CLICKED(IDC_BUTCLR, OnButclr)
	ON_BN_CLICKED(IDC_BUTD, OnButd)
	ON_BN_CLICKED(IDC_BUTEXP, OnButexp)
	ON_BN_CLICKED(IDC_BUTSIGN, OnButsign)
	ON_EN_CHANGE(IDC_VALUE, OnChangeValue)
	ON_WM_LBUTTONUP()
	ON_EN_SETFOCUS(IDC_VALUE, OnSetfocusValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyPad message handlers

void CKeyPad::OnBut0() {
	ClearSelect();
	m_Value += "0"; UpdateData(FALSE);
}

void CKeyPad::OnBut1() {
	ClearSelect();
	m_Value += "1"; UpdateData(FALSE);
}

void CKeyPad::OnBut2() {
	ClearSelect();
	m_Value += "2"; UpdateData(FALSE);
}

void CKeyPad::OnBut3() {
	ClearSelect();
	m_Value += "3"; UpdateData(FALSE);
}

void CKeyPad::OnBut4() {
	ClearSelect();
	m_Value += "4"; UpdateData(FALSE);
}

void CKeyPad::OnBut5() {
	ClearSelect();
	m_Value += "5"; UpdateData(FALSE);
}

void CKeyPad::OnBut6() {
	ClearSelect();
	m_Value += "6"; UpdateData(FALSE);
}

void CKeyPad::OnBut7() {
	ClearSelect();
	m_Value += "7"; UpdateData(FALSE);
}

void CKeyPad::OnBut8() {
	ClearSelect();
	m_Value += "8"; UpdateData(FALSE);
}

void CKeyPad::OnBut9() {
	ClearSelect();
	m_Value += "9"; UpdateData(FALSE);
}

void CKeyPad::OnButclr() {
	m_Value = ""; UpdateData(FALSE);
}

void CKeyPad::OnButd() {
	ClearSelect();
	m_Value += "."; UpdateData(FALSE);
}

void CKeyPad::OnButexp() {
	ClearSelect();
	m_Value += "e"; UpdateData(FALSE);
}

void CKeyPad::OnButsign() {
	ClearSelect();
	m_Value += "-"; UpdateData(FALSE);
}

void CKeyPad::SetValue(double value) {
	m_Value.Format("%f", value);
}

void CKeyPad::OnChangeValue() {
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.

	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CKeyPad::SetValue(short value) {
	m_Value.Format("%d", value);
}

BOOL CKeyPad::Edit(short& value) {
	CKeyPad dlg;
	dlg.SetValue(value);
	if (dlg.DoModal() == IDOK) {
		value = atoi(dlg.m_Value);
		return TRUE;
	}
	return FALSE;
}

BOOL CKeyPad::Edit(float& value) {
	CKeyPad dlg;
	dlg.m_Value.Format("%f", value);
	if (dlg.DoModal() == IDOK) {
		value = float(atof(dlg.m_Value));
		return TRUE;
	}
	return FALSE;
}

BOOL CKeyPad::Edit(double& value) {
	CKeyPad dlg;
	dlg.m_Value.Format("%f", value);
	if (dlg.DoModal() == IDOK) {
		value = atof(dlg.m_Value);
		return TRUE;
	}
	return FALSE;
}

BOOL CKeyPad::Edit(long& value) {
	CKeyPad dlg;
	dlg.m_Value.Format("%ld", value);
	if (dlg.DoModal() == IDOK) {
		value = atol(dlg.m_Value);
		return TRUE;
	}
	return FALSE;
}

void CKeyPad::ClearSelect() {
	int st, ed;
	m_cValue.GetSel(st, ed);
	if (st != ed) {
		m_cValue.Clear();
		m_cValue.GetWindowText(m_Value);
	}
}

void CKeyPad::OnLButtonUp(UINT nFlags, CPoint point) {
	CDialog::OnLButtonUp(nFlags, point);
}

void CKeyPad::OnSetfocusValue() {}