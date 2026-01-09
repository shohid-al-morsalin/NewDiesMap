#pragma once

class CAlignment : public CDialog {
	// Construction
public:
	void UpdateFixPosY(float dY);
	void UpdateFixPosX(float dX);
	void UpdateDialog();
	CAlignment(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlignment)
	enum { IDD = IDD_ALIGNMENT };
	CListBox	m_cResult;
	float	m_LeftXE;
	float	m_LeftXT;
	float	m_LeftYE;
	float	m_LeftYT;
	float	m_RightXE;
	float	m_RightXT;
	float	m_RightYE;
	float	m_RightYT;
	float	m_OffsetAngle;
	//}}AFX_DATA

	float oAngle, Tx1, Tx2, Ty1, Ty2;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CAlignment)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlignment)
	afx_msg void OnLeftreade();
	afx_msg void OnLeftreadt();
	afx_msg void OnRightreade();
	afx_msg void OnRightreadt();
	afx_msg void OnCalculate();
	virtual BOOL OnInitDialog();
	afx_msg void OnCalculate4();
	virtual void OnCancel();
	afx_msg void OnResetangle();
	afx_msg void OnKillfocusOffsetangle();
	afx_msg void OnCalculatex();
	afx_msg void OnCalculatey();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
