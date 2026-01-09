#pragma once
// SiCamSettingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "resource.h"

class CCameraView;
class CSI3300CamDlg;

class CSiCamSettingDlg : public CDialog {
	// Construction
public:
	// 	void SaveSettingToView();
	// 	void LoadSettingFromView();
	CSiCamSettingDlg(/*CSI3300CamDlg *pCam,*/ CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSiCamSettingDlg)
	enum { IDD = IDD_SI_CAM_SETTING };
	UINT	nTime;
	short	m_TargetExposure;
	short	m_TargetExposure2;
	//}}AFX_DATA

	int iGainIni;
	BOOL bAuto;
	BOOL bFocus;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSiCamSettingDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// 	CSI3300CamDlg *m_pCamDlg;
	// 	CCameraView *m_pCamView;
	void EnableButtons(BOOL bEnable);
	// Generated message map functions
	//{{AFX_MSG(CSiCamSettingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	afx_msg void OnAutoexposure();
	afx_msg void OnAutofocus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};