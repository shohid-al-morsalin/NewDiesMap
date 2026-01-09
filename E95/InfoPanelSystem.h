#pragma once
#include "ResizableFormView.h"
#include "xtabctrl.h"
#include "afxcmn.h"

// CInfoPanelSystem form view
class COnLineDlg;
class CLoadPortDlg;
class CSystemRobotDlg;
class CSystemStatusDlg;
class CSystem413Dlg;
class CSystemLTowerDlg;
class CRecoveryDlg;

class CInfoPanelSystem : public CResizableFormView {
	DECLARE_DYNCREATE(CInfoPanelSystem)

protected:
	CInfoPanelSystem();           // protected constructor used by dynamic creation
	virtual ~CInfoPanelSystem();

public:
	//{{AFX_DATA(CInfoPanelSystem)
	enum { IDD = IDD_INFOPANELSYSTEM };
	//}}AFX_DATA
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoPanelSystem)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CInfoPanelSystem)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void ClearAll();
	LRESULT OnCloseView(WPARAM wP, LPARAM lP);
	LRESULT OnRobotRecovery(WPARAM wP, LPARAM lP);
	LRESULT OnSTSelected(WPARAM wParam, LPARAM lParam);

	short nLoadPort;
	CXTabCtrl m_tabctrl;
	CRecoveryDlg* m_RecoveryDlg;
	CSystemLTowerDlg* m_SystemLTowerDlg;
	CSystem413Dlg* m_System413Dlg;
	CLoadPortDlg* m_LoadPortDlg[4];

	CSystemRobotDlg* m_SystemRobotDlg;
	COnLineDlg* m_OnLineDlg;
	CSystemStatusDlg* m_SystemStatusDlg;
};
