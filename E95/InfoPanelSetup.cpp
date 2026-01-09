// InfoPanelSetup.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "InfoPanelSetup.h"

#include "413App.h"
#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#include "SetupProp1Dlg.h"
#include "SetupProp2Dlg.h"
#include "SetupProp3Dlg.h"
#include "SetupProp31Dlg.h"
#include "SetupProb4Dlg.h"
#include "SetupProp41Dlg.h"
#include "SetupProp42Dlg.h"
#include "SetupProp43Dlg.h"
#include "SetupProp5Dlg.h"
#include "SetupProp51Dlg.h"
#include "SetupProp52Dlg.h"
#include "SetupProp6Dlg.h"
#include "SetupProp7Dlg.h"
#include "SetupProp71Dlg.h"
#include "SetupProp72Dlg.h"
#include "SetupProp73Dlg.h"
#include "SetupProp8Dlg.h"
#include "SetupProp81Dlg.h"
#include "SetupProp82Dlg.h"
#include "SetupProp83Dlg.h"
#include "SetupProp84Dlg.h"
#include "SetupProp9Dlg.h"
#include "SetupProp91Dlg.h"
#include "SetupProp10Dlg.h"
#include "SetupProp11Dlg.h"
#include "SetupProp12Dlg.h"
#include "SetupProp13Dlg.h"
#include "OnlineHSMSDlg.h"

// [01172024 ZHIMING
#include "SetupProp14Dlg.h"
// 03202024 Mahedi Kamal
#include "SetupProp15Dlg.h"
// ]
// 08092024 Mahedi Kamal
#include "SetupProp16Dlg.h"
// ]
 //[20260101 Mahmudul
#include "SetupProp92Dlg.h"
//]

#include "XTabCtrl.h"
#include "ST_SplitterWnd.h"

// CInfoPanelSetup

IMPLEMENT_DYNCREATE(CInfoPanelSetup, CResizableFormView)

CInfoPanelSetup::CInfoPanelSetup()
	: CResizableFormView(CInfoPanelSetup::IDD) {
	//{{AFX_DATA_INIT(CInfoPanelSetup)
	//}}AFX_DATA_INIT
	m_Prop1Dlg = NULL;
	m_Prop2Dlg = NULL;
	m_Prop3Dlg = NULL;
	m_Prop31Dlg = NULL;
	m_Prop4Dlg = NULL;
	m_Prop41Dlg = NULL;
	m_Prop42Dlg = NULL;
	m_Prop43Dlg = NULL;
	m_Prop5Dlg = NULL;
	m_Prop51Dlg = NULL;
	m_Prop52Dlg = NULL;
	m_Prop6Dlg = NULL;
	m_Prop7Dlg = NULL;
	m_Prop71Dlg = NULL;
	m_Prop72Dlg = NULL;
	m_Prop73Dlg = NULL;
	m_Prop8Dlg = NULL;
	m_Prop81Dlg = NULL;
	m_Prop82Dlg = NULL;
	m_Prop83Dlg = NULL;
	m_Prop84Dlg = NULL;
	m_Prop9Dlg = NULL;
	m_Prop91Dlg = NULL;
	m_Prop10Dlg = NULL;
	m_Prop11Dlg = NULL;
	m_Prop12Dlg = NULL;
	m_Prop13Dlg = NULL;

	m_HSMSDlg = NULL;

	// [01172024 ZHIMING
	m_Prop14Dlg = NULL;
	// 03202024 Mahedi Kamal
	m_Prop15Dlg = NULL;
	// ]
	// 08092024 Mahedi Kamal
	m_Prop16Dlg = NULL;
	// ]
	//[20260101 Mahmudul
	m_Prop92Dlg = NULL;
	//]

	pUDev->pInfoPanelSetup = this;
}

CInfoPanelSetup::~CInfoPanelSetup() {}

void CInfoPanelSetup::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelSetup)
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelSetup, CResizableFormView)
	//{{AFX_MSG_MAP(CInfoPanelSetup)
	ON_MESSAGE(WM_STSELECTED, OnSTSelected)
	ON_MESSAGE(WM_STDESELECTED, OnSTDeSelected)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CInfoPanelSetup diagnostics

#ifdef _DEBUG
void CInfoPanelSetup::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelSetup::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

// CInfoPanelSetup message handlers

void CInfoPanelSetup::OnInitialUpdate() {
	CResizableFormView::OnInitialUpdate();

	AddResizedControl(IDC_TAB1, TOP_LEFT, BOTTOM_RIGHT);
	AnchorControls();

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	short nTab = 0;

	// [01172024 ZHIMING
	if (p413App->Global.Confg.bConfocalMod) {
		m_Prop14Dlg = new CSetupProp14Dlg;
		if (m_Prop14Dlg) {
			m_Prop14Dlg->m_Parent = this;
			m_Prop14Dlg->name.Format("Confocal (%d)", nTab);
			m_Prop14Dlg->Create(CSetupProp14Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop14Dlg, "Confocal", nTab++);
		}
	}

	if (p413App->Global.Confg.bEchoMod) {
		/*m_Prop3Dlg = new CSetupProp3Dlg;
		if (m_Prop3Dlg) {
			m_Prop3Dlg->m_Parent = this;
			m_Prop3Dlg->name.Format("Echo (%d)", nTab);
			m_Prop3Dlg->Create(CSetupProp3Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop3Dlg, "Echo", nTab++);
		}*/
		m_Prop7Dlg = new CSetupProp7Dlg;
		if (m_Prop7Dlg) {
			m_Prop7Dlg->m_Parent = this;
			m_Prop7Dlg->name.Format("MeSets (%d)", nTab);
			m_Prop7Dlg->Create(CSetupProp7Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop7Dlg, "MeSets", nTab++);
		}
		m_Prop72Dlg = new CSetupProp72Dlg;
		if (m_Prop72Dlg) {
			m_Prop72Dlg->m_Parent = this;
			m_Prop72Dlg->name.Format("Echo Setting (%d)", nTab);
			m_Prop72Dlg->Create(CSetupProp72Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop72Dlg, "Echo Setting", nTab++);
		}
		if (p413App->Global.Confg.bBowWarp) {
			m_Prop91Dlg = new CSetupProp91Dlg;
			if (m_Prop91Dlg) {
				m_Prop91Dlg->m_Parent = this;
				m_Prop91Dlg->name.Format("Bow Warp (%d)", nTab);
				m_Prop91Dlg->Create(CSetupProp91Dlg::IDD, &m_tabctrl);
				m_tabctrl.AddTab(m_Prop91Dlg, "Bow Warp", nTab++);
			}
		}
		if (p413App->Global.Confg.bStressMod) {
			m_Prop71Dlg = new CSetupProp71Dlg;
			if (m_Prop71Dlg) {
				m_Prop71Dlg->m_Parent = this;
				m_Prop71Dlg->name.Format("Stress (%d)", nTab);
				m_Prop71Dlg->Create(CSetupProp71Dlg::IDD, &m_tabctrl);
				m_tabctrl.AddTab(m_Prop71Dlg, "Stress", nTab++);
			}
		}
	}
	if (p413App->Global.Confg.bTrimMod) {
		m_Prop81Dlg = new CSetupProp81Dlg;
		if (m_Prop81Dlg) {
			m_Prop81Dlg->m_Parent = this;
			m_Prop81Dlg->name.Format("Trim (%d)", nTab);
			m_Prop81Dlg->Create(CSetupProp81Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop81Dlg, "Trim", nTab++);
		}
		m_Prop83Dlg = new CSetupProp83Dlg;
		if (m_Prop83Dlg) {
			m_Prop83Dlg->m_Parent = this;
			m_Prop83Dlg->name.Format("Trim Setting (%d)", nTab);
			m_Prop83Dlg->Create(CSetupProp83Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop83Dlg, "Trim Setting", nTab++);
		}
	}
	if (p413App->Global.Confg.bThinFilmMod) {
		m_Prop4Dlg = new CSetupProp4Dlg;
		if (m_Prop4Dlg) {
			m_Prop4Dlg->m_Parent = this;
			m_Prop4Dlg->name.Format("Thin Film, (%d)", nTab);
			m_Prop4Dlg->Create(CSetupProp4Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop4Dlg, "Thin Film", nTab++);
		}
		m_Prop43Dlg = new CSetupProp43Dlg;
		if (m_Prop43Dlg) {
			m_Prop43Dlg->m_Parent = this;
			m_Prop43Dlg->name.Format("Thin Film Setting (%d)", nTab);
			m_Prop43Dlg->Create(CSetupProp43Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop43Dlg, "Thin Film Setting", nTab++);
		}
	}

	if (p413App->Global.Confg.bPatMatchMod) {

		// [03152024 ZHIMING activate until we know how to use
		m_Prop73Dlg = new CSetupProp73Dlg;
		if (m_Prop73Dlg) {
			m_Prop73Dlg->m_Parent = this;
			m_Prop73Dlg->name.Format("Positions (%d)", nTab);
			m_Prop73Dlg->Create(CSetupProp73Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop73Dlg, "Positions", nTab++);
		}
		// ]

		m_Prop5Dlg = new CSetupProp5Dlg;
		if (m_Prop5Dlg) {
			m_Prop5Dlg->m_Parent = this;
			m_Prop5Dlg->name.Format("Patterns (%d)", nTab);
			m_Prop5Dlg->Create(CSetupProp5Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop5Dlg, "Patterns", nTab++);
		}
		m_Prop52Dlg = new CSetupProp52Dlg;
		if (m_Prop52Dlg) {
			m_Prop52Dlg->m_Parent = this;
			m_Prop52Dlg->name.Format("Matching Setting (%d)", nTab);
			m_Prop52Dlg->Create(CSetupProp52Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop52Dlg, "Matching Setting", nTab++);
		}
		m_Prop51Dlg = new CSetupProp51Dlg;
		if (m_Prop51Dlg) {
			m_Prop51Dlg->m_Parent = this;
			m_Prop51Dlg->name.Format("Pattern Matching (%d)", nTab);
			m_Prop51Dlg->Create(CSetupProp51Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop51Dlg, "Pattern Matching", nTab++);
		}
	}
	//	m_Prop2Dlg = new CSetupProp2Dlg;
	if (m_Prop2Dlg) {
		m_Prop2Dlg->Create(CSetupProp2Dlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_Prop2Dlg, "SPC", nTab++);
	}
	if (Var.UIMode > 1) {
		m_Prop6Dlg = new CSetupProp6Dlg;
		if (m_Prop6Dlg) {
			m_Prop6Dlg->m_Parent = this;
			m_Prop6Dlg->name.Format("Motor Setting (%d)", nTab);
			m_Prop6Dlg->Create(IDD_SETUPPROP61, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop6Dlg, "Motor Setting", nTab++);
		}
	}
	if (p413App->Global.Confg.bEchoMicroscopeMod || p413App->Global.Confg.bThinFilmMicroscopeMod) {
		if (Var.UIMode > 1) {
			if (p413App->Global.Confg.bEchoMicroscopeMod || p413App->Global.Confg.bThinFilmMicroscopeMod) {
				m_Prop8Dlg = new CSetupProp8Dlg;
				if (m_Prop8Dlg) {
					m_Prop8Dlg->m_Parent = this;
					m_Prop8Dlg->name.Format("Screen Setting (%d)", nTab);
					m_Prop8Dlg->Create(CSetupProp8Dlg::IDD, &m_tabctrl);
					m_tabctrl.AddTab(m_Prop8Dlg, "Screen Setting", nTab++);
				}
			}
		}
	}
	if (Var.UIMode > 1) {
		if (p413App->Global.Confg.bSecsMod) {
			m_HSMSDlg = new COnlineHSMSDlg;
			if (m_HSMSDlg) {
				m_HSMSDlg->Create(COnlineHSMSDlg::IDD, &m_tabctrl);
				m_tabctrl.AddTab(m_HSMSDlg, "HSMS", nTab++);
			}
		}

		m_Prop9Dlg = new CSetupProp9Dlg;
		if (m_Prop9Dlg) {
			m_Prop9Dlg->m_Parent = this;
			m_Prop9Dlg->name.Format("Systm Setting (%d)", nTab);
			m_Prop9Dlg->Create(CSetupProp9Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop9Dlg, "System Setting", nTab++);
		}
	}

	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		m_Prop10Dlg = new CSetupProp10Dlg;
		if (m_Prop10Dlg) {
			m_Prop10Dlg->m_Parent = this;
			m_Prop10Dlg->name.Format("Wafer Transfer (%d)", nTab);
			m_Prop10Dlg->Create(CSetupProp10Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop10Dlg, "Wafer Transfer", nTab++);
		}
		if (Var.UIMode > 1) {
			m_Prop11Dlg = new CSetupProp11Dlg;
			if (m_Prop11Dlg) {
				m_Prop11Dlg->m_Parent = this;
				m_Prop11Dlg->name.Format("Stage Setting (%d)", nTab);
				m_Prop11Dlg->Create(CSetupProp11Dlg::IDD, &m_tabctrl);
				m_tabctrl.AddTab(m_Prop11Dlg, "Stage Setting", nTab++);
			}
		}
		//m_Prop82Dlg = new CSetupProp82Dlg;
		if (m_Prop82Dlg) {
			m_Prop82Dlg->m_Parent = this;
			m_Prop82Dlg->name.Format("About (%d)", nTab);
			m_Prop82Dlg->Create(CSetupProp82Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop82Dlg, "About", nTab++);
		}

		if (p413App->Global.Confg.bEchoMod) {
			if (Var.UIMode > 1) {
				if (p413App->Global.Confg.bEchoMicroscopeMod) {
					m_Prop12Dlg = new CSetupProp12Dlg;
					if (m_Prop12Dlg) {
						m_Prop12Dlg->m_Parent = this;
						m_Prop12Dlg->name.Format("Check Stage (%d)", nTab);
						m_Prop12Dlg->Create(CSetupProp12Dlg::IDD, &m_tabctrl);
						m_tabctrl.AddTab(m_Prop12Dlg, "Check Stage", nTab++);
					}
					//m_Prop1Dlg = new CSetupProp1Dlg;
					if (m_Prop1Dlg) {
						m_Prop1Dlg->m_Parent = this;
						m_Prop1Dlg->name.Format("Center Stage (%d)", nTab);
						m_Prop1Dlg->Create(CSetupProp1Dlg::IDD, &m_tabctrl);
						m_tabctrl.AddTab(m_Prop1Dlg, "Center Stage", nTab++);
					}
				}
				m_Prop84Dlg = new CSetupProp84Dlg;
				if (m_Prop84Dlg) {
					m_Prop84Dlg->m_Parent = this;
					m_Prop84Dlg->name.Format("Wafer Align (%d)", nTab);
					m_Prop84Dlg->Create(CSetupProp84Dlg::IDD, &m_tabctrl);
					m_tabctrl.AddTab(m_Prop84Dlg, "Wafer Align", nTab++);
				}
			}
		}
		if (p413App->Global.Confg.bEchoMicroscopeMod || p413App->Global.Confg.bThinFilmMicroscopeMod) {
			if (Var.UIMode > 1) {
				m_Prop41Dlg = new CSetupProp41Dlg;
				if (m_Prop41Dlg) {
					m_Prop41Dlg->m_Parent = this;
					m_Prop41Dlg->name.Format("Camera Analyzer(%d)", nTab);
					m_Prop41Dlg->Create(CSetupProp41Dlg::IDD, &m_tabctrl);
					m_tabctrl.AddTab(m_Prop41Dlg, "Camera Analyzer", nTab++);
				}
			}
		}
		if (p413App->Global.Confg.bThinFilmMod) {
			if (Var.UIMode > 2) {
				m_Prop42Dlg = new CSetupProp42Dlg;
				if (m_Prop42Dlg) {
					m_Prop42Dlg->m_Parent = this;
					m_Prop42Dlg->name.Format("Thin Film Analyzer (%d)", nTab);
					m_Prop42Dlg->Create(CSetupProp42Dlg::IDD, &m_tabctrl);
					m_tabctrl.AddTab(m_Prop42Dlg, "Thin Film Analyzer", nTab++);
				}
			}
		}
		if (p413App->Global.Confg.bEchoMod) {
			if (Var.UIMode > 2) {
				m_Prop31Dlg = new CSetupProp31Dlg;
				if (m_Prop31Dlg) {
					m_Prop31Dlg->m_Parent = this;
					m_Prop31Dlg->name.Format("Echo Analyzer (%d)", nTab);
					m_Prop31Dlg->Create(CSetupProp31Dlg::IDD, &m_tabctrl);
					m_tabctrl.AddTab(m_Prop31Dlg, "Echo Analyzer", nTab++);
				}
			}
		}
		if (p413App->Global.Confg.bEchoMicroscopeMod) {
			if (Var.UIMode > 2) {
				m_Prop13Dlg = new CSetupProp13Dlg;
				if (m_Prop13Dlg) {
					m_Prop13Dlg->m_Parent = this;
					m_Prop13Dlg->name.Format("Edge Analyzer (%d)", nTab);
					m_Prop13Dlg->Create(CSetupProp13Dlg::IDD, &m_tabctrl);
					m_tabctrl.AddTab(m_Prop13Dlg, "Edge Analyzer", nTab++);
				}
			}
		}
	}
	
	//[ 03202024 Mahedi Kamal
	if (p413App->Global.Confg.bRoughness) {
		m_Prop15Dlg = new CSetupProp15Dlg;
		if (m_Prop15Dlg) {
			m_Prop15Dlg->m_Parent = this;
			m_Prop15Dlg->name.Format("Roughness (%d)", nTab);
			m_Prop15Dlg->Create(CSetupProp15Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop15Dlg, "Roughness", nTab++);
		}
	}
	// ]
	//[ 08092024 Mahedi Kamal
	if (p413App->Global.Confg.bTaiko) {
		m_Prop16Dlg = new CSetupProp16Dlg;
		if (m_Prop16Dlg) {
			m_Prop16Dlg->m_Parent = this;
			m_Prop16Dlg->name.Format("Taiko (%d)", nTab);
			m_Prop16Dlg->Create(CSetupProp16Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop16Dlg, "Taiko", nTab++);
		}
	}
	// ]
	
	//[20260101 Mahmudul
	if (1) {
		m_Prop92Dlg = new CSetupProp92Dlg;
		if (m_Prop92Dlg) {
			m_Prop92Dlg->m_Parent = this;
			m_Prop92Dlg->name.Format("DiceMap (%d)", nTab);
			m_Prop92Dlg->Create(CSetupProp92Dlg::IDD, &m_tabctrl);
			m_tabctrl.AddTab(m_Prop92Dlg, "DiceMap", nTab++);
		}
	}
	//]
}

LRESULT CInfoPanelSetup::OnSTSelected(WPARAM wParam, LPARAM lParam) {
	m_tabctrl.ActivateCurrentTab();
	return 0;
}

LRESULT CInfoPanelSetup::OnSTDeSelected(WPARAM wParam, LPARAM lParam) {
	m_tabctrl.DeactivateCurrentTab();
	return 0;
}

void CInfoPanelSetup::OnDestroy() {
	if (m_Prop1Dlg) {
		m_Prop1Dlg->DestroyWindow();
		m_Prop1Dlg = NULL;
	}
	if (m_Prop2Dlg) {
		m_Prop2Dlg->DestroyWindow();
		m_Prop2Dlg = NULL;
	}
	if (m_Prop3Dlg) {
		m_Prop3Dlg->DestroyWindow();
		m_Prop3Dlg = NULL;
	}
	if (m_Prop31Dlg) {
		m_Prop31Dlg->DestroyWindow();
		m_Prop31Dlg = NULL;
	}
	if (m_Prop4Dlg) {
		m_Prop4Dlg->DestroyWindow();
		m_Prop4Dlg = NULL;
	}
	if (m_Prop41Dlg) {
		m_Prop41Dlg->DestroyWindow();
		m_Prop41Dlg = NULL;
	}
	if (m_Prop42Dlg) {
		m_Prop42Dlg->DestroyWindow();
		m_Prop42Dlg = NULL;
	}
	if (m_Prop43Dlg) {
		m_Prop43Dlg->DestroyWindow();
		m_Prop43Dlg = NULL;
	}
	if (m_Prop5Dlg) {
		m_Prop5Dlg->DestroyWindow();
		m_Prop5Dlg = NULL;
	}
	if (m_Prop51Dlg) {
		m_Prop51Dlg->DestroyWindow();
		m_Prop51Dlg = NULL;
	}
	if (m_Prop52Dlg) {
		m_Prop52Dlg->DestroyWindow();
		m_Prop52Dlg = NULL;
	}
	if (m_Prop6Dlg) {
		m_Prop6Dlg->DestroyWindow();
		m_Prop6Dlg = NULL;
	}
	if (m_Prop7Dlg) {
		m_Prop7Dlg->DestroyWindow();
		m_Prop7Dlg = NULL;
	}
	if (m_Prop71Dlg) {
		m_Prop71Dlg->DestroyWindow();
		m_Prop71Dlg = NULL;
	}
	if (m_Prop72Dlg) {
		m_Prop72Dlg->DestroyWindow();
		m_Prop72Dlg = NULL;
	}
	if (m_Prop73Dlg) {
		m_Prop73Dlg->DestroyWindow();
		m_Prop73Dlg = NULL;
	}
	if (m_Prop8Dlg) {
		m_Prop8Dlg->DestroyWindow();
		m_Prop8Dlg = NULL;
	}
	if (m_Prop81Dlg) {
		m_Prop81Dlg->DestroyWindow();
		m_Prop81Dlg = NULL;
	}
	if (m_Prop82Dlg) {
		m_Prop82Dlg->DestroyWindow();
		m_Prop82Dlg = NULL;
	}
	if (m_Prop83Dlg) {
		m_Prop83Dlg->DestroyWindow();
		m_Prop83Dlg = NULL;
	}
	if (m_Prop84Dlg) {
		m_Prop84Dlg->DestroyWindow();
		m_Prop84Dlg = NULL;
	}
	if (m_Prop9Dlg) {
		m_Prop9Dlg->DestroyWindow();
		m_Prop9Dlg = NULL;
	}
	if (m_Prop91Dlg) {
		m_Prop91Dlg->DestroyWindow();
		m_Prop91Dlg = NULL;
	}
	if (m_Prop10Dlg) {
		m_Prop10Dlg->DestroyWindow();
		m_Prop10Dlg = NULL;
	}
	if (m_Prop11Dlg) {
		m_Prop11Dlg->DestroyWindow();
		m_Prop11Dlg = NULL;
	}
	if (m_HSMSDlg) {
		m_HSMSDlg->DestroyWindow();
		m_HSMSDlg = NULL;
	}
	if (m_Prop12Dlg) {
		m_Prop12Dlg->DestroyWindow();
		m_Prop12Dlg = NULL;
	}
	if (m_Prop13Dlg) {
		m_Prop13Dlg->DestroyWindow();
		m_Prop13Dlg = NULL;
	}

	// [01172024 ZHIMING
	if (m_Prop14Dlg) {
		m_Prop14Dlg->DestroyWindow();
		m_Prop14Dlg = NULL;
	}
	//[ 03202024 Mahedi Kamal
	if (m_Prop15Dlg) {
		m_Prop15Dlg->DestroyWindow();
		m_Prop15Dlg = NULL;
	}
	//]

	//[ 08092024 Mahedi Kamal
	if (m_Prop16Dlg) {
		m_Prop16Dlg->DestroyWindow();
		m_Prop16Dlg = NULL;
	}
	//]

	CResizableFormView::OnDestroy();
}