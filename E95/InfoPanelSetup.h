#pragma once

class CSetupProp1Dlg;
class CSetupProp2Dlg;
class CSetupProp3Dlg;
class CSetupProp31Dlg;
class CSetupProp4Dlg;
class CSetupProp41Dlg;
class CSetupProp42Dlg;
class CSetupProp43Dlg;
class CSetupProp5Dlg;
class CSetupProp51Dlg;
class CSetupProp52Dlg;
class CSetupProp6Dlg;
class CSetupProp7Dlg;
class CSetupProp71Dlg;
class CSetupProp72Dlg;
class CSetupProp73Dlg;
class CSetupProp8Dlg;
class CSetupProp81Dlg;
class CSetupProp82Dlg;
class CSetupProp83Dlg;
class CSetupProp84Dlg;
class CSetupProp9Dlg;
class CSetupProp91Dlg;
class CSetupProp10Dlg;
class CSetupProp11Dlg;
class CSetupProp12Dlg;
class CSetupProp13Dlg;
class COnlineHSMSDlg;
class CSetupProp92Dlg; //20260101 Mahmudul

// [01172024 ZHIMING
class CSetupProp14Dlg;
// 03202024 Mahedi Kamal
class CSetupProp15Dlg;
// ]
// 08092024 Mahedi Kamal
class CSetupProp16Dlg;
// ]

// CInfoPanelSetup form view

#include "xtabctrl.h"
#include "ResizableFormView.h"

class CInfoPanelSetup : public CResizableFormView {
	DECLARE_DYNCREATE(CInfoPanelSetup)

protected:
	CInfoPanelSetup();           // protected constructor used by dynamic creation
	virtual ~CInfoPanelSetup();

public:
	CSetupProp1Dlg* m_Prop1Dlg;
	CSetupProp2Dlg* m_Prop2Dlg;
	CSetupProp3Dlg* m_Prop3Dlg;
	CSetupProp31Dlg* m_Prop31Dlg;
	CSetupProp4Dlg* m_Prop4Dlg;
	CSetupProp41Dlg* m_Prop41Dlg;
	CSetupProp42Dlg* m_Prop42Dlg;
	CSetupProp43Dlg* m_Prop43Dlg;
	CSetupProp5Dlg* m_Prop5Dlg;
	CSetupProp51Dlg* m_Prop51Dlg;
	CSetupProp52Dlg* m_Prop52Dlg;
	CSetupProp6Dlg* m_Prop6Dlg;
	CSetupProp7Dlg* m_Prop7Dlg;
	CSetupProp71Dlg* m_Prop71Dlg;
	CSetupProp72Dlg* m_Prop72Dlg;
	CSetupProp73Dlg* m_Prop73Dlg;
	CSetupProp8Dlg* m_Prop8Dlg;
	CSetupProp81Dlg* m_Prop81Dlg;
	CSetupProp82Dlg* m_Prop82Dlg;
	CSetupProp83Dlg* m_Prop83Dlg;
	CSetupProp84Dlg* m_Prop84Dlg;
	CSetupProp9Dlg* m_Prop9Dlg;
	CSetupProp91Dlg* m_Prop91Dlg;
	CSetupProp10Dlg* m_Prop10Dlg;
	CSetupProp11Dlg* m_Prop11Dlg;
	CSetupProp12Dlg* m_Prop12Dlg;
	CSetupProp13Dlg* m_Prop13Dlg;
	COnlineHSMSDlg* m_HSMSDlg;
	// [01172024 ZHIMING
	CSetupProp14Dlg* m_Prop14Dlg;
	// 03202024 Mahedi Kamal
	CSetupProp15Dlg* m_Prop15Dlg;
	// ]
	// 08092024 Mahedi Kamal
	CSetupProp16Dlg* m_Prop16Dlg;
	// ]
	CSetupProp92Dlg* m_Prop92Dlg;

	LRESULT OnSTSelected(WPARAM wParam, LPARAM lParam);
	LRESULT OnSTDeSelected(WPARAM wParam, LPARAM lParam);

	//{{AFX_DATA(CInfoPanelSetup)
	enum { IDD = IDD_INFOPANELSETUP };
	CXTabCtrl	m_tabctrl;
	//}}AFX_DATA
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoPanelSetup)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	//{{AFX_MSG(CInfoPanelSetup)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
