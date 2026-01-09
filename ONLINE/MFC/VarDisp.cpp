// VarDisp.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "strSOp.h"

#include "128/GEM/ObjectsContainer.h"
#include "GlobalDeviceContainer.h"
#include "ONLINE/GEM/GemFactory.h"
#include "ONLINE/GEM/SVID_LIST.h"
#include "ONLINE/INC/Define.h"
#include "ONLINE/INC/Event.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "ONLINE/ShareMemory/SharedMemQue.h"

#include "VarDisp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVarDisp dialog

CVarDisp::CVarDisp(CWnd* pParent /*=NULL*/)
	: CDialog(CVarDisp::IDD, pParent) {
	//{{AFX_DATA_INIT(CVarDisp)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_Type = 0;
}

void CVarDisp::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVarDisp)
	DDX_Control(pDX, IDC_EDIT_VARVAL, m_VarData);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVarDisp, CDialog)
	//{{AFX_MSG_MAP(CVarDisp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVarDisp message handlers

BOOL CVarDisp::OnInitDialog() {
	CDialog::OnInitDialog();
	CString WndTitle;
	WndTitle.Format("Information for %s %d", (m_Type == 1) ? "Variable ID " : "CEID ", m_VarNo);
	SetWindowText(WndTitle);

	std::string data;
	switch (m_Type) {
	case 1:
		GetSVIDData(data);
		break;
	case 2:
		GetCEIDData(data);
		break;
	case 3:
		GetECIDData(data);
		break;
	}
	m_VarData.SetWindowText(data.c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CVarDisp::WriteDataToList(CListCtrl& List, char* Data) {
	LV_COLUMN nListColumn;

	nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	nListColumn.fmt = LVCFMT_LEFT;
	nListColumn.cx = 100;
	nListColumn.iSubItem = 0;
	nListColumn.pszText = "Data";
	List.InsertColumn(0, &nListColumn);

	char Line[512];
	int nLen = strlen(Data);
	int BufInd = 0;
	int Row = 0;
	for (int i = 0; i < nLen; i++) {
		if (Data[i] == '\n') {
			Line[BufInd] = 0;
			List.InsertItem(Row++, Line);
			memset(Line, 0, sizeof Line);
			BufInd = 0;
			continue;
		}
		Line[BufInd++] = Data[i];
	}
	Line[BufInd] = 0;
	List.InsertItem(Row++, Line);
}

// obsolete: online and application is now a single process [9/11/2007 LYF]
void CVarDisp::PostReq() {
	CString WndTitle, ItemType, ItemName;
	short Request = 0;
	char Buf[126] = { 0 };
	switch (m_Type) {
	case REQ_VAR_DATA:
		Request = REQ_VAR_DATA;
		ItemType = "Variable ID";
		ItemName = _itoa(m_VarNo, Buf, 10);
		break;
	case REQ_CEID_DATA:
		Request = REQ_CEID_DATA;
		ItemType = "Collection Event ID";
		ItemName = _itoa(m_VarNo, Buf, 10);
		break;
	case REQ_PJDATA:
		Request = REQ_PJDATA;
		ItemType = "Process Job";
		ItemName = m_VarName.c_str();
		break;
	case REQ_CJDATA:
		Request = REQ_CJDATA;
		ItemType = "Control Job";
		ItemName = m_VarName.c_str();
		break;
	}
	if (Request) {
		WndTitle.Format("Information for %s %s", ItemType, ItemName);
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->Set(EVT_UI_DATA_REQUEST, Request, m_VarNo);
		pEvt->SetMsg((BYTE*)m_VarName.c_str(), m_VarName.size());
		pEvt->ReplyWnd = m_hWnd;
		CSharedMemQue EvtQue;
		EvtQue.OpenMemory("EVTMSG");
		EvtQue.WriteQue(pEvt);
		EvtQue.CloseMemory();
		SetWindowText(WndTitle);
	}
	m_VarData.SetWindowText("No data available");
}

// Note:  Yuen[2002/07/28 11:36]
void CVarDisp::GetSVIDData(std::string& data) {
	MakeSVIDReport(m_VarNo - 1, data);	// SVID list is zero offset [4/28/2013 Administrator]
}

void CVarDisp::GetCEIDData(std::string& data) {
	MakeCEIDReport(m_VarNo - 1, data);	// CEID list is zero offset [4/28/2013 Administrator]
}

void CVarDisp::GetECIDData(std::string& data) {
	MakeECIDReport(m_VarNo - 1, data);	// ECID list is zero offset [4/28/2013 Administrator]
}

void CVarDisp::MakeCEIDReport(short iCEID, std::string& data) {
	UINT CEID = CEIDCtlr.m_CEList.m_Ceid[iCEID].m_ceid;

	char Buf[512];

	UINT RptCount = CEIDCtlr.get_rptcount(CEID);

	data = "CEID: ";
	data += _itoa(CEID, Buf, 10);
	data += CEIDCtlr.check_rptflag(CEID) ? " (Enabled)" : " (Disabled)";

	std::string desc;
	CEIDCtlr.CeidDesc(CEID, desc);
	data += "\r\nDescription: " + desc;

	if (RptCount) {
		data += "\r\n";
		data += _itoa(RptCount, Buf, 10);
		data += " Report(s) attached";
		UINT* pReports = new UINT[RptCount];
		CEIDCtlr.getting_rptno(CEID, pReports);
		for (int nIndex = 0; nIndex < int(RptCount); nIndex++) {
			data += "\r\nReport ID: ";
			//data += _itoa(nIndex + 1, Buf, 10);
			//data += "ID= ";
			data += _itoa(pReports[nIndex], Buf, 10);
			data += "\r\n  SVID\t";
			UINT nSvIds = CEIDCtlr.get_svidcount(pReports[nIndex]);
			if (nSvIds) {
				UINT* pSvIds = new UINT[nSvIds];
				CEIDCtlr.get_svid(pReports[nIndex], pSvIds);
				for (int nIndex2 = 0; nIndex2 < int(nSvIds); nIndex2++) {
					data += _itoa(pSvIds[nIndex2], Buf, 10);	// SVID number [4/28/2013 Administrator]
					if (nIndex2 != nSvIds - 1) {
						data += " , ";
					}
				}
				delete[]pSvIds;
			}
		}
		delete[]pReports;
	}
}

void CVarDisp::MakeSVIDReport(short SVID, std::string& data) {
	data = "";
	std::string type;

	SecsItem* pItem = CGemFactory::get_svval(1, SVID, NULL);
	if (pItem) {
		type = pItem->TypeStr();
		pItem->GetData(data);
		delete pItem;
	}
	else {
		data = "DataNotFound";
		delete pItem;
		return;
	}

	// pItem no longer valid from this point onwards [9/14/2007 LYF]

	std::string desc;
	CEIDCtlr.SvidDesc(SVID, desc);

	if (type != "LIST") {
		desc += "\r\nFormat: " + type + "\r\nValue {";
		if (type != "ASCII") {
			desc += " ";
		}
		else {
			desc += " \"";
		}
	}
	else {
		desc += "\r\nFormat: " + type + "\r\nValue\r\n{";
		desc += "\r\n";
	}
	data = desc + data;
	if (type != "LIST") {
		if (type != "ASCII") {
			data += " ";
		}
	}

	switch (CEIDCtlr.m_SVList.GetIndex(SVID)) {
	case SVID_LIST::LINEMODE:
	case SVID_LIST::CONTROLSTATELINEMODE:
		data += "(";
		data += pGDev->pGEMObjs->GetLineModeStr() + ") "; //  [9/14/2007 LYF]
		break;
	case SVID_LIST::EQUIPMENTSTATUS:
		data += "(";
		data += pGDev->GetARAMSStateStr() + ") "; //  [9/14/2007 LYF]
		break;
	case SVID_LIST::LP1SLOTMAPSTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[0].CaObj.GetSlotmapStatusStr() + ") "; //  [9/14/2007 LYF]
		break;
	case SVID_LIST::LP2SLOTMAPSTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[1].CaObj.GetSlotmapStatusStr() + ") "; //  [9/14/2007 LYF]
		break;
	case SVID_LIST::LP1OCCUPANCYSTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[0].LPObj.GetOccupencyStr() + ") "; //  [9/14/2007 LYF]
		break;
	case SVID_LIST::LP2OCCUPANCYSTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[1].LPObj.GetOccupencyStr() + ") "; //  [9/14/2007 LYF]
		break;
	case SVID_LIST::LP1TRANSFERSTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[0].LPObj.GetTransferStatusStr() + ") "; //  [9/14/2007 LYF]
		break;
	case SVID_LIST::LP2TRANSFERSTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[1].LPObj.GetTransferStatusStr() + ") "; //  [9/14/2007 LYF]
		break;
	case SVID_LIST::LP1ACCESSMODE:
		data += "(";
		data += pGDev->pGEMObjs->Cms[0].LPObj.GetAccessModeStr() + ") ";
		break;
	case SVID_LIST::LP1ASSOCIATESTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[0].LPObj.GetAssociationStr() + ") ";
		break;
	case SVID_LIST::LP1RESERVATIONSTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[0].LPObj.GetReservationStr() + ") ";
		break;
	case SVID_LIST::LP2ACCESSMODE:
		data += "(";
		data += pGDev->pGEMObjs->Cms[1].LPObj.GetAccessModeStr() + ") ";
		break;
	case SVID_LIST::LP2ASSOCIATESTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[1].LPObj.GetAssociationStr() + ") ";
		break;
	case SVID_LIST::LP2RESERVATIONSTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[1].LPObj.GetReservationStr() + ") ";
		break;
	case SVID_LIST::LP1BARCODESTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[0].BCR.GetServiceStatusStr() + ") ";
		break;
	case SVID_LIST::LP2BARCODESTATUS:
		data += "(";
		data += pGDev->pGEMObjs->Cms[1].BCR.GetServiceStatusStr() + ") ";
		break;
	}
	if (type != "ASCII") {
		data += "}";
	}
	else {
		data += "\" }";
	}
}

void CVarDisp::MakeECIDReport(short ECID, std::string& data) {
	char ecval[128];
	SVID_LIST* p = &CEIDCtlr.m_SVList;
	short idx = p->GetIndex(ECID);
	if (idx == -1) {
		data = "ECID NOT FOUND";
		return;
	}
	strscpy(ecval, sizeof(ecval), p->m_Svid[idx].GetVChar());

	data = "ECID: " + tostring(ECID);
	data += "\r\nDescription: ";
	data += p->m_Svid[idx].m_svname;
	data += "\r\nFormat: " + typestr(p->m_Svid[idx].m_type);
	data += "\r\nValue ";
	if (p->m_Svid[idx].m_type != svASCII) {
		data += "{ ";
	}
	else {
		data += "{ \"";
	}
	data += ecval;
	if (p->m_Svid[idx].m_type != svASCII) {
		data += " }";
	}
	else {
		data += "\" }";
	}
}

std::string CVarDisp::tostring(short val) {
	std::string str;
	CString ss;

	ss.Format("%d", val);
	str = ss;
	return str;
}

std::string CVarDisp::typestr(short type) {
	switch (type) {
	case svLIST:
		return "LIST";
	case svASCII:
		return "ASCII";
	case svBINARY:
		return "BINARY";
	case svBOOLEAN:
		return "BOOLEAN";
	case svUNSIGN1:
		return "UNSIGN1";
	case svUNSIGN2:
		return "UNSIGN2";
	case svUNSIGN4:
		return "UNSIGN4";
	case svUNSIGN8:
		return "UNSIGN8";
	case svINT1:
		return "INT1";
	case svINT2:
		return "INT2";
	case svINT4:
		return "INT4";
	case svINT8:
		return "INT8";
	case svFLOAT4:
		return "FLOAT4";
	case svFLOAT8:
		return "FLOAT8";
	}
	return "UNKNOWN";
}