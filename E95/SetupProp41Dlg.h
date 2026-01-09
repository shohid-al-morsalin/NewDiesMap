#pragma once
// SetupProp41Dlg.h : header file
//

#include "Global413.h"
#include "SetupPropComm.h"
#include "SICam.h"
#include "ImgWnd.h"
#include "..\SRC\ResizableDialog.h"

#include "PackedDIB1.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp41Dlg dialog

class CSetupProp41Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	void LocalToRecipe() {};
	void RecipeToLocal();
	void ClearTextInfoAll();
	void Message(CString str);
	void DoFocus();
	void DoExposure();
	void StartCamera(CCamBase::CAMERAID No);
	CSetupProp41Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp41Dlg)
	enum { IDD = IDD_SETUPPROP41 };
	CButton	m_cSearch;
	CButton	m_cSnap;
	CButton	m_cFocus;
	CButton	m_cExposure;
	CListBox	m_cList;
	CButton	m_cCameraB;
	CButton	m_cCameraA;
	CImgWnd	m_cImgWnd1;
	CImgWnd	m_cImgWnd2;
	CSICam	m_cVideo;
	float	m_MinimumStep;
	float	m_Range;
	short	m_XOff;
	short	m_YOff;
	float	m_Range2;
	float	m_MinimunStep2;
	//}}AFX_DATA

	CPackedDIB DIB;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp41Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp41Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCameraa();
	afx_msg void OnCamerab();
	afx_msg void OnFocus();
	afx_msg void OnScore();
	afx_msg void OnClearlist();
	afx_msg void OnKillfocusMinimumstep();
	afx_msg void OnKillfocusRange();
	afx_msg void OnKillfocusXoff();
	afx_msg void OnKillfocusYoff();
	afx_msg void OnCamerasetting();
	afx_msg void OnMotioncontrol();
	afx_msg void OnExposure();
	afx_msg void OnContrast();
	afx_msg void OnGrey();
	afx_msg void OnContrast2();
	afx_msg void OnBinarize();
	afx_msg void OnGaussian();
	afx_msg void OnBinarizeh();
	afx_msg void OnBinarizel();
	afx_msg void OnContrast4();
	afx_msg void OnContrast5();
	afx_msg void OnSnap();
	afx_msg void OnSearcharound();
	afx_msg void OnKillfocusRange2();
	afx_msg void OnKillfocusMinimumstep2();
	afx_msg void OnAvehue();
	afx_msg void OnAvesat();
	afx_msg void OnDoublesnap();
	afx_msg void OnResetrnd1();
	//}}AFX_MSG
	long OnButClick(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
