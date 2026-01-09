// OnlineHSMSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "strSOp.h"
#include <string>
#include "E95.h"
#include "OnlineHSMSDlg.h"
#include ".\onlinehsmsdlg.h"
#include "..\ONLINE\mfc\ParaEditDlg.h"
#include "..\ONLINE\gem\CediListCtroller.h"
#include "..\ONLINE\general_utils.h"
#include "..\ONLINE\tcpip\HsmsParaMeter.h"

#include "SecsParam.h"

#include "GlobalDeviceContainer.h"
// extern CCediListCtroller m_CeidList;

//IMPLEMENT_DYNAMIC(COnlineHSMSDlg, CResizableDialog)
COnlineHSMSDlg::COnlineHSMSDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COnlineHSMSDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(COnlineHSMSDlg)
	//}}AFX_DATA_INIT
}

COnlineHSMSDlg::~COnlineHSMSDlg() {}

void COnlineHSMSDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnlineHSMSDlg)
	DDX_Control(pDX, IDC_PARALIST, m_ParaList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COnlineHSMSDlg, CResizableDialog)
	//{{AFX_MSG_MAP(COnlineHSMSDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PARALIST, OnLvnItemchangedParalist)
	ON_NOTIFY(NM_DBLCLK, IDC_PARALIST, OnNMDblclkParalist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// COnlineHSMSDlg message handlers

void COnlineHSMSDlg::OnLvnItemchangedParalist(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void COnlineHSMSDlg::OnNMDblclkParalist(NMHDR* pNMHDR, LRESULT* pResult) {
	// TODO: Add your control notification handler code here
	CString dwNo("");
	CString dwParaName("");
	CString dwParaValue("");
	CString dwParaRangeMin("");
	CString dwParaRangeMax("");
	CString dwParaDescript("");

	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_PARALIST);

	int iSel = pListCtrl->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	// if(iSel == -1)
	//	iSel = pListCtrl->GetNextItem(-1, LVNI_ALL | LVNI_FOCUSED);

	if (iSel > -1) {
		dwNo = pListCtrl->GetItemText(iSel, 0);
		dwParaName = pListCtrl->GetItemText(iSel, 1);
		dwParaValue = pListCtrl->GetItemText(iSel, 2);
		dwParaRangeMin = pListCtrl->GetItemText(iSel, 3);
		dwParaRangeMax = pListCtrl->GetItemText(iSel, 4);
		dwParaDescript = pListCtrl->GetItemText(iSel, 5);

		CParaEditDlg Dlg;
		Dlg.m_ParaName = dwParaName;
		Dlg.m_Value = dwParaValue;
		Dlg.m_ParaRangeMin = dwParaRangeMin;
		Dlg.m_ParaRangeMax = dwParaRangeMax;
		Dlg.m_ParaDesc = dwParaDescript;

		if (Dlg.DoModal() == IDOK) {
			char nBuf[150];
			// int item ;
			if (dwParaValue != Dlg.m_Value) {
				memset(nBuf, 0, sizeof nBuf);
				memcpy(nBuf, Dlg.m_Value.operator const char* (), Dlg.m_Value.GetLength());
				if (ParaVal_Setting(Dlg.m_ParaName, Dlg.m_Value, dwParaRangeMin, dwParaRangeMax)) {
					m_ParaList.SetItemText(iSel, 2, nBuf);
					ParaDB_Saving(Dlg.m_ParaName, Dlg.m_Value);
					AfxMessageBox("Value Set corrctly");
				}
				else {
					AfxMessageBox("Value Cannot be set");
				}
			}
		}
	}

	*pResult = 0;
}

BOOL COnlineHSMSDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_PARALIST, TOP_LEFT, BOTTOM_RIGHT);

	Init_Para_ListCtrl();
	Init_DataBase();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COnlineHSMSDlg::Init_Para_ListCtrl() {
	char* dwName[] =
	{
		("  NO"), ("Parameter Name"), ("    Value"),
			("  Range Min"), ("  Range Max"), ("     Description")
	};
	int nSize[] = { 40, 120 , 80, 80, 80, 170 };
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_PARALIST);
	pList->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	LV_COLUMN nListColumn;

	for (int i = 0; i < 6; i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwName[i];
		m_ParaList.InsertColumn(i, &nListColumn);
	}
}

void COnlineHSMSDlg::Init_DataBase() {
	for (int i = 1; i < CSecsParam::spLASTID; i++) {
		char no[64];
		sprintf(no, "%d", 1);
		int n = 1;
		int idx = m_ParaList.InsertItem(m_ParaList.GetItemCount(), no);
		m_ParaList.SetItemText(idx, n++, CEIDCtlr.m_SecsParam.Param[i].Name);
		m_ParaList.SetItemText(idx, n++, CEIDCtlr.m_SecsParam.Param[i].GetVStr());
		m_ParaList.SetItemText(idx, n++, CEIDCtlr.m_SecsParam.Param[i].GetRMinChar());
		m_ParaList.SetItemText(idx, n++, CEIDCtlr.m_SecsParam.Param[i].GetRMaxChar());
		m_ParaList.SetItemText(idx, n++, CEIDCtlr.m_SecsParam.Param[i].Description);
	}
}

BOOL COnlineHSMSDlg::ParaDB_Saving(CString Name, CString Value) {
	for (int i = 1; i < CSecsParam::spLASTID; i++) {
		CSecsParamItem* p = &CEIDCtlr.m_SecsParam.Param[i];
		if (p->Name == Name) {
			switch (p->type) {
			case 2:
				strscpy(p->cValue, MAXVALUELENGTH, Value.GetBuffer(0));
				break;
			default:
				p->iValue = atoi(Value);
				break;
			}
			p->SaveParam();
		}
		//		CEIDCtlr.m_SecsParam.Param[i].SaveParam();
	}
	return TRUE;
}

BOOL COnlineHSMSDlg::ParaVal_Setting(CString& pName, CString& pVal, CString& Min, CString& Max) {
	pName.TrimLeft();
	pName.TrimRight();
	int NewVal, nMin, nMax;
	NewVal = atoi(pVal);
	nMin = atoi(Min);
	nMax = atoi(Max);

	if (pName == "T3TIMER") {
		if ((NewVal >= nMin) && (NewVal <= nMax)) {
			//m_T3Timer = NewVal * 1000;
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spT3TIMER].Set(pName.GetBuffer(0), NULL,/*m_T3Timer*/NewVal * 1000, nMin, nMax);
			return TRUE;
		}
		return FALSE;
	}
	else if (pName == "T5TIMER") {
		if ((NewVal >= nMin) && (NewVal <= nMax)) {
			//m_T5Timer = NewVal * 1000;
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spT5TIMER].Set(pName.GetBuffer(0), NULL,/*m_T5Timer*/NewVal * 1000, nMin, nMax);
			return TRUE;
		}
		return FALSE;
	}
	else if (pName == "T6TIMER") {
		if ((NewVal >= nMin) && (NewVal <= nMax)) {
			//m_T6Timer = NewVal * 1000;
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spT6TIMER].Set(pName.GetBuffer(0), NULL,/*m_T6Timer*/NewVal * 1000, nMin, nMax);
			return TRUE;
		}
		return FALSE;
	}
	else if (pName == "T7TIMER") {
		if ((NewVal >= nMin) && (NewVal <= nMax)) {
			//m_T7Timer = NewVal * 1000;
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spT7TIMER].Set(pName.GetBuffer(0), NULL,/*m_T7Timer*/NewVal * 1000, nMin, nMax);
			return TRUE;
		}
		return FALSE;
	}
	else if (pName == "T8TIMER") {
		if ((NewVal >= nMin) && (NewVal <= nMax)) {
			//m_T8Timer = NewVal * 1000;
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spT8TIMER].Set(pName.GetBuffer(0), NULL,/*m_T8Timer*/NewVal * 1000, nMin, nMax);
			return TRUE;
		}
		return FALSE;
	}
	else if (pName == "SYSTEMBYTE") {
		sscanf(pVal.operator const char* (), "%8lx", &NewVal);
		sscanf(Min.operator const char* (), "%8lx", &nMin);
		sscanf(Max.operator const char* (), "%8lx", &nMax);
		if ((NewVal >= nMin) && (NewVal <= nMax)) {
			m_SystemByte = NewVal;
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spSYSTEMBYTE].Set(pName.GetBuffer(0), NULL, m_SystemByte, nMin, nMax);
			return TRUE;
		}
		return FALSE;
	}
	else if (pName == "SYSTEMBYTEINC") {
		if ((NewVal >= nMin) && (NewVal <= nMax)) {
			m_SysByteInc = NewVal;
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spSYSTEMBYTEINC].Set(pName.GetBuffer(0), NULL, m_SysByteInc, nMin, nMax);
			return TRUE;
		}
		return FALSE;
	}
	else if (pName == "ENTITY") {
		if ((NewVal >= nMin) && (NewVal <= nMax)) {
			m_ConnectionMode = NewVal;
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spENTITY].Set(pName.GetBuffer(0), NULL, m_ConnectionMode, nMin, nMax);
			return TRUE;
		}
		return FALSE;
	}
	else if (pName == "DEVICEID") {
		if ((NewVal >= nMin) && (NewVal <= nMax)) {
			m_ConnectionMode = NewVal;
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spDEVICEID].Set(pName.GetBuffer(0), NULL, m_ConnectionMode, nMin, nMax);
			return TRUE;
		}
		return FALSE;
	}
	else if (pName == "PASSIVEPORT") {
		if ((NewVal >= nMin) && (NewVal <= nMax)) {
			m_SocketPortNo = NewVal;
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spPASSIVEPORT].Set(pName.GetBuffer(0), NULL, m_SocketPortNo, nMin, nMax);
			return TRUE;
		}
		return FALSE;
	}
	else if (pName == "ACTIVEIP") {
		strscpy(m_cActIP, sizeof(m_cActIP), pVal.GetBuffer(0));
		CEIDCtlr.m_SecsParam.Param[CSecsParam::spACTIVEIP].Set(pName.GetBuffer(0), NULL, m_cActIP, nMin, nMax);
		return TRUE;
	}
	else if (pName == "ACTIVEPORT") {
		if ((NewVal >= nMin) && (NewVal <= nMax)) {
			m_ActPort = NewVal;
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spACTIVEPORT].Set(pName.GetBuffer(0), NULL, m_ActPort, nMin, nMax);
			return TRUE;
		}
		return FALSE;
	}
	else if (pName == "DEFTONLINE") {
		switch (atoi(pVal)) {
		default:				// [20240307 ZHIMING why is this intentionally immutable? $5 for answer
			return FALSE;
			break;
		case CHsmsParaMeter::DEF_LOCAL:
		case CHsmsParaMeter::DEF_REMOTE:
			m_DefOnlineState = (CHsmsParaMeter::DEFONLINE)atoi(pVal);
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spDEFAULTONLINESTATE].Set(pName.GetBuffer(0), NULL, m_DefOnlineState, nMin, nMax);
			break;
		}
		return TRUE;
	}
	else if (pName == "DEFTOFFLINE") {
		switch (atoi(pVal)) {
		default:				// [20240307 ZHIMING why is this intentionally immutable? $5 for answer
			return FALSE;
			break;
		case CHsmsParaMeter::DEF_HOST_OFF:
		case CHsmsParaMeter::DEF_EQP_OFF:
			m_DefOfflineState = (CHsmsParaMeter::DEFOFFLINE)atoi(pVal);
			CEIDCtlr.m_SecsParam.Param[CSecsParam::spDEFAULTOFFLINESTATE].Set(pName.GetBuffer(0), NULL, m_DefOfflineState, nMin, nMax);
			break;
		}
		return TRUE;
	}

	return FALSE;
}

void COnlineHSMSDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "COnlineHSMSDlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void COnlineHSMSDlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}