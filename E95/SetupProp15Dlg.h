#pragma once
#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"
#include "WaferMap.h"

// CSetupProp15Dlg dialog -- Added By Mahedi Kamal - 03202024

class CSetupProp15Dlg : public CResizableDialog, public CSetupPropComm
{
private:
	BOOL bTimer;
	//[ variables to show in roughnes tab
	double RH = 0;
	double	_Vf1 = 0;
	double	_Vf2 = 0;
	// [05162024 ZHIMING
	float	fRoughZPos = 0.0f;
	//]
	void UpdateSigmaRoughness();

public:
	CSetupProp15Dlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSetupProp15Dlg();
	enum { IDD = IDD_SETUPPROP15 };
	CListCtrl	m_cPoint;
public:
	void LocalToRecipe(); // not implimented
	void RecipeToLocal();  // not implimented
	void Renumber();
	void RenewParams();
public:
	virtual BOOL DestroyWindow();
public:
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedMeasureRoughness();
	afx_msg void OnBnClickedRead();
	afx_msg void OnRoughnessTimer();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillfocusNmeas();
	afx_msg void OnKillfocusSigmacalibr();
	afx_msg void OnKillfocusSigmacalibr2();
	afx_msg void OnKillfocusfReflOffset();
	afx_msg void OnKillfocusfScatOffset();
protected:
	virtual BOOL OnInitDialog();
	long OnAddalgnpoint(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
public:

	CWaferMap m_cWaferMap;
	afx_msg void OnCalibration();
	afx_msg void OnCalibration2();
	afx_msg void OnBnClicked15dlgRa();
	afx_msg void OnBnClicked15dlgRms();
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedMotioncontrol1();
	afx_msg void OnBnClickedRoughZpos();
	afx_msg void OnKillfocusRoughnessZPos();
	afx_msg void OnBnClickedRoughnessRecipe();
	afx_msg void OnBnClickedRoughnessReset();
};

//////

