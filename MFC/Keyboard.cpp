// Keyboard.cpp : implementation file
//

#include "stdafx.h"
#include "Keyboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyboard dialog

CKeyboard::CKeyboard(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyboard::IDD, pParent) {
	//{{AFX_DATA_INIT(CKeyboard)
	//}}AFX_DATA_INIT
	m_Caps = 0;
	m_Value = _T("");
}

void CKeyboard::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyboard)
	DDX_Control(pDX, IDC_STRING, m_cValue);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_STRING, m_Value);
}

BEGIN_MESSAGE_MAP(CKeyboard, CDialog)
	//{{AFX_MSG_MAP(CKeyboard)
	ON_BN_CLICKED(IDC_SPACE, OnSpace)
	ON_BN_CLICKED(IDC_CAPS, OnCaps)
	ON_BN_CLICKED(IDC_BUTZ, OnButz)
	ON_BN_CLICKED(IDC_BUTY, OnButy)
	ON_BN_CLICKED(IDC_BUTX, OnButx)
	ON_BN_CLICKED(IDC_BUTW, OnButw)
	ON_BN_CLICKED(IDC_BUTV, OnButv)
	ON_BN_CLICKED(IDC_BUTU, OnButu)
	ON_BN_CLICKED(IDC_BUTT9, OnButt9)
	ON_BN_CLICKED(IDC_BUTT1, OnButt1)
	ON_BN_CLICKED(IDC_BUTT0, OnButt0)
	ON_BN_CLICKED(IDC_BUTT, OnButt)
	ON_BN_CLICKED(IDC_BUTSLASH, OnButslash)
	ON_BN_CLICKED(IDC_BUTS, OnButs)
	ON_BN_CLICKED(IDC_BUTR, OnButr)
	ON_BN_CLICKED(IDC_BUTQ, OnButq)
	ON_BN_CLICKED(IDC_BUTP, OnButp)
	ON_BN_CLICKED(IDC_BUTO, OnButo)
	ON_BN_CLICKED(IDC_BUTN, OnButn)
	ON_BN_CLICKED(IDC_BUTM, OnButm)
	ON_BN_CLICKED(IDC_BUTL, OnButl)
	ON_BN_CLICKED(IDC_BUTK, OnButk)
	ON_BN_CLICKED(IDC_BUTJ, OnButj)
	ON_BN_CLICKED(IDC_BUTI, OnButi)
	ON_BN_CLICKED(IDC_BUTH, OnButh)
	ON_BN_CLICKED(IDC_BUTG, OnButg)
	ON_BN_CLICKED(IDC_BUTF, OnButf)
	ON_BN_CLICKED(IDC_BUTE, OnBute)
	ON_BN_CLICKED(IDC_BUTDOT, OnButdot)
	ON_BN_CLICKED(IDC_BUTD, OnButd)
	ON_BN_CLICKED(IDC_BUTCOLON, OnButcolon)
	ON_BN_CLICKED(IDC_BUTC, OnButc)
	ON_BN_CLICKED(IDC_BUTBSPC, OnButbspc)
	ON_BN_CLICKED(IDC_BUTB, OnButb)
	ON_BN_CLICKED(IDC_BUTA, OnButa)
	ON_BN_CLICKED(IDC_BUT8, OnBut8)
	ON_BN_CLICKED(IDC_BUT7, OnBut7)
	ON_BN_CLICKED(IDC_BUT6, OnBut6)
	ON_BN_CLICKED(IDC_BUT5, OnBut5)
	ON_BN_CLICKED(IDC_BUT4, OnBut4)
	ON_BN_CLICKED(IDC_BUT3, OnBut3)
	ON_BN_CLICKED(IDC_BUT2, OnBut2)
	ON_EN_KILLFOCUS(IDC_STRING, OnKillfocusString)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_QUIT, OnQuit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyboard message handlers

void CKeyboard::OnSpace() {
	ClearSelect();
	m_Value += " "; UpdateData(FALSE);
}

void CKeyboard::OnCaps() {
	// TODO: Add your control notification handler code here
	if (m_Caps) m_Caps = 0; else m_Caps = 1;
}

void CKeyboard::OnButz() {
	ClearSelect();
	if (m_Caps) m_Value += "Z"; else m_Value += "z";
	UpdateData(FALSE);
}

void CKeyboard::OnButy() {
	ClearSelect();
	if (m_Caps) m_Value += "Y"; else m_Value += "y";
	UpdateData(FALSE);
}

void CKeyboard::OnButx() {
	ClearSelect();
	if (m_Caps) m_Value += "X"; else m_Value += "x";
	UpdateData(FALSE);
}

void CKeyboard::OnButw() {
	ClearSelect();
	if (m_Caps) m_Value += "W"; else m_Value += "w";
	UpdateData(FALSE);
}

void CKeyboard::OnButv() {
	ClearSelect();
	if (m_Caps) m_Value += "V"; else m_Value += "v";
	UpdateData(FALSE);
}

void CKeyboard::OnButu() {
	ClearSelect();
	if (m_Caps) m_Value += "U"; else m_Value += "u";
	UpdateData(FALSE);
}

void CKeyboard::OnButt9() {
	ClearSelect();
	m_Value += "9"; UpdateData(FALSE);
}

void CKeyboard::OnButt1() {
	ClearSelect();
	m_Value += "1"; UpdateData(FALSE);
}

void CKeyboard::OnButt0() {
	ClearSelect();
	m_Value += "0"; UpdateData(FALSE);
}

void CKeyboard::OnButt() {
	ClearSelect();
	if (m_Caps) m_Value += "T"; else m_Value += "t";
	UpdateData(FALSE);
}

void CKeyboard::OnButslash() {
	ClearSelect();
	m_Value += "\\"; UpdateData(FALSE);
}

void CKeyboard::OnButs() {
	ClearSelect();
	if (m_Caps) m_Value += "S"; else m_Value += "s";
	UpdateData(FALSE);
}

void CKeyboard::OnButr() {
	ClearSelect();
	if (m_Caps) m_Value += "R"; else m_Value += "r";
	UpdateData(FALSE);
}

void CKeyboard::OnButq() {
	ClearSelect();
	if (m_Caps) m_Value += "Q"; else m_Value += "q";
	UpdateData(FALSE);
}

void CKeyboard::OnButp() {
	ClearSelect();
	if (m_Caps) m_Value += "P"; else m_Value += "p";
	UpdateData(FALSE);
}

void CKeyboard::OnButo() {
	ClearSelect();
	if (m_Caps) m_Value += "O"; else m_Value += "o";
	UpdateData(FALSE);
}

void CKeyboard::OnButn() {
	ClearSelect();
	if (m_Caps) m_Value += "N"; else m_Value += "n";
	UpdateData(FALSE);
}

void CKeyboard::OnButm() {
	ClearSelect();
	if (m_Caps) m_Value += "M"; else m_Value += "m";
	UpdateData(FALSE);
}

void CKeyboard::OnButl() {
	ClearSelect();
	if (m_Caps) m_Value += "L"; else m_Value += "l";
	UpdateData(FALSE);
}

void CKeyboard::OnButk() {
	ClearSelect();
	if (m_Caps) m_Value += "K"; else m_Value += "k";
	UpdateData(FALSE);
}

void CKeyboard::OnButj() {
	ClearSelect();
	if (m_Caps) m_Value += "J"; else m_Value += "j";
	UpdateData(FALSE);
}

void CKeyboard::OnButi() {
	ClearSelect();
	if (m_Caps) m_Value += "I"; else m_Value += "i";
	UpdateData(FALSE);
}

void CKeyboard::OnButh() {
	ClearSelect();
	if (m_Caps) m_Value += "H"; else m_Value += "h";
	UpdateData(FALSE);
}

void CKeyboard::OnButg() {
	ClearSelect();
	if (m_Caps) m_Value += "G"; else m_Value += "g";
	UpdateData(FALSE);
}

void CKeyboard::OnButf() {
	ClearSelect();
	if (m_Caps) m_Value += "F"; else m_Value += "f";
	UpdateData(FALSE);
}

void CKeyboard::OnBute() {
	ClearSelect();
	if (m_Caps) m_Value += "E"; else m_Value += "e";
	UpdateData(FALSE);
}

void CKeyboard::OnButdot() {
	ClearSelect();
	m_Value += "."; UpdateData(FALSE);
}

void CKeyboard::OnButd() {
	ClearSelect();
	if (m_Caps) m_Value += "D"; else m_Value += "d";
	UpdateData(FALSE);
}

void CKeyboard::OnButcolon() {
	ClearSelect();
	m_Value += ":"; UpdateData(FALSE);
}

void CKeyboard::OnButc() {
	ClearSelect();
	if (m_Caps) m_Value += "C"; else m_Value += "c";
	UpdateData(FALSE);
}

void CKeyboard::OnButbspc() {
	short length = m_Value.GetLength();
	if (length > 1) {
		CString str = m_Value.Left(length - 1);
		m_Value = str;
	}
	else if (length == 1) {
		m_Value = "";
	}
	UpdateData(FALSE);
}

void CKeyboard::OnButb() {
	ClearSelect();
	if (m_Caps) m_Value += "B"; else m_Value += "b";
	UpdateData(FALSE);
}

void CKeyboard::OnButa() {
	ClearSelect();
	if (m_Caps) m_Value += "A"; else m_Value += "a";
	UpdateData(FALSE);
}

void CKeyboard::OnBut8() {
	ClearSelect();
	m_Value += "8"; UpdateData(FALSE);
}

void CKeyboard::OnBut7() {
	ClearSelect();
	m_Value += "7"; UpdateData(FALSE);
}

void CKeyboard::OnBut6() {
	ClearSelect();
	m_Value += "6"; UpdateData(FALSE);
}

void CKeyboard::OnBut5() {
	ClearSelect();
	m_Value += "5"; UpdateData(FALSE);
}

void CKeyboard::OnBut4() {
	ClearSelect();
	m_Value += "4"; UpdateData(FALSE);
}

void CKeyboard::OnBut3() {
	ClearSelect();
	m_Value += "3"; UpdateData(FALSE);
}

void CKeyboard::OnBut2() {
	ClearSelect();
	m_Value += "2"; UpdateData(FALSE);
}

void CKeyboard::SetValue(CString msg) {
	m_Value = msg;
}

void CKeyboard::OnKillfocusString() {
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CKeyboard::OnClear() {
	// TODO: Add your control notification handler code here
	m_Value = "";	UpdateData(FALSE);
}

void CKeyboard::OnQuit() {
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CKeyboard::ClearSelect() {
	int st, ed;
	m_cValue.GetSel(st, ed);
	if (st != ed) {
		m_cValue.Clear();
		m_cValue.GetWindowText(m_Value);
	}
}