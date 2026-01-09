#pragma once

// CSetupProp92Dlg dialog
#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"
#include "DiceMap.h"

class CSetupProp92Dlg : public CResizableDialog, public CSetupPropComm
{
	

public:
	CSetupProp92Dlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSetupProp92Dlg();

// Dialog Data

	enum { IDD = IDD_SETUPPROP92 };


public:
	void LocalToRecipe();
	void RecipeToLocal();


	double m_dWaferDiameter;
	double m_dGapX;
	double m_dGapY;
	//PointD pTL = { 32.58,121.255 };
	//PointD pBL = { 32.24, 88.965 };
	//PointD pBR = { 56.629, 88.699 };
	PointD pTL;
	PointD pBL;
	PointD pBR;

	//PointD pTL = { -2 , 4 };
	//PointD pBL = { -4 , 2 };
	//PointD pBR = { -2 , 0 };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CDiceMap m_diceMap;
	void InitEditControls();
	void SetEditVal(int nID, double val);
	afx_msg void OnBnClickedButtonUpdateDiemap();
	double GetDoubleFromEdit(int nID);
	void ReadData();
	CButton m_maskCheck;
	afx_msg void OnBnClickedCheckExcludeDie();
	afx_msg void OnBnClickedUL();
	afx_msg void OnBnClickedLL();
	afx_msg void OnBnClickedLR();

	double ULx = 0.0, ULy = 0.0, LLx = 0.0, LLy = 0.0, LRx = 0.0, LRy = 0.0;
	CButton m_btnUL, m_btnLL, m_btnLR;
	afx_msg void OnBnClickedCalPitch();
	afx_msg void OnBnClickedPreconstructDie();
	afx_msg void OnBnClickedCreateDie();
};
