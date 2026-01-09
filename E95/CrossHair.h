#pragma once

class CCrossHair : public CDialog {
public:
	void SetX(short n, float dx);
	void SetY(short n, float dx);
	CCrossHair(CWnd* pParent = NULL);   // standard constructor

	short n;
	float x, Y, dx;
	float* pPX[4];
	float* pPY[4];

	enum { IDD = IDD_CROSSHAIR };
	CButton	m_cShowCrossHair;
	CListBox	m_cSpeed;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	afx_msg void OnUpleft();
	afx_msg void OnFirsttop();
	afx_msg void OnUpright();
	afx_msg void OnFirstright();
	afx_msg void OnBotright();
	afx_msg void OnFirstbottom();
	afx_msg void OnBotleft();
	afx_msg void OnFirstleft();
	afx_msg void OnRed();
	afx_msg void OnGreen();
	afx_msg void OnBlue();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusSpeed();
	afx_msg void OnOnOffCursor();
	DECLARE_MESSAGE_MAP()
};
