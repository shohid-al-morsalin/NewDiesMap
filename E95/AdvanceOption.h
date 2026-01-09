#pragma once

class CSICam;

/////////////////////////////////////////////////////////////////////////////
// CAdvanceOption dialog

class CAdvanceOption : public CDialog {
public:
	CAdvanceOption(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_ADVANCEOPTION };
	CButton	m_cHome;
	CButton	m_cResetXY;
	CButton	m_cResetRough;
	CButton	m_cResetEchoTopZ;
	CButton	m_cResetTopCWLZ;

	CSICam* pVideo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	afx_msg void OnHome();
	afx_msg void OnResetz();
	afx_msg void OnResetroughz();
	afx_msg void OnResetTopCWLz();
	virtual BOOL OnInitDialog();
	afx_msg void OnResetxy();
	DECLARE_MESSAGE_MAP()
};
