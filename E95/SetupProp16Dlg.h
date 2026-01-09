#pragma once
#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"
#include "WaferMap.h"

// CSetupProp16Dlg dialog -- Added By Mahedi Kamal - 08092024

class CSetupProp16Dlg : public CResizableDialog, public CSetupPropComm
{
public:
	CSetupProp16Dlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSetupProp16Dlg();
	enum { IDD = IDD_SETUPPROP16 };
public:
	void RecipeToLocal(); 
	void LocalToRecipe() {};
	void Renumber();
public:
	virtual BOOL DestroyWindow();
public:
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	virtual BOOL OnInitDialog();
	long OnAddalgnpoint(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()

public:
	CWaferMap m_cWaferMap;
	CListCtrl m_cPoint;

	afx_msg void OnBnClickedTaikoRecipe();
	afx_msg void OnBnClickedMotioncontrol2();

	//08142024 - START 

	CEdit m_radius;
	CEdit m_angle;
	CEdit m_ZPos;
	CEdit m_fromAngle;
	CEdit m_toAngle;
	CEdit m_distanceFromCenter;
	CEdit m_offsetAngle;
	CEdit m_waferRadius;

	CSICam	m_cVideo; 
	afx_msg void OnBnClickedCamerasettingTaiko();
	afx_msg void OnBnClickedBtnTrwGoXy();
	afx_msg void OnBnClickedBtnTrwSetz();
	bool UpdateAndValidate();
	void SetValue(); 
	afx_msg void OnBnClickedMeasureTaiko();
	CListCtrl m_cList;
	void SetupColumnMessage();
	void SetupColumnPos();
	CEdit m_minWidth;
	CEdit m_maxWidth;
	afx_msg void OnBnClickedTrwRemeas();
	CEdit m_Repeat;
	CEdit m_StepDistance;
	afx_msg void OnCbnSelchangeCombTrwStype();
	CComboBox m_comSType;
	enum SearchType { LINEAR, CIRCULAR };
	CEdit m_numOfPOints;
	CButton m_RemeasBtn;
	CImgWnd m_cImgWnd;
	CStatic m_cMouseMessage;
	long OnMoMove(WPARAM wP, LPARAM lP);
	CEdit m_RI;
	CEdit m_Exposure;
	void UpdateReport(int idx);
	afx_msg void OnBnClickedExportData();
	void ExportData(CString dir = DosUtil.GetResultDir());
	//08142024 - END
};
