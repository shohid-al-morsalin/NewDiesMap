#pragma once
#include "afxwin.h"
#include "resource.h"
#include "ResizableFormView.h"
#include "..\SRC\xSkinButton.h"

// CTitlePanel form view

class CTitlePanel : public CResizableFormView {
	DECLARE_DYNCREATE(CTitlePanel)

	short AuthenticateLevel;
	short AuthenLevelBak;

	// Form Data
public:
	//{{AFX_DATA(CTitlePanel)
	enum { IDD = IDD_TITLEPANEL };
	CxSkinButton	m_Blue;
	CxSkinButton	m_Amber;
	CxSkinButton	m_Green;
	CxSkinButton	m_Red;
	CStatic	m_cViewName;
	CString	m_DateTime;
	CString	m_CommState;
	CString	m_EquipState;
	CString	m_Message;
	CString	m_OnTime;
	//}}AFX_DATA
	char LTstate[4];

protected:
	CTitlePanel();           // protected constructor used by dynamic creation
	virtual ~CTitlePanel();

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CTitlePanel)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	//{{AFX_MSG(CTitlePanel)
	afx_msg void OnBnClickedLogin();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnLtwShow(WPARAM wP, LPARAM lP);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void AskLogin();
	void SetMessage(CString msg);
	void SetEquipmentState(CString state);
	void SetCommState(CString state);
	void UpdateLTower(char* state);
	void SetCurrentView(CString ViewName);
	void LogUserInfo(int Authentication, int status);
};
