#pragma once

// #include "afxcmn.h"
// #include "procjobinfo.h"
#include "ResizableFormView.h"
// #include "SYS/Var.h"
#include "xtabctrl.h"

class CProcJobPWC1DlgBase;
class CProcJobCompleteDlg;

class CInfoPanelMain : public CResizableFormView {
	DECLARE_DYNCREATE(CInfoPanelMain)

protected:
	CInfoPanelMain();           // protected constructor used by dynamic creation
	virtual ~CInfoPanelMain();

public:
	// Dialog Data
	//{{AFX_DATA(CInfoPanelMain)
	enum { IDD = IDD_INFOPANELMAIN };
	//}}AFX_DATA
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoPanelMain)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CInfoPanelMain)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void RestartPage(short ID, CString& CarrierID, CString& Recipe/*, CVar::DISKSELK mode*/);
	void DoProcessJobQueued(short ID, short type); // added short ID [12/18/2007 YUEN]
	void DoProcessComplete(short ID);// added short ID [12/18/2007 YUEN]
	LRESULT OnCloseView(WPARAM wP, LPARAM lP);
	LRESULT OnRobotRecovery(WPARAM wP, LPARAM lP);
	LRESULT OnSTSelected(WPARAM wParam, LPARAM lParam);
	CString InspectRecipeTabName;
	LRESULT OnCloseTab(WPARAM wP, LPARAM lP);
	LRESULT OnToProcJobInspectRecipe413(WPARAM wP, LPARAM lP);
	CProcJobPWC1DlgBase* pTmp3;
	CProcJobCompleteDlg* pTmp4;

	virtual void OnInitialUpdate();

	CXTabCtrl m_tabctrl;

	LRESULT OnToProcJobPWC1(WPARAM wP, LPARAM lP);
	LRESULT OnToProcJobPWC2(WPARAM wP, LPARAM lP);
	LRESULT OnToProcJobProcessing(WPARAM wP, LPARAM lP);
	LRESULT OnToProcJobCancelCarrier(WPARAM wP, LPARAM lP);
	LRESULT OnToProcJobUnloadCarrier(WPARAM wP, LPARAM lP);
	LRESULT OnToProcJobUnloadComplete(WPARAM wP, LPARAM lP);
	LRESULT OnToProcJobComplete(WPARAM wP, LPARAM lP);
	LRESULT OnToProcJobQueued(WPARAM wP, LPARAM lP);
	LRESULT OnToProcJobRecovery(WPARAM wP, LPARAM lP);
	LRESULT OnToProcJobDlg(WPARAM wP, LPARAM lP);
	LRESULT OnToProcJobManualDlg(WPARAM wP, LPARAM lP);

	void Log(CString msg, short lvl = 5);
};
